#ifndef __RT_COLOR_TOOLS_H___
#define __RT_COLOR_TOOLS_H___

#include <type_tools.h>
#include <maths_tools.h>



typedef union
{
    // Little Endian Storage
    // Access via "channel" macro
    struct
    {
        u8 A;
        u8 R;
        u8 G;
        u8 B;
    } MSB_channel;

    // Big Endian Storage
    // Access via "channel" macro
    struct
    {
        u8 B;
        u8 G;
        u8 R;
        u8 A;
    } LSB_channel;

    u32 value;
} Color32_RGB;


typedef struct
{
    r32 H; // [ 0, 360 ]
    r32 S; // [ 0, 001 ]
    r32 V; // [ 0, 001 ]
} Color32_HSV;

__RT_internal__ __RT_inline__ u8
BindValueTo8BitColorChannel(const r32 value_min,
                            const r32 value_max,
                            const r32 value)
{
    __RT_ASSERT__(value_max > value_min);
    __RT_ASSERT__((value_max >= value) && (value_min <= value));

    return (u8)NormalizeToRange(value_min,
                                value_max,
                                0.0f,
                                255.0f,
                                value);
}


__RT_internal__ __RT_inline__ void
RGB32ToHSV32(const Color32_RGB* restrict const rgb_source,
             _mut_ Color32_HSV* restrict const hsv_result)
{
    __RT_ASSERT__(rgb_source);
    __RT_ASSERT__(hsv_result);

    // Normalize [ TOLERANCE, 1.0f ]
    r32 rgb_r = NormalizeToRange(0.0f,
                                 255.0f,
                                 0.0f,
                                 1.0f,
                                 rgb_source->channel.R);

    r32 rgb_g = NormalizeToRange(0.0f,
                                 255.0f,
                                 0.0f,
                                 1.0f,
                                 rgb_source->channel.G);

    r32 rgb_b = NormalizeToRange(0.0f,
                                 255.0f,
                                 0.0f,
                                 1.0f,
                                 rgb_source->channel.B);


    r32 rgb_min = rgb_r;
    r32 rgb_max = rgb_r;
    r32 chroma  = 0.0f;

    // Determine min rgb component
    if (rgb_min > rgb_g)
    {
        rgb_min = rgb_g;
    }

    if (rgb_min > rgb_b)
    {
        rgb_min = rgb_b;
    }

    // Determine max rgb component
    if (rgb_max < rgb_g)
    {
        rgb_max = rgb_g;
    }

    if (rgb_max < rgb_b)
    {
        rgb_max = rgb_b;
    }

    // Assign chroma
    __RT_ASSERT__(rgb_max >= rgb_min);
    chroma = rgb_max - rgb_min;

    // Asign value
    r32 value = rgb_max;
    __RT_ASSERT__(value >= 0.0f);
    __RT_ASSERT__(value <= 360.0f);

    // Assign hue
    r32 hue = 0.0f;
    if (chroma == 0.0f)
    {
        // Intentional nop
    }
    else if (value == rgb_r)
    {
        hue = 60.0f * ( (rgb_g - rgb_b) / chroma );
    }
    else if (value == rgb_g)
    {
        hue = 60.0f * ( 2.0f + ((rgb_b - rgb_r) / chroma) );
    }
    else if (value == rgb_b)
    {
        hue = 60.0f * ( 4.0f + ((rgb_r - rgb_g) / chroma) );
    }
    else
    {
        __RT_ASSERT__(false);
    }

    if (hue < 0.0f)
    {
        hue += 360.0f;
    }

    __RT_ASSERT__(hue >= 0.0f);
    __RT_ASSERT__(hue <= 360.0f);

    // Assign saturation
    r32 saturation = 0.0f;
    if (value != 0.0f)
    {
        saturation = chroma/value;
    }

    hsv_result->H = hue;
    hsv_result->S = saturation;
    hsv_result->V = value;

    // [ cfarvin::DEBUG ] [ cfarvin::REMOVE ]
    __RT_ASSERT__(hsv_result->H == hue);
    __RT_ASSERT__(hsv_result->S == saturation);
    __RT_ASSERT__(hsv_result->V == value);
}


