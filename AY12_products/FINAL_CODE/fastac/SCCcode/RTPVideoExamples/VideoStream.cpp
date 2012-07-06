/* VideoStream.cpp
 *
 * Copyright iRobot 2001 - 2009. Unpublished 
 */
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdexcept>

#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>

#include <VideoStream.hpp>

/* using namespace jrtplib; */

static void* rtpSessionThread(void *arg);


Decoder::Decoder(CodecID type)
{
    // Ensure ffmpeg is initialized
    avcodec_init();
    avcodec_register_all();

    AVCodec* codec = avcodec_find_decoder(type);
    m_context = avcodec_alloc_context();
    avcodec_get_context_defaults(m_context);	// get proper defaults
    m_avframe = avcodec_alloc_frame();
    avcodec_open(m_context, codec);
}

Decoder::~Decoder()
{
    // Clean up ffmpeg
    av_free(m_avframe);
    avcodec_close(m_context);
    av_free(m_context);
}

bool Decoder::decode(uint8_t* buffer, unsigned int index)
{
    int newFrame = 0;
    int len = avcodec_decode_video(m_context, m_avframe, &newFrame, buffer, index);
    return newFrame;
}

void Decoder::fillFrameBuffer(unsigned char* buffer)
{
    AVPicture rgbpic;

    avpicture_fill(&rgbpic, buffer, PIX_FMT_RGB24,
                   m_context->width, m_context->height);

    SwsContext *img_convert_ctx = sws_getContext( m_context->width, m_context->height,m_context->pix_fmt , m_context->width, m_context->height,PIX_FMT_RGB24 , SWS_BICUBIC, NULL, NULL, NULL );
    if ( !img_convert_ctx ){
                    printf ("Unable to initialize image scaling context");
    				exit(1);
    }
    // converts to RGB so we can display it in opengl
    sws_scale( img_convert_ctx, ((AVPicture*)m_avframe)->data, ((AVPicture*)m_avframe)->linesize, 0, m_context->height, (&rgbpic)->data, (&rgbpic)->linesize );

    sws_freeContext(img_convert_ctx);
}


VideoStream::VideoStream(CodecID codecType, 
                         unsigned int width,
                         unsigned int height,
                         unsigned int x, 
                         unsigned int y,
                         unsigned int port,
                         std::string multicastIP) :
    m_width(width),
    m_height(height),
    m_x(x),
    m_y(y),
    m_port(port),
    m_multicastIP(multicastIP),
    m_decoder(codecType)
{
    pthread_mutex_init(&m_mutex, NULL);

    m_buffer = new unsigned char[3*width*height];  // RGB is 3 bytes per pixel

    // Create RTP session for video stream
    int id = pthread_create(&m_rtpPthread, NULL, rtpSessionThread, this);
    if (id < 0) {
        fprintf(stderr, "cound not create thread \n");
        exit(-1);
    }
}


VideoStream::~VideoStream()
{
    pthread_mutex_destroy(&m_mutex);
    delete [] m_buffer;
}


unsigned char* VideoStream::acquireBuffer()
{
    pthread_mutex_lock(&m_mutex);
    return m_buffer;
}


void VideoStream::releaseBuffer() 
{ 
    pthread_mutex_unlock(&m_mutex);
}


void VideoStream::decode(uint8_t* buffer, unsigned int index)
{
    int new_frame = m_decoder.decode(buffer, index);

    if (new_frame) {
        unsigned char* buf = acquireBuffer();
        m_decoder.fillFrameBuffer(buf);
        releaseBuffer();
    }
}


