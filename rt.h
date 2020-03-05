#ifndef __RT_H__
#define __RT_H__

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <rt_settings.h>
#include <macro_tools.h>
#include <type_tools.h>
#include <maths_tools.h>
#include <color_tools.h>
#include <material_tools.h>
#include <entity_tools.h>
#include <image_tools.h>

#endif // __RT_H__






//
#if 0
//

//
// ARCHIVE
//

/* Sphere* sphere_arr = CreateSpheres(num_spheres); */
/* sphere_arr[0].position.x = +0.40f; */
/* sphere_arr[0].position.y = +0.75f; */
/* sphere_arr[0].position.z = -1.10f; */
/* sphere_arr[0].radius     = +0.25f; */
/* sphere_arr[0].material.color.value = 0x000000FF; */

/* sphere_arr[1].position.x = -0.40f; */
/* sphere_arr[1].position.y = -0.75f; */
/* sphere_arr[1].position.z = -1.10f; */
/* sphere_arr[1].radius     = +0.25f; */
/* sphere_arr[1].material.color.value = 0x0000FF00; */

// From IntersectSphere( ... )
// Correct normal vector direction if needed.
// We seek the normal opposite the direction of the ray.
/* if (sphere->radius > magnitude) */
/* { */
/*     v3ScalarMul(&intersection->normal_vector, */
/*                 -1.0f, */
/*                 &intersection->normal_vector); */
/* } */

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

/* #define CLAMP_MAX(value, max) if(value>max){value=max;} */
/* #define CLAMP_MIN(value, min) if(value<min){value=min;} */

//
#endif // 0
//
