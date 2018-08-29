#pragma once

#include <thread>
#include <chrono>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

class Tracker {

    void tracking_fun();
    std::thread tracking_th;
    std::atomic<bool> do_track;
    dlib::chip_details chip;

    class FaceTracker* interface;
    class Ffmpeg* ffmpeg;
    class Emotions* emotions;

    dlib::matrix<dlib::rgb_alpha_pixel> face_image;
    dlib::rectangle               face_rect;
    dlib::full_object_detection   face_shape;
    int face_count;
    
public:
    Tracker(Ffmpeg* ffmpeg, FaceTracker* interface);
    ~Tracker();
    double fps;
    void getLandmarks(float* landmarks);
    void getFaceChip(unsigned char* faceChip);
    void getEmotions(float* emo);
    int  getFaceCount();
};
