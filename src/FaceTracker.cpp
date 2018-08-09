#include "../include/FaceTracker.h"
#include "Ffmpeg.h"
#include "Tracker.h"

using namespace std;

FaceTracker::FaceTracker(char* device, int w, int h):
w(w), h(h), callback(NULL) {
    ffmpeg  = new Ffmpeg(device, w, h);
    tracker = new Tracker(ffmpeg, this);
}

FaceTracker::~FaceTracker() {
    delete tracker;
    delete ffmpeg;
}

void FaceTracker::update() {
    tracker->getLandmarks(faceTrack.landmarks);
    tracker->getEmotions (faceTrack.emotions );
    tracker->getFaceChip (faceTrack.face     );
    if(callback!=NULL) callback();
}

void FaceTracker::log() {
	printf("Tracker speed: %2.1ffps\n", tracker->fps);
    printf("Emotions: \n");
    for(int i=0; i < 5; i++) printf("%0.1f ", faceTrack.emotions[i]); 
    printf("\n");

    printf("Landmarks: \n");
    printf("[%0.1f %0.1f] ... [%0.1f %0.1f]", 
    	faceTrack.landmarks[0  ], faceTrack.landmarks[1  ],
    	faceTrack.landmarks[134], faceTrack.landmarks[135]); 
    printf("\n");
}

void FaceTracker::setCallback(void(*cb)()) {
	callback = cb;
}
