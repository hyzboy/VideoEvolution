#pragma once

#include"DataType.h"
#include"FrameConvert.h"

extern "C"
{
    #include<libavcodec/avcodec.h>
}

class FrameRecviver
{
protected:

    Size2u frame_size;

    AVRational frame_rate;

public:

    virtual ~FrameRecviver()=default;

    virtual void SetFrameRate(const AVRational &fr){frame_rate=fr;}
    virtual bool OnFrame(const AVFrame *frame)=0;

public:

    const uint GetWidth()const{return frame_size.width;}
    const uint GetHeight()const{return frame_size.height;}
};//

class RGBAFrameRecviver:public FrameRecviver
{
private:

    AVPixelFormat src_format=AV_PIX_FMT_NONE;

    FrameConvert *convert=nullptr;
    
private:

    bool OnFrame(const AVFrame *frame);

public:

    virtual ~RGBAFrameRecviver()
    {
        if(convert)
            delete convert;
    }

    virtual bool OnFrameRGBA(const uint8 *)=0;
};//class RGBAFrameRecviver:public FrameRecviver
