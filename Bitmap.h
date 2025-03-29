#pragma once

#include"DataType.h"
#include<malloc.h>
#include<string.h>
#include<string>

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define TILE_SCALE_MOVE_BIT 8

bool SaveToTGA(const char *filename,void *data,const uint16 width,const uint16 height,const uint8 bit,const bool flip);

template<typename D,uint N> class Bitmap
{
    int width,height;

    uint32 line_bytes;
    uint32 total_bytes;

    D *data;

public:

    const int       GetWidth    ()const{return width;}
    const int       GetHeight   ()const{return height;}
    const int       GetColorByte()const{return N;}
          D *       GetData     ()const{return data;}

    const uint32    GetLineBytes()const{return line_bytes;}
    const uint32    GetTotalBytes()const{return total_bytes;}

public:

    Bitmap()
    {
        width=height=0;
        line_bytes=total_bytes=0;
        data=nullptr;
    }

    Bitmap(int w,int h)
    {
        if(w>0&&h>>0)
        {
            width=w;
            height=h;

            line_bytes=width*N*sizeof(D);
            total_bytes=line_bytes*height;

            data=new D[width*height*N];

            memset(data,0,total_bytes);
        }
        else
        {
            width=height=0;
            line_bytes=total_bytes=0;
            data=nullptr;
        }
    }

    ~Bitmap()
    {
        delete[] data;
    }

    void Clear()
    {
        if(!data)return;

        memset(data,0,total_bytes);
    }

    void CopyFrom(const Bitmap<D,N> *bmp)
    {
        if(!bmp)return;
        if(width!=bmp->width)return;
        if(height!=bmp->height)return;

        memcpy(data,bmp->data,total_bytes);
    }

    D *GetPointer(const int row)
    {
        if(row<0||row>=height)return(nullptr);

        return data+(row*width)*N;
    }

    D *GetPointer(const int x,const int y)
    {
//         if(x<0||x>=width)return(nullptr);
//         if(y<0||y>=height)return(nullptr);

        return data+(x+(y*width))*N;
    }

    bool GetPixel(D *color,const int x,const int y) const
    {
        if(x<0||x>=width
         ||y<0||y>=height)return(false);

        memcpy(color,data+(y*width+x)*N,N*sizeof(D));
        return(true);
    }

    void GetPixelByFloatPos(D *color,const float fx,const float fy) const
    {
        const uint x=fx*width;
        const uint y=fy*height;

        memcpy(color,data+(y*width+x)*N,N*sizeof(D));
    }

    void SaveToTGA(const std::string &filename,const bool flip)
    {
        ::SaveToTGA(filename.c_str(),data,width,height,N<<3,flip);
    }
};//template<typename N> class Bitmap

using Bitmap8=Bitmap<uint8,1>;
using Bitmap16=Bitmap<uint8,2>;
using Bitmap24=Bitmap<uint8,3>;
using Bitmap32=Bitmap<uint8,4>;

using Bitmap1f=Bitmap<float,1>;
using Bitmap3f=Bitmap<float,3>;

Bitmap8 *ConvertToU8(const Bitmap1f *);

