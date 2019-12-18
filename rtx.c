#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <rtx.h>

//
// Endianness
//
#ifndef __LSB__
#define __LSB__ 1 // 1=LSB/Little Endian, 0=MSB/Big Endian
#if __LSB__
#define _color_channel LSB_channel
#else
#define _color_channel MSB_channel
#endif // __LSB__ (if)
#endif // __LSB__ (ifndef)

//
// Image Properties
//
// 720p
#define IMAGE_WIDTH  1280
#define IMAGE_HEIGHT 720
#define ASPECT_RATIO (r32)(IMAGE_WIDTH/(r32)IMAGE_HEIGHT)

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
    assert(pix_arr && image_width && image_height);
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
SetScaleVector(v3* const scale_vec)
{
    assert(scale_vec);
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
        scale_ratio = (IMAGE_WIDTH/(r32)IMAGE_HEIGHT);
        v3Set(scale_vec, scale_ratio, 1.0f, 1.0f);
    }
    else
    {
        scale_ratio = (IMAGE_HEIGHT/(r32)IMAGE_WIDTH);
        v3Set(scale_vec, 1.0f, scale_ratio, 1.0f);
    }
}



// [ cfarvin::TODO ] This is a temporary hack to get things working.
// [ cfarvin::TODO ] Impl dynamic array or bring from UnderstoneEngine.
internal void
InitializeSpheres(Sphere** sphere_buffer,
                  size_t* const spheres_created,
                  ImagePlane* const img_plane)
{
#define __MAX_SPHERES__ 512
    assert(sphere_buffer && spheres_created);
    Sphere tmp_sphere_arr[__MAX_SPHERES__] = {0};

    tmp_sphere_arr[0] = CreateSphereRaw(+0.00f,
                                        +0.00f,
                                        img_plane->z_dist,
                                        +0.025f, // Radius
                                        0xFF0000FF,
                                        spheres_created);

    /* tmp_sphere_arr[1] = CreateSphereRaw(+0.50f, */
    /*                                     +0.00f, */
    /*                                     img_plane->z_dist, */
    /*                                     +0.50f, // Radius */
    /*                                     0xFF00FF00, */
    /*                                     spheres_created); */

    assert(*spheres_created <= __MAX_SPHERES__);
#undef __MAX_SPHERES__

    size_t sphere_bytes = sizeof(Sphere) * (*spheres_created);
    *sphere_buffer = (Sphere*)malloc(sphere_bytes);
    memcpy(*sphere_buffer, &tmp_sphere_arr, sphere_bytes);
}


