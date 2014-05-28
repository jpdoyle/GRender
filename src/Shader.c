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

#define mapUnVaryings(fn,out,vec,...) do { \
        Varyings* o__ = (out);    \
        const Varyings* v__ = (vec); \
        vec4##fn(o__->loc,v__->loc,##__VA_ARGS__); \
        vec3##fn(o__->color,v__->color,##__VA_ARGS__); \
        unsigned i; \
        for(i=0;i<o__->numAttributes;++i) { \
            unsigned n = o__->attributes[i].numValues; \
            vecN##fn(n,o__->attributePtrs[i], \
                       v__->attributePtrs[i], \
                       ##__VA_ARGS__); \
        } \
    } while(0)

#define mapBinVaryings(fn,out,first,second,...) do { \
        Varyings* o__ = (out);    \
        const Varyings* a__ = (first),  \
                      * b__ = (second); \
        vec4##fn(o__->loc,##__VA_ARGS__,a__->loc,b__->loc); \
        vec3##fn(o__->color,##__VA_ARGS__,a__->color,b__->color); \
        unsigned i; \
        for(i=0;i<o__->numAttributes;++i) { \
            unsigned n = o__->attributes[i].numValues; \
            (vecN##fn)(n,o__->attributePtrs[i], \
                       ##__VA_ARGS__, \
                       a__->attributePtrs[i], \
                       b__->attributePtrs[i]); \
        } \
    } while(0)

void addVaryings(Varyings* out,const Varyings* a,const Varyings* b) {
    mapBinVaryings(Add,out,a,b);
}
void subVaryings(Varyings* out,const Varyings* a,const Varyings* b) {
    mapBinVaryings(Sub,out,a,b);
}
void multVaryings(Varyings* out,const Varyings* v,float s) {
    mapUnVaryings(Mult,out,v,s);
}
void interpolateBetween(Varyings* out,float factor,
                                      const Varyings* first,
                                      const Varyings* second) {
    mapBinVaryings(Interpolate,out,first,second,factor);
}

float axisInterpStep(Axis axis,int firstCoord,
                               const Varyings* first,
                               const Varyings* second) {
    int startCoord = first->loc[axis],
        endCoord   = second->loc[axis];
    float factor;
    if(startCoord == endCoord) {
        return 1;
    }
    return (firstCoord-startCoord)/(float)(endCoord-startCoord);
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

