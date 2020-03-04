#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <rt.h>

int
main(int argc, char** argv)
{
    if (argc || argv) {} // [ cfarvin::TEMP ] Silence unused variable warning

    // Init XorShift
    XorShift32State = 42;

    // Init pixel array
    Color32_RGB* pixel_array = (Color32_RGB*)CreatePixelArray(IMAGE_WIDTH ,IMAGE_HEIGHT, 32);

    // Init pimary ray
    Ray  ray;
    ray.origin.x    = 0.0f;
    ray.origin.y    = 0.0f;
    ray.origin.z    = 0.0f;
    ray.direction.x = 0.0f;
    ray.direction.y = 0.0f;
    ray.direction.z = -1.0f;

    // Init spheres
    Sphere* sphere_arr = CreateRandomSpheres(NUM_SPHERES);

#if __RT_AA__
    v3 aa_pixel_color_accumulator  = { 0 };
#endif // __RT_AA_

    // Init loop defaults
    RayIntersection intersection               = { 0 };
    Color32_RGB     pixel_color                = { 0 };
    Color32_RGB     returned_pixel_color       = { 0 };
    size_t          pixel_array_idx            = 0;
    r32             global_magnitude_threshold = (r32)MAX_RAY_MAG;

    // Display startup message
    Log();

    // Raytrace
    for (size_t pix_y = 0; pix_y < IMAGE_HEIGHT; pix_y++)
    {
        for (size_t pix_x = 0; pix_x < IMAGE_WIDTH; pix_x++)
        {
            global_magnitude_threshold = (r32)MAX_RAY_MAG;        // Reset closest pixel distance
            DetermineBackgroundColor(pix_x, pix_y, &pixel_color); // Reset default pixel color
//
#if __RT_AA__
//
            v3Set(&aa_pixel_color_accumulator, 0, 0, 0); // Reset AA accumulator

            for (size_t aa_ray = 0; aa_ray < __RT_AA__RPP; aa_ray++)
            {
                SetRayDirectionByPixelCoordAA(&ray, pix_x, pix_y);
                v3Norm(&ray.direction);

                TraceSphereArray(&ray,
                                 &intersection,
                                 &global_magnitude_threshold,
                                 &returned_pixel_color,
                                 sphere_arr,
                                 NUM_SPHERES);

                if (intersection.does_intersect)
                {
                    aa_pixel_color_accumulator.channel.R += returned_pixel_color.channel.R;
                    aa_pixel_color_accumulator.channel.G += returned_pixel_color.channel.G;
                    aa_pixel_color_accumulator.channel.B += returned_pixel_color.channel.B;
                }
                else
                {
                    aa_pixel_color_accumulator.channel.R += pixel_color.channel.R;
                    aa_pixel_color_accumulator.channel.G += pixel_color.channel.G;
                    aa_pixel_color_accumulator.channel.B += pixel_color.channel.B;
                }

                // Remote possibility that a channel value may overflow.
                Assert( (FLT_MAX - aa_pixel_color_accumulator.x) > 255 );
                Assert( (FLT_MAX - aa_pixel_color_accumulator.y) > 255 );
                Assert( (FLT_MAX - aa_pixel_color_accumulator.z) > 255 );

            } // end: for (size_t aa_ray = 0; aa_ray < aa_rays_per_pixel; aa_ray++)

            pixel_color.channel.R = (u8)(aa_pixel_color_accumulator.channel.R / __RT_AA__RPP);
            pixel_color.channel.G = (u8)(aa_pixel_color_accumulator.channel.G / __RT_AA__RPP);
            pixel_color.channel.B = (u8)(aa_pixel_color_accumulator.channel.B / __RT_AA__RPP);
            /* pixel_color.channel.A = 0xFF; */
//
#else // __RT_AA__ (OFF)
//
            SetRayDirectionByPixelCoord(&ray, pix_x, pix_y);
            v3Norm(&ray.direction);

            TraceSphereArray(&ray,
                             &intersection,
                             &global_magnitude_threshold,
                             &returned_pixel_color,
                             sphere_arr,
                             NUM_SPHERES);

            if (intersection.does_intersect)
            {
                pixel_color.value = returned_pixel_color.value;
            }
//
#endif // __RT_AA__
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

    // Write to file
    WriteBitmap32(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rt.bmp");
    WritePPM32(pixel_array, IMAGE_WIDTH, IMAGE_HEIGHT, "rt.ppm");

    printf("[ success ]\n");
    return 0;
} // end: main(int argc, char** argv)
