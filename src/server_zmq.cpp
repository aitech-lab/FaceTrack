#include <cstdio>

#include "zhelpers.hpp"

#include "../include/FaceTracker.h"

using namespace std;
using namespace zmq;

context_t context(1);
socket_t publisher(context, ZMQ_PUB);
    
size_t size = sizeof(FaceTrack);

FaceTracker tracker((char*)"/dev/video0", 640, 480);

void callback() {
    tracker.log();
    s_sendmore(publisher, "Tracking");
    message_t msg(size);
    memcpy(msg.data(), &tracker.faceTrack, size);
    publisher.send(msg);
}

int main(void) {
    publisher.bind("tcp://*:5563");
    tracker.setCallback(callback);
    getchar();
}
