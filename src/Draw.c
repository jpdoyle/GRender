#include "Draw.h"
#include "Raster.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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

typedef enum {
    _CLIP_ERR,
    _CLIP_NEITHER,
    _CLIP_FIRST,
    _CLIP_SECOND,
    _CLIP_BOTH
} _ClipCode;

_ClipCode _clipLine(Axis axis,const Varyings* a,const Varyings* b,
                           Varyings* outA,   Varyings* outB) {
    int sgn = axis == AXIS_Z ? -1 : 1;
    int aInside = ( sgn*a->loc[axis] <= a->loc[3] &&
                   -sgn*a->loc[axis] <= a->loc[3]),
        bInside = ( sgn*b->loc[axis] <= b->loc[3] &&
                   -sgn*b->loc[axis] <= b->loc[3]);
    if(aInside && bInside) {
        copyVaryings(outA,a);
        copyVaryings(outB,b);
        return _CLIP_NEITHER;
    }
    printf("Clipping axis %d...\n",axis);
    if(aInside || bInside) {
        const Varyings* fst = aInside ? a : b,
                      * snd = aInside ? b : a;
        float t1 = (fst->loc[3]+sgn*fst->loc[axis]) /* w > -x */
                   /(fst->loc[3]+sgn*fst->loc[axis]-snd->loc[3]-sgn*snd->loc[axis]),
              t2 = (fst->loc[3]-sgn*fst->loc[axis]) /* w > x */
                   /(sgn*snd->loc[axis]-snd->loc[3]+fst->loc[3]-sgn*fst->loc[axis]);
        if(t1 < 0 || t1 > 1 || (t1 > t2 && t2 > 0)) {
            t1 = t2;
        }
        interpolateBetween(aInside ? outB : outA,t1,fst,snd);
        copyVaryings(aInside ? outA : outB,fst);
        return aInside ? _CLIP_SECOND : _CLIP_FIRST;
    }
    printf("Clipping both...");
    float t1 = (a->loc[3]+sgn*a->loc[axis]) /* w > -x */
               /(a->loc[3]+sgn*a->loc[axis]-b->loc[3]-sgn*b->loc[axis]),
          t2 = (a->loc[3]-sgn*a->loc[axis]) /* w > x */
               /(sgn*b->loc[axis]-b->loc[3]+a->loc[3]-sgn*a->loc[axis]);
    if((t1 < 0 || t1 > 1) && (t2 < 0 || t2 > 1)) {
        printf("Beyond range err: %f,%f\n",t1,t2);
        return _CLIP_ERR;
    }
    if(t1 > t2) {
        float tmp = t2;
        t2 = t1;
        t1 = tmp;
    }
    interpolateBetween(outA,t1,a,b);
    interpolateBetween(outB,t2,a,b);
    if(outA->loc[3] > 1e-6 && outB->loc[3] > 1e-6) {
        printf("Interp with %f,%f\n",t1,t2);
        return _CLIP_BOTH;
    }
    return _CLIP_ERR;
}

// Have at least 2*num slots in out
int _clipVaryings(Axis axis,unsigned num,const Varyings ** in,Varyings** out) {
    const Varyings* a = NULL,
                  * b = in[num-1];
    Varyings* tmpA = createVaryings(b->numAttributes,b->attributes),
            * tmpB = createVaryings(b->numAttributes,b->attributes);
    Varyings** start = out;
    int i;
    for(i=0;i<num;++i) {
        a = b;
        b = in[i];
        _ClipCode code = _clipLine(axis,a,b,tmpA,tmpB);
        switch(code) {
        case _CLIP_ERR:
            printf("line clip error\n");
            continue;
            break;
        case _CLIP_NEITHER:
            copyVaryings(*out++,b);
            break;
        case _CLIP_FIRST:
            copyVaryings(*out++,tmpA);
            copyVaryings(*out++,b);
            break;
        case _CLIP_SECOND:
            copyVaryings(*out++,tmpB);
            break;
        case _CLIP_BOTH:
            copyVaryings(*out++,tmpA);
            copyVaryings(*out++,tmpB);
        }
    }
    freeVaryings(tmpA);
    freeVaryings(tmpB);
    return (out-start);
}

/* tmp and out should have 27 slots */
int _clipTriangle(const Varyings* a,const Varyings* b,const Varyings* c,Varyings** tmp,
                                                                        Varyings** out) {
    int n = 3;
    const Varyings * arr[] = { a,b,c };
    n = _clipVaryings(AXIS_X,n,arr,out);
    printf("X-clipped to %d\n",n);
    const Varyings** tmp2 = malloc(sizeof(const Varyings*)*n);
    int i;
    if(n) {
        for(i=0;i<n;++i) {
            tmp2[i] = out[i];
        }
        n = _clipVaryings(AXIS_Y,n,tmp2,tmp); 
        printf("Y-clipped to %d\n",n);
    }
    if(n) {
        free(tmp2);
        tmp2 = malloc(sizeof(const Varyings*)*n);
        for(i=0;i<n;++i) {
            tmp2[i] = tmp[i];
        }
        n = _clipVaryings(AXIS_Z,n,tmp2,out); 
        printf("Z-clipped to %d\n",n);
    }
    free(tmp2);
    return n;
}

