#include <cstdio>
#include <unistd.h>
#include "../include/FaceTracker.h"

FaceTracker tracker((char*)"/dev/video0", 640, 480);

void callback() {
    tracker.log();
}

int main(void) {
    tracker.setCallback(callback);
    getchar();
}
