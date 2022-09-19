//
// Created by xiaguoqiang on 2018/6/6.
//

#include <unistd.h>
#include <pthread.h>
#include "Neptune/Core/Neptune.h"
#include "MediaRenderer.h"
#include "NdkLogger.h"
#include "CallbackTypes.h"
#include "../native-lib.h"
#include "DLNAController.h"
//主要需要修改的地方

NPT_SET_LOCAL_LOGGER("MediaRenderer")

cast_callbacks_t mcb;
PLT_UPnP *upnp;
PLT_CtrlPointReference ctrlPoint;
DLNAController *controller;
bool currentAppPlaying = true;
NPT_String friendName;
MediaRenderer::MediaRenderer(const char *friendly_name,
                             bool show_ip     /* = false */,
                             const char *uuid        /* = NULL */,
                             unsigned int port        /* = 0 */,
                             const char *serialNumber,
                             bool port_rebind /* = false */,
                             cast_callbacks_t *cb)
        : PLT_MediaRenderer(friendly_name, show_ip, uuid, port, serialNumber) {
    memset(&mcb, 0, sizeof(cast_callbacks_t));
    mcb.cls = cb->cls;
    mcb.url_player_open = cb->url_player_open;
    mcb.url_player_stop = cb->url_player_stop;
    mcb.url_player_pause = cb->url_player_pause;
    mcb.url_player_play = cb->url_player_play;
    mcb.url_player_seek = cb->url_player_seek;
    mcb.url_player_setvolume = cb->url_player_setvolume;
    mcb.url_player_getduration = cb->url_player_getduration;
    mcb.url_player_getpostion = cb->url_player_getpostion;
    friendName = NPT_String(friendly_name);
    printf("Name[%s], Show IP[%d], UUID[%s], Port[%u], Port Rebind[%d]",
           friendly_name, show_ip, uuid, port, port_rebind);
    if (upnp == NULL) {
        upnp = new PLT_UPnP();
        ctrlPoint = new PLT_CtrlPoint();
        upnp->AddCtrlPoint(ctrlPoint);
        controller = new DLNAController(ctrlPoint, false, NULL, ctrlPoint, NULL);
        upnp->Start();
    }
}


// 测试阶段
//第一步：quicklook中ex_unisen.cpp中upnp，controller初始化
//第二步：quicklook中ex_unisen 中仿照getActiveRenders 搜索设备
//第三步：quicklook中ex_unisen 中bind设备
//第四步：在接受到手机控制事件时，OnPause：暂停 OnStop：停止 OnSeek：拉动进度条 OnPlay：播放（和暂停对立） OnSetAVTransportURI：设置播放地址
//OnSetVolume：设置声音 OnSetMute：设置静音 等，的时候调用quicklook中ex_unisen 中分辨对应的客户端函数
//以上为测试阶段，默认开启爱奇艺客户端，可以直接搜索绑定
//----------------------------------------------------------------
// 第二阶段
// 默认杀死kill -9 *** 爱奇艺tv端口
// 在接受到投屏url时，根据referfence，开启应用 （这里需要在测试阶段确认是否有客户说的标识字段）
// 以上开启应用需要走docallback 回调给java程序开启应用，这里不会可以找我帮忙。
// 初始化搜索原来在MediaRenderer初始化的时候就绑定了，第二阶段得延后，因为app 是先投屏，才启得app 所以绑定不到设备
MediaRenderer::~MediaRenderer() {

}

NPT_Result MediaRenderer::SetupServices() {
    NPT_CHECK(PLT_MediaRenderer::SetupServices());
    return NPT_SUCCESS;
}