// Create a new RTP session.  If multicast is not being used then multicastIP
// should be set to an empty string.
static RTPSession createRtpSession(const std::string& multicastIP, 
                                   unsigned int port)
{
    RTPSession session;

    //if (setenv("LOGNAME", "video", 0) != 0) {
    //    throw std::runtime_error("Error setting LOGNAME environment variable");
    //}

    // Set up session params
    RTPSessionParams sessionparams;
    sessionparams.SetUsePollThread(false);
    sessionparams.SetMaximumPacketSize(1400);
    sessionparams.SetAcceptOwnPackets(true);
    sessionparams.SetOwnTimestampUnit(1.0f/900000.0f);
    sessionparams.SetResolveLocalHostname(false); 
    sessionparams.SetSessionBandwidth(9000000);
    sessionparams.SetReceiveMode(RTPTransmitter::AcceptAll);

    RTPUDPv4TransmissionParams transparams;
    transparams.SetPortbase(port);

    int status = session.Create(sessionparams, &transparams);
    if (status < 0) {
        throw std::runtime_error("Error creating RTP session");
    }

    // Join multicast groups if they are specified
    if (multicastIP.size() > 0) {
        if (!session.SupportsMulticasting()) {
            throw std::runtime_error("Multicast not supported!");
        } else {
            int joinip = ntohl(inet_addr(multicastIP.c_str())); 

            RTPIPv4Address joinaddr(joinip, port);
            int jstatus = session.JoinMulticastGroup(joinaddr);

            if (jstatus < 0) {
                throw std::runtime_error("Unable to join multicast group");
            }
        }
    }

    return session;
}


// Maximum possible frame sent over the network
#define MAX_FRAME_SIZE 131072

// Thread function for RTP session.
static void* rtpSessionThread(void *arg)
{
    VideoStream* video = reinterpret_cast<VideoStream*>(arg);

    u_int8_t bigBuffer[MAX_FRAME_SIZE];

    unsigned int lastPacketTimestamp = 0;
    unsigned int currentIndex = 0;
    double last_time = 0;

    RTPSession session = createRtpSession(video->getMulticastIP(), 
                                          video->getPort());

    while (1) {
        session.Poll();

        // Distribute data from the session to connected clients if we've
        // got anything
        session.BeginDataAccess();

        if (session.GotoFirstSourceWithData()) {
            do {
                RTPPacket *packet = NULL;

                while ((packet = session.GetNextPacket()) != NULL) {
                    if ((packet->GetPayloadLength() > sizeof(bigBuffer)) || 
                        (packet->GetPayloadLength() == 0)) {
                        // Free the packet, we're not going to use it.
                        session.DeletePacket(packet);
                        continue; // Exit this level of the loop and drop it
                    }

                    // Check timestamps for new data.  A new timestamp means
                    // this is from a different time.
                    if (packet->GetTimestamp() != lastPacketTimestamp) {
                        video->decode((uint8_t*)&bigBuffer[0], currentIndex);

                        currentIndex = 0;
                        memset(&bigBuffer[0], 0, sizeof(bigBuffer));
                    } // End new timestamp optimization.


                    // Copy data into buffer
                    if (currentIndex + packet->GetPayloadLength() > sizeof(bigBuffer)) {
                        throw std::runtime_error("Frame buffer overflow");
                    }

                    memcpy(&bigBuffer[currentIndex], packet->GetPayloadData(),
                           packet->GetPayloadLength());
                    currentIndex += packet->GetPayloadLength();

                    // Mark our last timestamp
                    lastPacketTimestamp = packet->GetTimestamp();

                    // Free the packet.
                    session.DeletePacket(packet);
                } 
            } while (session.GotoNextSourceWithData()); 
        }

        session.EndDataAccess();
        RTPTime delay(0, 100); // 100usec

        // Update More Data
        bool moreData;
        session.WaitForIncomingData(delay, &moreData);
    }

    // Leave the session while sending BYE.
    RTPTime timeout(0.75f); //  Wait briefly.
    const char* reason = "Session Destroyed.";
    unsigned int reasonlen = strlen(reason);

    if (session.IsActive())
        session.BYEDestroy(timeout, reason, reasonlen);
}  
