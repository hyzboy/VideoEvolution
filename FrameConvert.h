#pragma once

#include"DataType.h"

extern "C"
{
    #include<libavutil/pixfmt.h>
    #include<libswscale/swscale.h>
}

typedef uint8_t *FrameData[8];
typedef int FrameLinesize[8];

class FrameConvert
{
    AVPixelFormat src_fmt,dst_fmt;
    Size2u frame_size;

    SwsContext *ctx;

    FrameData dst_data;
    FrameLinesize dst_linesize;

private:

    friend FrameConvert *InitFrameConvert(enum AVPixelFormat dst,enum AVPixelFormat src,const Size2u &);

    FrameConvert(SwsContext *sc,enum AVPixelFormat dst,enum AVPixelFormat src,const Size2u &);

public:

    ~FrameConvert();

    void Convert(const FrameData &src_data,const FrameLinesize &src_linesize);

    const FrameData &GetData()const{return dst_data;}
    const FrameLinesize &GetLinesize()const{return dst_linesize;}

    const uint8_t *GetData(const int index)const{return dst_data[index];}
    const int GetLinesize(const int index)const{return dst_linesize[index];}
};//class FrameConvert

FrameConvert *InitFrameConvert(enum AVPixelFormat dst,enum AVPixelFormat src,const Size2u &src_size);
