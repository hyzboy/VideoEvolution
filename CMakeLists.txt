CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(VideoEvolution)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include)

FILE(GLOB_RECURSE LIBYUV_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include/libyuv/*.h)
FILE(GLOB_RECURSE LIBYUV_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/source/*.cc)

SOURCE_GROUP("libyuv\\Header" FILES ${LIBYUV_HEADER})
SOURCE_GROUP("libyuv\\Source" FILES ${LIBYUV_SOURCE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include)

SET(VIDEO_SOURCE    VideoDecoder.cpp
                    VideoDecoder.h
                    VideoEncoder.cpp
                    VideoEncoder.h
                    FrameConvert.cpp
                    FrameConvert.h
                    FrameRecviver.h
                    RGBAFrameRecviver.cpp
                    )

SET(IMAGE_SOURCE    Bitmap.cpp
                    Bitmap.h
                    stb_image.h)

SOURCE_GROUP("Audio" FILES ${AUDIO_SOURCE})
SOURCE_GROUP("Video" FILES ${VIDEO_SOURCE})
SOURCE_GROUP("Image" FILES ${IMAGE_SOURCE})

add_executable(VideoEvolution  main.cpp
                                DataType.h
                                #${AUDIO_SOURCE}
                                ${VIDEO_SOURCE} ${IMAGE_SOURCE}
                                ${LIBYUV_HEADER} ${LIBYUV_SOURCE})

target_link_libraries(VideoEvolution
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avcodec.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avdevice.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avfilter.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avformat.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avutil.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/postproc.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/swresample.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/swscale.lib)
