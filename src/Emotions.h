#pragma once

#include <vector>
#include <dlib/svm_threaded.h>

const int face_landmarks[] = {
    17,18,19,20,21, 22,23,24,25,26,      // 10 brows left-right
    36,37,38,39,40,41,                   // 6 left eye
    42,43,44,45,46,47,                   // 6 rgit eye
    48,49,50,51,52,53,54,55,56,57,58,59, // 12 lips outer
    60,61,62,63,64,65,66,67              // 8 lips inner
};
const int landmarks_count = sizeof(face_landmarks)/sizeof(int);
typedef dlib::matrix<double,landmarks_count,1> sample_t;
typedef dlib::one_vs_one_trainer<dlib::any_trainer<sample_t> > ovo_trainer;
typedef dlib::radial_basis_kernel<sample_t> rbf_kernel;

class Emotions {

public:
    Emotions();
    ~Emotions();

    void add_face(int cat, dlib::full_object_detection& det);
    void train();
    double predict(dlib::full_object_detection& det);
    void update(dlib::full_object_detection& det);
    
    std::vector<std::vector<dlib::full_object_detection>> faces;
    std::vector<float> emotions;

private:
    void extract_features();
    std::vector<sample_t> samples;
    std::vector<double>   labels;

    dlib::one_vs_one_decision_function<
        ovo_trainer, 
        dlib::decision_function<rbf_kernel>> 
    predictor;
};
