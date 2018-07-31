#include <cstdio>
#include <SDL2/SDL.h>

#include "zhelpers.hpp"

#include "../include/FaceTracker.h"

size_t size = sizeof(FaceTrack);

using namespace zmq;
using namespace std;


int main(void) {
    
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

    //  Prepare our context and subscriber
    context_t context(1);
    socket_t subscriber (context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5563");
    subscriber.setsockopt( ZMQ_SUBSCRIBE, "Tracking", 1);

    while (1) {
        //  Read envelope with address
        string address = s_recv(subscriber);
        message_t msg;
        subscriber.recv(&msg);
        FaceTrack* faceTrack = (FaceTrack*) msg.data();
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
        //surface->pixels = faceTrack->face;
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
