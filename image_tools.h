#ifndef __RT_IMAGE_TOOLS_H___
#define __RT_IMAGE_TOOLS_H___

#include <color_tools.h>
#include <maths_tools.h>



__RT_internal__ void*
CreatePixelArray(const size_t image_width,
                 const size_t image_height,
                 const size_t bit_depth)
{
    __RT_ASSERT__(image_width);
    __RT_ASSERT__(image_height);
    __RT_ASSERT__(bit_depth >= 8);

    // [ cfarvin::REVISIT ] Aligned alloc?
    void* ret = calloc((image_width*image_height),
                       (bit_depth * sizeof(u8)));
    __RT_ASSERT__(ret);
    return ret;
}


#pragma pack( push, 1 )
typedef struct
{
    u16 magic_number;
    u32 file_size;
    u16 reserved_1;
    u16 reserved_2;
    u32 pix_arr_offset;

    u32 bitmap_header_size;
    u32 bitmap_width;
    u32 bitmap_height;
    u16 num_color_planes;
    u16 bits_per_pix;
    u32 compression_method;
    u32 pix_arr_size;
    u32 horizontal_pix_per_meter;
    u32 vertical_pix_per_meter;
    u32 num_colors_in_palete;
    u32 num_important_colors;
} BitmapHeader;
#pragma pack( pop )


__RT_internal__ __RT_call__ void
WritePPM32(Color32_RGB* const pixel_array,
           u32          image_width,
           u32          image_height,
           const char*  image_name)
{
    __RT_ASSERT__(pixel_array && image_width && image_height);

    FILE* ppm_file = fopen(image_name, "w");
    __RT_ASSERT__(ppm_file);

    char ppm_header[MAX_PPM_HEADER_SIZE];
    u32 success = snprintf(ppm_header,
                           MAX_PPM_HEADER_SIZE,
                           "P3\n%d %d\n255\n",
                           image_width,
                           image_height);
    __RT_ASSERT__((success > 0) &&
                  (success < MAX_PPM_HEADER_SIZE));
    fwrite(ppm_header, success, 1, ppm_file);

    // [ cfarvin::TODO ] [ cfarvin::DEBUG ]
    // This is as temporary as it is bad.
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

            __RT_ASSERT__(success > 0);
            __RT_ASSERT__(success < MAX_PPM_TRIPPLET_SIZE);

            fwrite(rgb_tripplet, success, 1, ppm_file);
            pix_idx++;
        }
    }

    __RT_ASSERT__(pix_idx == (image_height * image_width));
    fclose(ppm_file);
}


__RT_internal__ __RT_call__ void
WriteBitmap32(Color32_RGB* const pixel_array,
              u32            image_width,
              u32            image_height,
              const char*    image_name)

{
    __RT_ASSERT__(pixel_array && image_width && image_height);
    u32 pixel_array_size = sizeof(u32) * image_width * image_height;

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
    __RT_ASSERT__(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pixel_array,  pixel_array_size, 1, bitmap_file);

    fclose(bitmap_file);
}

#endif __RT_IMAGE_TOOLS_H___
