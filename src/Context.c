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

void clearColorBuffer(Context* ct,const Color4 color) {
    unsigned i,j;
    Uint8* pixels = ct->surface->pixels;
    for(i=0;i<ct->_width*ct->_height;++i) {
        for(j=0;j<3;++j) {
            pixels[i*3+j] = 255*color[j]+0.5;
        }
    }
}

void clearDepthBuffer(Context* ct,float depth) {
    unsigned i;
    for(i=0;i<ct->_width*ct->_height;++i) {
        ct->_depth[i] = depth;
    }
}

void clearBuffers(Context* ct,const Color4 color,float depth) {
    clearColorBuffer(ct,color);
    clearDepthBuffer(ct,depth);
}

