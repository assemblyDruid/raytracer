#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <rtx.h>

//
// Image Properties
//
// Square
/* #define IMAGE_WIDTH  720 */
/* #define IMAGE_HEIGHT 720 */

// 720p
#define IMAGE_WIDTH  1280
#define IMAGE_HEIGHT 720

// Full HD
/* #define IMAGE_WIDTH  1920 */
/* #define IMAGE_HEIGHT 1080 */

// 4K
/* #define IMAGE_WIDTH  3840 */
/* #define IMAGE_HEIGHT 2160 */

#define ASPECT_RATIO  (r32)(IMAGE_WIDTH/(r32)IMAGE_HEIGHT)
#define NUM_PIXELS    IMAGE_WIDTH*IMAGE_HEIGHT
#define H_FEILD_OF_VIEW 114
// #define V_FEILD_OF_VIEW 150 [ cfarvin::NOTE ] Unused (for now?)
#define IMAGE_NAME    "rtx.bmp"


internal void
WriteBitmap(u32* pixarr, u32 image_width, u32 image_height)
{
    assert(pixarr && image_width && image_height);
    u32 pixarr_size = sizeof(u32) * NUM_PIXELS;

    BitmapHeader bitmap_header = {0};
    bitmap_header.magic_number = 0x4D42;
    bitmap_header.file_size = pixarr_size + sizeof(BitmapHeader);
    bitmap_header.pix_arr_offset = sizeof(BitmapHeader);
    bitmap_header.bitmap_header_size = sizeof(BitmapHeader) - 14;
    bitmap_header.bitmap_width = image_width;
    bitmap_header.bitmap_height = image_height;
    bitmap_header.num_color_planes = 1;
    bitmap_header.bits_per_pix = 32;
    bitmap_header.pix_arr_size = pixarr_size;

    FILE* bitmap_file = fopen(IMAGE_NAME, "wb");
    assert(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pixarr,  pixarr_size, 1, bitmap_file);
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning

    BasicMathsTest();
    XorShift32State = 42;

    u32* pixarr = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray  ray     = {{0, 0, 0}, {0, 0, -1}};

    for (size_t ii = 0; ii < 1000000; ii++)
    {
        r32 rand = NormalBoundedXorShift32();
        assert(rand >= 0.0f && rand <= 1.0f);
    }

    size_t  num_spheres = 0;
    Sphere sphere_arr[3] = {0};
    sphere_arr[0] = CreateSphereRaw(-0.00f,
                                    -3.00f,
                                    -9.0f,
                                    +5.0f, // Radius
                                    0xFF00CA00,
                                    &num_spheres);

    sphere_arr[1] = CreateSphereRaw(+0.00f,
                                    +0.00f,
                                    -3.50f,
                                    +0.5f, // Radius
                                    0xFF0000CA,
                                    &num_spheres);

    size_t pixarr_idx = 0;
    bool does_intersect = false;
    Color pix_color = {0};
    r32 distance = MAX_RAY_DIST;
    r32 aspect_ratio = IMAGE_WIDTH/(r32)IMAGE_HEIGHT;
    for (size_t pix_y = 0;
         pix_y < IMAGE_HEIGHT;
         pix_y++)
    {
        for (size_t pix_x = 0;
             pix_x < IMAGE_WIDTH;
             pix_x++)
        {
            r32 closest_obj_t = (r32)MAX_RAY_DIST;
            pix_color.value = 0xFF202020; // Default color
            pixarr_idx++;
            ray.direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f) * aspect_ratio;
            ray.direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;

            for (size_t sphere_index = 0;
                 sphere_index < num_spheres;
                 sphere_index++)
            {
                Sphere sphere = sphere_arr[sphere_index];
                does_intersect = DoesIntersectSphere(&ray, &sphere, &distance);
                if ( does_intersect &&
                     (distance < closest_obj_t) &&
                     (distance <= MAX_RAY_DIST) &&
                     (distance >= MIN_RAY_DIST))
                {
                    closest_obj_t = distance;
                    pix_color.channel.R = (u8)(sphere.material.color.channel.R / distance);
                    pix_color.channel.G = (u8)(sphere.material.color.channel.G / distance);
                    pix_color.channel.B = (u8)(sphere.material.color.channel.B / distance);
                }
            }

            pixarr[pixarr_idx] = pix_color.value;
        }
    }

    WriteBitmap(pixarr, IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("[ success ]\n");
    return 0;
}
