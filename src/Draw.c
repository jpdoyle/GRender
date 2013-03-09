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
    // For clipping
    int*       isOnScreen = malloc(sizeof(int)*(1+maxIndex));
    for(i=0;i<=maxIndex;++i) {
        varyings[i] = NULL;
        isOnScreen[i] = 0;
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

                // Perspective division
                vec3Mult(varyings[index]->loc,
                         varyings[index]->loc,
                         1/varyings[index]->loc[3]);

                isOnScreen[index] = 
                                (fabs(varyings[index]->loc[0]) < 1 &&
                                 fabs(varyings[index]->loc[1]) < 1 &&
                                 fabs(varyings[index]->loc[2]) < 1);
                
                // Viewport transformation
                float viewportScaleX = ct->viewport.width/2.0,
                      viewportScaleY = ct->viewport.height/2.0;
                float viewportShiftX = ct->viewport.x+viewportScaleX,
                      viewportShiftY = ct->viewport.y+viewportScaleY;
                varyings[index]->loc[0] = varyings[index]->loc[0]
                                           *viewportScaleX
                                          +viewportShiftX;
                varyings[index]->loc[1] = varyings[index]->loc[1]
                                           *viewportScaleY
                                          +viewportShiftY;
                // PSYCH! The Y coordinate system is negated!
                varyings[index]->loc[1] = ct->_height
                                          -varyings[index]->loc[1];
            }
        }
        switch(shape) {
        case SHAPE_POINT: {
                unsigned index = shapeIndices[0];
                int clip = !isOnScreen[index];
                if(!clip) {
                    rasterPoint(ct,varyings[index]);
                }
            }
            break;
        case SHAPE_LINE: {
                int clip = !(isOnScreen[shapeIndices[0]] ||
                             isOnScreen[shapeIndices[1]]);
                if(!clip) {
                    rasterLine(ct,varyings[shapeIndices[0]],
                                  varyings[shapeIndices[1]]);
                }
            }
            break;
        case SHAPE_TRIANGLE: {
                int clip = !(isOnScreen[shapeIndices[0]] ||
                             isOnScreen[shapeIndices[1]] ||
                             isOnScreen[shapeIndices[2]]);
                
                if(!clip && ct->cullBackFace) {
                    const float* a = varyings[shapeIndices[0]]->loc,
                               * b = varyings[shapeIndices[1]]->loc,
                               * c = varyings[shapeIndices[2]]->loc;
                    // Calculates double the 2D signed area of the
                    // triangle in order to find the winding. See
                    // <LINK HERE> for details.
                    float doubleSignedArea = (b[0]-a[0])*(c[1]-a[1])-(c[0]-a[0])*(b[1]-a[1]);
                    Winding winding = (doubleSignedArea > 0) ? WINDING_CW : 
                                                               WINDING_CCW;
                    clip = (winding != ct->frontFace);
                }

                if(!clip) {
                    rasterTriangle(ct,varyings[shapeIndices[0]],
                                      varyings[shapeIndices[1]],
                                      varyings[shapeIndices[2]]);
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
    free(varyings);
    free(isOnScreen);
}
