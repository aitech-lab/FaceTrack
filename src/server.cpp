#include <cstdio>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "../include/FaceTracker.h"

using namespace std;

size_t size = sizeof(FaceTrack);

FaceTracker tracker((char*)"/dev/video0", 640, 480);
FaceTrack* faceTrack;

void callback() {
    tracker.log();
    memcpy(faceTrack, &tracker.faceTrack, size);
}

int main(void) {
    
    key_t key = ftok("track", 65);
    int shmid = shmget(key, size, 0666|IPC_CREAT);
    faceTrack = (FaceTrack*) shmat(shmid, (void*)0, 0);

    tracker.setCallback(callback);
    getchar();

    shmdt(faceTrack);
}
