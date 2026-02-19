cmake_minimum_required(VERSION 3.16)

project(ffmpeg_player LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ffmpeg_player main.cpp
    Thread.h Thread.cpp
    DemuxThread.h DemuxThread.cpp
    Queue.h
    AVPacketQueue.h AVPacketQueue.cpp
    AVFrameQueue.h AVFrameQueue.cpp
    DecodeThread.h DecodeThread.cpp
    AudioOutput.h AudioOutput.cpp
    VideoOutput.h VideoOutput.cpp
    AVSync.h
)

########################现阶段不可移植，后续更改#######################
#ffmpeg根目录
set(FFMPEG_DIR ${CMAKE_SOURCE_DIR}/third_party/ffmpeg)
#SDL2根目录
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/third_party/SDL2)

#ffmpeg头文件
target_include_directories(ffmpeg_player
    PRIVATE
        ${FFMPEG_DIR}/include
)
#SDL2头文件
target_include_directories(ffmpeg_player
    PRIVATE
        ${SDL2_DIR}/include
)

#ffmpeg链接库
target_link_libraries(ffmpeg_player
    PRIVATE
        ${FFMPEG_DIR}/lib/libavcodec.dll.a
        ${FFMPEG_DIR}/lib/libavformat.dll.a
        ${FFMPEG_DIR}/lib/libavutil.dll.a
        ${FFMPEG_DIR}/lib/libswresample.dll.a
        ${FFMPEG_DIR}/lib/libavdevice.dll.a
        ${FFMPEG_DIR}/lib/libavfilter.dll.a
        ${FFMPEG_DIR}/lib/libswscale.dll.a
)
#SDL2链接库
target_link_libraries(ffmpeg_player
    PRIVATE
        ${SDL2_DIR}/lib/libSDL2main.a
        ${SDL2_DIR}/lib/libSDL2.dll.a
)

include(GNUInstallDirs)
install(TARGETS ffmpeg_player
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
