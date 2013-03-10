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
    Vec4 firstTmp,secondTmp;
    vec4Mult(firstTmp,first->loc,1-factor);
    vec4Mult(secondTmp,second->loc,factor);
    vec4Add(out->loc,firstTmp,secondTmp);

    Color4 colorTmp1,colorTmp2;
    vec4Mult(colorTmp1,first->color,1-factor);
    vec4Mult(colorTmp2,second->color,factor);
    vec4Add(out->color,colorTmp1,colorTmp2);

    float buf1[256],buf2[256];
    unsigned i;
    for(i=0;i<out->numAttributes;++i) {
        unsigned n = out->attributes[i].numValues;
        vecNMult(n,buf1,first->attributePtrs[i],1-factor);
        vecNMult(n,buf2,second->attributePtrs[i],factor);
        vecNAdd(n,out->attributePtrs[i],buf1,buf2);
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

