//
// Created by 夏国强 on 2018/9/4.
//

#ifndef __MEDIASERVER_H__
#define __MEDIASERVER_H__

#include "stdint.h"

#if defined (WIN32) && defined(DLL_EXPORT)
# define AIR_API __declspec(dllexport)
#else
# define AIR_API
#endif

#define MEDIA_RENDER_CTL_MSG_BASE                       		 (0x100)
#define MEDIA_RENDER_TOCONTRPOINT_SET_MEDIA_DURATION            (MEDIA_RENDER_CTL_MSG_BASE+0)
#define MEDIA_RENDER_TOCONTRPOINT_SET_MEDIA_POSITION            (MEDIA_RENDER_CTL_MSG_BASE+1)
#define MEDIA_RENDER_TOCONTRPOINT_SET_MEDIA_PLAYINGSTATE        (MEDIA_RENDER_CTL_MSG_BASE+2)
#define MEDIA_RENDER_TOCONTRPOINT_SET_MEDIA_MUTE                (MEDIA_RENDER_CTL_MSG_BASE+3)
#define MEDIA_RENDER_TOCONTRPOINT_SET_CACHEPOSITION             (MEDIA_RENDER_CTL_MSG_BASE+4)
#define MEDIA_RENDER_TOCONTRPOINT_SET_MEDIA_CACHE               (MEDIA_RENDER_CTL_MSG_BASE+5)

struct cast_callbacks_s {
    void *cls;
    /* Compulsory callback functions */
    int(*url_player_open) (void *cls, char *url, float fPosition,char *mediaInfo);
    void(*url_player_play) (void *cls);
    void(*url_player_pause)(void *cls);
    void(*url_player_stop) (void *cls);
    void(*url_player_seek)(void *cls, long fPosition);
    void(*url_player_setvolume)(void *cls, int volume);
    void(*url_player_showphoto)(void *cls, unsigned char *data, long long size);
    long(*url_player_getduration)(void *cls);
    long(*url_player_getpostion)(void *cls);
    int(*url_player_isplaying)(void *cls);
    int(*url_player_ispaused)(void *cls);
    int(*player_message)(void *cls,unsigned char *message);

    void(*audio_player_init)(void *cls, int bits, int channels, int samplerate, int isaudio);
    void(*audio_player_process)(void *cls, const void *buffer, int buflen, double timestamp, uint32_t seqnum);
    void(*audio_player_destroy)(void *cls);
    void(*audio_player_setvolume)(void *cls, int volume);//1-100
    void(*audio_player_setmetadata) (void *cls, const void *buffer, int buflen);
    void(*audio_player_setcoverart)(void *cls, const void *buffer, int buflen);
    void(*audio_player_flush)(void *cls);

    void(*mirroring_player_play)(void *cls, int width, int height, const void *buffer, int buflen, int payloadtype, long long unsigned timestamp);
    void(*mirroring_player_process)(void *cls, const void *buffer, int buflen, int payloadtype, long long unsigned timestamp);
    void(*mirroring_player_stop)(void *cls);
    void(*mirroring_player_live)(void *cls);
};
typedef struct cast_callbacks_s cast_callbacks_t;

#ifdef __cplusplus
extern "C" {
#endif
int startMediaServer(char *friendname, int width, int height, cast_callbacks_t *cb);
void  stopMediaServer();

#ifdef __cplusplus
}
#endif

#endif
