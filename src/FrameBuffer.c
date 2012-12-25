#include "FrameBuffer.h"
#include <stdlib.h>

FrameBuffer* createFrameBuffer(unsigned width,unsigned height) {
    FrameBuffer* ret = malloc(sizeof(FrameBuffer));

    ret->depth = malloc(width*height*sizeof(float));
    if(!ret->depth) {
        free(ret);
        return NULL;
    }
    ret->surface = SDL_CreateRGBSurface(0,width,height,24,0,0,0,0);
    if(!ret->surface) {
        free(ret->depth);
        free(ret);
        return NULL;
    }

    ret->width        = width;
    ret->height       = height;
    ret->depthEnabled = 0;
    ret->smoothShade  = 1;
    ret->fillShapes   = 1;

    return ret;
}

void freeFrameBuffer(FrameBuffer* buf) {
    if(!buf) {
        return;
    }
    if(buf->surface) {
        SDL_FreeSurface(buf->surface);
    }
    if(buf->depth) {
        free(buf->depth);
    }
    if(buf) {
        free(buf);
    }
}

void clearColorBuffer(FrameBuffer* buf,const Color4 color) {
    unsigned i,j;
    Uint8* pixels = buf->surface->pixels;
    for(i=0;i<buf->width*buf->height;++i) {
        for(j=0;j<3;++j) {
            pixels[i*3+j] = 255*color[j]+0.5;
        }
    }
}

void clearDepthBuffer(FrameBuffer* buf,float depth) {
    unsigned i;
    for(i=0;i<buf->width*buf->height;++i) {
        buf->depth[i] = depth;
    }
}

void clearBuffers(FrameBuffer* buf,const Color4 color,float depth) {
    clearColorBuffer(buf,color);
    clearDepthBuffer(buf,depth);
}

