#include"RadeonImageFilters.h"
#include"DataType.h"
#include<string>

namespace
{
    rif_int             status  =RIF_SUCCESS;
    rif_context         context =nullptr;
    rif_command_queue   queue   =nullptr;
    rif_image_filter    filter  =nullptr;

    rif_image_desc      image_desc{};

    rif_image           input_image =nullptr;
    rif_image           output_image=nullptr;

    Size2u              image_size;

    uint8               *final_output_bitmap=nullptr;
}

bool InitRIF(const Size2u &size)
{
    image_size=size;

    int deviceCount = 0;
    status = rifGetDeviceCount(RIF_BACKEND_API_DIRECTX12, &deviceCount);
    if ( status!=RIF_SUCCESS ) return(false);

    if (deviceCount > 0 || status)
    {
        status = rifCreateContext(RIF_API_VERSION, RIF_BACKEND_API_DIRECTX12, 0, nullptr, &context);
        if (status != RIF_SUCCESS || !context) return(false);
    }

    status = rifContextCreateCommandQueue(context, &queue);
    if (status != RIF_SUCCESS || !queue) return(nullptr);;

    status = rifContextCreateImageFilter(context, RIF_IMAGE_FILTER_FILMIC_TONEMAP, &filter);
    if (status != RIF_SUCCESS)return(nullptr);

    status = rifImageFilterSetParameter1f(filter, "exposure", 1.0f/8.0f);
    if (status != RIF_SUCCESS)return(nullptr);

    status = rifImageFilterSetParameter1f(filter, "contrast", 1.0f);
    if (status != RIF_SUCCESS)return(nullptr);

    status = rifImageFilterSetParameter1u(filter, "applyToneMap", RIF_TRUE);
    if (status != RIF_SUCCESS)return(nullptr);

    //Create input and output images
    image_desc.image_width = image_size.width;
    image_desc.image_height = image_size.height;
    image_desc.num_components = 4; // rgb image
    image_desc.type = RIF_COMPONENT_TYPE_UINT8;

    final_output_bitmap=new uint8[image_size.width*image_size.height*4];

    return true;
}

void CloseRIF()
{
    delete[] final_output_bitmap;
    
    //Free resources
    rifObjectDelete(queue);

    rifObjectDelete(filter);

    rifObjectDelete(context);
}

const uint8 *RIFProcess(const uint8 *input,const Size2u &image_size)
{
    status = rifContextCreateImage(context, &image_desc, input, &input_image);
    if (status != RIF_SUCCESS)
        return(nullptr);

    status = rifContextCreateImage(context, &image_desc, nullptr, &output_image);
    if (status != RIF_SUCCESS)
        return(nullptr);

    status = rifCommandQueueAttachImageFilter(queue, filter, input_image, output_image);
    if (status != RIF_SUCCESS)
        return(nullptr);

    //execute queue
    status = rifContextExecuteCommandQueue(context, queue, nullptr, nullptr, nullptr);

    rif_image_desc desc;
    size_t retSize;
    rif_int status = rifImageGetInfo(output_image, RIF_IMAGE_DESC, sizeof(desc), &desc, &retSize);
    if (status != RIF_SUCCESS)
    {
        return(nullptr);
    }
    rif_uchar* data;
    status = rifImageMap(output_image, RIF_IMAGE_MAP_READ, (void**)&data);
    if(status==RIF_SUCCESS)
    {
        uint8 *sp=data;
        uint8 *tp=final_output_bitmap;

        for ( uint row=0;row<image_size.height;++row )
        {
            memcpy(tp,sp,image_size.width*4);

            sp+=desc.image_row_pitch;
            tp+=image_size.width*4;
        }

        status = rifImageUnmap(output_image,data);

        return final_output_bitmap;
    }

    return(nullptr);
}

void RIFEnd(void *data)
{
    rifCommandQueueDetachImageFilter(queue, filter);
    rifObjectDelete(output_image);
    rifObjectDelete(input_image);
}