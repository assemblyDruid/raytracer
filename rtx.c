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

_internal_ void
WritePPM(u32* const      pixel_array,
         u32             image_width,
         u32             image_height,
         const u8* const image_name)
{
    Assert(pixel_array && image_width && image_height);

    FILE* ppm_file = fopen(IMAGE_NAME, "w");
    Assert(ppm_file);

    u8* ppm_header;
    u32 success = snprintf(ppm_header, MAX_PPM_HEADER_SIZE,
                           "P3\n%d %d\n255\n",
                           image_width,
                           image_height);
    Assert(success > 0 && success < MAX_PPM_HEADER_SIZE);

    u8* ppm_image_data = calloc(MAX_PPM_RGB_TRIPPLET_SIZE * NUM_PIXELS,
                                sizeof(u8));
    Assert(ppm_image_data);

    u8 chars_last_written = 0;
    u8 rgb_tripplet[MAX_PPM_RGB_TRIPPLET_SIZE];
    for (size_t y = 0; y < image_height; y++)
    {
        for (size_t x = 0; x < image_width; x++)
        {
            chars_last_written = snprintf(rgb_tripplet,
                                          "%d%d%d",
                );
        }
    }

    fclose(ppm_file);
}


_internal_ void
WriteBitmap(u32* const      pixel_array,
            u32             image_width,
            u32             image_height,
            const u8* const image_name)

{
    Assert(pixel_array && image_width && image_height);
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
    Assert(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pixel_array,  pixel_array_size, 1, bitmap_file);

    fclose(bitmap_file);
}

_internal_ void
Log()
{
    printf("\nRaytracing...\n");
    if (__RTX_DEBUG__)
    {
        printf("RTX debug tooling ON\n");
    }
    else
    {
        printf("RTX debug tooling OFF\n");
    }
    if (__RTX_AA__)
    {
        printf("Antialiasing:     ON\n");
    }
    else
    {
        printf("Antialiasing:     OFF\n");
    }

    if (__LSB__)
    {
        printf("Compiled for:     LSB\n");
    }
    else
    {
        printf("Compiled for:     MSB\n");
    }

    printf("Tolerance:        %f\n", (r32)TOLERANCE);
    printf("Maximum ray mag:  %f\n", (r32)MAX_RAY_MAG);
    printf("Minimum ray mag:  %f\n", (r32)MIN_RAY_MAG);
    printf("Image height:     %d\n", IMAGE_HEIGHT);
    printf("Image width:      %d\n", IMAGE_WIDTH);
}


_internal_ _inline_ void
TraceSpheres(Ray*          const ray,
             RayCollision* const collision,
             r32*          const collision_mag_threshold,
             bool*         const does_intersect,
             Color32*      const return_color,
             Sphere*       const sphere_arr,
             const size_t        num_spheres)
{
    Assert(ray);
    Assert(collision);
    Assert(does_intersect);
    Assert(return_color);
    Assert(sphere_arr);
    Assert(collision_mag_threshold);
    Assert(*collision_mag_threshold >= 0);

    *does_intersect = false;
    for (size_t sphere_index = 0; sphere_index < num_spheres; sphere_index++)
    {

        if (DoesIntersectSphere(ray, &sphere_arr[sphere_index], collision)
            && (collision->magnitude < *collision_mag_threshold))
        {
            *does_intersect = true;
            *collision_mag_threshold = collision->magnitude;
            return_color->value = sphere_arr[sphere_index].material.color.value;
        }
    }
}

