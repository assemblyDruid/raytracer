#ifndef __RT_MATHS_TOOLS_H___
#define __RT_MATHS_TOOLS_H___

#include <math.h>
#include <float.h>

#include <rt_settings.h>
#include <macro_tools.h>
#include <type_tools.h>



static double _PLANK_C__RT_ON__ST_ = 0.000000000000000000000000000000000662607015;
static double _C_AIR_       = 299700000.0;
// static double _C_VACCUME_   = 299792458.0;

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
    __RT_ASSERT__(max_source_range > min_source_range);
    __RT_ASSERT__(max_target_range > min_target_range);
    __RT_ASSERT__(max_source_range != min_source_range);
    __RT_ASSERT__(max_target_range != min_target_range);
    __RT_ASSERT__(num_to_normalize >= min_source_range &&
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
    __RT_ASSERT__(MAX_RAY_MAG > MIN_RAY_MAG);
    return ((old_value - MIN_RAY_MAG) * (1.0f / FLT_MAX));
}


__RT_internal__ __RT_inline__ void
DegreesToRadians(const r32 degrees,
                 _mut_ r32* restrict const result)
{
    __RT_ASSERT__(result);
    *result = degrees * (_PI_ / 180.0f);
}


__RT_internal__ __RT_inline__ void
RadiansToDegrees(const r32 radians,
                 _mut_ r32* restrict const result)
{
    __RT_ASSERT__(result);
    *result = radians * (180.0f / _PI_);
}



//
// v3
//
__RT_internal__ __RT_inline__ void
v3Set(_mut_ v3* restrict const result,
      const r32 x,
      const r32 y,
      const r32 z)
{
    __RT_ASSERT__(result);
    result->x = x;
    result->y = y;
    result->z = z;
}


__RT_internal__ __RT_inline__ bool
v3IsEqual(const v3* restrict const a,
          const v3* restrict const b)
{
    __RT_ASSERT__(a && b);
    return ( IsWithinTolerance(a->x, b->x) &&
             IsWithinTolerance(a->y, b->y) &&
             IsWithinTolerance(a->z, b->z) );
}


