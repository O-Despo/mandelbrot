#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "init.c"

#define S_WIDTH 1920
#define S_HEIGHT 1080
#define MAX_ITER 125
#define X_LOW -2.00
#define X_UP 0.47
#define Y_LOW -1.12
#define Y_UP 1.12

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
} App;

double xsl = X_LOW;
double xsu = X_UP;
double ysl = Y_LOW;
double ysu = Y_UP;

int sdlInit(App *app){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Failed");
        exit(1);
    }
    
    app->window = SDL_CreateWindow("MANDLBROT", 
            0, 0, S_WIDTH, S_HEIGHT, SDL_WINDOW_BORDERLESS);

    if(app->window == NULL){
        SDL_Log("Could not create window: %s", SDL_GetError());
        exit(1);
    }

    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_PRESENTVSYNC);
 
    if(app->renderer == NULL){
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        exit(1);
    }

    return 0;
}

double scaleY(int y){
    return y/(S_HEIGHT/fabs(ysu-ysl))+ ysl;
}

double scaleX(int x){
    return x/(S_WIDTH/fabs(xsu-xsl)) + xsl;
}

int iter;
double x2, y2, x, y, y00, x00;

int processPixel(int xi, int yi){
    iter = 0;

    x2 = 0;
    y2 = 0;

    x = 0;
    y = 0;

    x00 = scaleX(xi);
    y00 = scaleY(yi); 

    while ((x2 + y2) <= 4 && iter < MAX_ITER){
        y = 2 * x * y + y00;
        x = x2 - y2 + x00;
        x2 = x * x;
        y2 = y * y;
        iter+=1;
    }
    
    return iter;
}

int moveArrows(int i){
    if(i == 'l'){
        double scale_abs_diff = fabs(xsu-xsl)/10;
        xsu -= scale_abs_diff;
        xsl -= scale_abs_diff;
    } else if (i == 'r'){
        double scale_abs_diff = fabs(xsu-xsl)/10;
        xsu += scale_abs_diff;
        xsl += scale_abs_diff;
    } else if (i == 'd'){
        double scale_abs_diff = fabs(ysu-ysl)/10;
        ysu += scale_abs_diff;
        ysl += scale_abs_diff;
    } else if (i == 'u'){
        double scale_abs_diff = fabs(ysu-ysl)/10;
        ysu -= scale_abs_diff;
        ysl -= scale_abs_diff;
    }

    return 0;
}

int reCalc(int *buffer){
    long offset;
    int iter;

    for(int x = 0; x < S_WIDTH; x++){
        for(int y = 0; y < S_HEIGHT; y++){
            offset = x * S_WIDTH + y;
            iter = processPixel(x, y);
            *(buffer+offset) = iter*2;
        }
    }

    return 0;   
}

int main(int argc,char *argv[]){
    bool quit = 0;
    bool focusChange = 1;

    SDL_Event event;

    int rectWidth = S_WIDTH/5;
    SDL_Rect cursorRect = {
        S_WIDTH/2 - rectWidth/2,
        S_HEIGHT/2 - rectWidth/2,
        rectWidth,
        rectWidth
    };

    const SDL_Rect *cursorRectPointer = &cursorRect;

    App app;
    memset(&app, 0, sizeof(App));
  
    sdlInit(&app);

    int *buffer = malloc((S_WIDTH*S_HEIGHT)*sizeof(int));

    int offset;
    for(int x = 0; x < S_WIDTH; x++){
        for(int y = 0; y < S_HEIGHT; y++){
            offset = x * S_WIDTH + y;
            SDL_SetRenderDrawColor(app.renderer, 0, 0, *(buffer+offset), 255);
            SDL_RenderDrawPoint(app.renderer, x, y);
        }

    }

 
    if(buffer == NULL){
        exit(1);
    }
        
    while(!quit){
        if(focusChange == 1){
            reCalc(buffer);
            focusChange = 0;
       }

        int offset;
        for(int x = 0; x < S_WIDTH; x++){
            for(int y = 0; y < S_HEIGHT; y++){
                offset = x * S_WIDTH + y;
                SDL_SetRenderDrawColor(app.renderer, 0, 0, *(buffer+offset), 255);
                SDL_RenderDrawPoint(app.renderer, x, y);
            }
 
        }

        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT){
                quit = 1;
            }else if(event.type == SDL_MOUSEMOTION){
                cursorRect.x = event.motion.x - rectWidth/2; 
                cursorRect.y = event.motion.y - rectWidth/2; 
            }else if(event.type == SDL_KEYDOWN){ 
                focusChange = 1;
                if(event.key.keysym.sym == SDLK_q){
                    quit = 1;
                }else if(event.key.keysym.sym == SDLK_SPACE){
                    double temp_xsl = scaleX(cursorRect.x);
                    double temp_xsu = scaleX(cursorRect.x + cursorRect.w);
                    double temp_ysl = scaleY(cursorRect.y);
                    double temp_ysu = scaleY(cursorRect.y + cursorRect.h);
                     
                    xsl = temp_xsl;
                    xsu = temp_xsu;
                    ysl = temp_ysl;
                    ysu = temp_ysu;
                    focusChange = 1;
                }else if(event.key.keysym.sym == SDLK_UP){
                    moveArrows('u');
                }else if(event.key.keysym.sym == SDLK_DOWN){
                    moveArrows('d');
                }else if(event.key.keysym.sym == SDLK_LEFT){
                    moveArrows('l');
                }else if(event.key.keysym.sym == SDLK_RIGHT){
                    moveArrows('r');
                }
            }
        }

        SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(app.renderer, cursorRectPointer); 
        
        SDL_RenderPresent(app.renderer);
        SDL_Delay(0);
    }

    SDL_Quit();
    free(buffer);
    exit(0);
}
