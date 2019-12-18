#ifndef __RTX_H__
#define __RTX_H___

#define internal static
#define TOLERANCE 0.00001f

// Rays tavel in
#define MAX_RAY_DIST 5.0f
#define MIN_RAY_DIST 0.0f


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


typedef union
{
    // Little Endian Storage
    struct
    {
        u8 A;
        u8 R;
        u8 G;
        u8 B;
    } MSB_channel;

    // Big Endian Storage
    struct
    {
        u8 B;
        u8 G;
        u8 R;
        u8 A;
    } LSB_channel;

    u32 value;
} Color;


typedef struct
{
    r32 x;
    r32 y;
    r32 z;
} v3;


typedef struct
{
    r32 x;
    r32 y;
    r32 z;
    r32 w;
} v4;


typedef union
{
    struct
    {
        v3 i;
        v3 j;
        v3 k;
    } row;

    r32 arr[9];
    r32 arr2d[3][3];
} m3;


typedef union
{
    struct
    {
        v4 i;
        v4 j;
        v4 k;
        v4 n;
    } row;

    r32 arr[16];
    r32 arr2d[4][4];
} m4;


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
    const v3 LLHC;
    const v3 ULHC;
    const v3 URHC;
    const v3 LRHC;
    const r32 height;
    const r32 width;
    const r32 z_dist;
} ImagePlane;


typedef struct
{
    // [ cfarvin::TODO ] add scattering attributes
    Color color;
} Material;


typedef struct
{
    v3       position;
    r32      radius;
    Material material;
} Sphere;


internal Sphere
CreateSphere(v3* const position,
             r32 radius,
             Material* const material,
             size_t* const sphere_count_to_update)
{
    Sphere sphere = {0};
    sphere.position = *position;
    sphere.radius = radius;
    sphere.material = *material;

    if (sphere_count_to_update)
    {
        (*sphere_count_to_update)++;
    }

    return sphere;
}


internal Sphere
CreateSphereRaw(r32 xpos,
                r32 ypos,
                r32 zpos,
                r32 radius,
                u32 color,
                size_t* const sphere_count_to_update)
{
    Sphere sphere = {0};
    sphere.position.x = xpos;
    sphere.position.y = ypos;
    sphere.position.z = zpos;
    sphere.radius = radius;
    sphere.material.color.value = color;

    if (sphere_count_to_update)
    {
        (*sphere_count_to_update)++;
    }

    return sphere;
}



/* typedef struct */
/* { */
/*     v3       position; */
/*     r32      length; */
/*     Material material; */
/* } Cube; */


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


//
// v3
//
internal inline void
v3Set(v3* const result, r32 x, r32 y, r32 z)
{
    assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
}


