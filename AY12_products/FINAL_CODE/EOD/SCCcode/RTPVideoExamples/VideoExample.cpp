/* VideoExample.cpp
 * 
 * This is an example application that illustrates how to display video streams
 * from an Aware 2 robot. 
 * 
 * Video streams are encoded in MPEG1 and transmitted using the RTP protocol.
 *
 * This program uses the JRTP library to provide the RTP stack, FFMPEG for MPEG1
 * decompression, pthreads for mutexes and threading, and OpenGL for drawing.
 *
 * Copyright iRobot 2001 - 2009. Unpublished 
 */
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <VideoStream.hpp>


// List of active video streams 
std::vector<VideoStream*> streamVector;


// Draw a video frame on the screen.
void drawVideo(VideoStream* video)
{
    unsigned char* buf = video->acquireBuffer();

    glPixelZoom(1.0, -1.0);  
    glRasterPos2f(video->getX(), video->getY());
    glDrawPixels(video->getWidth(), video->getHeight(),
                 GL_RGB, GL_UNSIGNED_BYTE, buf);

    video->releaseBuffer();
}  


// OpenGL callback which updates all video images on the screen
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
 
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 640.0, 0.0, 241.0, 1.0, -1.0);    // Window size is 640x241
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Update each video stream on the display
    for (size_t i = 0; i < streamVector.size(); ++i) {
        drawVideo(streamVector[i]);
    }

    glPopMatrix();
    glutSwapBuffers();
    glutPostRedisplay();
}


int main(int argc, char *argv[])
{
    // Create the video streams depending on the cmd line arguments
    if (argc < 2 || (strcmp(argv[1], "-p") == 0)) {
        // PackBot EOD head and turret cameras
        streamVector.push_back(new VideoStream(CODEC_ID_MPEG1VIDEO, 256, 192, 
                                               0, 240, 9012));
        streamVector.push_back(new VideoStream(CODEC_ID_MPEG1VIDEO, 320, 240, 
                                               256, 240, 9002));
    } else if (strcmp(argv[1], "-w") == 0) {
        // Warrior chassis 
        streamVector.push_back(new VideoStream(CODEC_ID_MPEG4, 352, 240, 
                                               0, 240, 6980, "224.0.1.20"));
    } else {
        std::cout << "Usage: " << argv[0] << " [-w|-p]" << std::endl
                  << "-p  PackBot Example" << std::endl
                  << "-w  Warrior Example" << std::endl;
        return 1;
    }

    // Init OpenGL 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize((GLint)640, (GLint)241); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Video Example");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glShadeModel(GL_FLAT);
   
    glutDisplayFunc(display); 
    glutMainLoop();

    // Cleanup
    for (size_t i = 0; i < streamVector.size(); ++i) {
        delete streamVector[i];
    }
    return 0;
} 
