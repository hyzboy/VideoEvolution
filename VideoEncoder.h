#pragma once
#include"DataType.h"
extern "C"
{
#include <libavformat/avformat.h>
    #include<libavcodec/avcodec.h>
}

class EncodeOutput
{
public:

    virtual ~EncodeOutput()=default;

    virtual bool Write(const void *,const uint)=0;
};//class EncodeOutput

class VideoEncoder
{
protected:

    AVFormatContext* fmt_ctx;
    AVCodecContext* codec_ctx;

    Size2u frame_size;
    AVRational frame_rate;
    uint bit_rate;

    char filename[_MAX_PATH];

public:

    VideoEncoder(const char *fn,const uint br)
    {
        strcpy_s(filename,_MAX_PATH,fn);

        bit_rate=br;
    }

    AVFormatContext *GetFrmCtx(){return fmt_ctx;}
    AVCodecContext *GetCodecCtx(){return codec_ctx;}

    virtual ~VideoEncoder()=default;

    virtual void SetFrameRateSize(const AVRational &fr,const Size2u &size)
    {
        frame_size=size;
        frame_rate=fr;
    }

    virtual bool Init(int pass)=0;

    virtual bool WriteFrame(const uint8 *rgba_data)=0;

    virtual bool Finish()=0;
};//class VideoEncoder

VideoEncoder *CreateVideoEncoder(const char *filename,const uint bit_rate,const bool);
