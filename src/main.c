#include <SDL/SDL.h>
#include <stdio.h>
#include <time.h>
#include "FrameBuffer.h"
#include "Shapes.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP    32

#define FRAMES_PER_SECOND 20

int main(void) {
    unsigned ticksPerFrame = 1000/FRAMES_PER_SECOND;
    SDL_Surface* screen;
    FrameBuffer* fb;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr,"SDL init failed");
        return -1;
    }
    if(!(screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE))) {
        fprintf(stderr,"SDL video init failed");
        return -1;
    }
    if(!(fb = createFrameBuffer(SCREEN_WIDTH,SCREEN_HEIGHT))) {
        fprintf(stderr,"FrameBuffer init failed");
        return -1;
    }

    Color4 black;
    vec4Zero(&black);
    clearColorBuffer(fb,black);

    srand(time(NULL));

    SDL_WM_SetCaption("Lines!",NULL);

    Vertex center;
    {
        center.loc[0] = SCREEN_WIDTH/2.0;
        center.loc[1] = SCREEN_HEIGHT/2.0;

        center.color[0] = center.color[1] = center.color[2] = 1;
    }

    int running = 1;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = 0;
            } else if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_c) {
                    clearColorBuffer(fb,black);
                }
            } else if(event.type == SDL_MOUSEBUTTONDOWN) {
                Vertex point;
                point.loc[0] = event.button.x;
                point.loc[1] = event.button.y;
                    
                point.color[0] = rand()/(float)RAND_MAX;
                point.color[1] = rand()/(float)RAND_MAX;
                point.color[2] = rand()/(float)RAND_MAX;

                //plotPoint(fb,point.loc,point.color);
                plotLine(fb,center,point);
            }
        }
        
        Uint8* keys = SDL_GetKeyState(NULL);

        fb->smoothShade = (keys[SDLK_RSHIFT] || keys[SDLK_LSHIFT]);
        if(keys[SDLK_SPACE]) {
            Vertex line[2];
            line[0].loc[0] = rand()%SCREEN_WIDTH;
            line[0].loc[1] = rand()%SCREEN_HEIGHT;
            line[1].loc[0] = rand()%SCREEN_WIDTH;
            line[1].loc[1] = rand()%SCREEN_HEIGHT;

            line[0].color[0] = rand()/(float)RAND_MAX;
            line[0].color[1] = rand()/(float)RAND_MAX;
            line[0].color[2] = rand()/(float)RAND_MAX;
            line[1].color[0] = rand()/(float)RAND_MAX;
            line[1].color[1] = rand()/(float)RAND_MAX;
            line[1].color[2] = rand()/(float)RAND_MAX;

            //plotPoint(fb,line[0].loc,line[0].color);
            plotLines(fb,1,line);
        }

        refreshSurface(fb);
        SDL_BlitSurface(fb->surface,NULL,screen,NULL);
        SDL_Flip(screen);
        fflush(stdout);
    }

    freeFrameBuffer(fb);
    return 0;
}
