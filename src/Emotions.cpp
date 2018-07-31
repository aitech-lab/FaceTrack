#include "Emotions.h"

using namespace std;
using namespace dlib;

sample_t extract(full_object_detection& det);

Emotions::Emotions():
faces(5, std::vector<full_object_detection>(0)), 
emotions(5)
{
    try {
        deserialize("emotions_predictor.dat") >> predictor;
    } catch(exception& e) {
        cerr << e.what() << "\n";
    }
};

Emotions::~Emotions(){};

void Emotions::add_face(int cat, full_object_detection& det){
	if(cat>=5) return;
	faces[cat].push_back(det);
};

void Emotions::train(){
    extract_features();

    ovo_trainer trainer;

    krr_trainer<rbf_kernel> rbf_trainer;
    rbf_trainer.set_kernel(rbf_kernel(0.1));
    trainer.set_trainer(rbf_trainer);

    randomize_samples(samples, labels);
    cout << "cross validation: \n" << cross_validate_multiclass_trainer(trainer, samples, labels, 5) << endl;
    predictor  = trainer.train(samples, labels);
    serialize("emotions_predictor.dat") << predictor;
    
    int ok = 0;
    for(int i=0; i<samples.size(); i++) {
        if (predictor(samples[i])==labels[i]) ok++;
    }
    
    cout << "OK: " << ok*100.0/samples.size() << "%\n";
};

double Emotions::predict(full_object_detection& det){
    if (det.num_parts()!=68) return -1.0;
    return predictor(extract(det)); 
};

void Emotions::update(full_object_detection& det) {

    if (det.num_parts()!=68) return;
    int label = predict(det);
    // 
    // if(0 > label || label > 4) return;
    // 
    // emotions[label] += 0.1;
    // for(label=0; label<5; label++) {
    //     emotions[label] -=0.05;
    //     if (emotions[label]>1.0) emotions[label] = 1.0;
    //     if (emotions[label]<0.0) emotions[label] = 0.0;
    // }

    auto bft = predictor.get_binary_decision_functions();
    // run all the classifiers over the sample
    std::map<double,float> votes;
    for(auto i = bft.begin(); i != bft.end(); ++i) {
        auto s = i->second(extract(det));
        votes[i->first.first ] += s;
        votes[i->first.second] -= s;
    }

    for (auto i = votes.begin(); i != votes.end(); ++i) {
        if((int)i->first == label) i->second*=2.0;
        else i->second/=2.0;
        emotions[i->first] += (i->second - emotions[i->first])/50.0;
    }

}

// 
point get_center(full_object_detection& d) {
    point p(0.0,0.0);
    for(int i=0; i<landmarks_count; i++) {
        p+=d.part(face_landmarks[i]);
    }
    p/=landmarks_count;
    return p;
}

sample_t extract(full_object_detection& det) {
    #define M(i,j) length(det.part(i)-det.part(j))
    float k = M(36,45);
    point c = get_center(det);
    sample_t s;
    for(int i=0; i<landmarks_count; i++) {
        point p = det.part(face_landmarks[i]);
        s(i) = length(c-p)/k;
    }
    #undef M
    return move(s);
}

void Emotions::extract_features() {
    samples.clear();
    labels.clear();
	for(int c=0; c<5;c++) {
		for(auto&& f : faces[c]) {
			samples.push_back(extract(f));
			labels.push_back(c);
		}
	}
}
