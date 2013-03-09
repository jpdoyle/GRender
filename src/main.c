#include <SDL/SDL.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "Context.h"
#include "Draw.h"
#include "Vertex.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

#define FRAMES_PER_SECOND 40
#define SECONDS_PER_FRAME (1.0/FRAMES_PER_SECOND)

#define FOVY 70

#define ROTATION_SPEED 45
#define DEGREES_PER_PIXEL (1.0/20)

#define MOVEMENT_SPEED 4
#define TETRA_HORIZ_ROTATION_SPEED 45
#define TETRA_VERT_ROTATION_SPEED  51
#define TETRA_BOUNCE_RATE 2

#define TITLE "Pipeline Test"

void shadeVertex(const Uniforms* uniforms,
                 Vertex* vertex,
                 Varyings* varyings) {
    mat44MultVec3(varyings->loc,uniforms->modelViewProjection,
                                *vertex->loc);
    vec3Copy(varyings->color,*vertex->color);
}

void shadeFragment(const Uniforms* uniforms,
                   const Varyings* varyings,
                   Color3 out) {
    out[0] = varyings->color[0];
    out[1] = varyings->color[1];
    out[2] = varyings->color[2];
}

typedef struct {
    Vec3 loc;
    Color3 color;
} CustomVert;

CustomVert verts[] = { { { -1.73,-1.5,-1 }, { 1, 0, 0 } },
                       { {  1.73,-1.5,-1 }, { 0, 1, 0 } },
                       { {  0,   -1.5, 2 }, { 0, 0, 1 } },
                       { {  0,    1.5, 0 }, { 0, 1, 1 } } };

// draws the tetrahedron
unsigned indices[] = { 0,1,2,
                       1,0,3,
                       0,2,3,
                       2,1,3 };

void makeGrid(CustomVert** vertsOut,unsigned** indicesOut,unsigned* numEdgesOut,
              float x0,float z0,float x1,float z1,
              unsigned gridWidth, unsigned gridHeight) {
    float xStep = (x1-x0)/gridWidth,
          zStep = (z1-z0)/gridHeight;

    gridWidth  += 1;
    gridHeight += 1;

    unsigned vertexCount = 2*gridWidth+2*gridHeight;
    CustomVert* verts = malloc(sizeof(CustomVert)*vertexCount);

    Color3 white = { 1,1,1 };
    unsigned vertIndex = 0;
    float xLoc = x0,
          zLoc = z0;
    for(vertIndex=0;vertIndex<vertexCount;++vertIndex) {
        CustomVert* vert = verts+vertIndex;
        vert->loc[0] = xLoc;
        vert->loc[1] = 0;
        vert->loc[2] = zLoc;
        vec3Copy(vert->color,white);
        if(vertIndex < gridWidth) {
            xLoc += xStep;
        } else if(vertIndex >= gridWidth && 
                  vertIndex < gridWidth+gridHeight) {
            zLoc += zStep;
        } else if(vertIndex >= gridWidth+gridHeight &&
                  vertIndex <  gridWidth*2+gridHeight) {
            xLoc -= xStep;
        } else {
            zLoc -= zStep;
        }
    }

    unsigned  x,z;
    unsigned  numEdges = (gridWidth+1)+(gridHeight+1);
    unsigned* indices = malloc(sizeof(unsigned)*numEdges*2);
    unsigned  index = 0;
    for(x=0;x<=gridWidth;++x) {
        indices[index++] = x;
        indices[index++] = gridWidth*2+gridHeight-x;
    }
    for(z=0;z<=gridHeight;++z) {
        indices[index++] = gridWidth+z;
        if(z == 0) {
            indices[index++] = 0;
        } else {
            indices[index++] = gridWidth*2+gridHeight*2-z;
        }
    }

    *vertsOut    = verts;
    *indicesOut  = indices;
    *numEdgesOut = numEdges;
}

