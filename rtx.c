#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <rtx.h>

//
// Image Properties
//

// Debug Small
/* #define IMAGE_WIDTH  3 */
/* #define IMAGE_HEIGHT 3 */

// Debug Medium
/* #define IMAGE_WIDTH  9 */
/* #define IMAGE_HEIGHT 9 */

// Debug Large
/* #define IMAGE_WIDTH  27 */
/* #define IMAGE_HEIGHT 27 */

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

_internal_ void
WritePPM32(Color32* const pixel_array,
           u32            image_width,
           u32            image_height,
           const char*    image_name)
{
    Assert(pixel_array && image_width && image_height);

    FILE* ppm_file = fopen(image_name, "w");
    Assert(ppm_file);

    char ppm_header[MAX_PPM_HEADER_SIZE];
    u32 success = snprintf(ppm_header,
                           MAX_PPM_HEADER_SIZE,
                           "P3\n%d %d\n255\n",
                           image_width,
                           image_height);
    Assert((success > 0) &&
           (success < MAX_PPM_HEADER_SIZE));
    fwrite(ppm_header, success, 1, ppm_file);

    // [ cfarvin::TODO ] [ cfarvin::DEBUG ] This is as temporary as it is bad.
    char rgb_tripplet[MAX_PPM_TRIPPLET_SIZE];
    size_t pix_idx = 0;
    for (size_t y = 0; y < image_height; y++)
    {
        for (size_t x = 0; x < image_width; x++)
        {
            for (char idx = 0; idx < MAX_PPM_TRIPPLET_SIZE; idx++)
            {
                rgb_tripplet[(size_t)idx] = '\0';
            }

            success = snprintf(rgb_tripplet,
                               MAX_PPM_TRIPPLET_SIZE,
                               "%d %d %d\n",
                               pixel_array[pix_idx].channel.R,
                               pixel_array[pix_idx].channel.G,
                               pixel_array[pix_idx].channel.B);

            Assert(success > 0);
            Assert(success < MAX_PPM_TRIPPLET_SIZE);

            fwrite(rgb_tripplet, success, 1, ppm_file);
            pix_idx++;
        }
    }

    Assert(pix_idx == (image_height * image_width));
    fclose(ppm_file);
}


_internal_ void
WriteBitmap32(Color32* const pixel_array,
              u32            image_width,
              u32            image_height,
              const char*    image_name)

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

    FILE* bitmap_file = fopen(image_name, "wb");
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
        printf("RTX debug tools:      ON\n");
        printf("Math tests:           ON\n");
        TestMaths();
    }
    else
    {
        printf("RTX debug tools:      OFF\n");
        printf("Math tests:           OFF\n");
    }

    if (__RTX_AA__)
    {
        printf("Antialiasing:         ON\n");
    }
    else
    {
        printf("Antialiasing:         OFF\n");
    }

    if (__LSB__)
    {
        printf("Compiled for:         LSB\n");
    }
    else
    {
        printf("Compiled for:         MSB\n");
    }

    if (RENDER_COORD_DIR)
    {
        printf("Coord Directions:     Rendering\n");
    }
    else
    {
        printf("Coord Directions:     Not Rendering\n");
    }

    printf("Tolerance:            %f\n", (r32)TOLERANCE);
    printf("Maximum ray mag:      %f\n", (r32)MAX_RAY_MAG);
    printf("Minimum ray mag:      %f\n", (r32)MIN_RAY_MAG);
    printf("Image height:         %d\n", IMAGE_HEIGHT);
    printf("Image width:          %d\n", IMAGE_WIDTH);
}


