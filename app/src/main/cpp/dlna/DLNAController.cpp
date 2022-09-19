//
// Created by 夏国强 on 2019/2/26.
//

#include <android/log.h>
#include <jni.h>
#include <malloc.h>
#include <Platinum.h>
#include "DLNAController.h"
#include "NdkLogger.h"
#include "CallbackTypes.h"

DLNAController::DLNAController(PLT_CtrlPointReference &ctrlPoint, bool use_cache,
                               PLT_MediaContainerChangesListener *listener,
                               PLT_CtrlPointReference &ctrl_point,
                               PLT_MediaControllerDelegate *delegate)
        : PLT_SyncMediaBrowser(ctrlPoint, use_cache, listener),
          PLT_MediaController(ctrl_point, delegate) {
    wd_CurBrowseDirectoryStack.Push("0");

    PLT_MediaController::SetDelegate(this);

}

DLNAController::~DLNAController() {

}

void
DLNAController::GetCurMediaRenderer(PLT_DeviceDataReference &renderer) {
    NPT_AutoLock lock(wd_CurMediaRendererLock);
    if (wd_CurMediaRenderer.IsNull()) {
        //没有索索到设备
    } else {
        renderer = wd_CurMediaRenderer;
    }
}

const PLT_StringMap
DLNAController::getMediaRenderersNameTable() {
    const NPT_Lock<PLT_DeviceMap> &deviceList = wd_MediaRenderers;

    PLT_StringMap namesTable;
    NPT_AutoLock lock(wd_MediaServers);

    // create a map with the device UDN -> device Name
    const NPT_List<PLT_DeviceMapEntry *> &entries = deviceList.GetEntries();
    NPT_List<PLT_DeviceMapEntry *>::Iterator entry = entries.GetFirstItem();
//    namesTable.Put("ip", ip);
    while (entry) {
        PLT_DeviceDataReference device = (*entry)->GetValue();
        NPT_String name = device->GetFriendlyName();
        NPT_String ip = device->GetURLBase().GetHost();
        namesTable.Put((*entry)->GetKey(), name + "|" + ip);
        ++entry;
    }

    return namesTable;
}

/**
 绑定设备
 */
void DLNAController::chooseMediaRenderer(NPT_String chosenUUID) {
    NPT_AutoLock lock(wd_CurMediaRendererLock);

    wd_CurMediaRenderer = ChooseDevice(wd_MediaRenderers, chosenUUID);
}

///查找设备
PLT_DeviceDataReference
DLNAController::ChooseDevice(const NPT_Lock<PLT_DeviceMap> &deviceList, NPT_String chosenUUID) {
    PLT_DeviceDataReference *result = NULL;

    if (chosenUUID.GetLength()) {
        deviceList.Get(chosenUUID, result);
    }
    return result ? *result
                  : PLT_DeviceDataReference(); // return empty reference if not device was selected
}


/**
 推屏
 */
void
DLNAController::setRendererAVTransportURI(const char *uriStr, const char *metaData) {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device); //检查设备
    if (!device.IsNull()) {
        bool result = NPT_FAILED(
                SetAVTransportURI(device, 0, uriStr, metaData == NULL ? "" : metaData, NULL));
        if (result) {
            LOGD("set uri failed");
        }
    }
}


/**
 播放
 */
void
DLNAController::setRendererPlay() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        Play(device, 0, "1", NULL);
    }
}


/**
 退出播放
 */
void
DLNAController::setRendererStop() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        Stop(device, 0, NULL);
    }
}


/**
 暂停播放
 */
void
DLNAController::setRendererPause() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        Pause(device, 0, NULL);
    }
}


/**
 获取播放状态
 */
void
DLNAController::getTransportInfo() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        GetTransportInfo(device, 0, NULL);
    }
}

void
DLNAController::getMediaInfo() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        GetMediaInfo(device, 0, NULL);
    }
}

/*
 获取当前播放时间
 */
void
DLNAController::getTransportPlayTimeForNow() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        GetPositionInfo(device, 0, NULL);
    }
}


/**
 获取当前音量
 */
void
DLNAController::getRendererVolume() {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        GetVolume(device, 0, "Master", NULL);
    }
}

/**
 设置音量
 */
void
DLNAController::setRenderVolume(int volume) {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        SetVolume(device, 0, "Master", volume, NULL);
    }
}

/**
 快进
 */
void
DLNAController::setSeekTime(const char *command) {
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
//        NPT_String target = command;
        Seek(device, 0, "REL_TIME", command, NULL);
    }
}


/**
 发现设备进行代理回调
 */
bool
DLNAController::OnMRAdded(PLT_DeviceDataReference &device) {
    NPT_String uuid = device->GetUUID();
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d :: uuid :: %s\n", __FUNCTION__,
                        __LINE__, uuid.GetChars());
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d :: friendlyname :: %s\n", __FUNCTION__,
                        __LINE__, device->GetFriendlyName().GetChars());
    __android_log_print(ANDROID_LOG_ERROR, "xia", "%s:%d :: m_HostName :: %s\n", __FUNCTION__,
                        __LINE__, device->GetURLBase().GetHost().GetChars());

    // test if it's a media renderer
    PLT_Service *service;

    if (NPT_SUCCEEDED(
            device->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:*", service))) {
        NPT_AutoLock lock(wd_MediaRenderers);
        wd_MediaRenderers.Put(uuid, device);
    }
//    if(wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(onDMRAdded)])
//    {
//        [wd_Target.delegate onDMRAdded];
//    }
    return true;
}

