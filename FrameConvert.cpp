#include"FrameConvert.h"
extern "C"
{
    #include<libavutil/imgutils.h>
}

FrameConvert::FrameConvert(SwsContext *sc,enum AVPixelFormat dst,enum AVPixelFormat src,const Size2u &src_size)
{
    ctx=sc;

    dst_fmt=dst;
    src_fmt=src;

    frame_size=src_size;

    av_image_alloc(dst_data,dst_linesize,frame_size.width,frame_size.height,dst,1);
}

FrameConvert::~FrameConvert()
{
    av_freep(&dst_data[0]);
    sws_freeContext(ctx);
}

void FrameConvert::Convert(const FrameData &src_data,const FrameLinesize &src_linesize)
{
    sws_scale(  ctx,
                src_data,src_linesize,
                0,frame_size.height,
                dst_data,dst_linesize);
}

FrameConvert *InitFrameConvert(enum AVPixelFormat dst,enum AVPixelFormat src,const Size2u &src_size)
{
    SwsContext *sc=sws_getContext(  src_size.width,src_size.height,src,
                                    src_size.width,src_size.height,dst,
                                    SWS_SPLINE,
                                    nullptr,nullptr,nullptr);

    if(!sc)return(nullptr);

    return(new FrameConvert(sc,dst,src,src_size));
}