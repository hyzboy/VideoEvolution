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

    FrameFormat src_format;
    FrameFormat dst_format;

    AVRational frame_rate;

public:

    FrameRecviver(const AVPixelFormat &dst_pf)
    {
        dst_format.pixel_format=dst_pf;
    }
    virtual ~FrameRecviver()=default;

    virtual void SetFrameRate(const AVRational &fr){frame_rate=fr;}

    virtual bool OnFrame(const AVFrame *frame)=0;

public:

    const Size2u &GetSourceFrameSize()const{return src_format.size;}
    const Size2u &GetTargetFrameSize()const{return dst_format.size;}

    virtual void SetPass(int)=0;

    virtual Size2u ComputeDstFrameSize(const Size2u &src_size)
    {
        return src_size;    //这样等于不缩放
    }
};//

class RGBAFrameRecviver:public FrameRecviver
{
private:

    FrameConvert *convert=nullptr;
    
private:

    bool OnFrame(const AVFrame *frame);

public:

    RGBAFrameRecviver():FrameRecviver(AV_PIX_FMT_RGBA){}
    virtual ~RGBAFrameRecviver()
    {
        if(convert)
            delete convert;
    }

    virtual bool OnFrameRGBA(const uint8 *)=0;
};//class RGBAFrameRecviver:public FrameRecviver
