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
    Vec4 firstLoc,secondLoc;
    vec4Mult(firstLoc, first->loc, factor);
    vec4Mult(secondLoc,second->loc,1-factor);
    vec4Add(out->loc,firstLoc,secondLoc);

    Color4 firstColor,secondColor;
    vec4Mult(firstColor, first->color, factor);
    vec4Mult(secondColor,second->color,1-factor);
    vec4Add(out->color,firstColor,secondColor);

    float buf[256];
    unsigned i;
    for(i=0;i<out->numAttributes;++i) {
        unsigned n = out->attributes[i].numValues;
        vecNMult(n,out->attributePtrs[i],first->attributePtrs[i],
                                         factor);
        vecNMult(n,buf,second->attributePtrs[i],1-factor);
        vecNAdd(n,out->attributePtrs[i],out->attributePtrs[i],buf);
    }
}

