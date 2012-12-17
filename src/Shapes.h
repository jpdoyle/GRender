#ifndef SHAPES_H
#define SHAPES_H

#include "Types.h"
#include "FrameBuffer.h"

typedef struct {
    Vec4   loc;
    Color4 color;
} Vertex;

void plotLine     (FrameBuffer* buf,Vertex begin,Vertex end);
void plotLines    (FrameBuffer* buf,unsigned n,const Vertex* verts);

#endif
