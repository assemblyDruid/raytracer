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
        scale_ratio = (IMAGE_WIDTH/(r32)IMAGE_HEIGHT);
        v3Set(scale_vector, scale_ratio, 1.0f, 1.0f);
    }
    else
    {
        scale_ratio = (IMAGE_HEIGHT/(r32)IMAGE_WIDTH);
        v3Set(scale_vector, 1.0f, scale_ratio, 1.0f);
    }
}


// [ cfarvin::TODO ] This is a temporary hack to get things working.
// [ cfarvin::TODO ] Impl dynamic array or bring from UnderstoneEngine.
internal void
InitializeSpheres(Sphere** sphere_buffer,
                  size_t* const spheres_created)
{
#define __MAX_SPHERES__ 512
    Sphere tmp_sphere_arr[__MAX_SPHERES__] = {0};

    tmp_sphere_arr[0] = CreateSphereRaw(+0.00f,     // xpos
                                        +0.00f,     // ypos
                                        -1.00f,     // zpos
                                        +0.25f,     // radius
                                        0xFF0000FF, // color
                                        spheres_created);

    tmp_sphere_arr[1] = CreateSphereRaw(+0.75f,     // xpos
                                        +0.00f,     // ypos
                                        -1.00f,     // zpos
                                        +0.25f,     // radius
                                        0xFF00FF00, // color
                                        spheres_created);

    assert(*spheres_created <= __MAX_SPHERES__);
#undef __MAX_SPHERES__

    size_t sphere_bytes = sizeof(Sphere) * (*spheres_created);
    *sphere_buffer = (Sphere*)malloc(sphere_bytes);
    memcpy(*sphere_buffer, &tmp_sphere_arr, sphere_bytes);
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning

    BasicMathsTest();

    // [ cfarvin::TODO ] This is a temporary hack to get things working.
    size_t  num_spheres = 0;
    Sphere* sphere_arr  = NULL;
    InitializeSpheres(&sphere_arr, &num_spheres);
    assert(sphere_arr);

    u32* pix_arr = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray  ray     = {{0, 0, 0}, {0, 0, -1}};

    v3 scale_vector = {0};
    SetScaleVector(&scale_vector);

    v3 lower_left_corner = {-(r32)(IMAGE_WIDTH/2), -(r32)(IMAGE_HEIGHT/2), -1};
    v3Norm(&lower_left_corner);

    size_t pix_arr_idx = 0;
    Color  this_color  = {0};
    r32    _t_         = -1;
    for (size_t Y = 0;
         Y < IMAGE_HEIGHT;
         Y++)
    {
        for (size_t X = 0;
             X < IMAGE_WIDTH;
             X++)
        {
            r32 closest_obj_t = (r32)MAXIMUM_RAY_TIME;
            /* this_color.value  = 0xFFFFFFFF; */
            this_color.value = 0x00000000;
            pix_arr_idx = (IMAGE_WIDTH * Y) + X;

            ray.direction.x = lower_left_corner.x + (X/(r32)IMAGE_WIDTH) * scale_vector.x;
            ray.direction.y = lower_left_corner.y + (Y/(r32)IMAGE_HEIGHT) * scale_vector.y;
            v3Norm(&ray.direction);

            for (size_t sphere_index = 0;
                 sphere_index < num_spheres;
                 sphere_index++)
            {
                Sphere sphere = sphere_arr[sphere_index];
                if (DoesIntersectSphere(&ray, &sphere, &_t_) &&
                    (_t_ < closest_obj_t))
                {
                    closest_obj_t = _t_;

                    this_color._color_channel.R = (u8)(sphere.material.color._color_channel.R -
                                                    (2 * sphere.material.color._color_channel.R * _t_));
                    this_color._color_channel.G = (u8)(sphere.material.color._color_channel.G -
                                                    (2 * sphere.material.color._color_channel.G * _t_));
                    this_color._color_channel.B = (u8)(sphere.material.color._color_channel.B -
                                                    (2 * sphere.material.color._color_channel.B * _t_));

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
