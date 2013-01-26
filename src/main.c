#include <SDL/SDL.h>
#include <stdio.h>
#include <time.h>
#include "Context.h"
#include "Draw.h"
#include "Vertex.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

#define FRAMES_PER_SECOND 40
#define SECONDS_PER_FRAME (1.0/FRAMES_PER_SECOND)

#define ROTATION_SPEED 45
#define MOVEMENT_SPEED 2
#define TETRA_ROTATION_SPEED 45
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
                       { {  0,    1.5, 0 }, { 1, 1, 1 } } };

// draws the tetrahedron
unsigned indices[] = { 0,1,2,
                       1,0,3,
                       0,2,3,
                       2,1,3 };

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

    mat44Perspective(*matStackTop(ct->matrices[MATRIX_PROJECTION]),
                     70,SCREEN_WIDTH/(float)SCREEN_HEIGHT,1,50);

    mat44Ident(*matStackTop(ct->matrices[MATRIX_MODELVIEW]));

    Mat44 translate;
    mat44Translate(translate,0,0,-4);

    Mat44 rotateVert;
    mat44Ident(rotateVert);

    Mat44 tetraTransform;
    mat44Ident(tetraTransform);

    VertexArray* varr = createVertArray(0,NULL);
    varr->locs      = &verts[0].loc;
    varr->locStep   = sizeof(CustomVert);
    varr->colors    = &verts[0].color;
    varr->colorStep = sizeof(CustomVert);

    Color4 black;
    vec4Zero(black);

    SDL_WM_SetCaption(TITLE,NULL);

    char title[256];
    float fps = 0;
    float fpsLerp = 0.2;

    int running = 1;
    while(running) {
        Uint32 startTicks = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = 0;
            }
        }

        clearBuffers(ct,black,-30);
        
        Uint8* keys = SDL_GetKeyState(NULL);

        int vertRot  = 0,
            horizRot = 0;
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
            float vertDiff = vertRot*ROTATION_SPEED*SECONDS_PER_FRAME;
            mat44Rotate(tmp,vertDiff,-1,0,0);
            mat44Mult(rotateVert,tmp,rotateVert);
        }
        if(horizRot) {
            float horizDiff = horizRot*ROTATION_SPEED
                                      *SECONDS_PER_FRAME;
            mat44Rotate(tmp,horizDiff,0,1,0);
            mat44Mult(translate,tmp,translate);
        }

        mat44Rotate(tmp,TETRA_ROTATION_SPEED*SECONDS_PER_FRAME,0,1,0);
        mat44Mult(tetraTransform,tetraTransform,tmp);

        matStackPush(ct->matrices[MATRIX_MODELVIEW]);
        matStackMult(ct->matrices[MATRIX_MODELVIEW],rotateVert);
        matStackMult(ct->matrices[MATRIX_MODELVIEW],translate);
        matStackMult(ct->matrices[MATRIX_MODELVIEW],tetraTransform);

        drawShapeIndexed(ct,SHAPE_TRIANGLE,4,varr,indices);

        matStackPop(ct->matrices[MATRIX_MODELVIEW]);

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

    freeContext(ct);
    return 0;
}
