#include"FrameConvert.h"
extern "C"
{
    #include<libavutil/imgutils.h>
}

FrameConvert::FrameConvert(SwsContext *sc,const FrameFormat &src,const FrameFormat &dst)
{
    ctx=sc;

    src_frame_fmt=src;
    dst_frame_fmt=dst;

    av_image_alloc(dst_data,
                   dst_linesize,
                   dst_frame_fmt.size.width,
                   dst_frame_fmt.size.height,
                   dst_frame_fmt.pixel_format,
                   1);
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
                0,src_frame_fmt.size.height,
                dst_data,dst_linesize);
}

FrameConvert *InitFrameConvert(const FrameFormat &src,const FrameFormat &dst)
{
    SwsContext *sc=sws_getContext(  src.size.width,src.size.height,src.pixel_format,
                                    dst.size.width,dst.size.height,dst.pixel_format,
                                    SWS_SPLINE,
                                    nullptr,nullptr,nullptr);

    if(!sc)return(nullptr);

    return(new FrameConvert(sc,src,dst));
}