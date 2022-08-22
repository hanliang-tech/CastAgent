//
// Created by tangbull on 2018/6/27.
//

#include "DLNAServer.h"
#include "NdkLogger.h"
#include "MediaRenderer.h"
#include "../native-lib.h"

DLNAServer::DLNAServer() {
    LOGD("DLNAServer::DLNAServer()");
}

DLNAServer::~DLNAServer() {
    LOGD("DLNAServer::~DLNAServer()");
}

NPT_Result
DLNAServer::Start(const char *friendly_name, bool show_ip, const char *uuid,
                  unsigned int port,const char *serialNumber, bool port_rebind,cast_callbacks_t *cb) {
    MediaRenderer *renderer = new MediaRenderer(friendly_name, show_ip, uuid, port,serialNumber, port_rebind,cb);
    mDevice = renderer;
    mediaRenderer = renderer;
    mUPnP.AddDevice(mDevice);
    return mUPnP.Start();
}

NPT_Result DLNAServer::Stop() {
    if (mDevice.IsNull() || !mUPnP.IsRunning()) {
        return NPT_FAILURE;
    }
    mUPnP.Stop();
    return 0;
}

NPT_Result DLNAServer::Flush() {
    if (mDevice.IsNull() || !mUPnP.IsRunning()) {
        return NPT_FAILURE;
    }
    mediaRenderer->Stop();
    return 0;
}

