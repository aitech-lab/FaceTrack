#include <cstdio>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <SDL2/SDL.h>

#include "../include/FaceTracker.h"

#define SHM_SEM_NAME "face_tracker"

using namespace std;

size_t size = sizeof(FaceTrack);
FaceTrack* faceTrack;

int main(void) {

    // map shared memory
    int shm = shm_open(
        SHM_SEM_NAME, 
        O_RDWR, 
        S_IRUSR | S_IWUSR);
    faceTrack = (FaceTrack*)mmap(
        0, size, 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED, shm, 0);
    close(shm);
    
    // semaphore
    sem_t* sem = sem_open(SHM_SEM_NAME, O_CREAT, 0644, 0); 
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Tracking",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        FT_CHIP_SIZE,
        FT_CHIP_SIZE,
        0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    surface->format->format = SDL_PIXELFORMAT_RGBA32;

    //  Read envelope with address
    SDL_Surface* image = SDL_CreateRGBSurfaceFrom(
        faceTrack->face, 
        FT_CHIP_SIZE,   // width
        FT_CHIP_SIZE,   // height
        FT_COLORS*8,    // depth
        FT_CHIP_SIZE*FT_COLORS, // pitch
        0x0000ff,
        0x00ff00,
        0xff0000,
        0x000000);
 
    SDL_Event event;
    int quit = 0;
    while (!quit) {

        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }

        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(image);
        SDL_SetRenderDrawColor(renderer, 255,255,255,40);
        for(int i=0; i<5; i++) {
            int e = faceTrack->emotions[i]*20.0;
            SDL_Rect r;
            if (e > 0) 
            	r = {i*10,128-e, 9, e};
            else 
            	r = {i*10,128, 9, -e};
            SDL_RenderFillRect(renderer, &r);
        } 
        for(int i=0; i<68; i++) {
            SDL_Rect r = {(int)faceTrack->landmarks[i*2]-1, (int)faceTrack->landmarks[i*2+1]-1, 2, 2};
            SDL_RenderFillRect(renderer, &r);
        } 
        SDL_BlitSurface(image, NULL, surface, NULL);
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(30);
    }
    SDL_FreeSurface(image);

    SDL_DestroyWindow(window);
    SDL_Quit();

    // unmap shared memory
    munmap(faceTrack, size);

    return 0;
}
