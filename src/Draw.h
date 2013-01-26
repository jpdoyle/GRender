#ifndef DRAW_H
#define DRAW_H

#include "Context.h"
#include "Vertex.h"

typedef enum {
    SHAPE_POINT,
    SHAPE_LINE,
    SHAPE_TRIANGLE,
    SHAPE_COUNT
} Shape;

void drawShape       (Context* ct,Shape shape,unsigned num,
                      VertexArray* vertices);
void drawShapeIndexed(Context* ct,Shape shape,unsigned num,
                      VertexArray* vertices,unsigned* indices);

#endif
