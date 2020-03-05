#ifndef __RT_ENTITY_TOOLS_H___
#define __RT_ENTITY_TOOLS_H___

#include <rt_settings.h>
#include <macro_tools.h>
#include <type_tools.h>
#include <maths_tools.h>
#include <color_tools.h>



typedef struct
{
    v3       position;
    r32      radius;
    Material material;
} Sphere;


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

    // [ cfarvin::REVISIT ]
    Material intersection_material;
} RayIntersection;


typedef struct
{
    v3 origin;
    v3 direction;
} Camera;



//
// PROTOTYPES (as needed)
//
__RT_internal__ __RT_inline__ void
IntersectSphere(const Ray*             restrict const ray,
                const Sphere*          restrict const sphere,
                _mut_ RayIntersection* restrict const intersection);

__RT_internal__ __RT_inline__ void
TraceSphereArray(const Ray*             restrict const ray,
                 _mut_ RayIntersection* restrict const intersection,
                 _mut_ r32*             restrict const global_magnitude_threshold,
                 _mut_ Color32_RGB*     restrict const return_color,
                 const Sphere*          restrict const sphere_arr,
                 const size_t                          num_spheres);

//
#if __RT_AA__reflections
//
__RT_internal__ __RT_inline__ void
ReflectRays(/* const Ray*             restrict const ray, */
    _mut_ RayIntersection* restrict const incident_intersection,
    /* _mut_ r32*             restrict const global_magnitude_threshold, */
    _mut_ Color32_RGB*     restrict const return_color,
    const Sphere*          restrict const sphere_arr,
    const size_t                          num_spheres,
    const size_t                          intersected_sphere_index)
{
    /* __RT_ASSERT__(ray); */
    __RT_ASSERT__(incident_intersection);
    __RT_ASSERT__(return_color);
    __RT_ASSERT__(sphere_arr);
    __RT_ASSERT__(num_spheres >= 1);
    __RT_ASSERT__(intersected_sphere_index <= num_spheres);
    /* __RT_ASSERT__(global_magnitude_threshold); */
    __RT_ASSERT__(num_spheres >= 1); // See: TraceSphere( ... );

    if (!incident_intersection->does_intersect)
    {
        return;
    }

    u16             bounces             = 0;
    r64             photon_energy       = 0;
    RayIntersection bounce_intersection = { 0 };
    Ray             bounce_ray          = { 0 };
    Color32_RGB     bounce_color        = { 0 };

    GetEnergyByColorRGB_eV(return_color, &photon_energy);
    photon_energy /=
        incident_intersection->intersection_material.absorbtion_coefficient;

    while (photon_energy > 0)
    {
        if (bounces >
            incident_intersection->intersection_material.max_generated_rays)
        {
            break;
        }

        v3Set(&bounce_ray.origin,
              incident_intersection->normal_vector.x,
              incident_intersection->normal_vector.y,
              incident_intersection->normal_vector.z);

        r32 xrand = (r32)XorShift32() + TOLERANCE;
        r32 yrand = (r32)XorShift32() + TOLERANCE;
        r32 zrand = (r32)XorShift32() + TOLERANCE;
        v3SetAndNorm(&bounce_ray.direction,
                     bounce_ray.origin.x +
                     NormalizeToRange((r32)TOLERANCE,
                                      (r32)(~(u32)0),
                                      (r32)TOLERANCE,
                                      (r32)__RT_AA__reflection_noise,
                                      xrand),
                     bounce_ray.origin.y +
                     NormalizeToRange((r32)TOLERANCE,
                                      (r32)(~(u32)0),
                                      (r32)TOLERANCE,
                                      (r32)__RT_AA__reflection_noise,
                                      yrand),
                     bounce_ray.origin.z +
                     NormalizeToRange((r32)TOLERANCE,
                                      (r32)(~(u32)0),
                                      (r32)TOLERANCE,
                                      (r32)__RT_AA__reflection_noise,
                                      zrand));

// Ensure that the reflected ray does not intersect
// the originating sphere at a point other than its
// origin.
#if __RT_DEBUG__
        RayIntersection test_intersection = { 0 };
        IntersectSphere(&bounce_ray,
                        &sphere_arr[intersected_sphere_index],
                        &test_intersection);
        if (test_intersection.does_intersect)
        {
            __RT_ASSERT__(v3IsEqual(&test_intersection.normal_vector,
                                    &bounce_ray.origin));
        }
#endif // __RT_DEBUG__

        TraceSphereArray(&bounce_ray,
                         &bounce_intersection,
                         &incident_intersection->magnitude,
                         &bounce_color,
                         sphere_arr,
                         num_spheres);

        if (bounce_intersection.does_intersect &&
            bounce_intersection.normal_vector.z >
            incident_intersection->normal_vector.z)
        {
            return_color->channel.R = bounce_color.channel.R;
            return_color->channel.G = bounce_color.channel.G;
            return_color->channel.B = bounce_color.channel.B;
        }

        bounces++;
    }
}
//
#endif // __RT_AA__reflections
//

