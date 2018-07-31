FaceTrack
---------

Example of small shared library, an dlib wrapper for face tracking and SVM based emotion classification.

Motivation
----------

Dlib is powerfull machine learning library and an excelent example of an academic c++ coding style. 
High level of abstraction in the library and abundant templating - leаd to very long compilation time :(
Compilation of any simplest app, risked to include some dlib headers, become endless.

So, isolation of some dlib based functionality in sepparate shared library, sounds good.

Dependencies
-------------

* cmake
* dlib
* SDL for clients
* zmq for zmq client/server

Compilation
-----------

* edit CMakeList.txt, set your dlib location path
* `cd build; cmake ..; make`
* `./face_track_test`

Demos
-----

* face_tracker.so - shared library
* face_track_test - library test
* face_track_[server/client] - shared memory communication
* face_track_[server/client]_zmq - zmq based communication
