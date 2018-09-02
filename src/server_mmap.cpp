#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

#include "../include/FaceTracker.h"

#define SHM_SEM_NAME "face_tracker"

using namespace std;

size_t size = sizeof(FaceTrack);
sem_t* sem = 0;
FaceTracker* tracker;
FaceTrack*   faceTrack;

void callback() {
    
    tracker->log();

    // semaphore wait 1 sec max
    struct timespec tm; clock_gettime(CLOCK_REALTIME, &tm); tm.tv_sec += 1;
    // check & lock semaphore
    sem_timedwait(sem, &tm);
    // copy data
    memcpy(faceTrack, &tracker->faceTrack, size);
    // unlock semaphore
    sem_post(sem);
}

void ctrl_c(int s){
    printf("Caught signal %d\n",s);

    tracker->setCallback(0);
    delete(tracker);
    munmap(faceTrack, size);
    
    exit(0); 
}

int main(int argc, char** argv) {

    if (argc <=1) return printf("usage: app /dev/video0\n");

    // shared memory    
    int shm = shm_open(
        SHM_SEM_NAME, 
        O_CREAT | O_RDWR, 
        S_IRUSR|S_IWUSR);
    faceTrack = (FaceTrack*)mmap(
        0, size, 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED, shm, 0);
    ftruncate(shm, size);
    close(shm);

    // named semaphore
    sem = sem_open(SHM_SEM_NAME, O_CREAT, 0644, 0);
    if (sem == 0) {
        fprintf(stderr, "Semaphore errno: %d\n%s\n", errno, strerror(errno));
        exit(1);
    }

    // ctrl_c
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Tracker
    tracker = new FaceTracker(argv[1], 640, 480);    
    tracker->setCallback(callback);

    pause();
 
}
