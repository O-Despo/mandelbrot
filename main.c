#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "init.c"

#define S_WIDTH 500
#define S_HEIGHT 500
#define MAX_ITER 250
#define X_LOW -2.00
#define X_UP 0.47
#define Y_LOW -1.12
#define Y_UP 1.12

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
} App;

typedef struct {
    unsigned short int x;
    unsigned short int y;
    unsigned short int rgb;
} BuffPoint;

double xsl = X_LOW;
double xsu = X_UP;
double ysl = Y_LOW;
double ysu = Y_UP;

double scale_abs_x;
double scale_div_x;

double scale_abs_y;
double scale_div_y;


int sdlInit(App *app){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Failed");
        exit(1);
    }
    
    app->window = SDL_CreateWindow("MANDLBROT", 
            0, 0, S_WIDTH, S_HEIGHT, 0);

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

int processPixel(int xi, int yi){
    int iter = 0;

    double x2 = 0;
    double y2 = 0;

    double x;
    double y;

    double x0 = scaleX(xi); 
    double y0 = scaleY(yi); 

    while ((x2 + y2) <= 4 && iter < MAX_ITER){
        y = 2 * x * y + y0;
        x = x2 - y2 + x0;
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
        ysu -= scale_abs_diff;
        ysl -= scale_abs_diff;
    } else if (i == 'u'){
        double scale_abs_diff = fabs(ysu-ysl)/10;
        ysu += scale_abs_diff;
        ysl += scale_abs_diff;
    }

    return 0;
}

int main(int argc,char *argv[]){
    bool quit = 0;
    bool focusChange = 1;

    SDL_Event event;

    SDL_Point point = {
        0,
        0 
    };

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

    BuffPoint pixelBuffer[S_WIDTH*S_HEIGHT];

    while(!quit){
        //SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        //SDL_RenderClear(app.renderer);
 
        for(int x = 0; x < S_WIDTH; x++){
            for(int y = 0; y < S_HEIGHT; y++){
                int iter = processPixel(x, y);
                SDL_SetRenderDrawColor(app.renderer, 0, 0, iter, 255);
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
    }

    SDL_Quit();
}
