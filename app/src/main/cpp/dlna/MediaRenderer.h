//
// Created by huzongyao on 2018/6/6.
//

#ifndef PLATINUMMEDIA_MEDIARENDER_H
#define PLATINUMMEDIA_MEDIARENDER_H
#include <android/log.h>

#include "../native-lib.h"

#include <Platinum.h>
#include "Platinum/Devices/MediaRenderer/PltMediaRenderer.h"
#include <jni.h>

class MediaRenderer : public PLT_MediaRenderer {
public:
    MediaRenderer(const char *friendly_name,
                  bool show_ip = false,
                  const char *uuid = NULL,
                  unsigned int port = 0,
                  const char *serialNumber=NULL,
                  bool port_rebind = false,
                  cast_callbacks_t *cb = NULL);


    ~MediaRenderer() override;

    // Http server handler
    NPT_Result
    ProcessHttpGetRequest(NPT_HttpRequest &request, const NPT_HttpRequestContext &context,
                          NPT_HttpResponse &response) override;

    // AVTransport methods
    NPT_Result OnNext(PLT_ActionReference &action) override;

    NPT_Result OnPause(PLT_ActionReference &action) override;

    NPT_Result OnPlay(PLT_ActionReference &action) override;

    NPT_Result OnPrevious(PLT_ActionReference &action) override;

    NPT_Result OnStop(PLT_ActionReference &action) override;

    NPT_Result OnSeek(PLT_ActionReference &action) override;

    NPT_Result OnSetAVTransportURI(PLT_ActionReference &action,const PLT_HttpRequestContext& context) override;

    // RenderingControl methods
    NPT_Result OnSetVolume(PLT_ActionReference &action) override;

    NPT_Result OnSetMute(PLT_ActionReference &action) override;

    NPT_Result OnGetPositionInfo(PLT_ActionReference &action) override;

    NPT_String FormatTimeStamp(NPT_UInt32 seconds);

    NPT_Result Stop();

   // NPT_Result MediaRenderer::SearchServer();

   // NPT_Result MediaRenderer::BindServer()

private:
    NPT_Result SetupServices() override;

    NPT_Result DoJavaCallback(int type, const char *param1 = "",
                              const char *param2 = "",
                              const char *param3 = "");

    int is_begin_with(const char *str1,  const char *str2);

};


#endif //PLATINUMMEDIA_MEDIARENDER_H
