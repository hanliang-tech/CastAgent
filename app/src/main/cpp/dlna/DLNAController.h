//
// Created by 夏国强 on 2019/2/26.
//

#ifndef QUICKLOOK_DLNACONTROLLER_H
#define QUICKLOOK_DLNACONTROLLER_H




#include <PltLeaks.h>
#include <PltDownloader.h>
#include <Neptune.h>
#include <Platinum.h>

#include "Neptune/Core/NptThreads.h"
#include "Neptune/Core/NptJson.h"
#include "Neptune/Core/NptMap.h"
#include "Platinum/Core/PltDeviceData.h"

typedef NPT_Map<NPT_String, NPT_String>              PLT_StringMap;
typedef NPT_Lock<PLT_StringMap>                      PLT_LockStringMap;
typedef NPT_Map<NPT_String, NPT_String>::Entry       PLT_StringMapEntry;

class DLNAController  :   public PLT_SyncMediaBrowser,
                          public PLT_MediaController,
                          public PLT_MediaControllerDelegate{
public:
    DLNAController(PLT_CtrlPointReference &ctrlPoint, bool use_cache,
                       PLT_MediaContainerChangesListener *listener,
                       PLT_CtrlPointReference &ctrl_point,
                       PLT_MediaControllerDelegate *delegate);
    ~DLNAController();

    virtual bool OnMRAdded(PLT_DeviceDataReference &device);
    void GetCurMediaRenderer(PLT_DeviceDataReference& renderer);
    const PLT_StringMap getMediaRenderersNameTable();
    void setRendererAVTransportURI(const char *uriStr, const char *metaData);
    void chooseMediaRenderer(NPT_String chosenUUID);

    void setRendererPlay();
    void setRendererStop();
    void setRendererPause();
    void getTransportInfo();
    void getMediaInfo();
    void getTransportPlayTimeForNow();
    void getRendererVolume();
    void setRenderVolume(int volume);
    void setSeekTime(const char* command);


    void OnGetTransportInfoResult(
            NPT_Result               /* res */,
            PLT_DeviceDataReference& /* device */,
            PLT_TransportInfo*       /* info */,
            void*                    /* userdata */) ;

    void OnGetPositionInfoResult(
            NPT_Result               /* res */,
            PLT_DeviceDataReference& /* device */,
            PLT_PositionInfo*        /* info */,
            void*                    /* userdata */) ;

    void OnGetMediaInfoResult(
            NPT_Result               /* res */,
            PLT_DeviceDataReference& /* device */,
            PLT_MediaInfo*        /* info */,
            void*                    /* userdata */) ;

    void OnSeekResult(NPT_Result res,PLT_DeviceDataReference &device,void *userdata);

    void OnSetVolumeResult(NPT_Result res,PLT_DeviceDataReference& device,void* userdata);
    void OnGetVolumeResult( NPT_Result res,PLT_DeviceDataReference &device,const char *channel,NPT_UInt32 volume,void *userdata) ;

private:
    NPT_Mutex wd_CurMediaRendererLock;
    PLT_DeviceDataReference wd_CurMediaRenderer;
    NPT_Stack<NPT_String> wd_CurBrowseDirectoryStack;
    NPT_Lock<PLT_DeviceMap> wd_MediaRenderers;
    NPT_Lock<PLT_DeviceMap> wd_MediaServers;
    PLT_DeviceDataReference ChooseDevice(const NPT_Lock<PLT_DeviceMap>& deviceList, NPT_String chosenUUID);
    NPT_Result DoJavaCallback(int type, const char *param1 = "",
                              const char *param2 = "",
                              const char *param3 = "");
    char* long2charArr(long long  num);
};

#endif //QUICKLOOK_DLNACONTROLLER_H