_internal_ _inline_ void
TraceSphere(const Ray*             const ray,
            _mut_ RayIntersection* const intersection,
            const r32*             const global_magnitude_threshold,
            _mut_ Color32*         const return_color,
            const Sphere*          const sphere)
{
    Assert(ray);
    Assert(intersection);
    Assert(return_color);
    Assert(sphere);
    Assert(global_magnitude_threshold);
    Assert(*global_magnitude_threshold >= 0);

    r32 local_magnitude_threshold = *global_magnitude_threshold;
    intersection->does_intersect = false;

    IntersectSpheres(ray, sphere, intersection);

    if (intersection->does_intersect &&
        (intersection->magnitude <= local_magnitude_threshold))
    {
        intersection->does_intersect = true;
        local_magnitude_threshold = intersection->magnitude;
        return_color->value = sphere->material.color.value;
    }
}


_internal_ _inline_ void
TraceSphereArray(const Ray*             const ray,
                 _mut_ RayIntersection* const intersection,
                 const r32*             const global_magnitude_threshold,
                 _mut_ Color32*         const return_color,
                 const Sphere*          const sphere_arr,
                 const size_t                 num_spheres)
{
    Assert(ray);
    Assert(intersection);
    Assert(return_color);
    Assert(sphere_arr);
    Assert(global_magnitude_threshold);
    Assert(*global_magnitude_threshold >= 0);
    Assert(num_spheres >= 1); // Use TraceSphere( ... );

    for (size_t sphere_index = 0; sphere_index <= num_spheres; sphere_index++)
    {
        TraceSphere(ray,
                    intersection,
                    global_magnitude_threshold,
                    return_color,
                    &sphere_arr[sphere_index]);
    }
}

_internal_ _inline_ void
DetermineBackgroundColor(size_t pix_x,
                         size_t pix_y,
                         Color32* const return_color)
{
    if (pix_x) {} // [ cfarvin::TEMP ] Silence unused variable warning
    if (pix_y) {} // [ cfarvin::TEMP ] Silence unused variable warning
    Assert(return_color);

    // Red Gradient
    return_color->value = 0;
    return_color->channel.R = BindValueTo8BitColorChannel(0.0f,
                                                          (r32)IMAGE_HEIGHT,
                                                          (r32)pix_y);

    // Solid Red
    /* return_color->value = 0x00FF0000; */
}


