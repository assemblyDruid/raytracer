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
/* #define IMAGE_WIDTH  400 */
/* #define IMAGE_HEIGHT 400 */

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
#define IMAGE_NAME    "rtx.bmp"


_internal_ void
WriteBitmap(u32* pixel_array, u32 image_width, u32 image_height)
{
    assert(pixel_array && image_width && image_height);
    u32 pixel_array_size = sizeof(u32) * NUM_PIXELS;

    BitmapHeader bitmap_header = { 0 };
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

_internal_ void
Log()
{
    printf("\nRaytracing...\n");
    if (__RTX_DEBUG__)
    {
        printf("RTX debug tooling is: ON\n");
    }
    else
    {
        printf("RTX debug tooling is: OFF\n");
    }
    if (__RTX_AA__)
    {
        printf("Antialiasing is:      ON\n");
    }
    else
    {
        printf("Antialiasing is:      OFF\n");
    }

    if (__LSB__)
    {
        printf("Compiled for:         LSB\n");
    }
    else
    {
        printf("Compiled for:         MSB\n");
    }

    printf("Tolerance set to:     %f\n", (r32)TOLERANCE);
    printf("Maximum ray mag:      %f\n", (r32)MAX_RAY_MAG);
    printf("Minimum ray mag:      %f\n", (r32)MIN_RAY_MAG);
    printf("Image height:         %d\n", IMAGE_HEIGHT);
    printf("Image width:          %d\n", IMAGE_WIDTH);
}


// [ cfarvin::TODO ] Validation checks on this function
_internal_ _inline_ bool
TraceSpheres(Ray*          const ray,
             RayCollision* const collision,
             Sphere*       const sphere_arr,
             const size_t        num_spheres,
             Color32*      const pixel_color,
             r32*          const collision_mag_threshold)
{
    bool does_intersect = false;
    for (size_t sphere_index = 0; sphere_index < num_spheres; sphere_index++)
    {

        if (DoesIntersectSphere(ray, &sphere_arr[sphere_index], collision)
            && (collision->magnitude < *collision_mag_threshold))
        {
            does_intersect = true;
            *collision_mag_threshold = collision->magnitude;
            pixel_color->channel.R = BindValueTo8BitColorChannel(MIN_RAY_MAG,
                                                                 MAX_RAY_MAG,
                                                                 collision->magnitude);
        }
    }

    return does_intersect;
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // Silence unused variable warning

    _run_tests_;

    // Init XorShift
    XorShift32State = 42;

    // Init pixel array
    u32* pixel_array = (u32*)CreatePixelArray(IMAGE_WIDTH ,IMAGE_HEIGHT, 32);

    // Init pimary ray
    Ray  ray;
    ray.origin.x    = 0.0f;
    ray.origin.y    = 0.0f;
    ray.origin.z    = 0.0f;
    ray.direction.x = 0.0f;
    ray.direction.y = 0.0f;
    ray.direction.z = -1.0f;

    // Init spheres
    size_t num_spheres = 2;
    Sphere* sphere_arr = CreateSpheres(num_spheres);
    sphere_arr[0].position.x = -0.00f;
    sphere_arr[0].position.y = -0.00f;
    sphere_arr[0].position.z = -1.00f;
    sphere_arr[0].radius     = +0.35f;

    sphere_arr[1].position.x = -0.40f;
    sphere_arr[1].position.y = -0.75f;
    sphere_arr[1].position.z = -1.75f;
    sphere_arr[1].radius     = +0.35f;

#if __RTX_AA__
    size_t  aa_rays_per_pixel = 100;
    Color32 aa_pixel_color    = { 0 };
#endif // __RTX_AA_

    // Init loop defaults
    RayCollision collision               = { 0 };
    Color32      pixel_color             = { 0 };
    size_t       pixel_array_idx         = 0;
    r32          collision_mag_threshold = (r32)MAX_RAY_MAG;
    bool         collided                = false;


    // Display startup message
    Log();

    for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++)
    {
        for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++)
        {
            pixel_color.value  = 0xFF202020;            // Reset pixel color
            collision_mag_threshold = (r32)MAX_RAY_MAG; // Reset ray distance

            ray.direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
            ray.direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;

            v3Norm(&ray.direction);
            collided = TraceSpheres(&ray,
                                    &collision,
                                    sphere_arr,
                                    num_spheres,
                                    &pixel_color,
                                    &collision_mag_threshold);

//
#if __RTX_AA__
//
            if (collided)
            {
                bool aa_collided = false;
                /* r32 aa_pixel_color_accumulator = 0; */
                r32 aa_pixel_color_accumulator = pixel_color.channel.R;
                for (size_t aa_ray = 0; aa_ray < aa_rays_per_pixel; aa_ray++)
                {

                    ray.direction.x += NormalBoundedXorShift32();
                    ray.direction.y += NormalBoundedXorShift32();

                    v3Norm(&ray.direction);
                    aa_collided = TraceSpheres(&ray,
                                               &collision,
                                               sphere_arr,
                                               num_spheres,
                                               &aa_pixel_color,
                                               &collision_mag_threshold);

                    if (aa_collided && (collision.magnitude > collision_mag_threshold))
                    {
                        collision_mag_threshold = collision.magnitude;
                        aa_pixel_color_accumulator += BindValueTo8BitColorChannel(MIN_RAY_MAG,
                                                                                  MAX_RAY_MAG,
                                                                                  collision.magnitude);
                    }
                    else
                    {
                        aa_pixel_color_accumulator += pixel_color.channel.R;
                    }
                }

                pixel_color.channel.R = (u8)(aa_pixel_color_accumulator/(r32)aa_rays_per_pixel);
            }
//
#else // __RTX_AA__
//
            if (collided && (collision.magnitude < collision_mag_threshold))
            {
                collision_mag_threshold = collision.magnitude;
                pixel_color.channel.R = BindValueTo8BitColorChannel(MIN_RAY_MAG,
                                                                    MAX_RAY_MAG,
                                                                    collision.magnitude);
            }
//
#endif // __RTX_AA__
//
            pixel_array[pixel_array_idx] = pixel_color.value;
            pixel_array_idx++;
        }
    }

    WriteBitmap(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("[ success ]\n");
    return 0;
}
