#ifndef __RTX_H__
#define __RTX_H___

#define internal static
#define TOLERANCE 0.00001f

// [ cfarvin::TODO ] [ cfarvin::REMOVE ]
#include <math.h>

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


//
// Types
//
typedef enum
{
    false = 0,
    true = 1
} bool;

#pragma pack(push, 1)
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
#pragma pack(pop)


typedef struct
{
    r32 x;
    r32 y;
    r32 z;
} v3;


/* typedef struct */
/* { */
/*     v3 position; */
/* } Camera; */


typedef struct
{
    v3 origin;
    v3 direction;
} Ray;


typedef struct
{
    u32 color;
} Material;


typedef struct
{
    v3       position;
    r32      radius;
    Material material;
} Sphere;


typedef struct
{
    v3       position;
    r32      length;
    Material material;
} Cube;


//
// Maths
//
internal inline bool
IsWithinTolerance(r32 value, r32 target_value)
{
    r32 min = value - TOLERANCE;
    r32 max = value + TOLERANCE;
    if ((target_value >= min) && (target_value <= max))
    {
        return true;
    }

    return false;
}


internal inline r32
v3Mag(v3* const a)
{
    assert(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 sum2 = x2 + y2 + z2;
    return (r32)sqrt(sum2);
}


internal inline bool
v3IsNorm(v3* const a)
{
    if (IsWithinTolerance(v3Mag(a), 1.0f))
    {
        return true;
    }
    return false;
}


internal inline void
v3Set(v3* const result, r32 x, r32 y, r32 z)
{
    assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
}


internal inline void
v3Norm(v3* const a)
{
    assert(a);
    r32 magnitude = v3Mag(a);
    if (magnitude)
    {
        a->x /= magnitude;
        a->y /= magnitude;
        a->z /= magnitude;
    }
    else
    {
        assert(false);
        v3Set(a, 0.0f, 0.0f, 0.0f);
    }
}


internal inline void
v3SetAndNorm(v3* const result, r32 x, r32 y, r32 z)
{
    assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    v3Norm(result);
}


internal inline void
v3Copy(v3* const src, v3* const dest)
{
    assert(src && dest);
    v3Set(dest, src->x, src->y, src->z);
}


internal inline void
v3Add(v3* const a, v3* const b, v3* const result)
{
    assert(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}


internal inline void
v3Sub(v3* const a, v3* const b, v3* const result)
{
    assert(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}


internal inline void
v3ScalarMul(v3* const a, r32 scalar, v3* const result)
{
    assert(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
}


internal inline r32
v3Dot(v3* const a, v3* const b)
{
    assert(a && b);
    assert(v3IsNorm(a) && v3IsNorm(b));
    r32 result = 0;
    result += a->x * b->x;
    result += a->y * b->y;
    result += a->z * b->z;
    return result;
}


internal inline void
v3Cross(v3* const a, v3* const b, v3* const result)
{
    assert(a && b && result);
    r32 i = ((a->y * b->z) - (a->z * b->y));
    r32 j = ((a->z * b->x) - (a->x * b->z));
    r32 k = ((a->x * b->y) - (a->y * b->x));
    v3Set(result, i, j, k);
}


internal inline void
v3CrossAndNorm(v3* const a, v3* const b, v3* const result)
{
    assert(a && b && result);
    v3Cross(a, b, result);
    v3Norm(result);
}


//
// Intersection Calcultions
//
internal inline bool
DoesIntersectSphere(Ray* const ray,
                    Sphere* const sphere,
                    r32* const _t_)
{
    assert(ray && sphere);
    assert(v3IsNorm(&ray->direction));
    bool ret = false;

    // Quadratic
    v3 ray_to_sphere = {0};
    v3Sub(&ray->origin, &sphere->position, &ray_to_sphere);
    v3Norm(&ray_to_sphere);

    r32 a = v3Dot(&ray->direction, &ray->direction);
    r32 b = 2.0f * (v3Dot(&ray->direction, &ray_to_sphere));
    r32 c = v3Dot(&ray_to_sphere, &ray_to_sphere) -
        (sphere->radius * sphere->radius);
    r32 discriminant = (b * b) - (4.0f * a * c);

    if (discriminant >= 0)
    {
        ret = true;
    }

    if(_t_)
    {
        r32 root_a = ((b * -1.0f) - (r32)sqrt(discriminant)) / (2.0f * a);
        r32 root_b = ((b * -1.0f) + (r32)sqrt(discriminant)) / (2.0f * a);
        r32 min_root = (root_a < root_b) ? root_a : root_b;
        *_t_ = min_root;
    }

    return ret;
}


//
// Tests
//
internal void
BasicMathsTest()
{
    v3 A = {0};
    v3 B = {0};
    v3 Result = {0};

    // v3Add()
    v3Set(&A, 1.0f, 1.0f, 1.0f);
    v3Set(&B, 1.0f, 1.0f, 1.0f);
    v3Add(&A, &B, &Result);
    assert(Result.x == 2 && Result.y == 2 && Result.z == 2);

    // v3Sub()
    v3Set(&A, 1.0f, 1.0f, 1.0f);
    v3Set(&B, 1.0f, 1.0f, 1.0f);
    v3Sub(&A, &B, &Result);
    assert(Result.x == 0 && Result.y == 0 && Result.z == 0);

    // v3ScalarMul()
    v3Set(&A, 1.0f, 1.0f, 1.0f);
    v3ScalarMul(&A, 5.0f, &Result);
    assert(Result.x == 5 && Result.y == 5 && Result.z == 5);

    // v3Mag()
    v3Set(&A, 1.0f, 1.0f, 1.0f);
    assert(v3Mag(&A) == (r32)sqrt(3));
    v3Set(&A, 0.0f, 0.0f, 0.0f);
    assert(v3Mag(&A) == 0);

    // v3Norm()
    v3Set(&A, 1.0f, 2.0f, 3.0f);
    r32 NormMagResult = v3Mag(&A);
    assert(NormMagResult = (r32)sqrt(14));
    v3Set(&Result,
          (A.x / NormMagResult),
          (A.y / NormMagResult),
          (A.z / NormMagResult));
    v3Norm(&A);
    assert(A.x == Result.x);
    assert(A.y == Result.y);
    assert(A.z == Result.z);

    // v3IsNorm(), v3SetAndNorm()
    v3SetAndNorm(&A, 1.0f, 2.0f, 3.0f);
    assert(v3IsNorm(&A));

    // v3Dot()
    v3SetAndNorm(&A, 1.0f, 2.0f, 3.0f);
    v3SetAndNorm(&B, 4.0f, 5.0f, 6.0f);
    r32 result = v3Dot(&A, &B);
    r32 manual = (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
    assert(result == manual);

    // v3Cross()
    v3Set(&A, 1.0f, 2.0f, 3.0f);
    v3Set(&B, 1.0f, 5.0f, 7.0f);
    v3Cross(&A, &B, &Result);
    assert(IsWithinTolerance(Result.x, -1));
    assert(IsWithinTolerance(Result.y, -4));
    assert(IsWithinTolerance(Result.z,  3));
}

#endif // __RTX_H__