internal inline bool
v3IsEqual(v3* const a, v3* const b)
{
    assert(a && b);
    return ( IsWithinTolerance(a->x, b->x) &&
             IsWithinTolerance(a->y, b->y) &&
             IsWithinTolerance(a->z, b->z) );
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
// v4
//
internal inline void
v4Set(v4* const result, r32 x, r32 y, r32 z, r32 w)
{
    assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
}


internal inline bool
v4IsEqual(v4* const a, v4* const b)
{
    assert(a && b);
    return (IsWithinTolerance(a->x, b->x) &&
            IsWithinTolerance(a->y, b->y) &&
            IsWithinTolerance(a->z, b->z) &&
            IsWithinTolerance(a->w, b->w) );
}


internal inline r32
v4Mag(v4* const a)
{
    assert(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 w2 = a->w * a->w;
    r32 sum2 = x2 + y2 + z2 + w2;
    return (r32)sqrt(sum2);
}


internal inline bool
v4IsNorm(v4* const a)
{
    if (IsWithinTolerance(v4Mag(a), 1.0f))
    {
        return true;
    }
    return false;
}


internal inline void
v4Norm(v4* const a)
{
    assert(a);
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
        assert(false);
        v4Set(a, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}


internal inline void
v4SetAndNorm(v4* const result, r32 x, r32 y, r32 z, r32 w)
{
    assert(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
    v4Norm(result);
}


internal inline void
v4Add(v4* const a, v4* const b, v4* const result)
{
    assert(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
    result->w = a->w + b->w;
}


internal inline void
v4Sub(v4* const a, v4* const b, v4* const result)
{
    assert(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
    result->w = a->w - b->w;
}


internal inline void
v4ScalarMul(v4* const a, r32 scalar, v4* const result)
{
    assert(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
    result->w = a->w * scalar;
}


internal inline r32
v4Dot(v4* const a, v4* const b)
{
    assert(a && b);
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
internal void
m3Set(m3* const a, r32 b)
{
    assert(a);
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        a->arr[idx] = b;
    }
}


internal bool
m3IsEqual(m3* const a, m3* const b)
{
    assert(a && b);
    return( (v3IsEqual(&a->row.i, &b->row.i)) &&
            (v3IsEqual(&a->row.j, &b->row.j)) &&
            (v3IsEqual(&a->row.k, &b->row.k)) );
}


// [ cfarvin::TODO ] Measure & improve
internal void
m3Ident(m3* const result)
{
    assert(result);
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        result->arr[idx] = 0;
    }

    for (uint8_t idx = 0; idx < 3; idx++)
    {
        result->arr2d[idx][idx] = 1;
    }
}


internal inline void
m3Mult(m3* const a, m3* const b, m3* const result)
{
    assert(a && b && result);
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
internal void
m4Set(m4* const a, r32 b)
{
    assert(a);
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        a->arr[idx] = b;
    }
}


internal bool
m4IsEqual(m4* const a, m4* const b)
{
    assert(a && b);
    return( (v4IsEqual(&a->row.i, &b->row.i)) &&
            (v4IsEqual(&a->row.j, &b->row.j)) &&
            (v4IsEqual(&a->row.k, &b->row.k)) &&
            (v4IsEqual(&a->row.n, &b->row.n)) );
}


// [ cfarvin::TODO ] Measure & improve
internal void
m4Ident(m4* const result)
{
    assert(result);
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        result->arr[idx] = 0;
    }

    for (uint8_t idx = 0; idx < 4; idx++)
    {
        result->arr2d[idx][idx] = 1;
    }
}


internal inline void
m4Mult(m4* const a, m4* const b, m4* const result)
{
    assert(a && b && result);
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
internal inline bool
DoesIntersectSphere(Ray* const ray,
                    Sphere* const sphere,
                    r32* const distance)
{
    assert(ray && sphere);
    bool ret = false;

    // Quadratic
    r32 sphere_radius_sq = sphere->radius * sphere->radius;
    v3 ray_to_sphere = {0};
    v3Sub(&ray->origin, &sphere->position, &ray_to_sphere);
    assert(v3Mag(&ray_to_sphere) >= sphere_radius_sq);

    r32 ray_dir_mag = v3Mag(&ray->direction);
    r32 a = ray_dir_mag * ray_dir_mag;
    r32 b = 2.0f * (v3Dot(&ray->direction, &ray_to_sphere));
    r32 c = v3Dot(&ray_to_sphere, &ray_to_sphere) - sphere_radius_sq;
    r32 discriminant = (b * b) - (4.0f * a * c);
    assert(c > 0);

    if (discriminant >= 0)
    {
        ret = true;
    }

    if(distance && ret)
    {
        r32 dist_a = ((b * -1.0f) + (r32)sqrt(discriminant)) / (2.0f * a);
        r32 dist_b = ((b * -1.0f) - (r32)sqrt(discriminant)) / (2.0f * a);
        r32 dist = (dist_a <= dist_b) ? dist_a : dist_b;
        assert(dist >= 0);
        /* if (dist < 0) { dist *= -1; } */
        *distance = dist;
    }

    return ret;
}


//
// Tests
//
internal void
BasicMathsTest()
{
    //
    // v3 Tests
    //
    v3 v3A = {0};
    v3 v3B = {0};
    v3 v3Result = {0};

    // v3Add()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3Set(&v3B, 1.0f, 1.0f, 1.0f);
    v3Add(&v3A, &v3B, &v3Result);
    assert(v3Result.x == 2 && v3Result.y == 2 && v3Result.z == 2);

    // v3Sub()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3Set(&v3B, 1.0f, 1.0f, 1.0f);
    v3Sub(&v3A, &v3B, &v3Result);
    assert(v3Result.x == 0 && v3Result.y == 0 && v3Result.z == 0);

    // v3ScalarMul()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3ScalarMul(&v3A, 5.0f, &v3Result);
    assert(v3Result.x == 5 && v3Result.y == 5 && v3Result.z == 5);

    // v3Mag()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    assert(v3Mag(&v3A) == (r32)sqrt(3));
    v3Set(&v3A, 0.0f, 0.0f, 0.0f);
    assert(v3Mag(&v3A) == 0);

    // v3Norm()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    r32 NormMagv3Result = v3Mag(&v3A);
    assert(NormMagv3Result = (r32)sqrt(14));
    v3Set(&v3Result,
          (v3A.x / NormMagv3Result),
          (v3A.y / NormMagv3Result),
          (v3A.z / NormMagv3Result));
    v3Norm(&v3A);
    assert(v3A.x == v3Result.x);
    assert(v3A.y == v3Result.y);
    assert(v3A.z == v3Result.z);

    // v3IsNorm(), v3SetAndNorm()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    assert(v3IsNorm(&v3A));

    // v3Dot()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    v3SetAndNorm(&v3B, 4.0f, 5.0f, 6.0f);
    r32 v3ScalarA = v3Dot(&v3A, &v3B);
    r32 v3ScalarB = (v3A.x * v3B.x) +
        (v3A.y * v3B.y) +
        (v3A.z * v3B.z);
    assert(v3ScalarA == v3ScalarB);

    // v3Cross()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    v3Set(&v3B, 1.0f, 5.0f, 7.0f);
    v3Cross(&v3A, &v3B, &v3Result);
    assert(IsWithinTolerance(v3Result.x, -1));
    assert(IsWithinTolerance(v3Result.y, -4));
    assert(IsWithinTolerance(v3Result.z,  3));

    //
    // v4 Tests
    //
    v4 v4A = {0};
    v4 v4B = {0};
    v4 v4Result = {0};

    // v4Add()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Set(&v4B, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Add(&v4A, &v4B, &v4Result);
    assert(v4Result.x == 2 &&
           v4Result.y == 2 &&
           v4Result.z == 2 &&
           v4Result.w == 2);

    // v4Sub()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Set(&v4B, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Sub(&v4A, &v4B, &v4Result);
    assert(v4Result.x == 0 &&
           v4Result.y == 0 &&
           v4Result.z == 0 &&
           v4Result.w == 0);

    // v4ScalarMul()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4ScalarMul(&v4A, 5.0f, &v4Result);
    assert(v4Result.x == 5 &&
           v4Result.y == 5 &&
           v4Result.z == 5 &&
           v4Result.w == 5);

    // v4Mag()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    assert(v4Mag(&v4A) == (r32)sqrt(4));
    v4Set(&v4A, 0.0f, 0.0f, 0.0f, 0.0f);
    assert(v4Mag(&v4A) == 0);

    // v4Norm()
    v4Set(&v4A, 1.0f, 2.0f, 3.0f, 4.0f);
    r32 NormMagv4Result = v4Mag(&v4A);
    assert(IsWithinTolerance(NormMagv4Result, (r32)sqrt(30)));
    v4Set(&v4Result,
          (v4A.x / NormMagv4Result),
          (v4A.y / NormMagv4Result),
          (v4A.z / NormMagv4Result),
          (v4A.w / NormMagv4Result));
    v4Norm(&v4A);
    assert(v4A.x == v4Result.x);
    assert(v4A.y == v4Result.y);
    assert(v4A.z == v4Result.z);

    // v4IsNorm(), v4SetAndNorm()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    assert(v4IsNorm(&v4A));

    // v4Dot()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    v4SetAndNorm(&v4B, 4.0f, 5.0f, 6.0f, 4.0);
    r32 v4ScalarA = v4Dot(&v4A, &v4B);
    r32 v4ScalarB = (v4A.x * v4B.x) +
        (v4A.y * v4B.y) +
        (v4A.z * v4B.z) +
        (v4A.w * v4B.w);
    assert(v4ScalarA == v4ScalarB);


    //
    // m3 Tests
    //
    m3 m3A = {0};
    m3 m3Result = {0};

    // m3Ident()
    m3Ident(&m3A);
    r32 sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    assert(sum == 3);
    assert(m3A.arr2d[0][0] == 1);
    assert(m3A.arr2d[1][1] == 1);
    assert(m3A.arr2d[2][2] == 1);

    // m3Set(),
    m3Set(&m3A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    assert(sum == 0);

    m3Set(&m3A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    assert(sum == 9);

    // union m3
    m3A.row.i = (v3){1.5, 2.5, 3.5};
    m3A.row.j = (v3){4.5, 5.5, 6.5};
    m3A.row.k = (v3){7.5, 8.5, 9.5};
    v3Add(&m3A.row.i, &m3A.row.j, &v3Result);
    assert(v3Result.x == 6);
    assert(v3Result.y == 8);
    assert(v3Result.z == 10);
    assert(m3A.arr2d[2][0] == 7.5);
    assert(m3A.arr2d[2][1] == 8.5);
    assert(m3A.arr2d[2][2] == 9.5);
    assert(m3A.arr[0] == 1.5);
    assert(m3A.arr[1] == 2.5);
    assert(m3A.arr[2] == 3.5);

    // m3Mult()
    m3Ident(&m3A);
    m3 m3B = {0};
    m3Mult(&m3A, &m3B, &m3Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += 0;
    }
    assert(sum == 0);

    m3A.row.i = (v3){1, 2, 3};
    m3A.row.j = (v3){4, 5, 6};
    m3A.row.k = (v3){7, 8, 9};
    m3B.row.i = (v3){10, 20, 30};
    m3B.row.j = (v3){40, 50, 60};
    m3B.row.k = (v3){70, 80, 90};
    m3Mult(&m3A, &m3B, &m3Result);
    v3Set(&v3Result, 300, 360, 420);
    assert(v3IsEqual(&m3Result.row.i, &v3Result));
    v3Set(&v3Result, 660, 810, 960);
    assert(v3IsEqual(&m3Result.row.j, &v3Result));
    v3Set(&v3Result, 1020, 1260, 1500);
    assert(v3IsEqual(&m3Result.row.k, &v3Result));


    //
    // m4 Tests
    //
    m4 m4A = {0};
    m4 m4Result = {0};

    // m4Ident()
    m4Ident(&m4A);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    assert(sum == 4);
    assert(m4A.arr2d[0][0] == 1);
    assert(m4A.arr2d[1][1] == 1);
    assert(m4A.arr2d[2][2] == 1);

    // m4Set(),
    m4Set(&m4A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    assert(sum == 0);

    m4Set(&m4A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    assert(sum == 16);

    // union m4
    m4A.row.i = (v4){1.5, 2.5, 3.5, 4.5};
    m4A.row.j = (v4){4.5, 5.5, 6.5, 7.5};
    m4A.row.k = (v4){7.5, 8.5, 9.5, 1.0};
    v4Add(&m4A.row.i, &m4A.row.j, &v4Result);
    assert(v4Result.x == 6);
    assert(v4Result.y == 8);
    assert(v4Result.z == 10);
    assert(v4Result.w == 12);
    assert(m4A.arr2d[2][0] == 7.5);
    assert(m4A.arr2d[2][1] == 8.5);
    assert(m4A.arr2d[2][2] == 9.5);
    assert(m4A.arr2d[2][3] == 1.0);
    assert(m4A.arr[0] == 1.5);
    assert(m4A.arr[1] == 2.5);
    assert(m4A.arr[2] == 3.5);
    assert(m4A.arr[3] == 4.5);

    // m4Mult()
    m4Ident(&m4A);
    m4 m4B = {0};
    m4Mult(&m4A, &m4B, &m4Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += 0;
    }
    assert(sum == 0);

    m4A.row.i = (v4){1,  2,  3,    9};
    m4A.row.j = (v4){4,  5,  6,    8};
    m4A.row.k = (v4){7,  8,  9,    7};
    m4A.row.n = (v4){10, 11, 12,   6};
    m4B.row.i = (v4){10, 20, 30,  90};
    m4B.row.j = (v4){40, 50, 60,  80};
    m4B.row.k = (v4){70, 80, 90,  70};
    m4B.row.n = (v4){80, 90, 100, 60};
    m4Mult(&m4A, &m4B, &m4Result);
    v4Set(&v4Result, 1020, 1170, 1320, 1000);
    assert(v4IsEqual(&m4Result.row.i, &v4Result));
    v4Set(&v4Result, 1300, 1530, 1760, 1660);
    assert(v4IsEqual(&m4Result.row.j, &v4Result));
    v4Set(&v4Result, 1580, 1890, 2200, 2320);
    assert(v4IsEqual(&m4Result.row.k, &v4Result));
    v4Set(&v4Result, 1860, 2250, 2640, 2980);
    assert(v4IsEqual(&m4Result.row.n, &v4Result));
}

#endif // __RTX_H__