/**
 请求播放状态回调
 */
void
DLNAController::OnGetTransportInfoResult(NPT_Result res, PLT_DeviceDataReference &device,
                                         PLT_TransportInfo *info, void *userdata) {
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d :: \n", __FUNCTION__, __LINE__);

//    if (wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(getTransportInfoResponse:)]){
//        WDTransportResponseInfo *response = [[WDTransportResponseInfo alloc] initWithResult:res userData:(__bridge id)userdata deviceUUID:CHARTONSSTRING(device->GetUUID())];
//        response.cur_transport_state = CHARTONSSTRING(info->cur_transport_state);
//        response.cur_transport_status = CHARTONSSTRING(info->cur_transport_status);
//        response.cur_speed = CHARTONSSTRING(info->cur_speed);
//        [wd_Target.delegate getTransportInfoResponse:response];
//    }
}

/**
 请求播放状态回调
 */
void
DLNAController::OnGetMediaInfoResult(NPT_Result res, PLT_DeviceDataReference &device,
                                     PLT_MediaInfo *info, void *userdata) {
    char media_duration[32] = {0};
    sprintf(media_duration, "%lld", info->media_duration.ToSeconds());
}


/**
 请求播放进度回调
 */
void
DLNAController::OnGetPositionInfoResult(NPT_Result res, PLT_DeviceDataReference &device,
                                        PLT_PositionInfo *info, void *userdata) {
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d ::info->track %d \n", __FUNCTION__, __LINE__, info->track);
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d ::info->rel_time %d \n", __FUNCTION__, __LINE__, info->rel_time.ToSeconds());
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d ::info->track_duration %d \n", __FUNCTION__, __LINE__, info->track_duration.ToSeconds());
//    if (info->rel_time.operator!=() ||info->track_duration.operator!=(0)) {
        char rel_time[32] = {0};
        char track_duration[32] = {0};
        sprintf(rel_time, "%lld", info->rel_time.ToSeconds());
        sprintf(track_duration, "%lld", info->track_duration.ToSeconds());
//    }
//    free(rel_time);
//    free(track_duration);
//    rel_time = NULL;
//    track_duration = NULL;
//    if(wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(getTransportPositionInfoResponse:)]){
//        WDTransportPositionInfo *response = [[WDTransportPositionInfo alloc] initWithResult:res userData:(__bridge id)userdata deviceUUID:CHARTONSSTRING(device->GetUUID())];
//        response.track = info->track;
//        response.rel_count = info->rel_count;
//        response.abs_count = info->abs_count;
//        response.track_uri = CHARTONSSTRING(info->track_uri);
//        response.rel_time = LONGLONGSTRING(info->rel_time.ToSeconds());
//        response.abs_time = LONGLONGSTRING(info->abs_time.ToSeconds());
//        response.track_duration = LONGLONGSTRING(info->track_duration.ToSeconds());
//        response.track_metadata = CHARTONSSTRING(info->track_metadata);
//        [wd_Target.delegate getTransportPositionInfoResponse:response];
//    }
}

char *DLNAController::long2charArr(long long num) {

    char *arr = new char[8];
    int wei = 56;
    for (int i = 7; i >= 0; i--) {

        long long temp = num << wei;//左移到56-64位，将比它高的位丢失
        arr[i] = temp >> 56;//右移到1-8位，将比它低的位丢失
        wei -= 8;
    }
    return arr;
}

/**
 跳转进度回调
 */
void
DLNAController::OnSeekResult(NPT_Result res, PLT_DeviceDataReference &device, void *userdata) {
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d :: \n", __FUNCTION__, __LINE__);
//    if(wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(OnSeekResult:)])
//    {
//        NSInteger result = res;
//        WDTransportResponseInfo *response = [[WDTransportResponseInfo alloc] initWithResult:result userData:(__bridge id)userdata deviceUUID:CHARTONSSTRING(device->GetUUID())];
//        [wd_Target.delegate OnSeekResult:response];
//    }
}

/**
 获取当前播放音量回调
 */
void DLNAController::
OnGetVolumeResult(NPT_Result res, PLT_DeviceDataReference &device, const char *channel,
                  NPT_UInt32 volume, void *userdata) {
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d :: \n", __FUNCTION__, __LINE__);
    char cVolume[32] = {0};
    sprintf(cVolume, "%lld", volume);
//    if(wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(getVolumeResponse:)])
//    {
//        NSInteger result = res;
//        WDTransportVolumeInfo *volumeInfo = [[WDTransportVolumeInfo alloc] initWithResult:result userData:(__bridge id)userdata deviceUUID:CHARTONSSTRING(device->GetUUID())];
//        volumeInfo.channel = CHARTONSSTRING(channel);
//        volumeInfo.volume = volume;
//        [wd_Target.delegate getVolumeResponse:volumeInfo];
//    }
}

/**
 设置音量回调
 */
void DLNAController::
OnSetVolumeResult(NPT_Result res, PLT_DeviceDataReference &device, void *userdata) {
    __android_log_print(ANDROID_LOG_ERROR, "unisen", "%s:%d :: \n", __FUNCTION__, __LINE__);
//    if(wd_Target.delegate && [wd_Target.delegate respondsToSelector:@selector(setVolumeResponse:)])
//    {
//        WDTransportResponseInfo *response = [[WDTransportResponseInfo alloc] initWithResult:res userData:(__bridge id)userdata deviceUUID:CHARTONSSTRING(device->GetUUID())];
//        [wd_Target.delegate setVolumeResponse:response];
//
//    }
}
