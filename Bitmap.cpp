#include"Bitmap.h"
#include"stb_image.h"
#include<time.h>
#include<io.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<math.h>
#include<omp.h>

#include<stdio.h>

#pragma pack(push,1)
    struct TGAHeader
    {
        uint8 id;
        uint8 color_map_type;
        uint8 image_type;               // 1 colormap image ,2 true-color,3 grayscale

        uint16 color_map_first;
        uint16 color_map_length;
        uint8 color_map_size;

        uint16 x_origin;
        uint16 y_origin;

        uint16 width;
        uint16 height;
        uint8 bit;

        union
        {
            uint8 image_desc;
            struct
            {
                uint alpha_depth:4;
                uint reserved:1;
                uint direction:1;       //0 lower-left,1 upper left
            };
        };
    };
#pragma pack(pop)

bool SaveToTGA(const char *filename,void *data,const uint16 width,const uint16 height,const uint8 bit,const bool flip)
{
    int fp;
    
    _sopen_s(&fp,filename,_O_BINARY|_O_WRONLY|_O_CREAT|_O_TRUNC,_SH_DENYNO,S_IREAD|_S_IWRITE);

    if(fp==-1)return(false);

    TGAHeader header;

    memset(&header,0,sizeof(TGAHeader));

    if(bit==8)
        header.image_type=3;
    else
        header.image_type=2;

    header.width=width;
    header.height=height;
    header.bit=bit;
    header.alpha_depth=8;
    header.direction=flip?1:0;

    _write(fp,&header,sizeof(TGAHeader));
    _write(fp,data,width*height*(bit>>3));

    _close(fp);

    printf("Save Screen to %s\n",filename);

    return(true);
}

Bitmap8 *ConvertToU8(const Bitmap1f *src)
{
    const uint pixel_total=src->GetWidth()*src->GetHeight();

    Bitmap8 *result_bmp=new Bitmap8(src->GetWidth(),src->GetHeight());

    const float *src_color=src->GetData();
    uint8 *tar_color=result_bmp->GetData();

    for(uint i=0;i<pixel_total;i++)
    {
        *tar_color=uint8((*src_color)*255.0f);
        ++tar_color;
        ++src_color;
    }

    return result_bmp;
}