void _viewportTransform(const Context* ct,const Varyings* in,Varyings* out) {
    copyVaryings(out,in);
    vec4Mult(out->loc,out->loc,(1/in->loc[3]));
    float viewportScaleX = ct->viewport.width/2.0,
          viewportScaleY = ct->viewport.height/2.0;
    float viewportShiftX = ct->viewport.x+viewportScaleX,
          viewportShiftY = ct->viewport.y+viewportScaleY;
    out->loc[0] = out->loc[0]
                   *viewportScaleX
                  +viewportShiftX;
    out->loc[1] = out->loc[1]
                   *viewportScaleY
                  +viewportShiftY;
    // PSYCH! The Y coordinate system is negated!
    out->loc[1] = ct->_height
                  -out->loc[1];
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

    unsigned shapeIndices[6];
    Vertex* vertex = createVertex(vertices);

    vertAt(vertex,vertices,0);
    Varyings* tmp1[27],
            * tmp2[27];
    for(i=0;i<27;++i) {
        tmp1[i] = createVaryings(vertex->numAttributes,vertex->attributes);
        tmp2[i] = createVaryings(vertex->numAttributes,vertex->attributes);
    }
    Varyings* tmp = createVaryings(vertex->numAttributes,vertex->attributes);
    
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
                Varyings* v = varyings[index];
                if(v->loc[0] <= v->loc[3] && -v->loc[0] <= v->loc[3] &&
                   v->loc[1] <= v->loc[3] && -v->loc[1] <= v->loc[3] &&
                   v->loc[2] <= v->loc[3] && -v->loc[2] <= v->loc[3]) {
                    _viewportTransform(ct,varyings[index],tmp);
                    rasterPoint(ct,tmp);
                }
            }
            break;
        case SHAPE_LINE: {
                printf("DRAWING LINE: w = [%f,%f]\n",varyings[shapeIndices[0]]->loc[3],
                                                     varyings[shapeIndices[1]]->loc[3]);
                _ClipCode status = _clipLine(AXIS_X,varyings[shapeIndices[0]],
                                                    varyings[shapeIndices[1]],
                                                    tmp1[0],tmp1[1]);
                if(status != _CLIP_ERR) {
                    status = _clipLine(AXIS_Y,tmp1[0],tmp1[1],
                                              tmp1[2],tmp1[3]);
                    if(status != _CLIP_ERR) {
                        status = _clipLine(AXIS_Z,tmp1[2],tmp1[3],
                                                  tmp1[0],tmp1[1]);
                        if(status != _CLIP_ERR) {
                            printf("Drawing line w = [%f,%f]\n",tmp1[0]->loc[3],
                                                                tmp1[1]->loc[3]);
                            _viewportTransform(ct,tmp1[0],tmp1[2]);
                            _viewportTransform(ct,tmp1[1],tmp1[3]);
                            rasterLine(ct,tmp1[2],tmp1[3]);
                        }
                    }
                }
                if(status == _CLIP_ERR) {
                    printf("Clip error\n");
                }
            }
            break;
        case SHAPE_TRIANGLE: {
                printf("DRAWING TRI\n");
                int n = _clipTriangle(varyings[shapeIndices[0]],varyings[shapeIndices[1]],
                                      varyings[shapeIndices[2]],tmp1,tmp2);
                printf("Clipped to %d verts\n",n);
                int k;
                for(k=0;k<n;++k) {
                    _viewportTransform(ct,tmp2[k],tmp1[k]);
                }

                int clip = (n < 3);
                
                if(!clip && ct->cullBackFace) {
                    const float* a = tmp1[0]->loc,
                               * b = tmp1[1]->loc,
                               * c = tmp1[2]->loc;
                    // Calculates double the 2D signed area of the
                    // triangle in order to find the winding. See
                    // <LINK HERE> for details.
                    float doubleSignedArea = (b[0]-a[0])*(c[1]-a[1])
                                             -(c[0]-a[0])*(b[1]-a[1]);
                    Winding winding = (doubleSignedArea > 0) ? WINDING_CW : 
                                                               WINDING_CCW;
                    clip = (winding != ct->frontFace);
                }

                if(!clip) {
                    printf("Drawing %d verts\n",n);
                    for(k=1;k<n-1;++k) {
                        rasterTriangle(ct,tmp1[0],tmp1[k],tmp1[k+1]);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    freeVertex(vertex);
    for(i=0;i<maxIndex;++i) {
        if(varyings[i]) {
            freeVaryings(varyings[i]);
        }
    }
    for(i=0;i<27;++i) {
        freeVaryings(tmp1[i]);
        freeVaryings(tmp2[i]);
    }
    freeVaryings(tmp);
    free(varyings);
}
