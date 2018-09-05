#pragma once

#include <thread>

using namespace std;

class Ffmpeg {
    FILE* stream;
	thread read_th;
    bool do_work;
	void read_fun();
    int buffer_id;
    size_t frame_size;
    size_t frame_count;

public:
    Ffmpeg(char* device, int w, int h);
    ~Ffmpeg();
    unsigned char* get_frame();
    unsigned char* buffer;
    int w, h, c;
};

