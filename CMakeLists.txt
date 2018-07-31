cmake_minimum_required(VERSION 3.9)
project(face_tracker VERSION 1.0 DESCRIPTION "dlib based face tracking library")
include_directories(${CURRENT_SOURCE_DIR}/include)
add_library(face_tracker SHARED
    src/FaceTracker.cpp
    src/Ffmpeg.cpp
    src/Tracker.cpp
    src/Emotions.cpp
)

include(../lib/dlib/dlib/cmake)

# ZMQ
add_executable(face_tracker_server_zmq src/server_zmq.cpp)
target_link_libraries(face_tracker_server_zmq zmq face_tracker dlib::dlib)

add_executable(face_tracker_client_zmq src/client_zmq.cpp)
target_link_libraries(face_tracker_client_zmq SDL2 zmq)

# SHARED MEMORY
add_executable(face_tracker_server src/server.cpp)
target_link_libraries(face_tracker_server face_tracker dlib::dlib)

add_executable(face_tracker_client src/client.cpp)
target_link_libraries(face_tracker_client SDL2)

add_executable(face_tracker_test src/test.cpp)
target_link_libraries(face_tracker_test face_tracker dlib::dlib)
