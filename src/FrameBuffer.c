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
    Uint8* pixels = buf->surface->pixels;
    for(unsigned i=0;i<buf->width*buf->height;++i) {
        for(unsigned j=0;j<3;++j) {
            pixels[i*3+j] = 255*buf->pixels[i][j];
        }
    }
}

void clearColorBuffer(FrameBuffer* buf,const Color4 color) {
    for(unsigned i=0;i<buf->width*buf->height;++i) {
        vec4Copy(&(buf->pixels[i]),color);
    }
}

void clearDepthBuffer(FrameBuffer* buf,float depth) {
    for(unsigned i=0;i<buf->width*buf->height;++i) {
        buf->depth[i] = depth;
    }
}

void clearBuffers(FrameBuffer* buf,const Color4 color,float depth) {
    for(unsigned i=0;i<buf->width*buf->height;++i) {
        vec4Copy(&(buf->pixels[i]),color);
        buf->depth[i] = depth;
    }
}

void plotPoint(FrameBuffer* buf,const Vec4 point,const Color4 color) {
    unsigned index = point[1]*buf->width+point[0];
    if(buf->depthEnabled && buf->depth[index] >= point[2]) {
        return;
    }
    vec4Copy(&(buf->pixels[index]),color);
}

void plotPoints(FrameBuffer* buf,unsigned n,unsigned step,
                const Vec4* points,const Color4* colors) {
    unsigned i;
    for(i=0;i<n;++i,points = ((void*)points+step),
                    colors = ((void*)colors+step)) {
        plotPoint(buf,*points,*colors);
    }
}
