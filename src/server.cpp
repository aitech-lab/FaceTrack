#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "../include/FaceTracker.h"

using namespace std;

size_t size = sizeof(FaceTrack);

FaceTracker* tracker;
FaceTrack*  faceTrack;

void callback() {
    tracker->log();
    memcpy(faceTrack, &tracker->faceTrack, size);
}

int main(int argc, char** argv) {

    if (argc <=1) return printf("usage: app /dev/video0\n");

    tracker = new FaceTracker(argv[1], 800, 600);
    
    system("touch track");
    key_t key = ftok("track", 65);
    int shmid = shmget(key, size, 0666|IPC_CREAT);
    faceTrack = (FaceTrack*) shmat(shmid, (void*)0, 0);
    
    tracker->setCallback(callback);

    // wait for enter key
    getchar();
    
    tracker->setCallback(0);
    delete(tracker);
    
    shmdt(faceTrack);
}
