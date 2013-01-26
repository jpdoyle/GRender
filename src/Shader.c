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

void freeVaryings(Varyings* varyings) {
    if(!varyings) {
        return;
    }
    if(varyings->attributePtrs) {
        free(varyings->attributePtrs);
    }
    free(varyings);
}

void interpolateBetween(Varyings* out,float factor,
                                            const Varyings* first,
                                            const Varyings* second) {
    Vec4 diff;
    vec4Sub(diff,second->loc,first->loc);
    vec4Mult(diff,diff,factor);
    vec4Add(out->loc,first->loc,diff);

    Color4 colorDiff;
    vec4Sub(colorDiff,second->color,first->color);
    vec4Mult(colorDiff,colorDiff,factor);
    vec4Add(out->color,first->color,colorDiff);

    float buf[256];
    unsigned i;
    for(i=0;i<out->numAttributes;++i) {
        unsigned n = out->attributes[i].numValues;
        vecNSub(n,buf,second->attributePtrs[i],
                      first->attributePtrs[i]);
        vecNMult(n,buf,buf,factor);
        vecNAdd(n,out->attributePtrs[i],first->attributePtrs[i],buf);
    }
}

void interpolateAlongAxis(Varyings* out,Axis axis,int coord,
                                            const Varyings* first,
                                            const Varyings* second) {
    float startCoord = first->loc[axis],
          endCoord   = second->loc[axis];
    float factor;
    if(startCoord == endCoord) {
        factor = 1;
    } else {
        factor = (coord-startCoord)/(endCoord-startCoord);
    }
    interpolateBetween(out,factor,first,second);
    out->loc[axis] = coord;
}

