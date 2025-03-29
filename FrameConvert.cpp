#include"FrameConvert.h"
extern "C"
{
    #include<libavutil/imgutils.h>
}

FrameConvert::FrameConvert(SwsContext *sc,enum AVPixelFormat dst,enum AVPixelFormat src,const uint32_t w,const uint32_t h)
{
    ctx=sc;

    dst_fmt=dst;
    src_fmt=src;

    width=w;
    height=h;

    av_image_alloc(dst_data,dst_linesize,w,h,dst,1);
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
                0,height,
                dst_data,dst_linesize);
}

FrameConvert *InitFrameConvert(enum AVPixelFormat dst,enum AVPixelFormat src,const uint32_t w,const uint32_t h)
{
    SwsContext *sc=sws_getContext(  w,h,src,
                                    w,h,dst,
                                    SWS_FAST_BILINEAR,
                                    nullptr,nullptr,nullptr);

    if(!sc)return(nullptr);

    return(new FrameConvert(sc,dst,src,w,h));
}