extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/opt.h>
}

#include"VideoEncoder.h"
#include"libyuv/convert_from_argb.h"
#include<iostream>
#include<queue>

namespace
{
    struct EncodecName_by_ID
    {
        AVCodecID id;
        char name[32];
    };

    constexpr EncodecName_by_ID encodec_name_by_id[]=
    {
        {AV_CODEC_ID_H264       ,"h264_nvenc"},
        {AV_CODEC_ID_H264       ,"nvenc"},
        {AV_CODEC_ID_H264       ,"nvenc_h264"},
        

        {AV_CODEC_ID_HEVC       ,"hevc_nvenc"},
        {AV_CODEC_ID_HEVC       ,"nvenc_hevc"},

        {AV_CODEC_ID_H264       ,"h264_amf"},
        {AV_CODEC_ID_HEVC       ,"hevc_amf"},

        {AV_CODEC_ID_MJPEG      ,"mjpeg_qsv"},
        {AV_CODEC_ID_MPEG2VIDEO ,"mpeg2_qsv"},
        {AV_CODEC_ID_H264       ,"h264_qsv"},
        {AV_CODEC_ID_HEVC       ,"hevc_qsv"},
        {AV_CODEC_ID_VP9        ,"vp9_qsv"},

        {AV_CODEC_ID_NONE       ,""}
    };

    const AVCodec *GetAVEncodec(AVCodecID id,bool hardware)
    {
        //if(hardware)
        //{
            const AVCodec *codec=nullptr;

            for(auto &c:encodec_name_by_id)
            {
                if(c.id==id)
                {
                    codec=avcodec_find_encoder_by_name(c.name);

                    if(codec)
                    {
                        std::cout<<"use encoder: "<<c.name<<std::endl;
                        return codec;
                    }
                    else
                    {
                        std::cerr<<"don't support encoder: "<<c.name<<std::endl;
                    }
                }
            }
        //}

        return avcodec_find_encoder(id);
    }
}//namespace

class FFMPEGVideoEncoder:public VideoEncoder
{
    const AVCodec *codec;

    AVFrame *frame;

    AVStream *video_stream=nullptr;
    uint pts;

    const AVOutputFormat *out_fmt;

    AVPacket *packet;

    int m_video_stream_index = -1;
    int count = -1;

    bool b_initstream = false;

public:

    FFMPEGVideoEncoder(const char *fn,const AVCodec *eco,const uint br):VideoEncoder(fn,br)
    {
        codec=eco;

        frame=av_frame_alloc();
        codec_ctx=avcodec_alloc_context3(codec);
        
        out_fmt=av_guess_format(nullptr,filename,nullptr);

        if(avformat_alloc_output_context2(&fmt_ctx,out_fmt,nullptr,filename)<0)
        {
            std::cerr<<"alloc output context2 failed"<<std::endl;
            return;
        }

        pts=0;

        packet=av_packet_alloc();
    }

    ~FFMPEGVideoEncoder()
    {
        Finish();

        if(frame)
            av_frame_free(&frame);

        if(codec_ctx)
            avcodec_free_context(&codec_ctx);

        avformat_free_context(fmt_ctx);
    }

    void SetFrameRateSize(const AVRational &fr,const Size2u &s) override
    {
        VideoEncoder::SetFrameRateSize(fr,s);

        codec_ctx->bit_rate     =bit_rate;
        codec_ctx->width        =s.width;
        codec_ctx->height       =s.height;
        codec_ctx->framerate    =fr;
        codec_ctx->time_base.den=fr.num;
        codec_ctx->time_base.num=fr.den;
        codec_ctx->gop_size     =25;
        codec_ctx->max_b_frames =3;
        codec_ctx->pix_fmt      =AV_PIX_FMT_NV12;
        codec_ctx->codec_type   =AVMEDIA_TYPE_VIDEO;

        video_stream=avformat_new_stream(fmt_ctx,codec);
        video_stream->codecpar->codec_id    =fmt_ctx->video_codec_id;
        video_stream->codecpar->codec_type  =AVMEDIA_TYPE_VIDEO;
        video_stream->codecpar->width       =s.width;
        video_stream->codecpar->height      =s.height;
        video_stream->codecpar->format      =codec_ctx->pix_fmt;
        video_stream->codecpar->bit_rate    =bit_rate;
        video_stream->time_base             =codec_ctx->time_base;

        m_video_stream_index = 0;
    }

