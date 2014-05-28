#ifndef SHADER_H
#define SHADER_H

#include "Vertex.h"

typedef struct {
    unsigned numElements;
} UniformType;

typedef struct {
    Mat44    modelViewProjection;

    const UniformType* customTypes;
    float**  custom;
    unsigned numCustom;
} Uniforms;

typedef struct {
    Vec4   loc;
    Color3 color;

    const VertexAttribute* attributes;
    unsigned numAttributes;
    float**  attributePtrs;
} Varyings;

typedef void (*VertexShader)  (const Uniforms*,
                               Vertex*,
                               Varyings*);
typedef void (*FragmentShader)(const Uniforms*,
                               const Varyings*,
                               Color3);

Uniforms* createUniforms(unsigned numCustom,
                         const UniformType* customTypes);
void freeUniforms(Uniforms* uniforms);

Varyings* createVaryings(unsigned numAttributes,
                         const VertexAttribute* attributes);
void copyVaryings(Varyings* out,const Varyings* src);
void freeVaryings(Varyings* varyings);

void addVaryings(Varyings* out,const Varyings* a,const Varyings* b);
void subVaryings(Varyings* out,const Varyings* a,const Varyings* b);
void multVaryings(Varyings* out,const Varyings* v,float s);

void interpolateBetween(Varyings* out,float factor,
                                      const Varyings* first,
                                      const Varyings* second);

typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_Z
} Axis;

float axisInterpStep(Axis axis,int firstCoord,
                               const Varyings* first,
                               const Varyings* second);

void interpolateAlongAxis(Varyings* out,Axis axis,int coord,
                                            const Varyings* first,
                                            const Varyings* second);

#endif
