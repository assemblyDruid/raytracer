#ifndef __RT_SETTINGS_H___
#define __RT_SETTINGS_H___



// Debug/Rlease Settings
#if  __RT_DEBUG__
#define __RT_inline__    /* INLINE REMOVED */
#define __RT_internal__  /* STATIC REMOVED */
#define __RT_ASSERT__    assert
#else // __RT_DEBUG__
#define __RT_inline__    inline
#define __RT_internal__  static
#define __RT_ASSERT__    /* ASSERTION REMOVED */
#endif // __RT_DEBUG__


// Raytracing Settings
#define NUM_ENTITYS 6

// Image Settings
#define IMAGE_WIDTH  400
#define IMAGE_HEIGHT 400
/* #define IMAGE_WIDTH  1280 */
/* #define IMAGE_HEIGHT 720 */
/* #define IMAGE_WIDTH  1920 */
/* #define IMAGE_HEIGHT 1080 */
/* #define IMAGE_WIDTH  3840 */
/* #define IMAGE_HEIGHT 2160 */
#define ASPECT_RATIO (r32)(IMAGE_WIDTH/(r32)IMAGE_HEIGHT)

// Mode Settings
#define __RT_AA__            __RT_ON__
#define __RT_DEBUG__         __RT_OFF__
#define __RT_AA__reflections __RT_ON__

// Antialiasing & Reflection Settings
#define __RT_AA__RPP              25
#define __RT_AA__noise            2.15
#define __RT_AA__reflection_noise 0.105

// Endianness Settings
#ifndef __LSB__
#define __LSB__ 1 // 1=LSB/Little Endian, 0=MSB/Big Endian
#if __LSB__
#define channel LSB_channel
#else
#define channel MSB_channel
#endif // __LSB__ (if)
#endif // __LSB__ (ifndef)

// Min Max Settings
#define TOLERANCE             0.00001f
#define _PI_                  3.14159f
#define MAX_RAY_MAG           5.0f
#define MIN_RAY_MAG           0.0f
#define MAX_PPM_HEADER_SIZE   25
#define MAX_PPM_TRIPPLET_SIZE 15

// Coordinate Direction Coloring (Debug)
#define RENDER_COORD_DIR     __RT_OFF__
#define COORD_DIR_PIX_WEIGHT 1

#endif __RT_SETTINGS_H___
