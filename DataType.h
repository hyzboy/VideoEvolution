#pragma once

#include<stdint.h>

using int8=int8_t;
using int16=int16_t;
using int32=int32_t;
using int64=int64_t;

using uint8=uint8_t;
using uint16=uint16_t;
using uint32=uint32_t;
using uint64=uint64_t;

using uint=unsigned int;
using uchar=unsigned char;

template<typename T> struct vec2
{
    T x,y;

public:

    vec2()
    {
        x=y=0;
    }

    vec2(const T &_x,const T &_y)
    {
        set(_x,_y);
    }

    void set(const T &_x,const T &_y)
    {
        x=_x;
        y=_y;
    }
};//template<typename T> struct vec2

using vec2i=vec2<int>;
using vec2d=vec2<double>;

#define SAFE_CLEAR(obj)     if(obj)delete obj;

template<typename T> class SafeObject
{
    T *obj;

public:

    SafeObject(T *o)
    {
        obj=o;
    }

    ~SafeObject()
    {
        SAFE_CLEAR(obj);
    }

    T *operator ->(){return obj;}
};//template<typename T> class SafeObject
