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


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning
    BasicMathsTest();

    u32*   pix_arr = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray ray = {{0, 0, 0}, {0, 0, 0}};
    Sphere sphere = {{0.0f, 0.0f, -1.0f}, 0.250f, 0x00000000FF};
    v3 scale_vector = {0};
    SetScaleVector(&scale_vector);


    size_t pix_arr_idx = 0;
    u32    this_color;
    r32    x_component = 0;
    r32    y_component = 0;
    /* r32    z_component = 0; */
    r32    _t_ = -1;
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

            // Correct for the origin starting from LLHC and progressing
            // rightwards and upwards with screen coords bounded at {-1, 1}
            x_component = ((r32)X / (r32)IMAGE_WIDTH)  - 0.5f;
            y_component = ((r32)Y / (r32)IMAGE_HEIGHT) - 0.5f;

            // Correct for image scaling
            x_component *= scale_vector.x;
            y_component *= scale_vector.y;

            // Clip
            if (x_component > 1.0f) { x_component = 1.0f; }
            if (y_component > 1.0f) { y_component = 1.0f; }
            if (x_component < -1.0f) { x_component = -1.0f; }
            if (y_component < -1.0f) { y_component = -1.0f; }

            v3SetAndNorm(&ray.direction,
                         x_component,
                         y_component,
                         -1);


            if (DoesIntersectSphere(&ray, &sphere, &_t_))
            {
                point_of_intersection.x = ray.origin.x + (_t_ * ray.direction.x);
                point_of_intersection.y = ray.origin.y + (_t_ * ray.direction.y);
                point_of_intersection.z = ray.origin.z + (_t_ * ray.direction.z);
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