internal void
InitializeImagePlane(ImagePlane* const img_plane,
                     r32 x_max,
                     r32 x_min,
                     r32 y_max,
                     r32 y_min,
                     r32 z_min)
{
    assert(img_plane);
    v3*  LLHC   = (v3*)&(img_plane->LLHC);
    v3*  ULHC   = (v3*)&(img_plane->ULHC);
    v3*  URHC   = (v3*)&(img_plane->URHC);
    v3*  LRHC   = (v3*)&(img_plane->LRHC);
    r32* height = (r32*)&(img_plane->height);
    r32* width  = (r32*)&(img_plane->width);
    r32* z_dist = (r32*)&(img_plane->z_dist);

    v3Set(LLHC, x_min, y_min, z_min);
    v3Set(ULHC, x_min, y_max, z_min);
    v3Set(URHC, x_max, y_max, z_min);
    v3Set(LRHC, x_max, y_min, z_min);
    *z_dist = z_min;

    r32 abs_x_max = (x_max > 0) ? x_max : (-1 * x_max);
    r32 abs_x_min = (x_min > 0) ? x_min : (-1 * x_min);
    r32 abs_y_max = (y_max > 0) ? y_max : (-1 * y_max);
    r32 abs_y_min = (y_min > 0) ? y_min : (-1 * y_min);

    *height = abs_y_max + abs_y_min;
    *width  = abs_x_max + abs_x_min;
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning

    BasicMathsTest();

    u32* pix_arr = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray  ray     = {{0, 0, 0}, {0, 0, -1}};

    v3 scale_vec = {0};
    SetScaleVector(&scale_vec);

    /* v3 lower_left_corner = {-(r32)(IMAGE_WIDTH/2), -(r32)(IMAGE_HEIGHT/2), -1}; */
    /* v3Norm(&lower_left_corner); */

    ImagePlane img_plane = {0};
    InitializeImagePlane(&img_plane,
                         +1.0f,
                         -1.0f,
                         +1.0f,
                         -1.0f,
                         -0.10f);

    // [ cfarvin::TODO ] This is a temporary hack to get things working.
    size_t  num_spheres = 0;
    Sphere* sphere_arr  = NULL;
    InitializeSpheres(&sphere_arr, &num_spheres, &img_plane);
    assert(sphere_arr);

    size_t pix_arr_idx = 0;
    Color this_color = {0};
    r32 distance = MAX_RAY_DIST;
    r32 pixel_NDC_x = 0.0f;
    r32 pixel_NDC_y = 0.0f;
    r32 pixel_screen_x = 0.0f;
    r32 pixel_screen_y = 0.0f;
    r32 pixel_camera_x = 0.0f;
    r32 pixel_camera_y = 0.0f;
    for (size_t Y = 0;
         Y < IMAGE_HEIGHT;
         Y++)
    {
        for (size_t X = 0;
             X < IMAGE_WIDTH;
             X++)
        {
            r32 closest_obj_t = (r32)MAX_RAY_DIST;
            this_color.value = 0xFF202020;
            pix_arr_idx = (IMAGE_WIDTH * Y) + X;
            pixel_NDC_x = (r32)((X + 0.5f)/(r32)IMAGE_WIDTH);
            pixel_NDC_y = (r32)((Y + 0.5f)/(r32)IMAGE_HEIGHT);
            /* pixel_screen_x = img_plane.width * pixel_NDC_x - img_plane.LLHC.x; */
            /* pixel_screen_y = img_plane.height * pixel_NDC_y - img_plane.LLHC.y; */
            pixel_screen_x = 2 * pixel_NDC_x - 1;
            pixel_screen_y = 2 * pixel_NDC_y - 1;
            pixel_camera_x = pixel_screen_x * scale_vec.x;
            pixel_camera_y = pixel_screen_y * scale_vec.y;
            ray.direction.x = pixel_screen_x;
            ray.direction.y = pixel_screen_y;
            /* v3Norm&ray.direction); */

            // Intersect Spheres
            for (size_t sphere_index = 0;
                 sphere_index < num_spheres;
                 sphere_index++)
            {
                Sphere sphere = sphere_arr[sphere_index];
                if (DoesIntersectSphere(&ray, &sphere, &distance) &&
                    (distance < closest_obj_t))
                {
                    closest_obj_t = distance;

                    this_color._color_channel.R = (u8)(sphere.material.color._color_channel.R -
                                                    (sphere.material.color._color_channel.R * distance));
                    this_color._color_channel.G = (u8)(sphere.material.color._color_channel.G -
                                                    (sphere.material.color._color_channel.G * distance));
                    this_color._color_channel.B = (u8)(sphere.material.color._color_channel.B -
                                                    (sphere.material.color._color_channel.B * distance));

                    if (this_color._color_channel.R > 255) { this_color._color_channel.R = 255; }
                    if (this_color._color_channel.G > 255) { this_color._color_channel.G = 255; }
                    if (this_color._color_channel.B > 255) { this_color._color_channel.B = 255; }
                    if(this_color.value < 0) { this_color.value = 0; }
                }
            }

            pix_arr[pix_arr_idx] = this_color.value;
        }
    }

    WriteBitmap(pix_arr, IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("[ success ]\n");
    return 0;
}
