#ifndef VERTEX_H
#define VERTEX_H

#include "Types.h"

typedef struct {
    unsigned numValues;
    unsigned step;
} VertexAttribute;

typedef struct {
    Vec3*    locs;
    unsigned locStep;

    Color3*  colors;
    unsigned colorStep;

    const VertexAttribute* attributes;
    unsigned numAttributes;
    float**  attributePtrs;
} VertexArray;

typedef struct {
    const Vec3*   loc;
    const Color3* color;

    const VertexAttribute* attributes;
    unsigned numAttributes;
    const float** attributePtrs;
} Vertex;

VertexArray* createVertArray(unsigned numAttributes,
                             const VertexAttribute* attributes);
void freeVertArray(VertexArray* arr);

Vertex* createVertex(VertexArray* arr);
void    freeVertex(Vertex* vert);

void vertAt(Vertex* out,VertexArray* arr,unsigned i);

#endif
