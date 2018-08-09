#include <cstdio>
#include <errno.h>
#include <ctime>

#include "../include/FaceTracker.h"
#include "Tracker.h"
#include "Ffmpeg.h"
#include "Emotions.h"

#include "raw_image.h"

using namespace dlib;
using namespace std;

static frontal_face_detector detector;
static shape_predictor pose_model_68;
static shape_predictor pose_model_5;
static matrix<rgb_alpha_pixel> _face_image;

Tracker::Tracker(Ffmpeg* ffmpeg, FaceTracker* interface): 
ffmpeg(ffmpeg), interface(interface) {
    printf("Tracker\n");
    try {    
        // Load face detection and pose estimation models.
        detector = get_frontal_face_detector();
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model_68;
        deserialize("shape_predictor_5_face_landmarks.dat" ) >> pose_model_5;
    } catch(serialization_error& e) {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << endl << e.what() << endl;
    } catch(exception& e) {
        cout << e.what() << endl;
    }

    float m = (ffmpeg->w-ffmpeg->h)/2.0;
    chip = chip_details(rectangle(m,0,ffmpeg->w-m, ffmpeg->h), FT_CHIP_SIZE*FT_CHIP_SIZE);
    // chip = chip_details(rectangle(200,200,250,250), FT_CHIP_SIZE);
     
    // Launch read thread 
    do_track = true;
    thread th(&Tracker::tracking_fun, this); swap(th, tracking_th);

    emotions = new Emotions();
}

Tracker::~Tracker() {
    // close thread
    do_track = false;
    tracking_th.join();
    delete emotions;
}

void smooth_chips(chip_details& a, chip_details& b, float k) {
    a.rect.left()   += (b.rect.left()  -a.rect.left()  )/k;
    a.rect.top()    += (b.rect.top()   -a.rect.top()   )/k;
    a.rect.right()  += (b.rect.right() -a.rect.right() )/k;
    a.rect.bottom() += (b.rect.bottom()-a.rect.bottom())/k;
    a.angle += (b.angle - a.angle)/k;
}

void Tracker::tracking_fun()  {

    int m = (ffmpeg->w-ffmpeg->h)/2;
    
    while(do_track) {

        clock_t begin = clock();
        
        raw_image<rgb_pixel> img(ffmpeg->w, ffmpeg->h, ffmpeg->c, ffmpeg->get_frame());
        chip_details default_chip(rectangle(m, 0, ffmpeg->w-m, ffmpeg->h), FT_CHIP_SIZE*FT_CHIP_SIZE);
    
        array2d<unsigned char> gray;
        assign_image(gray, img);
      
        // Get face rectangles 
        std::vector<rectangle> _face_rects = detector(gray);
        if(_face_rects.size() > 0) {

            // get face closest to center
            float min_l = 1e9;
            int closest = 0;
            for(int i=0; i< _face_rects.size(); i++) {

                float l = length(center(_face_rects[i])-point(ffmpeg->w, ffmpeg->h)/2.0);
                if(l<min_l) {
                    min_l = l;
                    closest = i;
                }
            }
            // pose closest face
            full_object_detection _face_shape = pose_model_5(img, _face_rects[closest]);  
        
            // extract_image_chip(img, get_face_chip_details(shapes[0], 150, 0.25), face);
            float k = 0.8;
            chip_details _chip = get_face_chip_details(_face_shape, FT_CHIP_SIZE, k);
            smooth_chips(chip, _chip, 4.0);
            extract_image_chip(img, chip, _face_image);

            float m = k*FT_CHIP_SIZE/(1.0+2.0*k)-15.0;
            rectangle _face_rect(m, m, FT_CHIP_SIZE-m, FT_CHIP_SIZE-m);

            //std::vector<rectangle> _face_rects2 = detector(_face_image);
            //if(_face_rects2.size() == 0) continue;
            _face_shape = pose_model_68(_face_image, _face_rect); 
            if(face_shape.num_parts() == 68)
                for(int i=0; i<68; i++) face_shape.part(i)+= (_face_shape.part(i)-face_shape.part(i))/2.0;
            else
                swap(face_shape, _face_shape);
            swap(face_image, _face_image);
            swap(face_rect, _face_rect);
        } else {
            smooth_chips(chip, default_chip, 50.0); 
            extract_image_chip(img, chip, _face_image);
            swap(face_image, _face_image);
        }

        clock_t end = clock();
        fps = CLOCKS_PER_SEC/double(end-begin);

        emotions->update(face_shape);
        interface->update();
    }
}

void Tracker::getLandmarks(float* landmarks) {

    if(face_shape.num_parts() != 68) return;
    
    for(int i=0; i<68; i++) {
        point& p = face_shape.part(i);
        landmarks[i*2+0] = (float) p(0);
        landmarks[i*2+1] = (float) p(1);
    }
}

void Tracker::getFaceChip(unsigned char* faceChip) {
    if(face_image.size() == 0) return;
    memcpy(faceChip, (unsigned char*) face_image.begin(), FT_CHIP_SIZE*FT_CHIP_SIZE*FT_COLORS);
}

void Tracker::getEmotions(float* emo) {
    memcpy(emo, &emotions->emotions[0], 5*sizeof(float));
}