_internal_ _inline_ void
SetRayDirectionByPixelCoordAA(_mut_ Ray* const ray,
                              const size_t pix_x,
                              const size_t pix_y)
{
    const r32 xOr_contribution_x = NormalBoundedXorShift32();
    const r32 xOr_contribution_y = NormalBoundedXorShift32();
    const r32 x_numerator = (r32)pix_x + xOr_contribution_x;
    const r32 y_numerator = (r32)pix_y + xOr_contribution_y;

    ray->direction.x = ((x_numerator/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (y_numerator/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


_internal_ _inline_ void
SetRayDirectionByPixelCoord(_mut_ Ray* const ray,
                            const size_t pix_x,
                            const size_t pix_y)
{
    ray->direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


int
main(int argc, char** argv)
{
    if (argc || argv) {} // [ cfarvin::TEMP ] Silence unused variable warning

    // Init XorShift
    XorShift32State = 42;

    // Init pixel array
    Color32* pixel_array = (Color32*)CreatePixelArray(IMAGE_WIDTH ,IMAGE_HEIGHT, 32);

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
    sphere_arr[0].position.x = +0.40f;
    sphere_arr[0].position.y = +0.75f;
    sphere_arr[0].position.z = -1.10f;
    sphere_arr[0].radius     = +0.25f;
    sphere_arr[0].material.color.value = 0x000000FF;

    sphere_arr[1].position.x = -0.40f;
    sphere_arr[1].position.y = -0.75f;
    sphere_arr[1].position.z = -1.10f;
    sphere_arr[1].radius     = +0.25f;
    sphere_arr[1].material.color.value = 0x0000FF00;

#if __RTX_AA__
    size_t  aa_rays_per_pixel      = 5;
    v3 aa_pixel_color_accumulator  = { 0 };
#endif // __RTX_AA_

    // Init loop defaults
    RayIntersection intersection            = { 0 };
    Color32         pixel_color             = { 0 };
    Color32         returned_pixel_color    = { 0 };
    size_t          pixel_array_idx         = 0;
    r32             global_magnitude_threshold = (r32)MAX_RAY_MAG;

    // Display startup message
    Log();

    // Raytrace
    for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++)
    {
        for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++)
        {
            Assert(ray.origin.x == 0);
            Assert(ray.origin.y == 0);
            Assert(ray.origin.z == 0);

            global_magnitude_threshold = (r32)MAX_RAY_MAG;        // Reset intersection threshold
            DetermineBackgroundColor(pix_x, pix_y, &pixel_color); // Reset pixel color
//
#if __RTX_AA__
//
            v3Set(&aa_pixel_color_accumulator, 0, 0, 0); // Reset AA accumulator

            for (size_t aa_ray = 0; aa_ray < aa_rays_per_pixel; aa_ray++)
            {
                for (size_t sphere_index = 0; sphere_index < num_spheres; sphere_index++)
                {
                    SetRayDirectionByPixelCoordAA(&ray, pix_x, pix_y);
                    v3Norm(&ray.direction);

                    TraceSphere(&ray,
                                &intersection,
                                &global_magnitude_threshold,
                                &returned_pixel_color,
                                &sphere_arr[sphere_index]);

                    if (intersection.does_intersect)
                    {
                        global_magnitude_threshold = intersection.magnitude;

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
            } // end: for (size_t aa_ray = 0; aa_ray < aa_rays_per_pixel; aa_ray++)

            pixel_color.channel.R = (u8)(aa_pixel_color_accumulator.x / aa_rays_per_pixel);
            pixel_color.channel.G = (u8)(aa_pixel_color_accumulator.y / aa_rays_per_pixel);
            pixel_color.channel.B = (u8)(aa_pixel_color_accumulator.z / aa_rays_per_pixel);
//
#else // __RTX_AA__ (OFF)
//
            SetRayDirectionByPixelCoord(&ray, pix_x, pix_y);
            v3Norm(&ray.direction);

            TraceSphereArray(&ray,
                             &intersection,
                             &global_magnitude_threshold,
                             &returned_pixel_color,
                             &sphere_arr,
                             num_spheres);

            if (intersection.does_intersect)
            {
                global_magnitude_threshold = intersection.magnitude;
                pixel_color.value = returned_pixel_color.value;
            }
//
#endif // __RTX_AA__
//

//
#if RENDER_COORD_DIR
//
            // [ cfarvin::DEBUG ] Show ray progression direction x
            if (pix_x < COORD_DIR_PIX_WEIGHT)
            {
                pixel_color.channel.R = pixel_color.channel.G = 0;
                pixel_color.channel.B = ~BindValueTo8BitColorChannel(0.0f,
                                                                     (r32)IMAGE_HEIGHT,
                                                                     (r32)pix_y);
            }

            // [ cfarvin::DEBUG ] Show ray progression direction y
            if (pix_y < COORD_DIR_PIX_WEIGHT)
            {
                pixel_color.channel.R = pixel_color.channel.B = 0;
                pixel_color.channel.G = ~BindValueTo8BitColorChannel(0.0f,
                                                                     (r32)IMAGE_WIDTH,
                                                                     (r32)pix_x);
            }

            // [ cfarvin::DEBUG ] Show ray progression origin
            if (pix_x < COORD_DIR_PIX_WEIGHT && pix_y < COORD_DIR_PIX_WEIGHT)
            {
                pixel_color.channel.G = pixel_color.channel.B = 0;
                pixel_color.channel.R = 0xFF;
            }
//
#endif // RENDER_COORD_DIR
//

            pixel_array[pixel_array_idx].value = pixel_color.value;
            pixel_array_idx++;
        } // end: for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++)
    } // end: for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++)

    WriteBitmap32(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rtx.bmp");
    WritePPM32(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rtx.ppm");
    printf("[ success ]\n");
    return 0;
} // end: main(int argc, char** argv)
