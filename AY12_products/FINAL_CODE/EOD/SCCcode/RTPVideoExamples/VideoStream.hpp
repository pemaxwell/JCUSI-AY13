// Copyright iRobot 2001 - 2009. Unpublished 

#ifndef VIDEOSTREAM_HPP
#define VIDEOSTREAM_HPP

/* VideoStream.hpp
 * 
 * The VideoStream class handles the RTP session for a given video stream.  
 * It has a Decoder to decoder the stream using ffmpeg and convert it to an 
 * RGB image.  
 *
 * The following external libraries are used for the Video Streams:
 *
 * ffmpeg v3.0 - Used for mpeg decompression
 * jrtplib v3.7.1 - RTP library (see http://research.edm.uhasselt.be/~jori/page/index.php?n=CS.Jrtplib)
 * 
 */

#include <pthread.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

/* The decoder class is responsible for decoding video streams and converting
 * decoded frames into RGB images.  
 */
class Decoder
{
public:
    Decoder(CodecID type);
    ~Decoder();

    /* Decode a chunk of video.  buffer is a buffer of compressed video and 
     * index is the index into that buffer for new data since the last time 
     * decode was called.  Returns true if a full frame is ready, otherwise 
     * returns false.
     */
    bool decode(uint8_t* buffer, unsigned int index);

    /* Fill in a buffer with an RGB image.
     */
    void fillFrameBuffer(unsigned char* buffer);

private:
    AVCodecContext* m_context;     // pointer to context for codec
    AVFrame* m_avframe;            // pointer to decoded frame
};


/* The VideoStream creates and manages an RTP session and handles decoding video 
 * streams into RGB images.  
 */
class VideoStream
{
public:
    /* Create a new VideoStream.  If the stream is unicast then leave the 
     * multicastIP field blank.   
     */
    VideoStream(CodecID codecType,
                unsigned int width,
                unsigned int height,
                unsigned int x, 
                unsigned int y,
                unsigned int port,
                std::string multicastIP="");

    ~VideoStream();
    
    /* Acquire a pointer to the latest video frame.  The buffer will a 24-bit RGB image.
     * The buffer must be released immediately after use.
     */
    unsigned char* acquireBuffer();

    // Release the frame buffer mutex
    void releaseBuffer();

    /* Decode a chunk of video.  buffer is a buffer of compressed video and 
     * index is the index into that buffer for new data since the last time 
     * decode was called.  
     */
    void decode(uint8_t* buffer, unsigned int index);

    std::string getMulticastIP() { return m_multicastIP; }
    unsigned int getPort() { return m_port; }
    unsigned int getWidth() { return m_width; }
    unsigned int getHeight() { return m_height; }
    unsigned int getX() { return m_x; }
    unsigned int getY() { return m_y; }
    
private:
    // Disable default constructor and copying
    VideoStream();
    VideoStream(VideoStream&);
    VideoStream& operator=(VideoStream&);

    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_port;
    std::string m_multicastIP;

    Decoder m_decoder;

    unsigned char* m_buffer;
    pthread_mutex_t m_mutex;

    pthread_t m_rtpPthread; 
};


#endif