__RT_internal__ __RT_inline__ void
HSV32ToRGB32(const Color32_HSV* restrict const hsv_source,
             _mut_ Color32_RGB* restrict const rgb_result)
{
    __RT_ASSERT__(rgb_result);
    __RT_ASSERT__(hsv_source);
    __RT_ASSERT__(hsv_source->H >= 0.0f);
    __RT_ASSERT__(hsv_source->H <= 360.0f);
    __RT_ASSERT__(hsv_source->S >= 0.0f);
    __RT_ASSERT__(hsv_source->S <= 1.0f);
    __RT_ASSERT__(hsv_source->V >= 0.0f);
    __RT_ASSERT__(hsv_source->V <= 1.0f);

    // Assign hue_prime
    r32 hue_prime = hsv_source->H / 60.0f;

    // Assign chroma
    r32 chroma = hsv_source->V * hsv_source->S;
    __RT_ASSERT__(chroma >= 0.0f);
    __RT_ASSERT__(chroma <= 1.0f);

    // Assign 2nd largest color component
    r32 secondary_color = chroma *
        (1.0f - (r32)fabs(fmod(hue_prime, 2.0f) - 1.0f));
    __RT_ASSERT__(secondary_color >= 0.0f);
    __RT_ASSERT__(secondary_color <= 1.0f);

    // Compute rgb components
    r32 rgb_r = 0.0f;
    r32 rgb_g = 0.0f;
    r32 rgb_b = 0.0f;

    bool build_color_channels = true;
    r32 lightness_component = hsv_source->V - chroma;
    __RT_ASSERT__(lightness_component >= 0.0f);
    if (hue_prime < 0.0f)
    {
        build_color_channels = false;
    }

    __RT_ASSERT__(hue_prime <= 6.0f);
    if (build_color_channels)
    {
        if (hue_prime >= 0.0f && hue_prime <= 1.0f)
        {
            rgb_g = secondary_color;
            rgb_r = chroma;
        }
        if (hue_prime > 1.0f && hue_prime <= 2.0f)
        {
            rgb_r = secondary_color;
            rgb_g = chroma;
        }
        if (hue_prime > 2.0f && hue_prime <= 3.0f)
        {
            rgb_b = secondary_color;
            rgb_g = chroma;
        }
        if (hue_prime > 3.0f && hue_prime <= 4.0f)
        {
            rgb_g = secondary_color;
            rgb_b = chroma;
        }
        if (hue_prime > 4.0f && hue_prime <= 5.0f)
        {
            rgb_r = secondary_color;
            rgb_b = chroma;
        }
        if (hue_prime > 5.0f && hue_prime <= 6.0f)
        {
            rgb_b = secondary_color;
            rgb_r = chroma;
        }
    }

    rgb_r += lightness_component;
    rgb_g += lightness_component;
    rgb_b += lightness_component;

    __RT_ASSERT__(rgb_r >= 0.0f);
    __RT_ASSERT__(rgb_g >= 0.0f);
    __RT_ASSERT__(rgb_b >= 0.0f);
    __RT_ASSERT__(rgb_r <= 1.0f);
    __RT_ASSERT__(rgb_g <= 1.0f);
    __RT_ASSERT__(rgb_b <= 1.0f);

    // Assign rgb components
    rgb_result->channel.R = (u8)round(NormalizeToRange(0.0f,
                                                       1.0f,
                                                       0.0f,
                                                       255.0f,
                                                       rgb_r));

    rgb_result->channel.G = (u8)round(NormalizeToRange(0.0f,
                                                       1.0f,
                                                       0.0f,
                                                       255.0f,
                                                       rgb_g));

    rgb_result->channel.B = (u8)round(NormalizeToRange(0.0f,
                                                       1.0f,
                                                       0.0f,
                                                       255.0f,
                                                       rgb_b));
}


__RT_internal__ __RT_inline__ void
GetDominantWavelengthByColor(const Color32_HSV* restrict const hsv,
                             _mut_ r64*         restrict const wave_length_nm)
{
    __RT_ASSERT__(hsv);
    __RT_ASSERT__(wave_length_nm);

    r32 red_wavelength    = 682.5f;
    r32 violet_wavelength = 415.0f;
    r32 hue_map_max       = 270.0f;

    *wave_length_nm =  red_wavelength -
        (hue_map_max / (red_wavelength - violet_wavelength) * hsv->H);
}


__RT_internal__ __RT_inline__ void
GetEnergyByDominantWavelength_eV(const r64 wave_length_nm,
                                 _mut_ r64* restrict const energy)
{
    __RT_ASSERT__(wave_length_nm);
    __RT_ASSERT__(energy);
    r64 wave_length_meters = wave_length_nm * 0.000000001;
    r64 Joules = ((_PLANK_C__RT_ON__ST_ * _C_AIR_) / wave_length_meters);
    r64 eV = Joules * (6241509000000000000 / 0.9999999450036585);
    *energy = eV;
}

__RT_internal__ __RT_inline__ void
GetEnergyByColorRGB_eV(const Color32_RGB* restrict const rgb,
                       _mut_ r64*         restrict const energy)
{
    __RT_ASSERT__(rgb);
    __RT_ASSERT__(energy);

    Color32_HSV hsv;
    r64 wave_length_nm;
    RGB32ToHSV32(rgb, &hsv);
    GetDominantWavelengthByColor(&hsv, &wave_length_nm);
    GetEnergyByDominantWavelength_eV(wave_length_nm, energy);
}


__RT_internal__ __RT_inline__ void
GetEnergyByColorHSV(const Color32_HSV* restrict const hsv,
                    _mut_ r64*         restrict const energy)
{
    __RT_ASSERT__(hsv);
    __RT_ASSERT__(energy);

    r64 wave_length_nm = 0;
    GetDominantWavelengthByColor(hsv, &wave_length_nm);
    GetEnergyByDominantWavelength_eV(wave_length_nm, energy);
}


__RT_internal__ __RT_inline__ void
DetermineBackgroundColor(const size_t pix_x,
                         const size_t pix_y,
                         _mut_ Color32_RGB* restrict const return_color)
{
    if (pix_x) {} // [ cfarvin::TEMP ] Silence unused variable warning
    if (pix_y) {} // [ cfarvin::TEMP ] Silence unused variable warning
    __RT_ASSERT__(return_color);

    // Gradient
    return_color->value = 0;
    return_color->channel.R =
        BindValueTo8BitColorChannel(0.0f,
                                    (r32)IMAGE_HEIGHT + 2,
                                    (r32)pix_y + 1);
    return_color->channel.G =
        BindValueTo8BitColorChannel(0.0f,
                                    (r32)IMAGE_WIDTH + 2,
                                    (r32)pix_x + 1);
    return_color->channel.B =
        BindValueTo8BitColorChannel(0.0f,
                                    (r32)(pix_x + pix_y + 2),
                                    (r32)(pix_x + 1));

    // Solid Red
    /* return_color->value = 0x00FF0000; */

    // Solid Black
    /* return_color->value = 0x00000000; */
}


#endif // __RT_MATERIAL_TOOLS_H__
