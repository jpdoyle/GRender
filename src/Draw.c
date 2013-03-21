#include "Draw.h"
#include "Raster.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

unsigned vertsPerShape[SHAPE_COUNT] = {
    1, // SHAPE_POINT
    2, // SHAPE_LINE
    3  // SHAPE_TRIANGLE
};

void drawShape(Context* ct,Shape shape,unsigned num,
               VertexArray* vertices) {
    unsigned  vertCount = num*vertsPerShape[shape];
    unsigned* indices   = malloc(sizeof(unsigned)*vertCount);
    unsigned i;
    for(i=0;i<vertCount;++i) {
        indices[i] = i;
    }
    drawShapeIndexed(ct,shape,num,vertices,indices);
    free(indices);
}

void perspViewportTransform(Vec4 out,const Vec4 in,const Context* ct) {
    int bad = 0;
    int i;
    for(i=0;i<3;++i) {
        if(fabs(in[i]) > fabs(in[3])) {
            bad = 1;
        }
    }
    printf("bad:{ %f,%f,%f,%f }\n",in[0],in[1],in[2],in[3]);
    Vec4 result;
    vec4Mult(result,in,1/in[3]);
    // Viewport transformation
    float viewportScaleX = ct->viewport.width/2.0,
          viewportScaleY = ct->viewport.height/2.0;
    float viewportShiftX = ct->viewport.x+viewportScaleX,
          viewportShiftY = ct->viewport.y+viewportScaleY;
    result[0] = result[0]*viewportScaleX+viewportShiftX;
    result[1] = result[1]*viewportScaleY+viewportShiftY;
    // Since the Y coordinate system is negated
    result[1] = ct->_height-result[1];
    vec4Copy(out,result);
}

void fswap(float* a,float* b);

void clipLine(Axis axis,float* u1,float* u2,const Vec4 a,const Vec4 b) {
    float dAxis = b[axis]-a[axis],
          dw    = b[3]-a[3];
          // axis = -w
    float num1   = a[axis]+a[3],
          denom1 = a[axis]-b[axis]+a[3]-b[3],
          // axis = w
          num2   = a[axis]-a[3],
          denom2 = a[axis]-b[axis]+b[3]-a[3];
    if(dw/dAxis < -1) {
        *u1 = (dw > 0 ? 1 : -1)*INFINITY;
    } else if(denom1 == 0) {
        *u1 = INFINITY;
        if(num1 < 0) {
            *u1 = -*u1;
        }
    } else {
        *u1 = num1/denom1;
    }
    if(dw/dAxis > 1) {
        *u2 = (dw > 0 ? 1 : -1)*INFINITY;
    } else if(denom2 == 0) {
        *u2 = INFINITY;
        if(num2 < 0) {
            *u2 = -*u2;
        }
    } else {
        *u2 = num2/denom2;
    }
    if(dAxis < 0) {
        fswap(u1,u2);
    }
    //printf("clipped to %f,%f\n",*u1,*u2);
}

// need at least 15 Varyings* in out
void clipTriangle(Varyings** out,int* n,Varyings const** in) {
    const Varyings *edges[3][2] = { { in[0],in[1] },
                                    { in[1],in[2] },
                                    { in[2],in[0] } };
    *n = 0;
    int i,j;
    for(i=0;i<3;++i) {
        float u1,u2;
        clipLine(AXIS_Y,&u1,&u2,edges[i][0]->loc,edges[i][1]->loc);
        if(u1 < 1 && u2 > 0) {
            if(u1 < 0) {
                u1 = 0;
            }
            if(u2 > 1) {
                u2 = 1;
            }
            interpolateBetween(out[(*n)++],u1,edges[i][0],edges[i][1]);
            interpolateBetween(out[(*n)++],u2,edges[i][0],edges[i][1]);
        }
    }
    int startIndex = *n+1;
    int end = *n/2;
    for(i=0;i<end;++i) {
        float u1[2],u2[2];
        clipLine(AXIS_X,&u1[0],&u2[0],out[i*2]->loc,out[i*2+1]->loc);
        clipLine(AXIS_Z,&u1[1],&u2[1],out[i*2]->loc,out[i*2+1]->loc);
        float start = fmax(0,fmax(u1[0],u1[1])),
              end   = fmin(1,fmin(u1[0],u1[1]));
        if(start >= end) {
            *n = 0;
            return;
        }
        if(start > 0 && i == 0) {
            n++;
        }
        interpolateBetween(out[(*n)++],start,out[i*2],out[i*2+1]);
        interpolateBetween(out[(*n)++],end,out[i*2],out[i*2+1]);
        if(end < 1 && i == end-1) {
            interpolateBetween(out[startIndex],end,out[i*2],out[i*2+1]);
        }
    }
    *n -= startIndex-1;
    for(i=0;i<*n;++i) {
        out[i] = out[startIndex+i];
    }
}

