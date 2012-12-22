#include "FrameBuffer.h"
#include <stdlib.h>

FrameBuffer* createFrameBuffer(unsigned width,unsigned height) {
    FrameBuffer* ret = malloc(sizeof(FrameBuffer));
    ret->pixels = malloc(width*height*sizeof(Color4));
    if(!ret->pixels) {
        free(ret);
        return NULL;
    }
    ret->depth = malloc(width*height*sizeof(float));
    if(!ret->depth) {
        free(ret->pixels);
        free(ret);
        return NULL;
    }
    ret->surface = SDL_CreateRGBSurface(0,width,height,24,0,0,0,0);
    if(!ret->surface) {
        free(ret->depth);
        free(ret->pixels);
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
    if(buf->pixels) {
        free(buf->pixels);
    }
    if(buf->depth) {
        free(buf->depth);
    }
    if(buf) {
        free(buf);
    }
}

void refreshSurface(FrameBuffer* buf) {
    unsigned i,j;
    Uint8* pixels = buf->surface->pixels;
    for(i=0;i<buf->width*buf->height;++i) {
        for(j=0;j<3;++j) {
            pixels[i*3+j] = 255*buf->pixels[i][j];
        }
    }
}

void clearColorBuffer(FrameBuffer* buf,const Color4 color) {
    unsigned i;
    for(i=0;i<buf->width*buf->height;++i) {
        vec4Copy(buf->pixels[i],color);
    }
}

void clearDepthBuffer(FrameBuffer* buf,float depth) {
    unsigned i;
    for(i=0;i<buf->width*buf->height;++i) {
        buf->depth[i] = depth;
    }
}

void clearBuffers(FrameBuffer* buf,const Color4 color,float depth) {
    unsigned i;
    for(i=0;i<buf->width*buf->height;++i) {
        vec4Copy(buf->pixels[i],color);
        buf->depth[i] = depth;
    }
}

