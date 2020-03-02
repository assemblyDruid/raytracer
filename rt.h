#ifndef __RT_H__
#define __RT_H___

#include <math.h> // [ cfarvin::TODO ] [ cfarvin::REMOVE ]
#include <float.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>


//
// Image Properties
//
/* #define IMAGE_WIDTH  3 */
/* #define IMAGE_HEIGHT 3 */

/* #define IMAGE_WIDTH  9 */
/* #define IMAGE_HEIGHT 9 */

/* #define IMAGE_WIDTH  27 */
/* #define IMAGE_HEIGHT 27 */

#define IMAGE_WIDTH  400
#define IMAGE_HEIGHT 400

/* #define IMAGE_WIDTH  1280 */
/* #define IMAGE_HEIGHT 720 */

/* #define IMAGE_WIDTH  1920 */
/* #define IMAGE_HEIGHT 1080 */

/* #define IMAGE_WIDTH  3840 */
/* #define IMAGE_HEIGHT 2160 */

#define ASPECT_RATIO (r32)(IMAGE_WIDTH/(r32)IMAGE_HEIGHT)


//
// Macros
//
#ifndef ON
#define ON 1
#endif // _ON_

#ifndef OFF
#define OFF 0
#endif // _OFF_

#ifndef _mut_
#define _mut_ /* NOTICE: MUTABLE */
#endif // _mut_

// Modes
#define __RT_AA__    ON
#define __RT_DEBUG__ ON

// Debug
#ifdef  __RT_DEBUG__
#define __RT_inline__    /* INLINE REMOVED */
#define __RT_internal__  /* STATIC REMOVED */
#define Assert           assert
#else
#define __RT_inline__    inline
#define __RT_internal__  static
#define Assert           /* ASSERTION REMOVED */
#endif // ifdef __RT_DEBUG__

// Antialiasing Settings
#ifdef __RT_AA__
#define __RT_AA__noise 5
#endif // _RT_AA__

// Endianness
#ifndef __LSB__
#define __LSB__ 1 // 1=LSB/Little Endian, 0=MSB/Big Endian
#if __LSB__
#define channel LSB_channel
#else
#define channel MSB_channel
#endif // __LSB__ (if)
#endif // __LSB__ (ifndef)

// Min Max
#define TOLERANCE             0.00001f
#define _PI_                  3.14159f
#define _PLANK_CONST_         0.000000000000000000000000000000000662607015
#define _C_AIR_               299700000
#define MAX_RAY_MAG           5.0f
#define MIN_RAY_MAG           0.0f
#define MAX_PPM_HEADER_SIZE   25
#define MAX_PPM_TRIPPLET_SIZE 11

// Coordinate Direction Coloring (Debug)
#define RENDER_COORD_DIR     OFF
#define COORD_DIR_PIX_WEIGHT 1

// Non-debug decorators
#define __RT_call__      /* STDCALL */

#define CLAMP_MAX(value, max) if(value>max){value=max;}
#define CLAMP_MIN(value, min) if(value<min){value=min;}


//
// Types
//
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    r32;
typedef double   r64;


typedef enum
{
    false = 0,
    true = 1
} bool;


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

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif // WIN32
typedef union
{
    struct
    {
        r32 x;
        r32 y;
        r32 z;
    };

    union
    {
        // Little Endian Storage
        // Access via "channel" macro
        struct
        {
            r32 R;
            r32 G;
            r32 B;
        } MSB_channel;

        // Big Endian Storage
        // Access via "channel" macro
        struct
        {
            r32 B;
            r32 G;
            r32 R;
        } LSB_channel;
    };

    r32  arr[3];
} v3;
#ifdef _WIN32
#pragma warning( pop )
#endif // WIN32


#if _WIN32
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif // WIN32
typedef union
{
    struct
    {
        r32 x;
        r32 y;
        r32 z;
        r32 w;
    };

    union
    {
        // Little Endian Storage
        // Access via "channel" macro
        struct
        {
            r32 A;
            r32 R;
            r32 G;
            r32 B;
        } MSB_channel;

        // Big Endian Storage
        // Access via "channel" macro
        struct
        {
            r32 B;
            r32 G;
            r32 R;
            r32 A;
        } LSB_channel;
    };

    r32 arr[4];
} v4;
#if _WIN32
#pragma warning( pop )
#endif // WIN32


#if _WIN32
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif // WIN32
typedef union
{
    struct
    {
        v3 i;
        v3 j;
        v3 k;
    };

    r32 arr[9];
    r32 arr2d[3][3];
} m3;
#if _WIN32
#pragma warning( pop )
#endif // WIN32


#if _WIN32
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif // WIN32
typedef union
{
    struct
    {
        v4 i;
        v4 j;
        v4 k;
        v4 n;
    };

    r32 arr[16];
    r32 arr2d[4][4];
} m4;
#if _WIN32
#pragma warning( pop )
#endif // WIN32


typedef struct
{
    v3 origin;
    v3 direction;
} Ray;


typedef struct
{
    v3   position;
    v3   normal_vector;
    r32  magnitude;
    bool does_intersect;
} RayIntersection;


typedef struct
{
    v3 origin;
    v3 direction;
} Camera;


typedef struct
{
    const v3 LLHC;
    const v3 ULHC;
    const v3 URHC;
    const v3 LRHC;
    const r32 height;
    const r32 width;
    const r32 z_dist;
} ImagePlane;


typedef enum
{
    MATERIAL_CLASS_NONE,
    MATERIAL_CLASS_DIFFUSE,
    MATERIAL_CLASS_METAL
} MaterialClass;


