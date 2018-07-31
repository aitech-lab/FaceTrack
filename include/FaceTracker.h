#pragma once

#define CHIP_SIZE 256
#define LANDMARKS 68
#define EMOTIONS  5

typedef struct FaceTrack {                
    float landmarks[LANDMARKS*2];                
    float emotions[EMOTIONS];                    
    unsigned char face[CHIP_SIZE*CHIP_SIZE*3];
} FaceTrack;

class FaceTracker {

     class Ffmpeg*  ffmpeg;
     class Tracker* tracker;
     int w,h;
     void (*callback)();
public:
     FaceTracker(char* device, int w, int h);
     ~FaceTracker();

     void setCallback(void(*cb)()); 
     void update(void);
     void log(void);

     FaceTrack faceTrack;
};