_internal_ _inline_ void
DetermineBackgroundColor(size_t pix_x,
                         size_t pix_y,
                         Color32* const return_color)
{
    if (pix_x) {} // [ cfarvin::TEMP ] Silence unused variable warning
    Assert(return_color);

    // Red Downward Gradient
    return_color->value = 0;
    return_color->channel.R = BindValueTo8BitColorChannel(0.0f,
                                                          (r32)IMAGE_HEIGHT,
                                                          (r32)pix_y);
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // [ cfarvin::TEMP ] Silence unused variable warning

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
    sphere_arr[0].material.color.value = 0x000000FF;

    sphere_arr[1].position.x = -0.40f;
    sphere_arr[1].position.y = -0.75f;
    sphere_arr[1].position.z = -1.75f;
    sphere_arr[1].radius     = +0.35f;
    sphere_arr[1].material.color.value = 0x0000FF00;

#if __RTX_AA__
    size_t  aa_rays_per_pixel      = 5;
    v3 aa_pixel_color_accumulator  = { 0 };
#endif // __RTX_AA_

    // Init loop defaults
    RayCollision collision               = { 0 };
    Color32      pixel_color             = { 0 };
    Color32      returned_pixel_color    = { 0 };
    size_t       pixel_array_idx         = 0;
    r32          collision_mag_threshold = (r32)MAX_RAY_MAG;
    bool         collided                = false;


    // Display startup message
    Log();

    for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++)
    {
        for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++)
        {
            collision_mag_threshold = (r32)MAX_RAY_MAG;           // Reset ray distance
            DetermineBackgroundColor(pix_x, pix_y, &pixel_color); // Reset pixel color
//
#if __RTX_AA__
//
            v3Set(&aa_pixel_color_accumulator, 0, 0, 0); // Reset AA accumulator
            for (size_t aa_ray = 0; aa_ray < aa_rays_per_pixel; aa_ray++)
            {
                ray.direction.x = (((pix_x + NormalBoundedXorShift32())/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
                ray.direction.y = ((pix_y + NormalBoundedXorShift32())/(r32)IMAGE_HEIGHT) - 0.5f;
                ray.direction.z = -1.0f;

                v3Norm(&ray.direction);
                TraceSpheres(&ray,
                             &collision,
                             &collision_mag_threshold,
                             &collided,
                             &returned_pixel_color,
                             sphere_arr,
                             num_spheres);

                if (collided)
                {
                    aa_pixel_color_accumulator.x += returned_pixel_color.channel.R;
                    aa_pixel_color_accumulator.y += returned_pixel_color.channel.G;
                    aa_pixel_color_accumulator.z += returned_pixel_color.channel.B;
                }
                else
                {
                    aa_pixel_color_accumulator.x += pixel_color.channel.R;
                    aa_pixel_color_accumulator.y += pixel_color.channel.G;
                    aa_pixel_color_accumulator.z += pixel_color.channel.B;
                }

            }

            pixel_color.channel.R = aa_pixel_color_accumulator.x / aa_rays_per_pixel;
            pixel_color.channel.G = aa_pixel_color_accumulator.y / aa_rays_per_pixel;
            pixel_color.channel.B = aa_pixel_color_accumulator.z / aa_rays_per_pixel;
//
#else // __RTX_AA__
//
            ray.direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
            ray.direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;
            ray.direction.z = -1;

            v3Norm(&ray.direction);
            TraceSpheres(&ray,
                         &collision,
                         &collision_mag_threshold,
                         &collided,
                         &returned_pixel_color,
                         sphere_arr,
                         num_spheres);

            if (collided)
            {
                pixel_color.value = returned_pixel_color.value;
            }

//
#endif // __RTX_AA__
//
            if (pix_x <= 2) // [ cfarvin::DEBUG ] Show ray progression direction x
            {
                pixel_color.channel.R = pixel_color.channel.G = 0;
                pixel_color.channel.B = ~BindValueTo8BitColorChannel(0.0f,
                                                                     (r32)IMAGE_HEIGHT,
                                                                     (r32)pix_y);
            }

            if (pix_y <= 2) // [ cfarvin::DEBUG ] Show ray progression direction y
            {
                pixel_color.channel.R = pixel_color.channel.B = 0;
                pixel_color.channel.G = ~BindValueTo8BitColorChannel(0.0f,
                                                                     (r32)IMAGE_WIDTH,
                                                                     (r32)pix_x);

            }

            if (pix_x <= 2 && pix_y <= 2) // [ cfarvin::DEBUG ] Show ray progression direction
            {
                pixel_color.channel.G = pixel_color.channel.B = 0;
                pixel_color.channel.R = 0xFF;
            }


            pixel_array[pixel_array_idx] = pixel_color.value;
            pixel_array_idx++;
        }
    }

    WriteBitmap(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rtx.bmp");
    WritePPM(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rtx.bmp");
    printf("[ success ]\n");
    return 0;
}
