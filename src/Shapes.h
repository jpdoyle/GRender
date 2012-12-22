#ifndef SHAPES_H
#define SHAPES_H

#include "Types.h"
#include "FrameBuffer.h"

typedef struct {
    Vec3   loc;
    Color4 color;
} Vertex;

typedef Vertex Line[2];
typedef Vertex Triangle[3];

void plotPoint (FrameBuffer* buf,Vertex vert);
void plotPoints(FrameBuffer* buf,unsigned n,const Vertex* verts);

void drawLine (FrameBuffer* buf,Vertex begin,Vertex end);
void drawLines(FrameBuffer* buf,unsigned n,const Vertex* verts);

void drawSpansBetween(FrameBuffer* buf,const Line a,const Line b);

void drawTriangle (FrameBuffer* buf,Vertex a,Vertex b,Vertex c);
void drawTriangles(FrameBuffer* buf,unsigned n,const Vertex* verts);

#endif