int main(void) {
    unsigned ticksPerFrame = 1000/FRAMES_PER_SECOND;
    SDL_Surface* screen;
    Context* ct;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr,"SDL init failed");
        return -1;
    }
    if(!(screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,
                                   SCREEN_BPP,SDL_SWSURFACE))) {
        fprintf(stderr,"SDL video init failed");
        return -1;
    }
    if(!(ct = createContext(SCREEN_WIDTH,SCREEN_HEIGHT))) {
        fprintf(stderr,"Context init failed");
        return -1;
    }

    ct->depthEnabled = 1;
    ct->cullBackFace = 1;
    ct->frontFace    = WINDING_CCW;

    ct->vertShader = &shadeVertex;
    ct->fragShader = &shadeFragment;

    mat44Perspective(matStackTop(ct->matrices[MATRIX_PROJECTION]),
                     FOVY,SCREEN_WIDTH/(float)SCREEN_HEIGHT,1,30);

    printf("Projection:\n");
    mat44Print(stdout,matStackTop(ct->matrices[MATRIX_PROJECTION]));

    mat44Ident(matStackTop(ct->matrices[MATRIX_MODELVIEW]));

    Mat44 translate;
    mat44Translate(translate,0,0,-4);

    Mat44 rotateVert;
    mat44Ident(rotateVert);

    Mat44 tetraTransform;
    mat44Ident(tetraTransform);

    float tetraHorizRot = 0,
          tetraVertRot  = 0;

    VertexArray* varr = createVertArray(0,NULL);
    varr->locs      = &verts[0].loc;
    varr->locStep   = sizeof(CustomVert);
    varr->colors    = &verts[0].color;
    varr->colorStep = sizeof(CustomVert);

    Color4 black = { 0,0,0,0 };

    CustomVert* gridVerts;
    unsigned*   gridIndices;
    unsigned    gridNumEdges;
    makeGrid(&gridVerts,&gridIndices,&gridNumEdges,
             -6,-6,6,6,12,12);

    VertexArray* gridArr = createVertArray(0,NULL);

    gridArr->locs      = &gridVerts[0].loc;
    gridArr->locStep   = sizeof(CustomVert);
    gridArr->colors    = &gridVerts[0].color;
    gridArr->colorStep = sizeof(CustomVert);

    Mat44 gridTransform;
    mat44Translate(gridTransform,0,-5,0);

    SDL_WM_SetCaption(TITLE,NULL);

    char title[256];
    float fps = 0;
    float fpsLerp = 0.2;

    int numViewports = 1;

    int mouseCaptured = 0;
    int running = 1;
    int debugged = 0;
    while(running) {
        Uint32 startTicks = SDL_GetTicks();

        int vertRot  = 0,
            horizRot = 0;

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        mouseCaptured = !mouseCaptured;
                        SDL_ShowCursor(mouseCaptured ? SDL_DISABLE :
                                                       SDL_ENABLE);
                        break;
                    case SDLK_p:
                        debugged = 0;
                        break;
                    case SDLK_BACKQUOTE:
                        if(numViewports == 1) {
                            numViewports = 2;
                        } else {
                            numViewports = 1;
                        }
                        mat44Perspective(matStackTop(ct->matrices[MATRIX_PROJECTION]),
                                         FOVY,SCREEN_WIDTH/(float)numViewports/SCREEN_HEIGHT,1,30);
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if(mouseCaptured) {
                        SDL_WarpMouse(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
                        horizRot = -event.motion.xrel,
                        vertRot  = event.motion.yrel;
                    }
                    break;
                default:
                    break;
            }
        }

        if(!debugged) {
            printf("\nDebugging!\n\n");
        }

        clearBuffers(ct,black,1);
        
        Uint8* keys = SDL_GetKeyState(NULL);

        if(!mouseCaptured) {
            vertRot = horizRot = 0;
            if(keys[SDLK_UP]) {
                vertRot += 1;
            }
            if(keys[SDLK_DOWN]) {
                vertRot -= 1;
            }

            if(keys[SDLK_RIGHT]) {
                horizRot += 1;
            }
            if(keys[SDLK_LEFT]) {
                horizRot -= 1;
            }
        }

        int xDelta = 0,
            yDelta = 0,
            zDelta = 0;
        if(keys[SDLK_w]) { 
            zDelta -= 1;
        }
        if(keys[SDLK_s]) {
            zDelta += 1;
        }
        if(keys[SDLK_a]) { 
            xDelta -= 1;
        }
        if(keys[SDLK_d]) {
            xDelta += 1;
        }
        if(keys[SDLK_RSHIFT] || keys[SDLK_LSHIFT]) {
            yDelta -= 1;
        }
        if(keys[SDLK_SPACE]) {
            yDelta += 1;
        }

        Mat44 tmp;

        if(xDelta || yDelta || zDelta) {
            float xDiff = xDelta*MOVEMENT_SPEED*SECONDS_PER_FRAME,
                  yDiff = yDelta*MOVEMENT_SPEED*SECONDS_PER_FRAME,
                  zDiff = zDelta*MOVEMENT_SPEED*SECONDS_PER_FRAME;
            mat44Translate(tmp,-xDiff,-yDiff,-zDiff);
            mat44Mult(translate,tmp,translate);
        }

        if(vertRot) {
            float vertDiff = vertRot*(mouseCaptured ?
                                       DEGREES_PER_PIXEL :
                                       ROTATION_SPEED*SECONDS_PER_FRAME);
            mat44Rotate(tmp,vertDiff,-1,0,0);
            mat44Mult(rotateVert,tmp,rotateVert);
        }
        if(horizRot) {
            float horizDiff = horizRot*(mouseCaptured ?
                                       DEGREES_PER_PIXEL :
                                       ROTATION_SPEED*SECONDS_PER_FRAME);
            mat44Rotate(tmp,horizDiff,0,1,0);
            mat44Mult(translate,tmp,translate);
        }

        tetraHorizRot += TETRA_HORIZ_ROTATION_SPEED*SECONDS_PER_FRAME;
        tetraVertRot  += TETRA_VERT_ROTATION_SPEED *SECONDS_PER_FRAME;

        mat44Rotate(tetraTransform,tetraHorizRot,0,1,0);

        mat44Rotate(tmp,tetraHorizRot,0,1,0);
        Vec4 xAxis = { 1,0,0,1 };
        mat44MultVec4(xAxis,tmp,xAxis);
        mat44Rotate(tmp,tetraVertRot,xAxis[0],xAxis[1],xAxis[2]);
        mat44Mult(tetraTransform,tetraTransform,tmp);


        ct->viewport.width = ct->_width/numViewports;

        unsigned i;
        for(i=0;i<numViewports;++i) {
            ct->viewport.x = ct->_width-ct->viewport.width*(i+1);

            matStackPush(ct->matrices[MATRIX_MODELVIEW]);

                if(i == 0) {
                    matStackMult(ct->matrices[MATRIX_MODELVIEW],rotateVert);
                    matStackMult(ct->matrices[MATRIX_MODELVIEW],translate);
                } else {
                    mat44Translate(tmp,0,0,-5);
                    matStackMult(ct->matrices[MATRIX_MODELVIEW],tmp);
                }

                //drawShapeIndexed(ct,SHAPE_TRIANGLE,4,varr,indices);
                matStackPush(ct->matrices[MATRIX_MODELVIEW]);

                    matStackMult(ct->matrices[MATRIX_MODELVIEW],gridTransform);
                    
                    drawShapeIndexed(ct,SHAPE_LINE,gridNumEdges,gridArr,gridIndices);

                matStackPop(ct->matrices[MATRIX_MODELVIEW]);
    
                matStackPush(ct->matrices[MATRIX_MODELVIEW]);
    
                    matStackMult(ct->matrices[MATRIX_MODELVIEW],tetraTransform);
    
                    drawShapeIndexed(ct,SHAPE_TRIANGLE,4,varr,indices);

                matStackPop(ct->matrices[MATRIX_MODELVIEW]);
                
            matStackPop(ct->matrices[MATRIX_MODELVIEW]);
            
            debugged = 1;
        }

        SDL_BlitSurface(ct->surface,NULL,screen,NULL);
        SDL_Flip(screen);

        unsigned elapsedTime = SDL_GetTicks()-startTicks;
        unsigned currFps = 1000/elapsedTime;
        if(currFps > FRAMES_PER_SECOND) {
            currFps = FRAMES_PER_SECOND;
        }

        fps = fpsLerp*currFps+(1-fpsLerp)*fps;
        sprintf(title,"%s %u FPS",TITLE,(unsigned)(fps+0.5));
        SDL_WM_SetCaption(title,NULL);

        if(elapsedTime < ticksPerFrame) {
            SDL_Delay(ticksPerFrame-elapsedTime);
        }
    }

    free(gridVerts);
    free(gridIndices);

    freeVertArray(gridArr);
    freeVertArray(varr);

    freeContext(ct);
    return 0;
}
