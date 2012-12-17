#include "Shapes.h"
#include <math.h>
#include <stdio.h>

float mag2d(float x,float y) {
    return sqrt(x*x+y*y);
}

void fswap(float* a,float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void plotLine(FrameBuffer* buf,Vertex begin,Vertex end) {
    float x0 = begin.loc[0],y0 = begin.loc[1],z0 = begin.loc[2],
          x1 = end.loc[0],  y1 = end.loc[1],  z1 = end.loc[2];
    Color4 beginColor,endColor;
    vec4Copy(&beginColor,begin.color);
    vec4Copy(&endColor,end.color);

    int steep = (fabs(y1-y0) > fabs(x1-x0));
    if(steep) {
        fswap(&x0,&y0);
        fswap(&x1,&y1);
    }

    if(x0 > x1) {
        fswap(&x0,&x1);
        fswap(&y0,&y1);
        fswap(&z0,&z1);
        vec4Swap(&beginColor,&endColor);
    }

    if(x1 < 0 || (!steep && x0 >= buf->width) ||
                 (steep  && x0 >= buf->height)) {
        return;
    }
    
    float diffx = x1-x0,
          diffy = y1-y0,
          diffz = z1-z0;
    Color4 colorDiff;
    vec4Sub(&colorDiff,endColor,beginColor);
    float factor = 0,
          deltaFactor = 1/diffx;
    for(unsigned x=x0;x<=(unsigned)x1;++x) {
        float y = y0+diffy*factor,
              z = z0+diffz*factor;

        Vec4 loc;

        unsigned screenX = steep ? y : x,
                 screenY = steep ? x : y;

        loc[0] = screenX;
        loc[1] = screenY;
        loc[2] = z;

        Color4 color;
        if(buf->smoothShade) {
            Color4 colorChange;
            vec4Mult(&colorChange,colorDiff,factor);
            vec4Add(&color,beginColor,colorChange);
        } else {
            vec4Copy(&color,begin.color);
        }

        plotPoint(buf,loc,color);

        factor += deltaFactor;
    }
}

void plotLines(FrameBuffer* buf,unsigned n,const Vertex* verts) {
    for(unsigned i=0;i<n;++i) {
        plotLine(buf,verts[i*2],verts[i*2+1]);
    }
}
