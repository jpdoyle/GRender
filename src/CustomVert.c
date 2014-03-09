#include "CustomVert.h"

void cvertInterpolate(CustomVert* out,float factor,const CustomVert* a,
                                                   const CustomVert* b) {
    vec3Interpolate(out->loc,factor,a->loc,b->loc);
    vec3Interpolate(out->color,factor,a->color,b->color);
    vec3Interpolate(out->normal,factor,a->normal,b->normal);
}
