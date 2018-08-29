#pragma once

#define FT_CHIP_SIZE 512
#define FT_LANDMARKS 68
#define FT_EMOTIONS  5
#define FT_COLORS 4

typedef struct FaceTrack {
    float landmarks[FT_LANDMARKS*2];                
    float emotions[FT_EMOTIONS];                    
    unsigned char face[FT_CHIP_SIZE*FT_CHIP_SIZE*FT_COLORS];
    int   faceCount;
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
