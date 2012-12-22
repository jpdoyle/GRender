#include "Shapes.h"
#include <math.h>

float mag2d(float x,float y) {
    return sqrt(x*x+y*y);
}

void fswap(float* a,float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void plotPoint(FrameBuffer* buf,const Vertex vert) {
    int x = vert.loc[0],
        y = vert.loc[1];
    if(x < 0 || x >= buf->width ||
       y < 0 || y >= buf->height) {
        return;
    }
    unsigned index = vert.loc[1]*buf->width+vert.loc[0];
    if(buf->depthEnabled && buf->depth[index] >= vert.loc[2]) {
        return;
    }
    vec4Copy(buf->pixels[index],vert.color);
}

void plotPoints(FrameBuffer* buf,unsigned n,const Vertex* verts) {
    unsigned i;
    for(i=0;i<n;++i) {
        plotPoint(buf,verts[i]);
    }
}

void drawLine(FrameBuffer* buf,const Vertex begin,const Vertex end) {
    float x0 = begin.loc[0],y0 = begin.loc[1],z0 = begin.loc[2],
          x1 = end.loc[0],  y1 = end.loc[1],  z1 = end.loc[2];
    Color4 beginColor,endColor;
    vec4Copy(beginColor,begin.color);
    vec4Copy(endColor,end.color);

    int steep = (fabs(y1-y0) > fabs(x1-x0));
    if(steep) {
        fswap(&x0,&y0);
        fswap(&x1,&y1);
    }

    if(x0 > x1) {
        fswap(&x0,&x1);
        fswap(&y0,&y1);
        fswap(&z0,&z1);
        vec4Swap(beginColor,endColor);
    }

    if(x1 < 0 || (!steep && x0 >= buf->width) ||
                 (steep  && x0 >= buf->height)) {
        return;
    }
    
    float diffx = x1-x0,
          diffy = y1-y0,
          diffz = z1-z0;
    Color4 colorDiff;
    vec4Sub(colorDiff,endColor,beginColor);
    float factor = 0,
          deltaFactor = 1/diffx;
    unsigned x;
    for(x=x0;x<=(unsigned)x1;++x) {
        float y = y0+diffy*factor,
              z = z0+diffz*factor;
        
        Vertex vert;

        unsigned screenX = steep ? y : x,
                 screenY = steep ? x : y;

        vert.loc[0] = screenX;
        vert.loc[1] = screenY;
        vert.loc[2] = z;

        if(buf->smoothShade) {
            Color4 colorChange;
            vec4Mult(colorChange,colorDiff,factor);
            vec4Add(vert.color,beginColor,colorChange);
        } else {
            vec4Copy(vert.color,begin.color);
        }

        plotPoint(buf,vert);

        factor += deltaFactor;
    }
}

void drawLines(FrameBuffer* buf,unsigned n,const Vertex* verts) {
    unsigned i;
    for(i=0;i<n;++i) {
        drawLine(buf,verts[i*2],verts[i*2+1]);
    }
}

void drawSpansBetween(FrameBuffer* buf,const Line a,const Line b) {
    const Vertex* edges[2][2] = { { &(a[0]),&(a[1]) },
                                  { &(b[0]),&(b[1]) } };
    const Vertex* tmp;
    if(edges[0][0]->loc[1] > edges[0][1]->loc[1]) {
        tmp      = edges[0][0];
        edges[0][0] = edges[0][1];
        edges[0][1] = tmp;
    }
    if(edges[1][0]->loc[1] > edges[1][1]->loc[1]) {
        tmp      = edges[1][0];
        edges[1][0] = edges[1][1];
        edges[1][1] = tmp;
    }
    unsigned ydiff[2] = { edges[0][1]->loc[1]-edges[0][0]->loc[1],
                          edges[1][1]->loc[1]-edges[1][0]->loc[1] };
    unsigned y0 = fmax(edges[0][0]->loc[1],edges[1][0]->loc[1]),
             y1 = fmin(edges[0][1]->loc[1],edges[1][1]->loc[1]);

    if(ydiff[0] == 0 || ydiff[1] == 0 || y0 > y1) {
        return;
    }

    Color4 colorDiff[2];
    float xdiff[2];
    float zdiff[2];
    float factor[2];
    float factorStep[2];

    unsigned i;
    for(i=0;i<2;++i) {
        vec4Sub(colorDiff[i],edges[i][1]->color,edges[i][0]->color);
        xdiff[i] = edges[i][1]->loc[0]-edges[i][0]->loc[0];
        zdiff[i] = edges[i][1]->loc[2]-edges[i][0]->loc[2];
        factorStep[i] = 1.0/ydiff[i];
        factor[i]  = (y0-edges[i][0]->loc[1])/ydiff[i];
    }

    unsigned y;
    for(y=y0;y<=y1;++y) {
        Vertex line[2];
        for(i=0;i<2;++i) {
            if(buf->smoothShade) {
                Color4 colorDelta;
                vec4Mult(colorDelta,colorDiff[i],factor[i]);
                vec4Add(line[i].color,edges[i][0]->color,colorDelta);
            } else {
                vec4Copy(line[i].color,edges[0][0]->color);
            }
            line[i].loc[0] = edges[i][0]->loc[0]+xdiff[i]*factor[i];
            line[i].loc[1] = y;
            line[i].loc[2] = edges[i][0]->loc[2]+zdiff[i]*factor[i];

            factor[i] += factorStep[i];
        }
        drawLine(buf,line[0],line[1]);
    }
}

void drawTriangle(FrameBuffer* buf,Vertex a,Vertex b,Vertex c) {
    Line edges[3] = { { a,b },
                      { b,c },
                      { a,c } };
    unsigned tallestIndex = 0;
    unsigned i;
    for(i=1;i<3;++i) {
        if(fabs(edges[i][0].loc[1]-edges[i][1].loc[1]) > 
           fabs(edges[tallestIndex][0].loc[1]-edges[tallestIndex][1].loc[1])) {
            tallestIndex = i;
        }
    }
    unsigned shorterIndex1 = (tallestIndex+1)%3,
             shorterIndex2 = (tallestIndex+2)%3;
    if(!buf->smoothShade) {
        vec4Copy(edges[tallestIndex][0].color,a.color);
    }
    drawSpansBetween(buf,edges[tallestIndex],edges[shorterIndex1]);
    drawSpansBetween(buf,edges[tallestIndex],edges[shorterIndex2]);
}

void drawTriangles(FrameBuffer* buf,unsigned n,const Vertex* verts) {
    unsigned i;
    for(i=0;i<n;++i) {
        drawTriangle(buf,verts[i*3],verts[i*3+1],verts[i*3+2]);
    }
}

