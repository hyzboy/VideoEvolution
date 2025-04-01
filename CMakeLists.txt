CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(VideoEvolution)

find_package(libyuv CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/RadeonImageFilter/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/RadeonImageFilter/Windows/Dynamic-MT)

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

add_compile_options("/MP")

add_executable(VideoEvolution  main.cpp
                                DataType.h
                                RIFSupport.cpp
                                #${AUDIO_SOURCE}
                                ${VIDEO_SOURCE} ${IMAGE_SOURCE})

target_link_libraries(VideoEvolution PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avcodec.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avdevice.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avfilter.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avformat.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/avutil.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/postproc.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/swresample.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/swscale.lib
    yuv
    RadeonImageFilters
)
