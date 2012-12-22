#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <SDL/SDL.h>
#include "Types.h"

typedef struct {
    SDL_Surface* surface;
    unsigned width,height;
    Color4* pixels;
    float* depth;

    int depthEnabled;
    int smoothShade;
    int fillShapes;
} FrameBuffer;

FrameBuffer* createFrameBuffer(unsigned width,unsigned height);
void freeFrameBuffer(FrameBuffer* buf);

void refreshSurface(FrameBuffer* buf);

void clearColorBuffer(FrameBuffer* buf,const Color4 color);
void clearDepthBuffer(FrameBuffer* buf,float depth);
void clearBuffers(FrameBuffer* buf,const Color4 color,float depth);

#endif