    bool Init(int pass) override
    {
        avcodec_parameters_to_context(codec_ctx,video_stream->codecpar);
        
        av_opt_set(codec_ctx->priv_data,"preset","veryslow",0);
        av_opt_set(codec_ctx->priv_data,"crf","10",0);

        if(pass==1)
        {
            av_opt_set(codec_ctx->priv_data, "pass", "1", 0);
            av_opt_set(codec_ctx->priv_data, "stats_file", "ffmpeg2pass-0.log", 0);
        }
        else
        {
            av_opt_set(codec_ctx->priv_data, "pass", "2", 0);
            av_opt_set(codec_ctx->priv_data, "stats_file", "ffmpeg2pass-0.log", 0);
        }

        if(fmt_ctx->oformat->flags&AVFMT_GLOBALHEADER)
            codec_ctx->flags|=AV_CODEC_FLAG_GLOBAL_HEADER;

        if(avcodec_open2(codec_ctx,codec,nullptr)<0)
        {
            std::cerr<<"avcodec open failed!"<<std::endl;
            return(false);
        }

        avcodec_parameters_from_context(video_stream->codecpar,codec_ctx);

        if(avio_open2(&fmt_ctx->pb,filename,AVIO_FLAG_WRITE,nullptr,nullptr)<0)
        {
            std::cerr<<"avio open failed!"<<std::endl;
            return(false);
        }

        if(avformat_write_header(fmt_ctx,nullptr)<0)
        {
            std::cerr<<"write header failed"<<std::endl;
            return(false);
        }

        av_dump_format(fmt_ctx,0,filename,1);

        frame->format   =codec_ctx->pix_fmt;
        frame->width    =codec_ctx->width;
        frame->height   =codec_ctx->height;
    
        int ret = av_frame_get_buffer(frame, 0);

        if(ret<0)
        {
            std::cerr<<"av frame get buffer failed!"<<std::endl;
            return(false);
        }

        b_initstream = true;
        return(true);
    }

    bool encode(AVFrame* _frame)
    {
        int ret=avcodec_send_frame(codec_ctx, _frame);

        if(ret<0)
        {
            std::cerr<<"failed to send frame: "<<ret<<std::endl;
            return(false);
        }

        while(ret>=0)
        {
            ret=avcodec_receive_packet(codec_ctx,packet);

            if(ret==AVERROR(EAGAIN)||ret==AVERROR_EOF)
            {
                return(true);
            }
            else
            if(ret<0)
            {
                return(false);
            }

            if(ret==0)
            {
                count++;
                if (count % codec_ctx->gop_size == 0)
                    packet->flags |= AV_PKT_FLAG_KEY;

                if(packet->pts!=AV_NOPTS_VALUE)
                    packet->pts=av_rescale_q(packet->pts,codec_ctx->time_base,video_stream->time_base);
                if(packet->dts!=AV_NOPTS_VALUE)
                    packet->dts=av_rescale_q(packet->dts,codec_ctx->time_base,video_stream->time_base);

                packet->stream_index = m_video_stream_index;
                packet->duration = av_rescale_q(1, codec_ctx->time_base, video_stream->time_base);
                packet->pos = -1;

                std::cout<<'.';

                if(av_interleaved_write_frame(fmt_ctx,packet)<0)
                    return(false);

                av_packet_unref(packet);
            }
        }
        
        return(true);
    }

    bool WriteFrame(const uint8 *rgba_data) override
    {
        int ret=av_frame_make_writable(frame);

        if(ret<0)
            return(false);

        libyuv::ABGRToNV12( rgba_data,codec_ctx->width*4,
                            frame->data[0],frame->linesize[0],
                            frame->data[1],frame->linesize[1],
                            codec_ctx->width,codec_ctx->height);

        frame->pts=pts;
        ++pts;

        return encode(frame);
    }

    bool Finish() override
    {
        if(!encode(NULL))
            return(false);

        av_write_trailer(fmt_ctx);
        avio_close(fmt_ctx->pb);
        return(true);
    }
};//class FFMPEGVideoEncoder:public VideoEncoder

VideoEncoder *CreateVideoEncoder(const char *filename,const uint bit_rate,const bool use_hardware)
{
    constexpr AVCodecID codec_list[]
    {
        AV_CODEC_ID_H264,
        AV_CODEC_ID_HEVC,
        AV_CODEC_ID_VP9,
        AV_CODEC_ID_AV1
    };

    const AVCodec *codec=nullptr;
    
    for(AVCodecID id:codec_list)
    {
        codec=GetAVEncodec(id,use_hardware);
        if(codec)break;
    }

    if(!codec)
    {
        std::cerr<<"We didn't find a encoder[H264,HEVC,VP9,AV1]"<<std::endl;
        return nullptr;
    }

    return(new FFMPEGVideoEncoder(filename,codec,bit_rate));
}
