#include "Context.h"
#include <stdlib.h>

Context* createContext(unsigned width,unsigned height) {
    Context* ret = malloc(sizeof(Context));

    ret->_depth = malloc(width*height*sizeof(float));
    if(!ret->_depth) {
        free(ret);
        return NULL;
    }
    ret->surface = SDL_CreateRGBSurface(0,width,height,24,0,0,0,0);
    if(!ret->surface) {
        free(ret->_depth);
        free(ret);
        return NULL;
    }

    ret->_width       = width;
    ret->_height      = height;
    ret->depthEnabled = 0;

    ret->viewport.x      = 0;
    ret->viewport.y      = 0;
    ret->viewport.width  = ret->_width;
    ret->viewport.height = ret->_height;

    ret->uniforms = createUniforms(0,NULL);

    unsigned i;
    for(i=0;i<MATRIX_COUNT;++i) {
        ret->matrices[i] = createMatrixStack();
    }

    return ret;
}

void freeContext(Context* ct) {
    if(!ct) {
        return;
    }
    if(ct->surface) {
        SDL_FreeSurface(ct->surface);
    }
    if(ct->_depth) {
        free(ct->_depth);
    }
    if(ct->uniforms) {
        freeUniforms(ct->uniforms);
    }
    unsigned i;
    for(i=0;i<MATRIX_COUNT;++i) {
        freeMatrixStack(ct->matrices[i]);
    }
    free(ct);
}

void clearColorBuffer(Context* ct) {
    Uint8* pixels = ct->surface->pixels;
    size_t n = 3*(ct->_width*ct->_height);
    memset(pixels,0,n);
}

void clearDepthBuffer(Context* ct) {
    float* begin = ct->_depth;
    size_t n = ct->_width*ct->_height;
    memset(begin,0,n);
}

void clearBuffers(Context* ct) {
    clearColorBuffer(ct);
    clearDepthBuffer(ct);
}

void logMemFill(void* src,size_t srcLen,void* dst,size_t numRepeats) {
    memcpy(dst,src,srcLen);
    size_t n = 1;
    while(n < numRepeats) {
        size_t len = n;
        if(n+len > numRepeats) {
            len = numRepeats-n;
        }
        memcpy(dst+(n*srcLen),dst,len*srcLen);
        n += len;
    }
}

void fillColorBuffer(Context* ct,const Color4 color) {
    unsigned i,j;
    Uint8* pixels = ct->surface->pixels;
    Uint8* end    = pixels + 3*(ct->_width*ct->_height);
    Uint8 pixel[4];
    for(i=0;i<3;++i) {
        pixel[i] = 255*color[i]+0.5;
    }
    logMemFill(pixel,3*sizeof(Uint8),pixels,ct->_width*ct->_height);
    /* for(;pixels!=end;pixels += 3) { */
    /*     memcpy(pixels,pixel,3); */
    /* } */
}

void fillDepthBuffer(Context* ct,float depth) {
    logMemFill(&depth,sizeof(float),ct->_depth,
               ct->_width*ct->_height);
    /* float* begin = ct->_depth, */
    /*      * end   = begin + (ct->_width*ct->_height); */
    /* for(;begin!=end;++begin) { */
    /*     *begin = depth; */
    /* } */
}

void fillBuffers(Context* ct,const Color4 color,float depth) {
    fillColorBuffer(ct,color);
    fillDepthBuffer(ct,depth);
}

