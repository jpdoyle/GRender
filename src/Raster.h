#ifndef RASTER_H
#define RASTER_H

#include "Types.h"
#include "Shader.h"
#include "Context.h"

static inline void plotPoint(Context* ct,const Vec3 loc,
                             const Color3 color) {
    int x = loc[0],
        y = loc[1];

    unsigned index = y*ct->_width+x;
    if(!ct->depthEnabled || ct->_depth[index] > loc[2]) {
        ct->_depth[index] = loc[2];
        Uint8* c = ct->surface->pixels + index*3;
        c[0] = 255*color[0]+0.5;
        c[1] = 255*color[1]+0.5;
        c[2] = 255*color[2]+0.5;
    }
}

void rasterPoint(Context* ct,const Varyings* varyings);

void rasterLine(Context* ct,const Varyings* begin,
                            const Varyings* end);

void rasterSpansBetween(Context* ct,const Varyings* a1,
                                    const Varyings* a2,
                                    const Varyings* b1,
                                    const Varyings* b2);

void rasterTriangle(Context* ct,const Varyings* a,
                                const Varyings* b,
                                const Varyings* c);

#endif