typedef struct
{
    u8            max_generated_rays;
    MaterialClass material_class;
    r32           absorbtion_coefficient;
    Color32_RGB   color;
} Material;


typedef struct
{
    v3       position;
    r32      radius;
    Material material;
} Sphere;



//
// Methods
//
__RT_internal__ __RT_inline__ bool
IsWithinTolerance(const r32 value, const r32 target_value)
{
    r32 min = value - TOLERANCE;
    r32 max = value + TOLERANCE;
    return((target_value >= min) && (target_value <= max));
}


// Note: XorShift32State must be initialized to some value
//       before using XorShift32()
static u32 XorShift32State;
__RT_internal__ __RT_inline__ u32
XorShift32()
{
    u32 x = XorShift32State;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    XorShift32State = x;
    return x;
}


__RT_internal__ __RT_inline__ r32
NormalizeToRange(r32 min_source_range,
                 r32 max_source_range,
                 r32 min_target_range,
                 r32 max_target_range,
                 r32 num_to_normalize)
{
    Assert(max_source_range > min_source_range);
    Assert(max_target_range > min_target_range);
    Assert(max_source_range != min_source_range);
    Assert(max_target_range != min_target_range);
    Assert(num_to_normalize >= min_source_range &&
           num_to_normalize <= max_source_range);

    r32 ret = ((r32)(num_to_normalize - min_source_range) /
               (r32)(max_source_range - min_source_range)) *
        (max_target_range - min_target_range) + min_target_range;
    return ret;
}


// Note: XorShift32State must be initialized to some value
//       before using NormalBoundedXorShift32()
__RT_internal__ __RT_inline__ r32
NormalBoundedXorShift32()
{
    u32 max32 = ~(u32)0;
    r32 rand  = (r32)XorShift32() + (r32)TOLERANCE;
    return NormalizeToRange((r32)TOLERANCE,
                            (r32)max32,
                            (r32)TOLERANCE,
                            1.0f,
                            rand);
}


__RT_internal__ __RT_inline__ r32
NormalRayDistLerp(const r32 old_value)
{
    Assert(MAX_RAY_MAG > MIN_RAY_MAG);
    return ((old_value - MIN_RAY_MAG) * (1.0f / FLT_MAX));
}


__RT_internal__ __RT_inline__ u8
BindValueTo8BitColorChannel(const r32 value_min,
                            const r32 value_max,
                            const r32 value)
{
    Assert(value_max > value_min);
    Assert((value_max >= value) && (value_min <= value));

    return (u8)NormalizeToRange(value_min,
                                value_max,
                                0.0f,
                                255.0f,
                                value);
}


__RT_internal__ __RT_inline__ void
RGB32ToHSV32(const Color32_RGB* const rgb_source,
             _mut_ Color32_HSV* const hsv_result)
{
    Assert(rgb_source);
    Assert(hsv_result);

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
    Assert(rgb_max >= rgb_min);
    chroma = rgb_max - rgb_min;

    // Asign value
    r32 value = rgb_max;
    Assert(value >= 0.0f);
    Assert(value <= 360.0f);

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
        Assert(false);
    }

    if (hue < 0.0f)
    {
        hue += 360.0f;
    }

    Assert(hue >= 0.0f);
    Assert(hue <= 360.0f);

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
    Assert(hsv_result->H == hue);
    Assert(hsv_result->S == saturation);
    Assert(hsv_result->V == value);
}


