cmake_minimum_required(VERSION 3.4.1)
include(ndk-stl-config.cmake)
# 如果存在airplay 则编译airplay库
if (EXISTS "${CMAKE_HOME_DIRECTORY}/airplay")
    message("=========================== airplay source ===========================")
    include_directories(
            airplay/ed25519
            airplay/curve25519
            airplay/crypt
            airplay/explay
            airplay/explay/alac
            airplay/explay/aac_eld
            airplay/fdk/libAACdec/include
            airplay/fdk/libAACenc/include
            airplay/fdk/libPCMutils/include
            airplay/fdk/libFDK/include
            airplay/fdk/libSYS/include
            airplay/fdk/libMpegTPDec/include
            airplay/fdk/libMpegTPEnc/include
            airplay/fdk/libSBRdec/include
            airplay/fdk/libSBRenc/include
            airplay/fdk/libFDK/aarch64/include
            airplay/mdns/mDNSShared
            airplay/mdns/mDNSCore
            airplay/mdns/mDNSPosix
            airplay/plist/src
            airplay/plist/libcnary
            airplay/plist/include
            airplay/dmr
            ./
    )

    #    airplay 加解密相关
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/crypt crypt)
    file(GLOB_RECURSE curve25519 "${CMAKE_HOME_DIRECTORY}/airplay/curve25519/curve25519-donna.c")
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/dmr dmr)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/ed25519 ed25519)
    #    airplay 音频解码
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk fdk)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libAACdec/src fdk1)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libAACenc/src fdk2)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libFDK/src fdk3)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libMpegTPDec/src fdk4)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libMpegTPEnc/src fdk5)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libPCMutils/src fdk6)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libSBRdec/src fdk7)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libSBRenc/src fdk8)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/fdk/libSYS/src fdk9)
    #    airplay mdns发现相关
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/mdns/mDNSCore mdnsCore)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/mdns/mDNSPosix mdnsPosix)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/mdns/mDNSShared mdnsShared)
    #    airplay plist 解码
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/plist/libcnary plist)
    aux_source_directory(${CMAKE_HOME_DIRECTORY}/airplay/plist/src plist1)
    file(GLOB_RECURSE explay "${CMAKE_HOME_DIRECTORY}/airplay/explay/*.c")


    list(APPEND SRC_LIST ${crypt})
    list(APPEND SRC_LIST ${curve25519})
    list(APPEND SRC_LIST ${dmr})
    list(APPEND SRC_LIST ${ed25519})
    list(APPEND SRC_LIST ${fdk1})
    list(APPEND SRC_LIST ${fdk2})
    list(APPEND SRC_LIST ${fdk3})
    list(APPEND SRC_LIST ${fdk4})
    list(APPEND SRC_LIST ${fdk5})
    list(APPEND SRC_LIST ${fdk6})
    list(APPEND SRC_LIST ${fdk7})
    list(APPEND SRC_LIST ${fdk8})
    list(APPEND SRC_LIST ${fdk9})
    list(APPEND SRC_LIST ${mdnsCore})
    list(APPEND SRC_LIST ${mdnsPosix})
    list(APPEND SRC_LIST ${mdnsShared})
    list(APPEND SRC_LIST ${plist})
    list(APPEND SRC_LIST ${plist1})
    list(APPEND SRC_LIST ${explay})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

    # make exairplay only
    add_library(
            exairplay
            SHARED
            ${SRC_LIST})


    find_library(log-lib
            log)


    target_link_libraries(exairplay
            ${log-lib})
endif ()
if (EXISTS "${CMAKE_HOME_DIRECTORY}/dlna")
    message("=========================== dlna source ===========================")
    #  添加头文件
    include_directories(
            ${CMAKE_HOME_DIRECTORY}/native-lib.h
            dlna/Platinum/Platinum
            dlna/Platinum/Core
            dlna/Platinum/Devices/MediaServer
            dlna/Platinum/Devices/MediaConnect
            dlna/Platinum/Devices/MediaRenderer
            dlna/Platinum/Extras
            dlna/Neptune/Core
            dlna/
            )
    file(GLOB_RECURSE DLNA_SRC "${CMAKE_HOME_DIRECTORY}/dlna/*.cpp")
    list(APPEND DLNASRC ${DLNA_SRC})
    #    设置生成.so目录
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
    add_library(
            exdlna
            SHARED
            ${DLNASRC})

    find_library(log-lib
            log)


    target_link_libraries(exdlna
            ${log-lib})
endif ()
#本地库
#    设置库引入路径
LINK_DIRECTORIES(../jniLibs/${ANDROID_ABI})

include_directories(
        ${CMAKE_HOME_DIRECTORY}/*.h
)
file(GLOB_RECURSE NATIVE_SRC "${CMAKE_HOME_DIRECTORY}/native-lib.cpp")


add_library(
        native-lib
        SHARED
        ${NATIVE_SRC})


find_library(log-lib
        log)

target_link_libraries(native-lib
        exdlna
        exairplay
        ${log-lib})

