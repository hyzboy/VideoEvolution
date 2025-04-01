#include <iostream>
#include"VideoEncoder.h"
#include"VideoDecoder.h"
#include"FrameRecviver.h"
#include<stdlib.h>

bool InitRIF(const Size2u &);
void CloseRIF();
const uint8 *RIFProcess(const uint8 *input,const Size2u &image_size);
void RIFEnd(void *);

constexpr uint32_t ALIGN_PIXELS=8;

const uint32_t GetAlignValue(const uint32_t value)
{
    constexpr uint32_t tmp=~(ALIGN_PIXELS-1);

    return (value+ALIGN_PIXELS-1)&tmp;
}

class EvoFrameRecviver:public RGBAFrameRecviver
{
    VideoEncoder *rgb_encoder=nullptr;

    uint new_height=0;

    bool frame_init=false;

public:

    EvoFrameRecviver(VideoEncoder *rgb,const uint nh)
    {
        rgb_encoder=rgb;
        new_height=nh;
    }

    ~EvoFrameRecviver()
    {
        CloseRIF();

        rgb_encoder->Finish();
    }

    Size2u ComputeDstFrameSize(const Size2u &src_size) override
    {
        Size2u result;

        if(new_height>0)
        {
            result.height  =GetAlignValue(new_height);

            const double scale=double(new_height)/double(src_size.height);

            result.width   =GetAlignValue(double(src_size.width)*scale);
        }
        else
        {
            result.width   =GetAlignValue(src_size.width);
            result.height  =GetAlignValue(src_size.height);
        }

        rgb_encoder->SetFrameRateSize(frame_rate,result);

        frame_init=rgb_encoder->Init();

        InitRIF(result);

        return result;
    }

    bool OnFrameRGBA(const uint8 *rgba_data) override
    {
        const uint8 *output=RIFProcess(rgba_data,GetTargetFrameSize());

        bool result=rgb_encoder->WriteFrame(output);

        RIFEnd((void *)output);

        return result;
    }
};

uint32_t ConvertMovie(const char *src,const char *rgb,const uint32_t new_height,const uint32_t bit_rate,const bool use_hardware)
{    
    VideoEncoder *video_encoder=CreateVideoEncoder(rgb,bit_rate,false);
    FrameRecviver *frame_recv=new EvoFrameRecviver(video_encoder,new_height);
    VideoDecoder *video_decoder=CreateVideoDecoder(src,frame_recv,use_hardware);

    uint32_t frame_count=0;

    {
        video_decoder->Start();

        while(video_decoder->NextFrame())
            ++frame_count;

        video_encoder->Finish();
    }

    delete video_decoder;
    delete frame_recv;
    delete video_encoder;

    return frame_count;
}

bool Convert(const char *src,const char *rgb,const uint32_t bit_rate,const uint32_t new_height)
{
    std::cout<<"            input: "<<src<<std::endl;
    std::cout<<"      output(rgb): "<<rgb<<std::endl;
    std::cout<<"         bit_rate: "<<bit_rate<<std::endl;
    std::cout<<"       new height: "<<new_height<<std::endl;

    uint32_t frame_count;

    frame_count=ConvertMovie(src,rgb,new_height,bit_rate,true);

    if(frame_count==0)
    {
        std::cerr<<"first decoder/encoder failed, try use software decoder/encoder"<<std::endl;

        frame_count=ConvertMovie(src,rgb,new_height,bit_rate,false);
    }
        
    std::cout<<std::endl;

    if(frame_count>0)
    {
        std::cout<<"Movie Encoder Finished!"<<std::endl;
        std::cout<<"Total frame: "<<frame_count<<std::endl;

        return true;
    }
    else
    {
        std::cout<<"Movie Encoder Failed!"<<std::endl;

        return false;
    }
}

int main(int argc,char **argv)
{
    std::cout << "VideoEvolution\n";

    if(argc<5)
    {
        std::cout<<"Format:  VideoEvo [input] [output] [bit rate] [new height]\n";
        std::cout<<"Example: VideoEvo input.mp4 output.mp4 1048576 480\n\n";
        return 0;
    }

    long bit_rate=atol(argv[3]);
    long new_height;

    if(argc<5)
        new_height=0;
    else
        new_height=atol(argv[4]);

    Convert(argv[1],argv[2],bit_rate,new_height);

    return 0;
}