__RT_internal__ __RT_inline__ void
SetRayDirectionByPixelCoordAA(_mut_ Ray* restrict const ray,
                              const size_t       pix_x,
                              const size_t       pix_y)
{
    const r32 xOr_contribution_x =
        NormalizeToRange((r32)TOLERANCE,
                         (r32)(~(u32)0),
                         (r32)TOLERANCE,
                         (r32)__RT_AA__noise,
                         (r32)XorShift32() + (r32)TOLERANCE);

    const r32 xOr_contribution_y =
        NormalizeToRange((r32)TOLERANCE,
                         (r32)(~(u32)0),
                         (r32)TOLERANCE,
                         (r32)__RT_AA__noise,
                         (r32)XorShift32() + (r32)TOLERANCE);

    const r32 x_numerator = (r32)pix_x + xOr_contribution_x;
    const r32 y_numerator = (r32)pix_y + xOr_contribution_y;

    ray->direction.x =
        ((x_numerator/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (y_numerator/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


__RT_internal__ __RT_inline__ void
SetRayDirectionByPixelCoord(_mut_ Ray* restrict const ray,
                            const size_t       pix_x,
                            const size_t       pix_y)
{
    ray->direction.x = ((pix_x/(r32)IMAGE_WIDTH) - 0.5f ) * ASPECT_RATIO;
    ray->direction.y = (pix_y/(r32)IMAGE_HEIGHT) - 0.5f;
    ray->direction.z = -1;
}


__RT_internal__ Sphere*
CreateSpheres(const size_t sphere_count)
{
    return (Sphere*)calloc(sphere_count, sizeof(Sphere));
}


__RT_internal__ __RT_inline__ void
IntersectSphere(const Ray*             restrict const ray,
                const Sphere*          restrict const sphere,
                _mut_ RayIntersection* restrict const intersection)
{
    __RT_ASSERT__(ray && sphere && intersection);
    __RT_ASSERT__(v3IsNorm(&ray->direction));

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
    /* __RT_ASSERT__(c > 0); */

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

        // [ cfarvin::NOTE ] Bounces may be in any direction
        //__RT_ASSERT__(magnitude >= 0);

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

        v3Norm(&intersection->normal_vector);

        // [ cfarvin::REVSIT ]
        // Set intersection material
        intersection->intersection_material.max_generated_rays =
            sphere->material.max_generated_rays;
        intersection->intersection_material.material_class =
            sphere->material.material_class;
        intersection->intersection_material.absorbtion_coefficient =
            sphere->material.absorbtion_coefficient;
        intersection->intersection_material.color =
            sphere->material.color;
    }
}


__RT_internal__ __RT_inline__ void
TraceSphere(const Ray*             restrict const ray,
            _mut_ RayIntersection* restrict const intersection,
            _mut_ r32*             restrict const global_magnitude_threshold,
            _mut_ Color32_RGB*     restrict const return_color,
            const Sphere*          restrict const sphere)
{
    __RT_ASSERT__(ray);
    __RT_ASSERT__(intersection);
    __RT_ASSERT__(return_color);
    __RT_ASSERT__(sphere);
    __RT_ASSERT__(global_magnitude_threshold);

    IntersectSphere(ray, sphere, intersection);
    if (intersection->does_intersect &&
        (fabs(intersection->magnitude) < fabs(*global_magnitude_threshold)))
    {
        *global_magnitude_threshold = (r32)fabs(intersection->magnitude);
        return_color->value = sphere->material.color.value;
    }
}


__RT_internal__ __RT_inline__ void
TraceSphereArray(const Ray*             restrict const ray,
                 _mut_ RayIntersection* restrict const intersection,
                 _mut_ r32*             restrict const global_magnitude_threshold,
                 _mut_ Color32_RGB*     restrict const return_color,
                 const Sphere*          restrict const sphere_arr,
                 const size_t                          num_spheres)
{
    __RT_ASSERT__(ray);
    __RT_ASSERT__(intersection);
    __RT_ASSERT__(return_color);
    __RT_ASSERT__(sphere_arr);
    __RT_ASSERT__(global_magnitude_threshold);
    __RT_ASSERT__(num_spheres >= 2); // See: TraceSphere( ... );

    RayIntersection closestIntersection = { 0 };
    closestIntersection.magnitude = MAX_RAY_MAG;
//
#if __RT_AA__reflections
//
    size_t intersected_sphere_index = 0;
//
#endif // __RT_AA__reflections
//
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

            return_color->value =
                (sphere_arr[sphere_index]).material.color.value;
//
#if __RT_AA__reflections
//
            intersected_sphere_index = sphere_index;
//
#endif // __RT_AA__reflections
//
        }
    }

    if (closestIntersection.does_intersect &&
        fabs(closestIntersection.magnitude) < fabs(*global_magnitude_threshold))
    {
        intersection->does_intersect = true;
    }

//
#if __RT_AA__reflections
//
    ReflectRays(intersection,
                return_color,
                sphere_arr,
                num_spheres,
                intersected_sphere_index);
//
#endif // __RT_AA__reflections
//
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

#endif // __RT_ENTITY_TOOLS_H___