__RT_internal__ __RT_inline__ r32
v3Mag(const v3* restrict const a)
{
    __RT_ASSERT__(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 sum2 = x2 + y2 + z2;
    return (r32)sqrt(sum2);
}


__RT_internal__ __RT_inline__ bool
v3IsNorm(const v3* restrict const a)
{
    return(IsWithinTolerance(v3Mag(a), 1.0f));
}


__RT_internal__ __RT_inline__ void
v3Norm(v3* restrict const a)
{
    __RT_ASSERT__(a);
    r32 magnitude = v3Mag(a);
    if (magnitude)
    {
        a->x /= magnitude;
        a->y /= magnitude;
        a->z /= magnitude;
    }
    else
    {
        /* __RT_ASSERT__(false); */ // [ cfarvin::UNDO ]
        v3Set(a, 0.0f, 0.0f, 0.0f);
    }
}


__RT_internal__ __RT_inline__ void
v3SetAndNorm(_mut_ v3* restrict const result,
             const r32 x,
             const r32 y,
             const r32 z)
{
    __RT_ASSERT__(result);
    result->x = x;
    result->y = y;
    result->z = z;
    v3Norm(result);
}


__RT_internal__ __RT_inline__ void
v3Add(const v3* restrict const a,
      const v3* restrict const b,
      _mut_ v3* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}


__RT_internal__ __RT_inline__ void
v3Sub(const v3* restrict const a,
      const v3* restrict const b,
      _mut_ v3* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}


__RT_internal__ __RT_inline__ void
v3ScalarMul(const v3* restrict const a,
            const r32                scalar,
            _mut_ v3* restrict const result)
{
    __RT_ASSERT__(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
}


__RT_internal__ __RT_inline__ r32
v3Dot(const v3* restrict const a,
      const v3* restrict const b)
{
    __RT_ASSERT__(a && b);
    r32 result = 0;
    result += a->x * b->x;
    result += a->y * b->y;
    result += a->z * b->z;
    return result;
}


__RT_internal__ __RT_inline__ void
v3Cross(const v3* restrict const a,
        const v3* restrict const b,
        _mut_ v3* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    r32 i = ((a->y * b->z) - (a->z * b->y));
    r32 j = ((a->z * b->x) - (a->x * b->z));
    r32 k = ((a->x * b->y) - (a->y * b->x));
    v3Set(result, i, j, k);
}


__RT_internal__ __RT_inline__ void
v3CrossAndNorm(const v3* restrict const a,
               const v3* restrict const b,
               _mut_ v3* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    v3Cross(a, b, result);
    v3Norm(result);
}



//
// v4
//
__RT_internal__ __RT_inline__ void
v4Set(v4* restrict const result,
      const r32 x,
      const r32 y,
      const r32 z,
      const r32 w)
{
    __RT_ASSERT__(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
}


__RT_internal__ __RT_inline__ bool
v4IsEqual(const v4* restrict const a,
          const v4* restrict const b)
{
    __RT_ASSERT__(a && b);
    return (IsWithinTolerance(a->x, b->x) &&
            IsWithinTolerance(a->y, b->y) &&
            IsWithinTolerance(a->z, b->z) &&
            IsWithinTolerance(a->w, b->w) );
}


__RT_internal__ __RT_inline__ r32
v4Mag(const v4* const a)
{
    __RT_ASSERT__(a);
    r32 x2 = a->x * a->x;
    r32 y2 = a->y * a->y;
    r32 z2 = a->z * a->z;
    r32 w2 = a->w * a->w;
    r32 sum2 = x2 + y2 + z2 + w2;
    return (r32)sqrt(sum2);
}


__RT_internal__ __RT_inline__ bool
v4IsNorm(const v4* restrict const a)
{
    return(IsWithinTolerance(v4Mag(a), 1.0f));
}


__RT_internal__ __RT_inline__ void
v4Norm(_mut_ v4* restrict const a)
{
    __RT_ASSERT__(a);
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
        __RT_ASSERT__(false);
        v4Set(a, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}


__RT_internal__ __RT_inline__ void
v4SetAndNorm(_mut_ v4* restrict const result,
             const r32 x,
             const r32 y,
             const r32 z,
             const r32 w)
{
    __RT_ASSERT__(result);
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
    v4Norm(result);
}


__RT_internal__ __RT_inline__ void
v4Add(const v4* restrict const a,
      const v4* restrict const b,
      _mut_ v4* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
    result->w = a->w + b->w;
}


__RT_internal__ __RT_inline__ void
v4Sub(const v4* restrict const a,
      const v4* restrict const b,
      _mut_ v4* restrict const result)
{
    __RT_ASSERT__(a && b && result);
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
    result->w = a->w - b->w;
}


__RT_internal__ __RT_inline__ void
v4ScalarMul(const v4* restrict const a,
            const r32 scalar,
            _mut_ v4* restrict const result)
{
    __RT_ASSERT__(a && result);
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
    result->w = a->w * scalar;
}


__RT_internal__ __RT_inline__ r32
v4Dot(const v4* restrict const a,
      const v4* restrict const b)
{
    __RT_ASSERT__(a && b);
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
m3Set(_mut_ m3* restrict const a,
      const r32 b)
{
    __RT_ASSERT__(a);
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        a->arr[idx] = b;
    }
}


__RT_internal__ bool
m3IsEqual(const m3* restrict const a,
          const m3* restrict const b)
{
    __RT_ASSERT__(a && b);
    return( (v3IsEqual(&a->i, &b->i)) &&
            (v3IsEqual(&a->j, &b->j)) &&
            (v3IsEqual(&a->k, &b->k)) );
}


// [ cfarvin::TODO ] Measure & improve
__RT_internal__ void
m3Ident(_mut_ m3* restrict const result)
{
    __RT_ASSERT__(result);
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
m3Mult(const m3* restrict const a,
       const m3* restrict const b,
       _mut_ m3* restrict const result)
{
    __RT_ASSERT__(a && b && result);
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
    __RT_ASSERT__(a);
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        a->arr[idx] = b;
    }
}


__RT_internal__ bool
m4IsEqual(const m4* const a, const m4* const b)
{
    __RT_ASSERT__(a && b);
    return( (v4IsEqual(&a->i, &b->i)) &&
            (v4IsEqual(&a->j, &b->j)) &&
            (v4IsEqual(&a->k, &b->k)) &&
            (v4IsEqual(&a->n, &b->n)) );
}


// [ cfarvin::TODO ] Measure & improve
__RT_internal__ void
m4Ident(_mut_ m4* const result)
{
    __RT_ASSERT__(result);
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
m4Mult(const m4* restrict const a,
       const m4* restrict const b,
       _mut_ m4* restrict const result)
{
    __RT_ASSERT__(a && b && result);
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
    __RT_ASSERT__(v3Result.x == 2 && v3Result.y == 2 && v3Result.z == 2);

    // v3Sub()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3Set(&v3B, 1.0f, 1.0f, 1.0f);
    v3Sub(&v3A, &v3B, &v3Result);
    __RT_ASSERT__(v3Result.x == 0 && v3Result.y == 0 && v3Result.z == 0);

    // v3ScalarMul()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    v3ScalarMul(&v3A, 5.0f, &v3Result);
    __RT_ASSERT__(v3Result.x == 5 && v3Result.y == 5 && v3Result.z == 5);

    // v3Mag()
    v3Set(&v3A, 1.0f, 1.0f, 1.0f);
    __RT_ASSERT__(v3Mag(&v3A) == (r32)sqrt(3));
    v3Set(&v3A, 0.0f, 0.0f, 0.0f);
    __RT_ASSERT__(v3Mag(&v3A) == 0);

    // v3Norm()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    r32 NormMagv3Result = v3Mag(&v3A);
    __RT_ASSERT__(NormMagv3Result = (r32)sqrt(14));
    v3Set(&v3Result,
          (v3A.x / NormMagv3Result),
          (v3A.y / NormMagv3Result),
          (v3A.z / NormMagv3Result));
    v3Norm(&v3A);
    __RT_ASSERT__(v3A.x == v3Result.x);
    __RT_ASSERT__(v3A.y == v3Result.y);
    __RT_ASSERT__(v3A.z == v3Result.z);

    // v3IsNorm(), v3SetAndNorm()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    __RT_ASSERT__(v3IsNorm(&v3A));

    // v3Dot()
    v3SetAndNorm(&v3A, 1.0f, 2.0f, 3.0f);
    v3SetAndNorm(&v3B, 4.0f, 5.0f, 6.0f);
    r32 v3ScalarA = v3Dot(&v3A, &v3B);
    r32 v3ScalarB = (v3A.x * v3B.x) +
        (v3A.y * v3B.y) +
        (v3A.z * v3B.z);
    __RT_ASSERT__(v3ScalarA == v3ScalarB);

    // v3Cross()
    v3Set(&v3A, 1.0f, 2.0f, 3.0f);
    v3Set(&v3B, 1.0f, 5.0f, 7.0f);
    v3Cross(&v3A, &v3B, &v3Result);
    __RT_ASSERT__(IsWithinTolerance(v3Result.x, -1));
    __RT_ASSERT__(IsWithinTolerance(v3Result.y, -4));
    __RT_ASSERT__(IsWithinTolerance(v3Result.z,  3));



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
    __RT_ASSERT__(v4Result.x == 2 &&
                  v4Result.y == 2 &&
                  v4Result.z == 2 &&
                  v4Result.w == 2);

    // v4Sub()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Set(&v4B, 1.0f, 1.0f, 1.0f, 1.0f);
    v4Sub(&v4A, &v4B, &v4Result);
    __RT_ASSERT__(v4Result.x == 0 &&
                  v4Result.y == 0 &&
                  v4Result.z == 0 &&
                  v4Result.w == 0);

    // v4ScalarMul()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    v4ScalarMul(&v4A, 5.0f, &v4Result);
    __RT_ASSERT__(v4Result.x == 5 &&
                  v4Result.y == 5 &&
                  v4Result.z == 5 &&
                  v4Result.w == 5);

    // v4Mag()
    v4Set(&v4A, 1.0f, 1.0f, 1.0f, 1.0f);
    __RT_ASSERT__(v4Mag(&v4A) == (r32)sqrt(4));
    v4Set(&v4A, 0.0f, 0.0f, 0.0f, 0.0f);
    __RT_ASSERT__(v4Mag(&v4A) == 0);

    // v4Norm()
    v4Set(&v4A, 1.0f, 2.0f, 3.0f, 4.0f);
    r32 NormMagv4Result = v4Mag(&v4A);
    __RT_ASSERT__(IsWithinTolerance(NormMagv4Result, (r32)sqrt(30)));
    v4Set(&v4Result,
          (v4A.x / NormMagv4Result),
          (v4A.y / NormMagv4Result),
          (v4A.z / NormMagv4Result),
          (v4A.w / NormMagv4Result));
    v4Norm(&v4A);
    __RT_ASSERT__(v4A.x == v4Result.x);
    __RT_ASSERT__(v4A.y == v4Result.y);
    __RT_ASSERT__(v4A.z == v4Result.z);

    // v4IsNorm(), v4SetAndNorm()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    __RT_ASSERT__(v4IsNorm(&v4A));

    // v4Dot()
    v4SetAndNorm(&v4A, 1.0f, 2.0f, 3.0f, 4.0);
    v4SetAndNorm(&v4B, 4.0f, 5.0f, 6.0f, 4.0);
    r32 v4ScalarA = v4Dot(&v4A, &v4B);
    r32 v4ScalarB = (v4A.x * v4B.x) +
        (v4A.y * v4B.y) +
        (v4A.z * v4B.z) +
        (v4A.w * v4B.w);
    __RT_ASSERT__(v4ScalarA == v4ScalarB);



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
    __RT_ASSERT__(sum == 3);
    __RT_ASSERT__(m3A.arr2d[0][0] == 1);
    __RT_ASSERT__(m3A.arr2d[1][1] == 1);
    __RT_ASSERT__(m3A.arr2d[2][2] == 1);

    // m3Set(),
    m3Set(&m3A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    __RT_ASSERT__(sum == 0);

    m3Set(&m3A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += m3A.arr[idx];
    }
    __RT_ASSERT__(sum == 9);

    // union m3
    m3A.i = (v3){{1.5, 2.5, 3.5}};
    m3A.j = (v3){{4.5, 5.5, 6.5}};
    m3A.k = (v3){{7.5, 8.5, 9.5}};
    v3Add(&m3A.i, &m3A.j, &v3Result);
    __RT_ASSERT__(v3Result.x == 6);
    __RT_ASSERT__(v3Result.y == 8);
    __RT_ASSERT__(v3Result.z == 10);
    __RT_ASSERT__(m3A.arr2d[2][0] == 7.5);
    __RT_ASSERT__(m3A.arr2d[2][1] == 8.5);
    __RT_ASSERT__(m3A.arr2d[2][2] == 9.5);
    __RT_ASSERT__(m3A.arr[0] == 1.5);
    __RT_ASSERT__(m3A.arr[1] == 2.5);
    __RT_ASSERT__(m3A.arr[2] == 3.5);

    // m3Mult()
    m3Ident(&m3A);
    m3 m3B = { 0 };
    m3Mult(&m3A, &m3B, &m3Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 9; idx++)
    {
        sum += 0;
    }
    __RT_ASSERT__(sum == 0);

    m3A.i = (v3){{1, 2, 3}};
    m3A.j = (v3){{4, 5, 6}};
    m3A.k = (v3){{7, 8, 9}};
    m3B.i = (v3){{10, 20, 30}};
    m3B.j = (v3){{40, 50, 60}};
    m3B.k = (v3){{70, 80, 90}};
    m3Mult(&m3A, &m3B, &m3Result);
    v3Set(&v3Result, 300, 360, 420);
    __RT_ASSERT__(v3IsEqual(&m3Result.i, &v3Result));
    v3Set(&v3Result, 660, 810, 960);
    __RT_ASSERT__(v3IsEqual(&m3Result.j, &v3Result));
    v3Set(&v3Result, 1020, 1260, 1500);
    __RT_ASSERT__(v3IsEqual(&m3Result.k, &v3Result));



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
    __RT_ASSERT__(sum == 4);
    __RT_ASSERT__(m4A.arr2d[0][0] == 1);
    __RT_ASSERT__(m4A.arr2d[1][1] == 1);
    __RT_ASSERT__(m4A.arr2d[2][2] == 1);

    // m4Set(),
    m4Set(&m4A, 0);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    __RT_ASSERT__(sum == 0);

    m4Set(&m4A, 1);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += m4A.arr[idx];
    }
    __RT_ASSERT__(sum == 16);

    // union m4
    m4A.i = (v4){{1.5, 2.5, 3.5, 4.5}};
    m4A.j = (v4){{4.5, 5.5, 6.5, 7.5}};
    m4A.k = (v4){{7.5, 8.5, 9.5, 1.0}};
    v4Add(&m4A.i, &m4A.j, &v4Result);
    __RT_ASSERT__(v4Result.x == 6);
    __RT_ASSERT__(v4Result.y == 8);
    __RT_ASSERT__(v4Result.z == 10);
    __RT_ASSERT__(v4Result.w == 12);
    __RT_ASSERT__(m4A.arr2d[2][0] == 7.5);
    __RT_ASSERT__(m4A.arr2d[2][1] == 8.5);
    __RT_ASSERT__(m4A.arr2d[2][2] == 9.5);
    __RT_ASSERT__(m4A.arr2d[2][3] == 1.0);
    __RT_ASSERT__(m4A.arr[0] == 1.5);
    __RT_ASSERT__(m4A.arr[1] == 2.5);
    __RT_ASSERT__(m4A.arr[2] == 3.5);
    __RT_ASSERT__(m4A.arr[3] == 4.5);

    // m4Mult()
    m4Ident(&m4A);
    m4 m4B = { 0 };
    m4Mult(&m4A, &m4B, &m4Result);
    sum = 0;
    for (uint8_t idx = 0; idx < 16; idx++)
    {
        sum += 0;
    }
    __RT_ASSERT__(sum == 0);

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
    __RT_ASSERT__(v4IsEqual(&m4Result.i, &v4Result));
    v4Set(&v4Result, 1300, 1530, 1760, 1660);
    __RT_ASSERT__(v4IsEqual(&m4Result.j, &v4Result));
    v4Set(&v4Result, 1580, 1890, 2200, 2320);
    __RT_ASSERT__(v4IsEqual(&m4Result.k, &v4Result));
    v4Set(&v4Result, 1860, 2250, 2640, 2980);
    __RT_ASSERT__(v4IsEqual(&m4Result.n, &v4Result));



    //
    // XorShift32 Tests
    //
    u32 PrevXorState = XorShift32State;
    XorShift32State = 42;
    r32 rand = 0;
    for (size_t iter = 0; iter < 10000; iter++)
    {
        rand = NormalBoundedXorShift32();
        __RT_ASSERT__(rand >= 0);
        __RT_ASSERT__(rand <= 1);
    }

    // Reset XorShift32State
    XorShift32State = PrevXorState;
}

#endif // __RT_MATHS_TOOLS_H___
