#pragma once

#include"Bitmap.h"
extern "C"
{
    #include<libavcodec/avcodec.h>
}

class FrameRecviver;
class AudioDecoder;

/**
 * 解码器
 */
class VideoDecoder
{
protected:

    AVFormatContext *ctx;

    int width,height;

public:

    const int       GetWidth    ()const{return width;}
    const int       GetHeight   ()const{return height;}

    AVFormatContext *GetFrmCtx() {return ctx;};

public:

    virtual ~VideoDecoder()=default;

    virtual void Start()=0;                                                     ///<开始解码
    virtual bool NextFrame()=0;                                                 ///<获取一帧图片

    void Abort();                                                               ///<强退
};//class VideoDecoder

VideoDecoder *CreateVideoDecoder(const char *,FrameRecviver *,const bool);
