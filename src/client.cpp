#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <SDL2/SDL.h>
#include "../include/FaceTracker.h"

using namespace std;

size_t size = sizeof(FaceTrack);
FaceTrack* faceTrack;

int main(void) {
    
    key_t key = ftok("track", 65);
    int shmid = shmget(key, size, 0666|IPC_CREAT);
    faceTrack = (FaceTrack*) shmat(shmid, (void*)0, 0);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Tracking",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        256,
        256,
        0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    surface->format->format = SDL_PIXELFORMAT_RGB24;

    while (1) {
        //  Read envelope with address
        SDL_Surface* image = SDL_CreateRGBSurfaceFrom(
            faceTrack->face, 
            256,   // width
            256,   // height
            24,    // depth
            256*3, // pitch
            0xff0000,
            0x00ff00,
            0x0000ff,
            0x000000);
        SDL_BlitSurface(image, NULL, surface, NULL);
        SDL_FreeSurface(image);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(30);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    shmdt(faceTrack);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
