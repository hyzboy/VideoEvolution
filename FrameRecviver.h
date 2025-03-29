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

    uint width=0;
    uint height=0;

    AVRational frame_rate;

public:

    virtual ~FrameRecviver()=default;

    virtual void SetFrameRate(const AVRational &fr){frame_rate=fr;}
    virtual bool OnFrame(const AVFrame *frame)=0;

public:

    const uint GetWidth()const{return width;}
    const uint GetHeight()const{return height;}
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
