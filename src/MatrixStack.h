#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include "Types.h"

typedef struct {
    Mat44*   _stack;
    unsigned _stackSize;
    unsigned _stackTop;
} MatrixStack;

MatrixStack* createMatrixStack();
void freeMatrixStack(MatrixStack* stack);

void matStackPush(MatrixStack* stack);
void matStackPop(MatrixStack* stack);

Mat44* matStackTop(MatrixStack* stack);
void  matStackMult(MatrixStack* stack,Mat44 mat);

#endif