void drawShapeIndexed(Context* ct,Shape shape,unsigned num,
                      VertexArray* vertices,unsigned* indices) {
    mat44Mult(ct->uniforms->modelViewProjection,
              matStackTop(ct->matrices[MATRIX_PROJECTION]),
              matStackTop(ct->matrices[MATRIX_MODELVIEW]));

    unsigned i,j;
    unsigned indexCount = num*vertsPerShape[shape];
    unsigned maxIndex;
    for(i=0;i<indexCount;++i) {
        if(i==0 || indices[i] > maxIndex) {
            maxIndex = indices[i];
        }
    }

    // Vertices are processed lazily, because it's rather expensive
    // to process each one
    Varyings** varyings   = malloc(sizeof(Varyings*)*(1+maxIndex));
    for(i=0;i<=maxIndex;++i) {
        varyings[i] = NULL;
    }

    Varyings* tmpVaryings[20];
    for(i=0;i<20;++i) {
        tmpVaryings[i] = createVaryings(vertices->numAttributes,
                                        vertices->attributes);
    }

    unsigned shapeIndices[6];
    Vertex* vertex = createVertex(vertices);
    
    for(i=0;i<num;++i) {
        for(j=0;j<vertsPerShape[shape];++j) {
            unsigned index = indices[i*vertsPerShape[shape]+j];
            shapeIndices[j] = index;
           
            // Doesn't process vertices until they're used
            if(!varyings[index]) {
                vertAt(vertex,vertices,index);
                varyings[index] = createVaryings(vertex->numAttributes,
                                                 vertex->attributes);

                ct->vertShader(ct->uniforms,vertex,varyings[index]);
            }
        }
        switch(shape) {
        case SHAPE_POINT: {
                unsigned index = shapeIndices[0];
                float w = varyings[index]->loc[3];
                if(w > 0) {
                    int draw = 1;
                    int j;
                    for(j=0;j<3;++j) {
                        if(fabs(varyings[index]->loc[j]) > fabs(w)) {
                            draw = 0;
                            break;
                        }
                    }
                    if(draw) {
                        copyVaryings(tmpVaryings[0],varyings[index]);
                        perspViewportTransform(tmpVaryings[0]->loc,
                                               varyings[index]->loc,
                                               ct);
                        rasterPoint(ct,tmpVaryings[0]);
                    }
                }
            }
            break;
        case SHAPE_LINE: {
                Varyings* verts[] = { varyings[shapeIndices[0]],
                                      varyings[shapeIndices[1]] };
                float startFactor = 0,endFactor = 1;
                int j;
                for(j=0;j<3;++j) {
                    float u1,u2;
                    clipLine(j,&u1,&u2,verts[0]->loc,verts[1]->loc);
                    printf("u = [%f,%f]\n",u1,u2);
                    if(u1 > startFactor) {
                        startFactor = u1;
                    }
                    if(u2 < endFactor) {
                        endFactor = u2;
                    }
                }
                printf("clipped to %f,%f\n",startFactor,endFactor);
                if(startFactor < endFactor) {
                    printf("Start = { %f,%f,%f,%f }\n",
                                verts[0]->loc[0],verts[0]->loc[1],
                                verts[0]->loc[2],verts[0]->loc[3]);
                    printf("End = { %f,%f,%f,%f }\n",
                                verts[1]->loc[0],verts[1]->loc[1],
                                verts[1]->loc[2],verts[1]->loc[3]);
                    interpolateBetween(tmpVaryings[0],startFactor,verts[0],verts[1]);
                    perspViewportTransform(tmpVaryings[0]->loc,
                                           tmpVaryings[0]->loc,ct);
                    interpolateBetween(tmpVaryings[1],endFactor,verts[0],verts[1]);
                    perspViewportTransform(tmpVaryings[1]->loc,
                                           tmpVaryings[1]->loc,ct);
                    printf("z = %f,%f\n",tmpVaryings[0]->loc[2],
                                         tmpVaryings[1]->loc[2]);
                    rasterLine(ct,tmpVaryings[0],tmpVaryings[1]);
                }
            }
            break;
        case SHAPE_TRIANGLE: {
                break;
                const Varyings* verts[] = { varyings[shapeIndices[0]],
                                      varyings[shapeIndices[1]],
                                      varyings[shapeIndices[2]] };
                int clip = 0;
                if(ct->cullBackFace) {
                    const float* a = verts[0]->loc,
                               * b = verts[1]->loc,
                               * c = verts[2]->loc;
                    // Calculates double the 2D signed area of the
                    // triangle in order to find the winding. See
                    // <LINK HERE> for details.
                    float doubleSignedArea = (b[0]-a[0])*(c[1]-a[1])-(c[0]-a[0])*(b[1]-a[1]);
                    Winding winding = (doubleSignedArea > 0) ? WINDING_CCW : 
                                                               WINDING_CW;
                    clip = (winding != ct->frontFace);
                }

                if(!clip) {
                    int n;
                    clipTriangle(tmpVaryings,&n,verts);
                    int j;
                    for(j=0;j<n;++j) {
                        perspViewportTransform(tmpVaryings[j]->loc,
                                               tmpVaryings[j]->loc,ct);
                    }
                    for(j=0;j<n-2;++j) {
                        rasterTriangle(ct,tmpVaryings[0],
                                          tmpVaryings[j],
                                          tmpVaryings[j+1]);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    for(i=0;i<6;++i) {
        freeVaryings(tmpVaryings[i]);
    }
    freeVertex(vertex);
    for(i=0;i<maxIndex;++i) {
        if(varyings[i]) {
            freeVaryings(varyings[i]);
        }
    }
    free(varyings);
}
