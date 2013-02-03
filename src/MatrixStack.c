#include "MatrixStack.h"
#include <stdlib.h>

MatrixStack* createMatrixStack() {
    MatrixStack* ret = malloc(sizeof(MatrixStack));
    ret->_stack = malloc(8*sizeof(Mat44));
    ret->_stackSize = 8;
    ret->_stackTop = 0;
    
    return ret;
}

void freeMatrixStack(MatrixStack* stack) {
    free(stack->_stack);
}

void matStackPush(MatrixStack* stack) {
    ++stack->_stackTop;
    if(stack->_stackTop >= stack->_stackSize) {
        stack->_stackSize *= 2;
        stack->_stack = realloc(stack->_stack,
                                stack->_stackSize*sizeof(MatrixStack));
    }
    mat44Copy(stack->_stack[stack->_stackTop],
              stack->_stack[stack->_stackTop-1]);
}

void matStackPop(MatrixStack* stack) {
    if(stack->_stackTop) {
        --stack->_stackTop;
    }
}

float* matStackTop(MatrixStack* stack) {
    return stack->_stack[stack->_stackTop];
}

void matStackMult(MatrixStack* stack,Mat44 mult) {
    float* out = matStackTop(stack);
    mat44Mult(out,out,mult);
}

