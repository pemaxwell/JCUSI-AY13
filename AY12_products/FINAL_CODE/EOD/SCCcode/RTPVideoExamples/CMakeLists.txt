cmake_minimum_required(VERSION 2.6)

project( VideoExample )

include_directories(/usr/local/include  . )

link_directories(/usr/local/lib)


link_libraries( VideoExample
        avcodec
        avformat
        avutil
        pthread
        swscale
        glut
        GL
        GLU
        jrtp-3.8.1
        X11
        Xi
        Xmu
        m )


add_executable( VideoExample VideoStream.cpp VideoExample.cpp )
