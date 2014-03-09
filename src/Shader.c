#include "Shader.h"
#include <stdlib.h>

Uniforms* createUniforms(unsigned numCustom,
                         const UniformType* customTypes) {
    Uniforms* ret = malloc(sizeof(Uniforms));
    ret->numCustom = numCustom;
    ret->customTypes = customTypes;
    if(numCustom) {
        ret->custom = malloc(numCustom*sizeof(float*));
    } else {
        ret->custom = NULL;
    }

    return ret;
}

void freeUniforms(Uniforms* uniforms) {
    if(!uniforms) {
        return;
    }
    if(uniforms->custom) {
        free(uniforms->custom);
    }
    free(uniforms);
}

Varyings* createVaryings(unsigned numAttributes,
                         const VertexAttribute* attributes) {
    Varyings* ret = malloc(sizeof(Varyings));
    ret->numAttributes = numAttributes;
    ret->attributes    = attributes;
    if(numAttributes) {
        ret->attributePtrs = malloc(numAttributes*sizeof(float*));
        unsigned i;
        for(i=0;i<numAttributes;++i) {
            ret->attributePtrs[i] = malloc(attributes[i].numValues
                                            *sizeof(float));
        }
    } else {
        ret->attributePtrs = NULL;
    }

    return ret;
}

void copyVaryings(Varyings* out,const Varyings* src) {
    vec4Copy(out->loc,src->loc);
    vec3Copy(out->color,src->color);
    unsigned i;
    for(i=0;i<out->numAttributes;++i) {
        unsigned n = out->attributes[i].numValues;
        vecNCopy(n,out->attributePtrs[i],src->attributePtrs[i]);
    }
}

void freeVaryings(Varyings* varyings) {
    if(!varyings) {
        return;
    }
    if(varyings->attributePtrs) {
        unsigned i;
        for(i=0;i<varyings->numAttributes;++i) {
            free(varyings->attributePtrs[i]);
        }
        free(varyings->attributePtrs);
    }
    free(varyings);
}

void interpolateBetween(Varyings* out,float factor,
                                            const Varyings* first,
                                            const Varyings* second) {
    vec4Interpolate(out->loc,factor,first->loc,second->loc);

    vec4Interpolate(out->color,factor,first->color,second->color);

    unsigned i;
    for(i=0;i<out->numAttributes;++i) {
        unsigned n = out->attributes[i].numValues;
        vecNInterpolate(n,out->attributePtrs[i],factor,
                          first->attributePtrs[i],
                          second->attributePtrs[i]);
    }
}

void interpolateAlongAxis(Varyings* out,Axis axis,int coord,
                                            const Varyings* first,
                                            const Varyings* second) {
    int startCoord = first->loc[axis],
        endCoord   = second->loc[axis];
    float factor;
    if(startCoord == endCoord) {
        factor = 1;
    } else {
        factor = (coord-startCoord)/(float)(endCoord-startCoord);
    }
    interpolateBetween(out,factor,first,second);
    out->loc[axis] = coord;
}