//接受到手机下一集消息时，调用
NPT_Result MediaRenderer::OnNext(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String uri, meta;
    PLT_Service *service;
    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    NPT_CHECK_SEVERE(action->GetArgumentValue("NextURI", uri));
    NPT_CHECK_SEVERE(action->GetArgumentValue("NextURIMetaData", meta));
    service->SetStateVariable("NextAVTransportURI", uri);
    service->SetStateVariable("NextAVTransportURIMetaData", meta);
    NPT_CHECK_SEVERE(action->SetArgumentsOutFromStateVariable());
//    DoJavaCallback(CALLBACK_EVENT_ON_NEXT, uri, meta);
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnPause(PLT_ActionReference &action) {
    PLT_Service *service;
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d \n", __func__, __LINE__);
    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    service->SetStateVariable("TransportState", "PAUSED_PLAYBACK");
    service->SetStateVariable("TransportStatus", "OK");
    // controller
    //controller->setRendererPause();
    if (!currentAppPlaying) {
        controller->setRendererPause();
    } else {
        mcb.url_player_pause(mcb.cls);
    }
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnPrevious(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ： \n", __func__, __LINE__);
//    DoJavaCallback(CALLBACK_EVENT_ON_PREVIOUS);
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnStop(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    PLT_Service *service;
    if (!currentAppPlaying) {
        controller->setRendererStop();
    } else {
        mcb.url_player_stop(mcb.cls);
    }
    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    service->SetStateVariable("TransportState", "STOPPED");
    service->SetStateVariable("TransportStatus", "OK");
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::Stop() {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);;
    PLT_Service *service;
    if (!currentAppPlaying) {
        controller->setRendererStop();
    } else {
        mcb.url_player_stop(mcb.cls);
    }

    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    service->SetStateVariable("TransportState", "STOPPED");
    service->SetStateVariable("TransportStatus", "OK");
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnPlay(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d \n", __func__, __LINE__);
    NPT_String uri, meta;
    PLT_Service *service;
    // look for value set previously by SetAVTransportURI
    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURI", uri));
    NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURIMetaData", meta));
    // if not set, use the current file being played
    service->SetStateVariable("TransportState", "TRANSITIONING");
    service->SetStateVariable("TransportStatus", "OK");
    if (!currentAppPlaying) {
        controller->setRendererAVTransportURI(uri, meta);
        controller->setRendererPlay();
        controller->getTransportPlayTimeForNow();
    } else {
        mcb.url_player_play(mcb.cls);
    }
    // just return success because the play actions are asynchronous
    service->SetStateVariable("TransportState", "PLAYING");
    service->SetStateVariable("TransportStatus", "OK");
    service->SetStateVariable("AVTransportURI", uri);
    service->SetStateVariable("AVTransportURIMetaData", meta);
    service->SetStateVariable("NextAVTransportURI", "");
    service->SetStateVariable("NextAVTransportURIMetaData", "");
    if (&action) {
        NPT_CHECK_SEVERE(action->SetArgumentsOutFromStateVariable());
    }
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnSeek(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String unit, target;
    NPT_CHECK_SEVERE(action->GetArgumentValue("Unit", unit));
    NPT_CHECK_SEVERE(action->GetArgumentValue("Target", target));
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d target ： %s\n", __func__, __LINE__,
                        target.GetChars());
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d Unit ： %s\n", __func__, __LINE__,
                        unit.GetChars());
    if (!unit.Compare("REL_TIME")) {
        // converts target to seconds
        NPT_UInt32 seconds;
        NPT_CHECK_SEVERE(PLT_Didl::ParseTimeStamp(target, seconds));
        __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d target ： %s\n", __func__, __LINE__,
                            target.GetChars());
        const char *secondString = NPT_String::FromInteger(seconds).GetChars();
        if (!currentAppPlaying) {
            controller->setSeekTime(target.GetChars());
        } else {
            mcb.url_player_seek(mcb.cls, seconds * 1000);
        }
    }
    return NPT_SUCCESS;
}

typedef struct
{
    char *url;
    char *meta;
}playInfo;

void * searchService(void *arg) {
    playInfo* info = (playInfo*)arg;
    char url[strlen(info->url)];
    char meta[strlen(info->meta)];
    strcpy(meta, info->meta);
    strcpy(url, info->url);
    pthread_detach(pthread_self());
    NPT_String serverIp, clientIp;
    bool isFind = false;
    while (!currentAppPlaying && !isFind) {
        NPT_List<NPT_IpAddress> ips;
        PLT_UPnPMessageHelper::GetIPAddresses(ips);
        NPT_String currentServerId;

        if (ips.GetItemCount()) {
            serverIp = ips.GetFirstItem()->ToString();
        }

        const PLT_StringMap rendersNameTable = controller->getMediaRenderersNameTable();
        if (rendersNameTable.GetEntries().GetItemCount() != 0) {
            NPT_List<PLT_StringMapEntry *>::Iterator entry = rendersNameTable.GetEntries().GetItem(
                    1);
            while (entry) {
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "server ip %s",
//                                    serverIp.GetChars());
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "client ip %s",
//                                    clientIp.GetChars());
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "(*entry)->GetValue() %s",
//                                    (*entry)->GetValue().GetChars());
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "friendName %s",
//                                    friendName.GetChars());
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "friendName.Compare((*entry)->GetValue().Split(\"|\").GetItem(0)->GetChars()) %d",
//                                    friendName.Compare((*entry)->GetValue().Split("|").GetItem(0)->GetChars()));
//                __android_log_print(ANDROID_LOG_ERROR, "xia", "serverIp.Compare(clientIp) %d",
//                                    serverIp.Compare(clientIp));
                clientIp = NPT_String((*entry)->GetValue().Split("|").GetItem(1)->GetChars());
                NPT_String deviceName = NPT_String((*entry)->GetValue().Split("|").GetItem(0)->GetChars());
                if (friendName.Compare(deviceName) != 0 &&
                    serverIp.Compare(clientIp) == 0) {
                    controller->chooseMediaRenderer((*entry)->GetKey());
                    __android_log_print(ANDROID_LOG_ERROR, "xia", "选择设备");
                    isFind = true;
                    __android_log_print(ANDROID_LOG_ERROR, "xia", "url :: %s:：\n", url);
                    controller->setRendererAVTransportURI(url, meta);
                    controller->getTransportPlayTimeForNow();
                    controller->setRendererPlay();
                }
                ++entry;
                __android_log_print(ANDROID_LOG_ERROR, "xia", "搜索设备");
            }
            if(!isFind)
                sleep(1);
            else
                sleep(3);
            __android_log_print(ANDROID_LOG_ERROR, "xia", "搜索设备一轮完毕");
        }
    }
//    free(url);
//    free(meta);
//    free(info);
//    search_service = 0;
    __android_log_print(ANDROID_LOG_ERROR, "xia", "出循环");
    return NULL;
}

NPT_Result MediaRenderer::OnSetAVTransportURI(PLT_ActionReference &action,
                                              const PLT_HttpRequestContext &context) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String uri, meta, serverIp, clientIp;
    PLT_Service *service;
    NPT_CHECK_SEVERE(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
    NPT_CHECK_SEVERE(action->GetArgumentValue("CurrentURI", uri));
    NPT_CHECK_SEVERE(action->GetArgumentValue("CurrentURIMetaData", meta));
    // if not playing already, just keep around uri & metadata
    // and wait for play command
    service->SetStateVariable("TransportState", "STOPPED");
    service->SetStateVariable("TransportStatus", "OK");
    service->SetStateVariable("TransportPlaySpeed", "1");
    service->SetStateVariable("AVTransportURI", uri);
    service->SetStateVariable("AVTransportURIMetaData", meta);
    service->SetStateVariable("NextAVTransportURI", "");
    service->SetStateVariable("NextAVTransportURIMetaData", "");
    if (uri.IsEmpty())
        return 0;
    char *p = (char *) uri;
    char *q = (char *) meta;
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    const NPT_String *agent = context.GetRequest().GetHeaders().GetHeaderValue(
            NPT_HTTP_HEADER_USER_AGENT);
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    __android_log_print(ANDROID_LOG_ERROR, "xia", "-------url------- %s", uri.GetChars());
    char *o;
    if(agent != nullptr){
        __android_log_print(ANDROID_LOG_ERROR, "xia", "-------agent------- %s", agent->GetChars());
        o = (char *) agent->GetChars();
    }else{
       o = "";
    }
    int result = mcb.url_player_open(mcb.cls, p, 0, o);
//    __android_log_print(ANDROID_LOG_ERROR, "xia", "-------result------- ::%d",result);
    if (result == 3) {
        currentAppPlaying = false;
        playInfo info = {
                p,
                q
        };
        NPT_CHECK_SEVERE(action->SetArgumentsOutFromStateVariable());
        pthread_t  tid;
        int error =  pthread_create(&tid, NULL, searchService, (void*)&info);
        if(error != 0){
            __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ： %s\n", __func__, __LINE__,strerror(error));
        }
        sleep(1);
    } else if (result == 2) {
        currentAppPlaying = true;
    }
    return NPT_SUCCESS;
}


NPT_Result MediaRenderer::OnSetVolume(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String volume;
    NPT_CHECK_SEVERE(action->GetArgumentValue("DesiredVolume", volume));
    __android_log_print(ANDROID_LOG_ERROR, "xia", "volume::  %s", volume.GetChars());
    int result = 0;
    volume.ToInteger(result, false);
    if (!currentAppPlaying) {
        controller->setRenderVolume(result);
    } else {
        mcb.url_player_setvolume(mcb.cls, result);
    }
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::OnSetMute(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String mute;
    NPT_CHECK_SEVERE(action->GetArgumentValue("DesiredMute", mute));
    if (!currentAppPlaying) {
        controller->setRenderVolume(0);
    } else {
        mcb.url_player_setvolume(mcb.cls, 0);
    }
    return NPT_SUCCESS;
}


NPT_Result MediaRenderer::OnGetPositionInfo(PLT_ActionReference &action) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    NPT_String length, position;
    length = NPT_String::FromInteger(mcb.url_player_getduration(mcb.cls));
    position = NPT_String::FromInteger(mcb.url_player_getpostion(mcb.cls));


    NPT_CHECK_SEVERE(action->SetArgumentValue("Track", "0"));

    NPT_CHECK_SEVERE(action->SetArgumentValue("TrackMetaData", ""));

    NPT_CHECK_SEVERE(action->SetArgumentValue("TrackURI", ""));

    if (!currentAppPlaying) {
        NPT_CHECK_SEVERE(action->SetArgumentValue("RelTime", PLT_Didl::FormatTimeStamp(
                0).GetChars()));

        NPT_CHECK_SEVERE(action->SetArgumentValue("AbsTime", PLT_Didl::FormatTimeStamp(
                0).GetChars()));


        NPT_CHECK_SEVERE(action->SetArgumentValue("TrackDuration",
                                                  PLT_Didl::FormatTimeStamp(
                                                          0).GetChars()));
    } else {
        NPT_CHECK_SEVERE(action->SetArgumentValue("RelTime", PLT_Didl::FormatTimeStamp(
                mcb.url_player_getpostion(mcb.cls) / 1000).GetChars()));

        NPT_CHECK_SEVERE(action->SetArgumentValue("AbsTime", PLT_Didl::FormatTimeStamp(
                mcb.url_player_getpostion(mcb.cls) / 1000).GetChars()));


        NPT_CHECK_SEVERE(action->SetArgumentValue("TrackDuration",
                                                  PLT_Didl::FormatTimeStamp(
                                                          mcb.url_player_getduration(mcb.cls) /
                                                          1000).GetChars()));
    }


    NPT_CHECK_SEVERE(action->SetArgumentValue("RelCount", "2147483647"));

    NPT_CHECK_SEVERE(action->SetArgumentValue("AbsCount", "2147483647"));
    return NPT_SUCCESS;
}

NPT_Result MediaRenderer::ProcessHttpGetRequest(NPT_HttpRequest &request,
                                                const NPT_HttpRequestContext &context,
                                                NPT_HttpResponse &response) {
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d ：\n", __func__, __LINE__);
    // get the address of who sent us some data back
    NPT_String ip_address = context.GetRemoteAddress().GetIpAddress().ToString();
    NPT_String method = request.GetMethod();
    NPT_String protocol = request.GetProtocol();
    NPT_HttpUrl url = request.GetUrl();
    const NPT_String *agent = request.GetHeaders().GetHeaderValue(NPT_HTTP_HEADER_USER_AGENT);
    PLT_DeviceSignature signature = PLT_HttpHelper::GetDeviceSignature(request);
    return PLT_DeviceHost::ProcessHttpGetRequest(request, context, response);
}

int MediaRenderer::is_begin_with(const char *str1, const char *str2) {
    if (str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    const char *p = str2;
    int i = 0;
    while (*p != '\0') {
        if (*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}
/**
* 搜索DLNA
*/
//void MediaRenderer::SearchServer() {

//}
/**
* 绑定
*/
//void MediaRenderer::BindServer() {

//}