__RT_internal__ _inline void
HSV32ToRGB32(const Color32_HSV* const hsv_source,
             _mut_ Color32_RGB* const rgb_result)
{
    Assert(rgb_result);
    Assert(hsv_source);
    Assert(hsv_source->H >= 0.0f);
    Assert(hsv_source->H <= 360.0f);
    Assert(hsv_source->S >= 0.0f);
    Assert(hsv_source->S <= 1.0f);
    Assert(hsv_source->V >= 0.0f);
    Assert(hsv_source->V <= 1.0f);

    // Assign hue_prime
    r32 hue_prime = hsv_source->H / 60.0f;

    // Assign chroma
    r32 chroma = hsv_source->V * hsv_source->S;
    Assert(chroma >= 0.0f);
    Assert(chroma <= 1.0f);

    // Assign 2nd largest color component
    r32 secondary_color = chroma *
        (1.0f - (r32)fabs(fmod(hue_prime, 2.0f) - 1.0f));
    Assert(secondary_color >= 0.0f);
    Assert(secondary_color <= 1.0f);

    // Compute rgb components
    r32 rgb_r = 0.0f;
    r32 rgb_g = 0.0f;
    r32 rgb_b = 0.0f;

    bool build_color_channels = true;
    r32 lightness_component = hsv_source->V - chroma;
    Assert(lightness_component >= 0.0f);
    if (hue_prime < 0.0f)
    {
        build_color_channels = false;
    }

    Assert(hue_prime <= 6.0f);
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

    Assert(rgb_r >= 0.0f);
    Assert(rgb_g >= 0.0f);
    Assert(rgb_b >= 0.0f);
    Assert(rgb_r <= 1.0f);
    Assert(rgb_g <= 1.0f);
    Assert(rgb_b <= 1.0f);

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
GetDominantWavelengthByColor(const Color32_HSV* const hsv,
                             _mut_ r64*         const wave_length_nm)
{
    Assert(hsv);
    Assert(wave_length_nm);

    r32 red_wavelength    = 682.5f;
    r32 violet_wavelength = 415.0f;
    r32 hue_map_max       = 270.0f;

    *wave_length_nm =  red_wavelength -
        (hue_map_max / (red_wavelength - violet_wavelength) * hsv->H);
}


__RT_internal__ __RT_inline__ void
GetEnergyByDominantWavelength_eV(const r64 wave_length_nm,
                                 _mut_ r64* const energy)
{
    Assert(wave_length_nm);
    Assert(energy);

    r64 wave_length_meters = wave_length_nm / 1000000000;
    r64 Joules = ((_PLANK_CONST_ * _C_AIR_) / wave_length_meters);
    r64 eV = ((1000000 * wave_length_meters) / 1.2398) * Joules;
    *energy = (r32)eV;
}

__RT_internal__ __RT_inline__ void
GetEnergyByColorRGB_eV(const Color32_RGB* const rgb,
                    _mut_ r64*         const energy)
{
    Assert(rgb);
    Assert(energy);

    Color32_HSV hsv;
    r64 wave_length_nm;
    RGB32ToHSV32(rgb, &hsv);
    GetDominantWavelengthByColor(&hsv, &wave_length_nm);
    GetEnergyByDominantWavelength_eV(wave_length_nm, energy);
}


__RT_internal__ __RT_inline__ void
GetEnergyByColorHSV(const Color32_HSV* const hsv,
                    _mut_ r64*         const energy)
{
    Assert(hsv);
    Assert(energy);

    r32 wave_length_nm = 0;
    GetDominantWavelengthByColor(hsv, &wave_length_nm);
    GetEnergyByDominantWavelength_eV(wave_length_nm, energy);
}


__RT_internal__ __RT_inline__ void
DegreesToRadians(const r32 degrees,
                 _mut_ r32* const result)
{
    Assert(result);
    *result = degrees * (_PI_ / 180.0f);
}


__RT_internal__ __RT_inline__ void
RadiansToDegrees(const r32 radians,
                 _mut_ r32* const result)
{
    Assert(result);
    *result = radians * (180.0f / _PI_);
}


__RT_internal__ void*
CreatePixelArray(const size_t image_width,
                 const size_t image_height,
                 const size_t bit_depth)
{
    Assert(image_width);
    Assert(image_height);
    Assert(bit_depth >= 8);
    void* ret = calloc((image_width*image_height),
                       (bit_depth * sizeof(u8)));
    Assert(ret);
    return ret;
}


__RT_internal__ Sphere*
CreateSpheres(const size_t sphere_count)
{
    return (Sphere*)calloc(sphere_count, sizeof(Sphere));
}


/* __RT_internal__ Sphere */
/* CreateSphere(v3*       const position, */
/*              r32             radius, */
/*              Material* const material, */
/*              size_t*   const sphere_count_to_update) */
/* { */
/*     Sphere sphere = { 0 }; */
/*     sphere.position = *position; */
/*     sphere.radius = radius; */
/*     sphere.material = *material; */

/*     if (sphere_count_to_update) */
/*     { */
/*         (*sphere_count_to_update)++; */
/*     } */

/*     return sphere; */
/* } */


/* __RT_internal__ Sphere */
/* CreateSphereRaw(r32 xpos, */
/*                 r32 ypos, */
/*                 r32 zpos, */
/*                 r32 radius, */
/*                 u32 color, */
/*                 size_t* const sphere_count_to_update) */
/* { */
/*     Sphere sphere = { 0 }; */
/*     sphere.position.x = xpos; */
/*     sphere.position.y = ypos; */
/*     sphere.position.z = zpos; */
/*     sphere.radius = radius; */
/*     sphere.material.color.value = color; */

/*     if (sphere_count_to_update) */
/*     { */
/*         (*sphere_count_to_update)++; */
/*     } */

/*     return sphere; */
/* } */


/* typedef struct */
/* { */
/*     v3       position; */
/*     r32      length; */
/*     Material material; */
/* } Cube; */


//
// v3
//
__RT_internal__ __RT_inline__ void
v3Set(_mut_ v3* const result,
      const r32 x,
      const r32 y,
      const r32 z)
{
    Assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
}


__RT_internal__ __RT_inline__ bool
v3IsEqual(const v3* const a, const v3* const b)
{
    Assert(a && b);
    return ( IsWithinTolerance(a->x, b->x) &&
             IsWithinTolerance(a->y, b->y) &&
             IsWithinTolerance(a->z, b->z) );
}


__RT_internal__ __RT_inline__ r32
v3Mag(const v3* const a)
{
    Assert(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 sum2 = x2 + y2 + z2;
    return (r32)sqrt(sum2);
}


__RT_internal__ __RT_inline__ bool
v3IsNorm(const v3* const a)
{
    return(IsWithinTolerance(v3Mag(a), 1.0f));
}


__RT_internal__ __RT_inline__ void
v3Norm(v3* const a)
{
    Assert(a);
    r32 magnitude = v3Mag(a);
    if (magnitude)
    {
        a->x /= magnitude;
        a->y /= magnitude;
        a->z /= magnitude;
    }
    else
    {
        /* Assert(false); */ // [ cfarvin::UNDO ]
        v3Set(a, 0.0f, 0.0f, 0.0f);
    }
}


__RT_internal__ __RT_inline__ void
v3SetAndNorm(_mut_ v3* const result,
             const r32 x,
             const r32 y,
             const r32 z)
{
    Assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    v3Norm(result);
}


__RT_internal__ __RT_inline__ void
v3Add(const v3* const a,
      const v3* const b,
      _mut_ v3* const result)
{
    Assert(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}


__RT_internal__ __RT_inline__ void
v3Sub(const v3* const a,
      const v3* const b,
      _mut_ v3* const result)
{
    Assert(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}


__RT_internal__ __RT_inline__ void
v3ScalarMul(const v3* const a,
            const r32 scalar,
            _mut_ v3* const result)
{
    Assert(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
}


__RT_internal__ __RT_inline__ r32
v3Dot(const v3* const a, const v3* const b)
{
    Assert(a && b);
    r32 result = 0;
    result += a->x * b->x;
    result += a->y * b->y;
    result += a->z * b->z;
    return result;
}


__RT_internal__ __RT_inline__ void
v3Cross(const v3* const a,
        const v3* const b,
        _mut_ v3* const result)
{
    Assert(a && b && result);
    r32 i = ((a->y * b->z) - (a->z * b->y));
    r32 j = ((a->z * b->x) - (a->x * b->z));
    r32 k = ((a->x * b->y) - (a->y * b->x));
    v3Set(result, i, j, k);
}


__RT_internal__ __RT_inline__ void
v3CrossAndNorm(const v3* const a,
               const v3* const b,
               _mut_ v3* const result)
{
    Assert(a && b && result);
    v3Cross(a, b, result);
    v3Norm(result);
}


//
// v4
//
__RT_internal__ __RT_inline__ void
v4Set(v4* const result,
      const r32 x,
      const r32 y,
      const r32 z,
      const r32 w)
{
    Assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
}


__RT_internal__ __RT_inline__ bool
v4IsEqual(const v4* const a, const v4* const b)
{
    Assert(a && b);
    return (IsWithinTolerance(a->x, b->x) &&
            IsWithinTolerance(a->y, b->y) &&
            IsWithinTolerance(a->z, b->z) &&
            IsWithinTolerance(a->w, b->w) );
}


__RT_internal__ __RT_inline__ r32
v4Mag(const v4* const a)
{
    Assert(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 w2 = a->w * a->w;
    r32 sum2 = x2 + y2 + z2 + w2;
    return (r32)sqrt(sum2);
}


__RT_internal__ __RT_inline__ bool
v4IsNorm(const v4* const a)
{
    return(IsWithinTolerance(v4Mag(a), 1.0f));
}


__RT_internal__ __RT_inline__ void
v4Norm(_mut_ v4* const a)
{
    Assert(a);
    r32 magnitude = v4Mag(a);
    if (magnitude)
    {
        a->x /= magnitude;
        a->y /= magnitude;
        a->z /= magnitude;
        a->w /= magnitude;
    }
    else
    {
        Assert(false);
        v4Set(a, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}


__RT_internal__ __RT_inline__ void
v4SetAndNorm(_mut_ v4* const result,
             const r32 x,
             const r32 y,
             const r32 z,
             const r32 w)
{
    Assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
    v4Norm(result);
}


__RT_internal__ __RT_inline__ void
v4Add(const v4* const a,
      const v4* const b,
      _mut_ v4* const result)
{
    Assert(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
    result->w = a->w + b->w;
}


__RT_internal__ __RT_inline__ void
v4Sub(const v4* const a,
      const v4* const b,
      _mut_ v4* const result)
{
    Assert(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
    result->w = a->w - b->w;
}


__RT_internal__ __RT_inline__ void
v4ScalarMul(const v4* const a,
            const r32 scalar,
            _mut_ v4* const result)
{
    Assert(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
    result->w = a->w * scalar;
}


__RT_internal__ __RT_inline__ r32
v4Dot(const v4* const a, const v4* const b)
{
    Assert(a && b);
    r32 result = 0;
    result += a->x * b->x;
    result += a->y * b->y;
    result += a->z * b->z;
    result += a->w * b->w;
    return result;
}


//
// m3
//
__RT_internal__ void
m3Set(_mut_ m3* const a, const r32 b)
{
    Assert(a);
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        a->arr[idx] = b;
    }
}


__RT_internal__ bool
m3IsEqual(const m3* const a, const m3* const b)
{
    Assert(a && b);
    return( (v3IsEqual(&a->i, &b->i)) &&
            (v3IsEqual(&a->j, &b->j)) &&
            (v3IsEqual(&a->k, &b->k)) );
}


// [ cfarvin::TODO ] Measure & improve
__RT_internal__ void
m3Ident(m3* const result)
{
    Assert(result);
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        result->arr[idx] = 0;
    }

    for (uint8_t idx = 0; idx < 3; idx++)
    {
        result->arr2d[idx][idx] = 1;
    }
}


__RT_internal__ __RT_inline__ void
m3Mult(const m3* const a,
       const m3* const b,
       _mut_ m3* const result)
{
    Assert(a && b && result);
    for (uint8_t col = 0; col < 3; col++)
    {
        for (uint8_t row = 0; row < 3; row++)
        {
            result->arr2d[row][col] =
                a->arr2d[row][0] * b->arr2d[0][col] +
                a->arr2d[row][1] * b->arr2d[1][col] +
                a->arr2d[row][2] * b->arr2d[2][col];
        }
    }
}


//
// m4
//
__RT_internal__ void
m4Set(_mut_ m4* const a, const r32 b)
{
    Assert(a);
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        a->arr[idx] = b;
    }
}


__RT_internal__ bool
m4IsEqual(const m4* const a, const m4* const b)
{
    Assert(a && b);
    return( (v4IsEqual(&a->i, &b->i)) &&
            (v4IsEqual(&a->j, &b->j)) &&
            (v4IsEqual(&a->k, &b->k)) &&
            (v4IsEqual(&a->n, &b->n)) );
}


// [ cfarvin::TODO ] Measure & improve
__RT_internal__ void
m4Ident(_mut_ m4* const result)
{
    Assert(result);
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        result->arr[idx] = 0;
    }

    for (uint8_t idx = 0; idx < 4; idx++)
    {
        result->arr2d[idx][idx] = 1;
    }
}


__RT_internal__ __RT_inline__ void
m4Mult(const m4* const a,
       const m4* const b,
       _mut_ m4* const result)
{
    Assert(a && b && result);
    for (uint8_t col = 0; col < 4; col++)
    {
        for (uint8_t row = 0; row < 4; row++)
        {
            result->arr2d[row][col] =
                a->arr2d[row][0] * b->arr2d[0][col] +
                a->arr2d[row][1] * b->arr2d[1][col] +
                a->arr2d[row][2] * b->arr2d[2][col] +
                a->arr2d[row][3] * b->arr2d[3][col];
        }
    }
}


//
// Intersection Calcultions
//
__RT_internal__ __RT_inline__ void
IntersectSphere(const Ray*             const ray,
                const Sphere*          const sphere,
                _mut_ RayIntersection* const intersection)
{
    Assert(ray && sphere && intersection);
    Assert(v3IsNorm(&ray->direction));

    // Quadratic
    r32 sphere_radius_sq = sphere->radius * sphere->radius;
    v3 ray_to_sphere = { 0 };
    v3Sub(&ray->origin, &sphere->position, &ray_to_sphere);

    // Ray magnitude must fall within boundaries
    r32 ray_dir_mag = v3Mag(&ray->direction);
    if(!(ray_dir_mag >= MIN_RAY_MAG &&
         ray_dir_mag <= MAX_RAY_MAG))
    {
        intersection->does_intersect = false;
        return;
    }

    r32 a = ray_dir_mag * ray_dir_mag;
    r32 b = 2.0f * (v3Dot(&ray->direction, &ray_to_sphere));
    r32 c = v3Dot(&ray_to_sphere, &ray_to_sphere) - sphere_radius_sq;
    /* Assert(c > 0); */

    r32 discriminant = (b * b) - (4.0f * a * c);
    if (discriminant >= 0.0f)
    {
        intersection->does_intersect = true;
    }
    else
    {
        intersection->does_intersect = false;
    }


    // Build intersection data
    if (intersection->does_intersect)
    {
        // Set intersection magnitude
        r32 magnitude = ((b * -1.0f) - (r32)sqrt(discriminant))
            / (2.0f * a);

        // [ cfarvin::TODO ] Failing this assertion suddenly.
        Assert(magnitude >= 0);

        intersection->magnitude = magnitude;

        // Set intersection position
        v3Set(&intersection->position,
              ray->origin.x + (magnitude*ray->direction.x),
              ray->origin.y + (magnitude*ray->direction.y),
              ray->origin.z + (magnitude*ray->direction.z));

        // Set intersection normal vector
        v3Sub(&sphere->position,
              &intersection->position,
              &intersection->normal_vector);

        // Correct normal vector direction if needed.
        // We seek the normal opposite the direction of the ray.
        /* if (sphere->radius > magnitude) */
        /* { */
        /*     v3ScalarMul(&intersection->normal_vector, */
        /*                 -1.0f, */
        /*                 &intersection->normal_vector); */
        /* } */
        v3Norm(&intersection->normal_vector);
    }
}


__RT_internal__ __RT_inline__ void
TraceSphere(const Ray*             const ray,
            _mut_ RayIntersection* const intersection,
            _mut_ r32*             const global_magnitude_threshold,
            _mut_ Color32_RGB*     const return_color,
            const Sphere*          const sphere)
{
    Assert(ray);
    Assert(intersection);
    Assert(return_color);
    Assert(sphere);
    Assert(global_magnitude_threshold);
    Assert(*global_magnitude_threshold >= 0);

    IntersectSphere(ray, sphere, intersection);
    if (intersection->does_intersect &&
        (intersection->magnitude < *global_magnitude_threshold))
    {
        *global_magnitude_threshold = intersection->magnitude;
        return_color->value = sphere->material.color.value;
    }
}


__RT_internal__ __RT_inline__ void
TraceSphereArray(const Ray*             const ray,
                 _mut_ RayIntersection* const intersection,
                 _mut_ r32*             const global_magnitude_threshold,
                 _mut_ Color32_RGB*     const return_color,
                 const Sphere*          const sphere_arr,
                 const size_t                 num_spheres)
{
    Assert(ray);
    Assert(intersection);
    Assert(return_color);
    Assert(sphere_arr);
    Assert(global_magnitude_threshold);
    Assert(*global_magnitude_threshold >= 0);
    Assert(num_spheres >= 1); // See: TraceSphere( ... );

    RayIntersection closestIntersection = { 0 };
    closestIntersection.magnitude = MAX_RAY_MAG;

    for (size_t sphere_index = 0;
         sphere_index < num_spheres;
         sphere_index++)
    {
        IntersectSphere(ray, &sphere_arr[sphere_index], intersection);
        if (intersection->does_intersect &&
            (intersection->magnitude < closestIntersection.magnitude))
        {
            closestIntersection.does_intersect = true;
            closestIntersection.magnitude = intersection->magnitude;
            v3Set(&closestIntersection.normal_vector,
                  intersection->normal_vector.x,
                  intersection->normal_vector.y,
                  intersection->normal_vector.z);

            return_color->value = (sphere_arr[sphere_index]).material.color.value;
        }
    }

    if (closestIntersection.does_intersect &&
        closestIntersection.magnitude < *global_magnitude_threshold)
    {

        intersection->does_intersect = true;
    }
}


//
// Image Tools
//
__RT_internal__ __RT_call__ void
WritePPM32(Color32_RGB* const pixel_array,
           u32          image_width,
           u32          image_height,
           const char*  image_name)
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

            Assert(success > 0);
            Assert(success < MAX_PPM_TRIPPLET_SIZE);

            fwrite(rgb_tripplet, success, 1, ppm_file);
            pix_idx++;
        }
    }

    Assert(pix_idx == (image_height * image_width));
    fclose(ppm_file);
}


__RT_internal__ __RT_call__ void
WriteBitmap32(Color32_RGB* const pixel_array,
              u32            image_width,
              u32            image_height,
              const char*    image_name)

{
    Assert(pixel_array && image_width && image_height);
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
    Assert(bitmap_file);

    fwrite(&bitmap_header, sizeof(BitmapHeader), 1, bitmap_file);
    fwrite(pixel_array,  pixel_array_size, 1, bitmap_file);

    fclose(bitmap_file);
}


//
// Material Tools
//
__RT_internal__ __RT_call__  void
GetDefaultMaterialByClass(_mut_ Material*     const material,
                          const MaterialClass       material_class)
{
    Assert(material);

    switch(material_class)
    {
    case MATERIAL_CLASS_DIFFUSE:
    {
        material->max_generated_rays = 10;
        material->absorbtion_coefficient = (r32)0.85;
        break;
    }
    case MATERIAL_CLASS_NONE:
    {
        material->max_generated_rays = 20;
        material->absorbtion_coefficient = (r32)0.35;
        break;
    }
    }
}


__RT_internal__ __RT_inline__ void
BlendColorByMaterial(const Material*    const material,
                     const Color32_RGB* const input_color,
                     _mut_ Color32_RGB* const return_color)
{
    Assert(material);
    Assert(material->max_generated_rays > 0);
    Assert(material->material_class != MATERIAL_CLASS_NONE);
    Assert(material->absorbtion_coefficient >= 0
           && material->absorbtion_coefficient <= 1);
    Assert(input_color);
    Assert(return_color);

    // [ cfarvin::TODO ] [ cfarvin::FINDME ]
    /* *(return_color->channel.R) = */

    return;
}


__RT_internal__ __RT_inline__ void
DetermineBackgroundColor(const size_t pix_x,
                         const size_t pix_y,
                         _mut_ Color32_RGB* const return_color)
{
    if (pix_x) {} // [ cfarvin::TEMP ] Silence unused variable warning
    if (pix_y) {} // [ cfarvin::TEMP ] Silence unused variable warning
    Assert(return_color);

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


__RT_internal__ __RT_inline__ void
SetRayDirectionByPixelCoordAA(_mut_ Ray*   const ray,
                              const size_t       pix_x,
                              const size_t       pix_y,
                              const r32          aa_noise_level)
{
    const r32 xOr_contribution_x =
        NormalizeToRange((r32)TOLERANCE,
                         (r32)(~(u32)0),
                         (r32)TOLERANCE,
                         aa_noise_level,
                         (r32)XorShift32() + (r32)TOLERANCE);

    const r32 xOr_contribution_y =
        NormalizeToRange((r32)TOLERANCE,
                         (r32)(~(u32)0),
                         (r32)TOLERANCE,
                         aa_noise_level,
                         (r32)XorShift32() + (r32)TOLERANCE);

    const r32 x_numerator = (r32)pix_x + xOr_contribution_x;
    const r32 y_numerator = (r32)pix_y + xOr_contribution_y;

    ray->direction.x =
        ((x_numerator/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (y_numerator/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


__RT_internal__ __RT_inline__ void
SetRayDirectionByPixelCoord(_mut_ Ray*   const ray,
                            const size_t       pix_x,
                            const size_t       pix_y)
{
    ray->direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


__RT_internal__ __RT_call__ Sphere*
CreateRandomSpheres(size_t num_spheres)
{
    Sphere* sphere_arr = CreateSpheres(num_spheres);
    for (size_t sphere_index = 0;
         sphere_index < num_spheres;
         sphere_index++)
    {
        // Positions
        sphere_arr[sphere_index].position.x =
            NormalizeToRange((r32)TOLERANCE,
                             (r32)(~(u32)0),
                             -1.0f,
                             +1.0f,
                             (r32)XorShift32());

        sphere_arr[sphere_index].position.y =
            NormalizeToRange((r32)TOLERANCE,
                             (r32)(~(u32)0),
                             -1.0f,
                             +1.0f,
                             (r32)XorShift32());

        sphere_arr[sphere_index].position.z =
            NormalizeToRange((r32)TOLERANCE,
                             (r32)(~(u32)0),
                             -2.0f,
                             -1.0f,
                             (r32)XorShift32());

        // Radius
        sphere_arr[sphere_index].radius =
            NormalizeToRange((r32)TOLERANCE,
                             (r32)(~(u32)0),
                             0.15f,
                             0.30f,
                             (r32)XorShift32());

        // Materials
        sphere_arr[sphere_index].material.color.channel.R =
            BindValueTo8BitColorChannel((r32)TOLERANCE,
                                        (r32)(~(u32)0),
                                        (r32)XorShift32());
        sphere_arr[sphere_index].material.color.channel.G =
            BindValueTo8BitColorChannel((r32)TOLERANCE,
                                        (r32)(~(u32)0),
                                        (r32)XorShift32());
        sphere_arr[sphere_index].material.color.channel.B =
            BindValueTo8BitColorChannel((r32)TOLERANCE,
                                        (r32)(~(u32)0),
                                        (r32)XorShift32());

#if 0
//
#if __RT_DEBUG__
//
        // Log
        printf("  Sphere [%zd]:\n", sphere_index);
        printf("    Position: (%2.2f, %2.2f, %2.2f)\n",
               sphere_arr[sphere_index].position.x,
               sphere_arr[sphere_index].position.y,
               sphere_arr[sphere_index].position.z);
        printf("    Radius: %2.2f\n", sphere_arr[sphere_index].radius);
        printf("    Material.color: (%d, %d, %d)\n:",
               sphere_arr[sphere_index].material.color.channel.R,
               sphere_arr[sphere_index].material.color.channel.G,
               sphere_arr[sphere_index].material.color.channel.B);
        fflush(stdout);
//
#endif // __RT_DEBUG__
//
#endif // 0

    }

    return sphere_arr;
}


//
// Tests
//
__RT_call__ void
TestMaths()
{
    //
    // v3 Tests
    //
    v3 v3A = { 0 };
    v3 v3B = { 0 };
    v3 v3Result = { 0 };

    // v3Add()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3Set(&v3B, 1.0f, 1.0f, 1.0f);
    v3Add(&v3A, &v3B, &v3Result);
    Assert(v3Result.x == 2 && v3Result.y == 2 && v3Result.z == 2);

    // v3Sub()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3Set(&v3B, 1.0f, 1.0f, 1.0f);
    v3Sub(&v3A, &v3B, &v3Result);
    Assert(v3Result.x == 0 && v3Result.y == 0 && v3Result.z == 0);

    // v3ScalarMul()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3ScalarMul(&v3A, 5.0f, &v3Result);
    Assert(v3Result.x == 5 && v3Result.y == 5 && v3Result.z == 5);

    // v3Mag()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    Assert(v3Mag(&v3A) == (r32)sqrt(3));
    v3Set(&v3A, 0.0f, 0.0f, 0.0f);
    Assert(v3Mag(&v3A) == 0);

    // v3Norm()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    r32 NormMagv3Result = v3Mag(&v3A);
    Assert(NormMagv3Result = (r32)sqrt(14));
    v3Set(&v3Result,
          (v3A.x / NormMagv3Result),
          (v3A.y / NormMagv3Result),
          (v3A.z / NormMagv3Result));
    v3Norm(&v3A);
    Assert(v3A.x == v3Result.x);
    Assert(v3A.y == v3Result.y);
    Assert(v3A.z == v3Result.z);

    // v3IsNorm(), v3SetAndNorm()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    Assert(v3IsNorm(&v3A));

    // v3Dot()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    v3SetAndNorm(&v3B, 4.0f, 5.0f, 6.0f);
    r32 v3ScalarA = v3Dot(&v3A, &v3B);
    r32 v3ScalarB = (v3A.x * v3B.x) +
        (v3A.y * v3B.y) +
        (v3A.z * v3B.z);
    Assert(v3ScalarA == v3ScalarB);

    // v3Cross()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    v3Set(&v3B, 1.0f, 5.0f, 7.0f);
    v3Cross(&v3A, &v3B, &v3Result);
    Assert(IsWithinTolerance(v3Result.x, -1));
    Assert(IsWithinTolerance(v3Result.y, -4));
    Assert(IsWithinTolerance(v3Result.z,  3));

    //
    // v4 Tests
    //
    v4 v4A = { 0 };
    v4 v4B = { 0 };
    v4 v4Result = { 0 };

    // v4Add()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Set(&v4B, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Add(&v4A, &v4B, &v4Result);
    Assert(v4Result.x == 2 &&
           v4Result.y == 2 &&
           v4Result.z == 2 &&
           v4Result.w == 2);

    // v4Sub()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Set(&v4B, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Sub(&v4A, &v4B, &v4Result);
    Assert(v4Result.x == 0 &&
           v4Result.y == 0 &&
           v4Result.z == 0 &&
           v4Result.w == 0);

    // v4ScalarMul()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4ScalarMul(&v4A, 5.0f, &v4Result);
    Assert(v4Result.x == 5 &&
           v4Result.y == 5 &&
           v4Result.z == 5 &&
           v4Result.w == 5);

    // v4Mag()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    Assert(v4Mag(&v4A) == (r32)sqrt(4));
    v4Set(&v4A, 0.0f, 0.0f, 0.0f, 0.0f);
    Assert(v4Mag(&v4A) == 0);

    // v4Norm()
    v4Set(&v4A, 1.0f, 2.0f, 3.0f, 4.0f);
    r32 NormMagv4Result = v4Mag(&v4A);
    Assert(IsWithinTolerance(NormMagv4Result, (r32)sqrt(30)));
    v4Set(&v4Result,
          (v4A.x / NormMagv4Result),
          (v4A.y / NormMagv4Result),
          (v4A.z / NormMagv4Result),
          (v4A.w / NormMagv4Result));
    v4Norm(&v4A);
    Assert(v4A.x == v4Result.x);
    Assert(v4A.y == v4Result.y);
    Assert(v4A.z == v4Result.z);

    // v4IsNorm(), v4SetAndNorm()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    Assert(v4IsNorm(&v4A));

    // v4Dot()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    v4SetAndNorm(&v4B, 4.0f, 5.0f, 6.0f, 4.0);
    r32 v4ScalarA = v4Dot(&v4A, &v4B);
    r32 v4ScalarB = (v4A.x * v4B.x) +
        (v4A.y * v4B.y) +
        (v4A.z * v4B.z) +
        (v4A.w * v4B.w);
    Assert(v4ScalarA == v4ScalarB);


    //
    // m3 Tests
    //
    m3 m3A = { 0 };
    m3 m3Result = { 0 };

    // m3Ident()
    m3Ident(&m3A);
    r32 sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    Assert(sum == 3);
    Assert(m3A.arr2d[0][0] == 1);
    Assert(m3A.arr2d[1][1] == 1);
    Assert(m3A.arr2d[2][2] == 1);

    // m3Set(),
    m3Set(&m3A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    Assert(sum == 0);

    m3Set(&m3A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    Assert(sum == 9);

    // union m3
    m3A.i = (v3){{1.5, 2.5, 3.5}};
    m3A.j = (v3){{4.5, 5.5, 6.5}};
    m3A.k = (v3){{7.5, 8.5, 9.5}};
    v3Add(&m3A.i, &m3A.j, &v3Result);
    Assert(v3Result.x == 6);
    Assert(v3Result.y == 8);
    Assert(v3Result.z == 10);
    Assert(m3A.arr2d[2][0] == 7.5);
    Assert(m3A.arr2d[2][1] == 8.5);
    Assert(m3A.arr2d[2][2] == 9.5);
    Assert(m3A.arr[0] == 1.5);
    Assert(m3A.arr[1] == 2.5);
    Assert(m3A.arr[2] == 3.5);

    // m3Mult()
    m3Ident(&m3A);
    m3 m3B = { 0 };
    m3Mult(&m3A, &m3B, &m3Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += 0;
    }
    Assert(sum == 0);

    m3A.i = (v3){{1, 2, 3}};
    m3A.j = (v3){{4, 5, 6}};
    m3A.k = (v3){{7, 8, 9}};
    m3B.i = (v3){{10, 20, 30}};
    m3B.j = (v3){{40, 50, 60}};
    m3B.k = (v3){{70, 80, 90}};
    m3Mult(&m3A, &m3B, &m3Result);
    v3Set(&v3Result, 300, 360, 420);
    Assert(v3IsEqual(&m3Result.i, &v3Result));
    v3Set(&v3Result, 660, 810, 960);
    Assert(v3IsEqual(&m3Result.j, &v3Result));
    v3Set(&v3Result, 1020, 1260, 1500);
    Assert(v3IsEqual(&m3Result.k, &v3Result));


    //
    // m4 Tests
    //
    m4 m4A = { 0 };
    m4 m4Result = { 0 };

    // m4Ident()
    m4Ident(&m4A);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    Assert(sum == 4);
    Assert(m4A.arr2d[0][0] == 1);
    Assert(m4A.arr2d[1][1] == 1);
    Assert(m4A.arr2d[2][2] == 1);

    // m4Set(),
    m4Set(&m4A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    Assert(sum == 0);

    m4Set(&m4A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    Assert(sum == 16);

    // union m4
    m4A.i = (v4){{1.5, 2.5, 3.5, 4.5}};
    m4A.j = (v4){{4.5, 5.5, 6.5, 7.5}};
    m4A.k = (v4){{7.5, 8.5, 9.5, 1.0}};
    v4Add(&m4A.i, &m4A.j, &v4Result);
    Assert(v4Result.x == 6);
    Assert(v4Result.y == 8);
    Assert(v4Result.z == 10);
    Assert(v4Result.w == 12);
    Assert(m4A.arr2d[2][0] == 7.5);
    Assert(m4A.arr2d[2][1] == 8.5);
    Assert(m4A.arr2d[2][2] == 9.5);
    Assert(m4A.arr2d[2][3] == 1.0);
    Assert(m4A.arr[0] == 1.5);
    Assert(m4A.arr[1] == 2.5);
    Assert(m4A.arr[2] == 3.5);
    Assert(m4A.arr[3] == 4.5);

    // m4Mult()
    m4Ident(&m4A);
    m4 m4B = { 0 };
    m4Mult(&m4A, &m4B, &m4Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += 0;
    }
    Assert(sum == 0);

    m4A.i = (v4){{1,  2,  3,    9}};
    m4A.j = (v4){{4,  5,  6,    8}};
    m4A.k = (v4){{7,  8,  9,    7}};
    m4A.n = (v4){{10, 11, 12,   6}};
    m4B.i = (v4){{10, 20, 30,  90}};
    m4B.j = (v4){{40, 50, 60,  80}};
    m4B.k = (v4){{70, 80, 90,  70}};
    m4B.n = (v4){{80, 90, 100, 60}};
    m4Mult(&m4A, &m4B, &m4Result);
    v4Set(&v4Result, 1020, 1170, 1320, 1000);
    Assert(v4IsEqual(&m4Result.i, &v4Result));
    v4Set(&v4Result, 1300, 1530, 1760, 1660);
    Assert(v4IsEqual(&m4Result.j, &v4Result));
    v4Set(&v4Result, 1580, 1890, 2200, 2320);
    Assert(v4IsEqual(&m4Result.k, &v4Result));
    v4Set(&v4Result, 1860, 2250, 2640, 2980);
    Assert(v4IsEqual(&m4Result.n, &v4Result));


    //
    // XorShift32 Tests
    //
    u32 PrevXorState = XorShift32State;
    XorShift32State = 42;
    r32 rand = 0;
    for (size_t iter = 0; iter < 10000; iter++)
    {
        rand = NormalBoundedXorShift32();
        Assert(rand >= 0);
        Assert(rand <= 1);
    }

    // Reset XorShift32State
    XorShift32State = PrevXorState;
}


__RT_internal__ __RT_call__ void
Log()
{
    printf("\nRaytracing...\n");
    if (__RT_DEBUG__)
    {
        printf("RT debug tools:       ON\n");
        printf("Math tests:           ON\n");
        TestMaths();
    }
    else
    {
        printf("RT debug tools:       OFF\n");
        printf("Math tests:           OFF\n");
    }

    if (__RT_AA__)
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
    fflush(stdout);
}

#endif // __RT_H__