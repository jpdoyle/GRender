#ifndef RASTER_H
#define RASTER_H

#include "Types.h"
#include "Shader.h"
#include "Context.h"

void plotPoint(Context* ct,const Vec3 loc,const Color3 color);

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
