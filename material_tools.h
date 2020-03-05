#ifndef __RT_MATERIAL_TOOLS_H___
#define __RT_MATERIAL_TOOLS_H___

#include <rt_settings.h>
#include <macro_tools.h>
#include <type_tools.h>
#include <color_tools.h>



typedef enum
{
    MATERIAL_CLASS_N__RT_ON__E,
    MATERIAL_CLASS_DIFFUSE,
    MATERIAL_CLASS_METAL
} MaterialClass;


typedef struct
{
    u16           max_generated_rays;
    MaterialClass material_class;
    r32           absorbtion_coefficient;
    Color32_RGB   color;
} Material;


__RT_internal__ __RT_call__  void
GetDefaultMaterialByClass(_mut_ Material*     restrict const material,
                          const MaterialClass                material_class)
{
    __RT_ASSERT__(material);

    switch(material_class)
    {
        case MATERIAL_CLASS_DIFFUSE:
        {
            material->max_generated_rays = 3;
            material->absorbtion_coefficient = (r32)0.85;
            break;
        }
        case MATERIAL_CLASS_METAL:
        {
            material->max_generated_rays = 3;
            material->absorbtion_coefficient = (r32)0.05;
            break;
        }

        case MATERIAL_CLASS_N__RT_ON__E:
        {
            material->max_generated_rays = 0;
            material->absorbtion_coefficient = (r32)0.0f;
            break;
        }
    }
}


__RT_internal__ __RT_inline__ void
BlendColorByMaterial(const Material*    restrict const material,
                     const Color32_RGB* restrict const input_color,
                     _mut_ Color32_RGB* restrict const return_color)
{
    __RT_ASSERT__(material);
    __RT_ASSERT__(material->max_generated_rays > 0);
    __RT_ASSERT__(material->material_class != MATERIAL_CLASS_N__RT_ON__E);
    __RT_ASSERT__(material->absorbtion_coefficient >= 0
                  && material->absorbtion_coefficient <= 1);
    __RT_ASSERT__(input_color);
    __RT_ASSERT__(return_color);

    // [ cfarvin::TODO ] [ cfarvin::FINDME ]
    /* *(return_color->channel.R) = */

    return;
}

#endif // __RT_MATERIAL_TOOLS_H__
