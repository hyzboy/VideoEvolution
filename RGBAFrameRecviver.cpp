#include"FrameRecviver.h"
#include<iostream>

extern "C"
{
    #include<libavutil/pixdesc.h>
}

namespace
{
    void out(const char *str,const FrameFormat &ff)
    {
        std::cout<<str<<"_size:   "<<ff.size.width<<"x"<<ff.size.height<<std::endl
                 <<str<<"_format: "<<av_get_pix_fmt_name(ff.pixel_format)<<std::endl;
    }
}//namespace

bool RGBAFrameRecviver::OnFrame(const AVFrame *frame)
{
    if(!convert)
    {
        src_format.pixel_format =AVPixelFormat(frame->format);
        src_format.size.width   =frame->width;
        src_format.size.height  =frame->height;

        dst_format.size=ComputeDstFrameSize(src_format.size);

        out("src",src_format);
        out("dst",dst_format);

        convert=InitFrameConvert(src_format,dst_format);
    }

    convert->Convert(frame->data,frame->linesize);

    return OnFrameRGBA(convert->GetData(0));
}
