#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <rtx.h>

//
// Image Properties
//
// 720p
#define IMAGE_WIDTH  1280
#define IMAGE_HEIGHT 720

// Full HD
/* #define IMAGE_WIDTH  1920 */
/* #define IMAGE_HEIGHT 1080 */

// 4K
/* #define IMAGE_WIDTH  3840 */
/* #define IMAGE_HEIGHT 2160 */
#define NUM_PIXELS   IMAGE_WIDTH*IMAGE_HEIGHT
#define IMAGE_NAME   "rtx.bmp"


internal void
WriteBitmap(u32* pix_arr, u32 image_width, u32 image_height)
{
    assert(pix_arr);
    u32 pix_arr_size = sizeof(u32) * NUM_PIXELS;

    BitmapHeader bitmap_header = {0};
    bitmap_header.magic_number = 0x4D42;
    bitmap_header.file_size = pix_arr_size + sizeof(BitmapHeader);
    bitmap_header.pix_arr_offset = sizeof(BitmapHeader);
    bitmap_header.bitmap_header_size = sizeof(BitmapHeader) - 14;
    bitmap_header.bitmap_width = image_width;
    bitmap_header.bitmap_height = image_height;
    bitmap_header.num_color_planes = 1;
    bitmap_header.bits_per_pix = 32;
    bitmap_header.pix_arr_size = pix_arr_size;

    FILE* bitmap_file = fopen(IMAGE_NAME, "wb");
    assert(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pix_arr,  pix_arr_size, 1, bitmap_file);
}


internal void
SetScaleVector(v3* const scale_vector)
{
    assert(scale_vector);
    r32 scale_ratio = 0.0f;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4127)
#endif
    if (IMAGE_WIDTH > IMAGE_HEIGHT)
#ifdef _WIN32
#pragma warning(pop)
#endif
    {
        scale_ratio = 1 / ((r32)IMAGE_HEIGHT / (r32)IMAGE_WIDTH);
        v3Set(scale_vector, scale_ratio, 1.0f, 0.0f);
    }
    else
    {
        scale_ratio = 1 / ((r32)IMAGE_WIDTH / (r32)IMAGE_HEIGHT);
        v3Set(scale_vector, 1.0f, scale_ratio, 0.0f);
    }
}


internal void
InitializeImagePlane(ImagePlane* const image_plane,
                     r32 x_max,
                     r32 x_min,
                     r32 y_max,
                     r32 y_min,
                     r32 z_min)
{
    v3*  LLHC            = (v3*)&(image_plane->LLHC);
    v3*  ULHC            = (v3*)&(image_plane->ULHC);
    v3*  URHC            = (v3*)&(image_plane->URHC);
    v3*  LRHC            = (v3*)&(image_plane->LRHC);
    r32* vertical_span   = (r32*)&(image_plane->vertical_span);
    r32* horizontal_span = (r32*)&(image_plane->horizontal_span);
    r32* plane_z         = (r32*)&(image_plane->plane_z);

    v3Set(LLHC, x_min, y_min, z_min);
    v3Set(ULHC, x_min, y_max, z_min);
    v3Set(URHC, x_max, y_max, z_min);
    v3Set(LRHC, x_max, y_min, z_min);
    *plane_z = z_min;

    r32 abs_x_max = (x_max > 0) ? x_max : (-1 * x_max);
    r32 abs_x_min = (x_min > 0) ? x_min : (-1 * x_min);
    r32 abs_y_max = (y_max > 0) ? y_max : (-1 * y_max);
    r32 abs_y_min = (y_min > 0) ? y_min : (-1 * y_min);

    *vertical_span   = abs_y_max - abs_y_min;
    *horizontal_span = abs_x_max - abs_x_min;
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning
    BasicMathsTest();

    u32*   pix_arr = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray ray = {{0, 0, 0}, {0, 0, 0}};
    v3 scale_vector = {0};
    SetScaleVector(&scale_vector);

    ImagePlane image_plane = {0};
    InitializeImagePlane(&image_plane,
                         +1.0f,
                         -1.0f,
                         +1.0f,
                         -1.0f,
                         -0.5f);

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4204)
#endif
    Sphere sphere = {{0.0f, 0.0f, (r32)image_plane.plane_z}, 0.5f, {0x00000000FF}};
#ifdef _WIN32
#pragma warning(pop)
#endif

    size_t pix_arr_idx = 0;
    u32    this_color;
    r32    x_component = 0;
    r32    y_component = 0;
    /* r32    z_component = 0; */
    r32    distance = -1;
    v3    point_of_intersection = {0};
    v3    normal_vector = {0};
    for (size_t Y = 0;
         Y < IMAGE_HEIGHT;
         Y++)
    {
        for (size_t X = 0;
             X < IMAGE_WIDTH;
             X++)
        {
            this_color  = 0;
            pix_arr_idx = (IMAGE_WIDTH * Y) + X;

            x_component = image_plane.LLHC.x + ((r32)(X/(r32)IMAGE_WIDTH));
            y_component = image_plane.LLHC.y + ((r32)(Y/(r32)IMAGE_HEIGHT));

            // Correct for image scaling
            x_component *= scale_vector.x;
            y_component *= scale_vector.y;

            // Clip
            /* if (x_component > 1.0f) { x_component = 1.0f; } */
            /* if (y_component > 1.0f) { y_component = 1.0f; } */
            /* if (x_component < -1.0f) { x_component = -1.0f; } */
            /* if (y_component < -1.0f) { y_component = -1.0f; } */

            v3SetAndNorm(&ray.direction,
                         x_component,
                         y_component,
                         -1);

            if (DoesIntersectSphere(&ray, &sphere, &distance))
            {
                point_of_intersection.x = ray.origin.x + (distance * ray.direction.x);
                point_of_intersection.y = ray.origin.y + (distance * ray.direction.y);
                point_of_intersection.z = ray.origin.z + (distance * ray.direction.z);
                v3Sub(&point_of_intersection, &sphere.position, &normal_vector);
                v3Norm(&normal_vector);

                if (normal_vector.x < 0) { normal_vector.x *= -1; };
                if (normal_vector.y < 0) { normal_vector.y *= -1; };
                if (normal_vector.z < 0) { normal_vector.z *= -1; };

                this_color = 0x000000FF;
                this_color = this_color << 8;
                this_color += (u8)(normal_vector.x * 255);
                this_color = this_color << 8;
                this_color += (u8)(normal_vector.y * 255);
                this_color = this_color << 8;
                this_color += (u8)(normal_vector.z * 255);
            }

            pix_arr[pix_arr_idx] = this_color;;
        }
    }

    WriteBitmap(pix_arr, IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("[ success ]\n");
    return 0;
}
