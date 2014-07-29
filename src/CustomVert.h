#ifndef CUSTOMVERT_H
#define CUSTOMVERT_H

#include "Types.h"

typedef struct {
    Vec3 loc;
    Color3 color;
    Vec3 normal;
    /*Vec3 _lights[3];*/
} CustomVert;

void cvertInterpolate(CustomVert* out,float factor,const CustomVert* a,
                                                   const CustomVert* b);

#endif

