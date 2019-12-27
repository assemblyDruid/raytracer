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
#define IMAGE_WIDTH  400
#define IMAGE_HEIGHT 400

// 720p
/* #define IMAGE_WIDTH  1280 */
/* #define IMAGE_HEIGHT 720 */

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


_internal_ void
WriteBitmap(u32* pixel_array, u32 image_width, u32 image_height)
{
    assert(pixel_array && image_width && image_height);
    u32 pixel_array_size = sizeof(u32) * NUM_PIXELS;

    BitmapHeader bitmap_header = {0};
    bitmap_header.magic_number = 0x4D42;
    bitmap_header.file_size = pixel_array_size + sizeof(BitmapHeader);
    bitmap_header.pix_arr_offset = sizeof(BitmapHeader);
    bitmap_header.bitmap_header_size = sizeof(BitmapHeader) - 14;
    bitmap_header.bitmap_width = image_width;
    bitmap_header.bitmap_height = image_height;
    bitmap_header.num_color_planes = 1;
    bitmap_header.bits_per_pix = 32;
    bitmap_header.pix_arr_size = pixel_array_size;

    FILE* bitmap_file = fopen(IMAGE_NAME, "wb");
    assert(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pixel_array,  pixel_array_size, 1, bitmap_file);
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning

    BasicMathsTest();
    XorShift32State = 42; // Initialize XorShift

    u32* pixel_array = (u32*)calloc(IMAGE_WIDTH * IMAGE_HEIGHT, sizeof(u32));
    Ray  ray     = {{0, 0, 0}, {0, 0, -1}};

    for (size_t ii = 0; ii < 1000000; ii++)
    {
        r32 rand = NormalBoundedXorShift32();
        assert(rand >= 0.0f && rand <= 1.0f);
    }

    size_t  num_spheres = 0;
    Sphere sphere_arr[3] = {0};
    sphere_arr[0] = CreateSphereRaw(-0.00f,
                                    -0.00f,
                                    -1.00f,
                                    +0.35f, // Radius
                                    0xFF00CA00,
                                    &num_spheres);

    /* sphere_arr[1] = CreateSphereRaw(+0.00f, */
    /*                                 -0.50f, */
    /*                                 -1.75f, */
    /*                                 +1.00f, // Radius */
    /*                                 0xFF0000CA, */
    /*                                 &num_spheres); */

    RayCollision collision = {0};
    size_t pixel_array_idx = 0;
    bool does_intersect = false;
    Color32 pixel_color = {0};
    r32 aspect_ratio = IMAGE_WIDTH/(r32)IMAGE_HEIGHT;
    r32 closest_object_distance = (r32)MAX_RAY_DIST;
    u8 RAYSpp = 1; // [ cfarvin::TODO ] Splits into quadrants. Why?
    u8 HITSpp = 0;
    for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++) // y pixels
    {
        for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++) // x pixels
        {
            pixel_color.value = 0xFF202020; // Reset pixel color
            closest_object_distance = (r32)MAX_RAY_DIST; // Reset ray distance
            r32 red = 0;
            r32 green = 0;
            r32 blue = 0;
            HITSpp = 0;

            for (u8 aa_ray = 0; aa_ray < RAYSpp; aa_ray++) // Antialiasing (rays per pixel)
            {
                ray.direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f + NormalBoundedXorShift32()) * aspect_ratio;
                ray.direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f + NormalBoundedXorShift32();
                v3Norm(&ray.direction);

                for (size_t sphere_index = 0; sphere_index < num_spheres; sphere_index++) // spheres
                {
                    Sphere sphere = sphere_arr[sphere_index];
                    does_intersect = DoesIntersectSphere(&ray, &sphere, &collision);
                    if (does_intersect && (collision.distance < closest_object_distance))
                    {
                        closest_object_distance = collision.distance;
                        red   += collision.normal_vector.x;
                        green += collision.normal_vector.y;
                        blue  += collision.normal_vector.z;
                        HITSpp++;
                    }
                }
            }

            r32 hit_ratio  = HITSpp/(r32)RAYSpp;
            r32 miss_ratio = (RAYSpp - HITSpp)/(r32)RAYSpp;
            pixel_color.channel.R = (u8)( (miss_ratio * pixel_color.channel.R) + (hit_ratio * red   * 127.5) );
            pixel_color.channel.G = (u8)( (miss_ratio * pixel_color.channel.G) + (hit_ratio * green * 127.5) );
            pixel_color.channel.B = (u8)( (miss_ratio * pixel_color.channel.B) + (hit_ratio * blue  * 127.5) );

            pixel_array[pixel_array_idx] = pixel_color.value;
            pixel_array_idx++;
        }
    }

    WriteBitmap(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("[ success ]\n");
    return 0;
}
