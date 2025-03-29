#include"FrameRecviver.h"
#include<iostream>

extern "C"
{
    #include<libavutil/pixdesc.h>
}

bool RGBAFrameRecviver::OnFrame(const AVFrame *frame)
{
    if(src_format==AV_PIX_FMT_NONE)
    {
        frame_size.width=frame->width;
        frame_size.height=frame->height;

        src_format=AVPixelFormat(frame->format);

        std::cout<<"size: "<<frame_size.width<<"x"<<frame_size.height<<std::endl
                 <<"format: "<<av_get_pix_fmt_name(src_format)<<std::endl;

        if(src_format!=AV_PIX_FMT_RGBA)
            convert=InitFrameConvert(AV_PIX_FMT_RGBA,src_format,frame_size);
    }

    if(src_format==AV_PIX_FMT_RGBA)
    {
        return OnFrameRGBA((uint8 *)(frame->data[0]));
    }
    else
    {
        convert->Convert(frame->data,frame->linesize);

        return OnFrameRGBA(convert->GetData(0));
    }
}
