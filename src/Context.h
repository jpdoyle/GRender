#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL/SDL.h>
#include "Types.h"
#include "Shader.h"
#include "MatrixStack.h"

enum {
    MATRIX_MODELVIEW,
    MATRIX_PROJECTION,
    MATRIX_COUNT
};

enum {
    FACE_NONE  = 0,
    FACE_FRONT = 1<<0,
    FACE_BACK  = 1<<1
};

typedef enum {
    WINDING_CCW,
    WINDING_CW
} Winding;

typedef struct {
    unsigned _width,_height;
    float*   _depth;

    SDL_Surface*   surface;
    MatrixStack*   matrices[MATRIX_COUNT];

    Uniforms*      uniforms;
    VertexShader   vertShader;
    FragmentShader fragShader;

    struct {
        unsigned x,y,width,height;
    } viewport;

    int depthEnabled;

    unsigned cullBackFace;
    Winding  frontFace;
} Context;

Context* createContext(unsigned width,unsigned height);
void freeContext(Context* ct);

void clearColorBuffer(Context* ct,const Color4 color);
void clearDepthBuffer(Context* ct,float depth);
void clearBuffers(Context* ct,const Color4 color,float depth);

#endif
