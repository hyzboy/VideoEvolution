#pragma once

#include"DataType.h"

extern "C"
{
    #include<libavutil/pixfmt.h>
    #include<libswscale/swscale.h>
}

typedef uint8_t *FrameData[8];
typedef int FrameLinesize[8];

struct FrameFormat
{
    AVPixelFormat pixel_format;
    Size2u size;

public:

    FrameFormat()
    {
        pixel_format=AV_PIX_FMT_NONE;
        size.width=size.height=0;
    }

    FrameFormat(const AVPixelFormat &pf,const Size2u &s)
    {
        pixel_format=pf;
        size=s;
    }
};//FrameFormat

void SetFormat(FrameFormat *ff,const AVFrame *);

class FrameConvert
{
    FrameFormat src_frame_fmt;
    FrameFormat dst_frame_fmt;

    SwsContext *ctx;

    FrameData dst_data;
    FrameLinesize dst_linesize;

private:

    friend FrameConvert *InitFrameConvert(const FrameFormat &src,const FrameFormat &dst);

    FrameConvert(SwsContext *sc,const FrameFormat &src,const FrameFormat &dst);

public:

    ~FrameConvert();

    void Convert(const FrameData &src_data,const FrameLinesize &src_linesize);

    const FrameData &GetData()const{return dst_data;}
    const FrameLinesize &GetLinesize()const{return dst_linesize;}

    const uint8_t *GetData(const int index)const{return dst_data[index];}
    const int GetLinesize(const int index)const{return dst_linesize[index];}
};//class FrameConvert

FrameConvert *InitFrameConvert(const FrameFormat &src,const FrameFormat &dst);
