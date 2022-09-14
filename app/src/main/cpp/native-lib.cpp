//
// Created by �Ĺ�ǿ on 2018/6/8.
//

#include <jni.h>
#include <stdlib.h>
#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include "StdString.h"
#include <string>
#include <map>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <linux/in6.h>
#include <DLNAServer.h>
#include <NdkLogger.h>
#include <CallbackTypes.h>
#include "native-lib.h"
#include <math.h>
#include <assert.h>
#include  <signal.h>
#include <asm/fcntl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <asm/resource.h>
#include <net/if.h>
#include <signal.h>

#ifdef LOG_TAG

#undef LOG_TAG

#define LOG_TAG "native-lib"


#endif

#define DBG	1

#define SCREEN_WIDTH 1920//1280
#define SCREEN_HEIGHT 1080//720
#define LEGAL_HOST	"swx"
#define LEGAL_USER	"swx"

#define CALLBACK_CLASS "com/ex/unisen/cast/CastServer"
#define CALLBACK_METHOD "onNEvent"
#define CALLBACK_SIGN "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)I"


#define CALLBACK_CLASS_GETPOSITION "com/ex/unisen/activity/VideoPlayerActivity"
#define CALLBACK_METHOD_GETPOSITION "getCurrentPosition"
#define CALLBACK_METHOD_GETDURATION "getDuration"

#define CALLBACK_SIGN_GETPOSITION "()Ljava/lang/Long"


char remote_ip[64] = {0};
static int running = false;
static int mMirrorStatus = 0; //0:������ֹͣ״̬ 1:���������� 2:���ڲ���һ֡����,��ֹ�ж�
static int mVideoStatus = 0; //0:ֹͣ����״̬ 1:��������״̬ -1:����ֹͣ����
static int mAudioStatus = 0; //0:ֹͣ��Ƶ����״̬ 1:��Ƶ����״̬

struct video_frame
{
	unsigned char* data;
	int len;
	int pts;
};
static pthread_mutex_t mVideoLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mAvplayerLock = PTHREAD_MUTEX_INITIALIZER;

int disconnect_flags = 0;

static DLNAServer *dlnaServer = NULL;
static JavaVM* gVM = NULL;

static JavaVM*   	g_vm = NULL;
static jclass g_callbackClass;
static jmethodID g_callbackMethod;


static jmethodID g_callbackMethod_position;
static jmethodID g_callbackMethod_duration;

extern int openUrl(char *url);
extern void audio_player_destroy(void *cls);
extern void mirroring_player_stop(void *cls);
extern void mirroring_player_play(void *cls, int width, int height, const void *buffer, int buflen, int payloadtype, uint64_t timestamp);

char *bin2hex(const unsigned char *buf, int len) {
    int i;
    char *hex = (char *) malloc(len * 2 + 1);
    for (i = 0; i < len * 2; i++) {
        int val = (i % 2) ? buf[i / 2] & 0x0f : (buf[i / 2] & 0xf0) >> 4;
        hex[i] = (val < 10) ? '0' + val : 'a' + (val - 10);
    }
    hex[len * 2] = 0;
    return hex;
}

const char *ToString(long i) {
    static char buffer[sizeof(i)*3 + 1];  // Size could be a bit tighter
    char *p = &buffer[sizeof(buffer)];
    *(--p) = '\0';
    lldiv_t qr;
    qr.quot = i;
    do {
        qr = lldiv(qr.quot, 10);
        *(--p) = std::abs(qr.rem) + '0';
    } while (qr.quot);
    if (i < 0) {
        *(--p) = '-';
    }
    return p;
}

int DoJavaCallback(int type, const char *param1,
                                         const char *param2,
                                         const char *param3) {
    LOGD("xia ::param2 :::%s  :: param3::%s",param2,param3);
    if (g_vm == NULL) {
        LOGE("g_vm = NULL!!!");
        return NPT_FAILURE;
    }
    int status;
    JNIEnv *env = NULL;
    bool isAttach = false;
    status = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = g_vm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOGE("MediaRenderer::DoJavaCallback Failed: %d", status);
            return NPT_FAILURE;
        }
        isAttach = true;
    }
    jstring jParam1 = NULL;
    jstring jParam2 = NULL;
    jstring jParam3 = NULL;
    jclass inflectClass = g_callbackClass;
    jmethodID inflectMethod = g_callbackMethod;
    if (inflectClass == NULL || inflectMethod == NULL) {
        if (env->ExceptionOccurred()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        if (isAttach) {
            g_vm->DetachCurrentThread();
        }
		return NPT_ERROR_NO_SUCH_CLASS;
    }
    //LOGD("TYPE: %d\nPARAM1: %s\nPARAM1: %s", type, param1, param2);
    jParam1 = env->NewStringUTF(param1);
    jParam2 = env->NewStringUTF(param2);
    jParam3 = env->NewStringUTF(param3);
	jint r = env->CallStaticIntMethod(inflectClass, inflectMethod, type, jParam1, jParam2, jParam3);
    env->DeleteLocalRef(jParam1);
    env->DeleteLocalRef(jParam2);
    env->DeleteLocalRef(jParam3);
    if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    if (isAttach) {
        g_vm->DetachCurrentThread();
    }
    return r;
}

long getDuration(){

    if (g_vm == NULL) {
        LOGE("g_vm = NULL!!!");
        return NPT_FAILURE;
    }
    int status;
    JNIEnv *env = NULL;
    bool isAttach = false;
    status = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = g_vm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOGE("MediaRenderer::DoJavaCallback Failed: %d", status);
            return NPT_FAILURE;
        }
        isAttach = true;
    }

    jclass inflectClass = g_callbackClass;
    jmethodID inflectMethod = g_callbackMethod_duration;
    if (inflectClass == NULL || inflectMethod == NULL) {
	if (env->ExceptionOccurred()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        if (isAttach) {
            g_vm->DetachCurrentThread();
        }
        return 0;
    }

    jlong result = env->CallStaticLongMethod(inflectClass,inflectMethod);
    if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    if (isAttach) {
        g_vm->DetachCurrentThread();
    }
    return result;
}

long getPosition(){

	if (g_vm == NULL) {
		LOGE("g_vm = NULL!!!");
		return NPT_FAILURE;
	}
	int status;
	JNIEnv *env = NULL;
	bool isAttach = false;
	status = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
	if (status != JNI_OK) {
		status = g_vm->AttachCurrentThread(&env, NULL);
		if (status < 0) {
			LOGE("MediaRenderer::DoJavaCallback Failed: %d", status);
			return NPT_FAILURE;
		}
		isAttach = true;
	}

	jclass inflectClass = g_callbackClass;
	jmethodID inflectMethod = g_callbackMethod_position;
	if (inflectClass == NULL || inflectMethod == NULL) {
		if (env->ExceptionOccurred()) {
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		if (isAttach) {
			g_vm->DetachCurrentThread();
		}
		return 0;
	}

	jlong result = env->CallStaticLongMethod(inflectClass,inflectMethod);
	if (env->ExceptionOccurred()) {
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	if (isAttach) {
		g_vm->DetachCurrentThread();
	}
	return result;
}

int str_startsWith(char * str, char * search_str) {
	if ((str == NULL) || (search_str == NULL)) return false;
	return (strstr(str, search_str) == str);
}


void VideoQueueRelease() {
	pthread_mutex_lock(&mVideoLock);
	pthread_mutex_unlock(&mVideoLock);
}

void VideoQueueInit() {
	VideoQueueRelease();
}

bool VideoQueueBuffer(unsigned char* buffer, int len, int pts) {
	if (len <= 0) return false;

	struct video_frame *frame = (struct video_frame *)malloc(sizeof(*frame));
	frame->data = buffer;
	frame->len = len;
	frame->pts = pts;

	pthread_mutex_lock(&mVideoLock);
	pthread_mutex_unlock(&mVideoLock);
	return true;
}


uint64_t ntp_to_pts(long long int ntp)
{	
	uint64_t m;
	uint64_t l;	
	m = (ntp >> 32) * 1000000;	
	l = (ntp & 0xffffffffl);
	l = (l * 1000 / 4294967296l) * 1000;	
	m = m + l;	
	return m;
}

int64_t get_current_time_ms()
{	
	struct timeval tv;	
	gettimeofday(&tv, NULL);	
	return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


//url:��Ƶ��ַ
//fPosition:��Ƶ�ٷֱȶϵ�
//dPosition:��Ƶ�����ϵ�(x1000)
//�����Ǵ�url ������������

int url_player_open(void *cls, char *url, float fPosition,char *mediaInfo)
{
	__android_log_print(ANDROID_LOG_ERROR, "xia", "url_player_open agent::  %s\n", mediaInfo);
	mVideoStatus = 1;
	char agent[100] = {0};
	strcpy(agent,mediaInfo);
    int result = DoJavaCallback(CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI, url, agent,NULL);
    return result;
}

//������ò������Ŀ�ʼ����
void url_player_play(void *cls)
{
	mVideoStatus = 1;
	DoJavaCallback(CALLBACK_EVENT_ON_PLAY, NULL, NULL,NULL);
}

//������ò���������ͣ

void url_player_pause(void *cls) 
{
	if(mVideoStatus == 1) {
		DoJavaCallback(CALLBACK_EVENT_ON_PAUSE, NULL, NULL, NULL);
	}

}
//������ò������Ľ���
void url_player_stop(void *cls)
{
	__android_log_print(ANDROID_LOG_ERROR, "xia", "url_player_stop");
	if(mVideoStatus == 1){
		__android_log_print(ANDROID_LOG_ERROR, "xia", "url_player_stop1");
		mVideoStatus = 0;
		DoJavaCallback(CALLBACK_EVENT_ON_STOP, NULL, NULL,NULL);
	}
}

//������ò�����������������
void url_player_seek(void *cls, long fPosition) 
{
	if(mVideoStatus == 1) {
		DoJavaCallback(CALLBACK_EVENT_ON_SEEK, ToString(fPosition), NULL, NULL);
	}

}

	//������ò�����������������С
void url_player_setvolume(void *cls, int volume) 
{
	if(mVideoStatus == 1) {
		DoJavaCallback(CALLBACK_EVENT_ON_SET_VOLUME, "", ToString(volume), "");
	}

}

//�����ȡ����������ʱ��
long url_player_getduration(void *cls)
{
	if(mVideoStatus == 1) {
		long duration = getDuration();
		if (duration < 0){
			duration = 0;
		}
		return duration;
	}
	return -1;
		
}
long url_player_getpostion(void *cls) {
	if (mVideoStatus == 1) {
		long position = getPosition();
		if (position < 0){
			position = 0;
		}
		return position;
	}
	return -1;

}
int url_player_isplaying(void *cls)
{
	return 1;
	
}
int url_player_ispaused(void *cls) 
{
	return 1;
}

//������Ҫ������Ƶ�������Ĳ����� ��channel
void audio_player_init(void *cls, int bits, int channels, int samplerate, int isaudio) {
	if (mVideoStatus) {
		url_player_stop(cls);
		dlnaServer->Flush();
		mVideoStatus = 0;
	}
	mAudioStatus = 1;
}

//�����buffer pcm����������Ƶ������
void audio_player_process(void *cls, const void *buffer, int buflen, double timestamp, uint32_t seqnum) 
{
	if(!mAudioStatus) return;
}

void audio_player_destroy(void *cls)
{
	if(!mAudioStatus) return;
	mAudioStatus = 0;
}

void audio_player_setvolume(void *cls, int volume)
{
	if(!mAudioStatus) return;
	float mVol;
	mVol = 1.0f + (float)volume/30;
}

void audio_player_setmetadata(void *cls, const void *buffer, int buflen)
{
	if(!mAudioStatus) return;
}

void audio_player_setcoverart(void *cls, const void *buffer, int buflen)
{
	if(!mAudioStatus) return;
}

void audio_player_flush(void *cls)
{
	if(!mAudioStatus) return;
}

char nal_start[4] = {0, 0, 0, 1};

//��Ƶ������
	//data Ϊh264����Ƶ����Ҫ����������(����Ϊ���� pps sps)
void mirroring_player_process(void *cls, const void *buffer, int buflen, int payloadtype,
								  long long unsigned timestamp)

{
	if(mMirrorStatus != 1) return;	//����Ƿ񲥷�������
	if (buflen > 0) {
		int pktsize;
		unsigned char *pktdata = NULL;
		if (payloadtype == 1)
		{
			VideoQueueRelease();
			int spsnalsize;
			int ppsnalsize;
			unsigned char *head = (unsigned  char *)buffer;
			spsnalsize = ((uint32_t)head[6] << 8) | ((uint32_t)head[7]);
			ppsnalsize = ((uint32_t)head[9 + spsnalsize] << 8) | ((uint32_t)head[10 + spsnalsize]);
			pktsize = 4 + spsnalsize + 4 + ppsnalsize;
			pktdata = (unsigned char *)malloc(pktsize);
			memcpy(pktdata, nal_start, 4);
			memcpy(pktdata + 4, head + 8, spsnalsize);
			memcpy(pktdata + 4 + spsnalsize, nal_start, 4);
			memcpy(pktdata + 4 + spsnalsize + 4, head + 11 + spsnalsize, ppsnalsize);
		} else if (payloadtype == 0) {
			int		       rLen;
			unsigned char *data;
			pktdata = (unsigned char *)malloc(buflen);
			memcpy(pktdata, buffer, buflen);
			pktsize = buflen;
			rLen = 0;
			data = (unsigned char *)pktdata;
			while (rLen < pktsize) {
				rLen += 4 + (((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (uint32_t)data[3]);
				memcpy(data, nal_start, 4);
				data = (unsigned char *)pktdata + rLen;
			}
		}
	}
}

//��Ƶ��ʼ��������

//data Ϊh264����Ƶ����Ҫ����������(����Ϊ���� pps sps,payloadtype Ϊ1��ʱ��Ϊ����)
void mirroring_player_play(void *cls, int width, int height, const void *buffer, int buflen,
							   int payloadtype, long long unsigned timestamp)

{
	if (mVideoStatus)
	{
		url_player_stop(cls);
		dlnaServer->Flush();
		mVideoStatus = 0;
	}
	mMirrorStatus = -1;
	VideoQueueInit();

	mMirrorStatus = 1;		//������׼������
	disconnect_flags = 1;
	mirroring_player_process(cls, buffer, buflen, payloadtype, timestamp);
}

//��Ƶֹͣ�ص�
void mirroring_player_stop(void *cls)
{
	VideoQueueRelease();
	if(mMirrorStatus == 0) return;
	mMirrorStatus = -1;
	pthread_mutex_lock(&mAvplayerLock);
	pthread_mutex_unlock(&mAvplayerLock);
	mMirrorStatus = 0;						//��������ֹͣ
	disconnect_flags = 0;
}

void *airplay_video_handle_thread(void *arg) {
	struct video_frame *frame;
	pthread_detach(pthread_self());
	while(running) {
		if(mMirrorStatus == 1) {
			pthread_mutex_lock(&mVideoLock);
			frame = NULL;
			pthread_mutex_unlock(&mVideoLock);

			if(frame) {
				pthread_mutex_lock(&mAvplayerLock);
				pthread_mutex_unlock(&mAvplayerLock);
				free(frame->data);
				free(frame);
			}
		} else
			usleep(100);
	}
	return NULL;
}
void *airplay_rightkey_handle_thread(void *arg) {
	int presscount = 0;
	while(running) {
		sleep(1);
	}
	return NULL;
}
void *airplay_mirror_live_handle_thread(void *arg) {
    while (running) {
        sleep(3);
        while(disconnect_flags && running) {
            if(disconnect_flags == 30) {
                mirroring_player_stop(NULL);
                break;
            }
            sleep(1);
        }
    }
    return NULL;
}

//�����������
void mirroring_player_live(void *cls){
    if(mMirrorStatus) {
        disconnect_flags = 1;
    }
}

//extern "C"
JNIEXPORT jint JNICALL
Java_com_ex_unisen_cast_CastServer_startCastServer( JNIEnv* env, jobject thiz,
                                                    jstring devicesName, jstring serialNumber, jstring deviceUuid) {
    pthread_t  airplay_video_handle;
    pthread_t  airplay_mirror_live_handle;
    pthread_t  airplay_rightkey_handle;
    cast_callbacks_t cb;
    jclass myclass = env->FindClass(CALLBACK_CLASS);
    g_callbackClass = (jclass) env->NewGlobalRef(myclass);
    g_callbackMethod = env->GetStaticMethodID(g_callbackClass, CALLBACK_METHOD, CALLBACK_SIGN);

    g_callbackMethod_position = env->GetStaticMethodID(g_callbackClass, CALLBACK_METHOD_GETPOSITION, "()J");

    g_callbackMethod_duration = env->GetStaticMethodID(g_callbackClass, CALLBACK_METHOD_GETDURATION, "()J");

    memset(&cb, 0, sizeof(cast_callbacks_t));

    cb.cls = env;
    cb.url_player_open = url_player_open;
    cb.url_player_play = url_player_play;
    cb.url_player_pause = url_player_pause;
    cb.url_player_stop = url_player_stop;
    cb.url_player_seek = url_player_seek;
    cb.url_player_setvolume = url_player_setvolume;
    cb.url_player_getduration = url_player_getduration;
    cb.url_player_getpostion = url_player_getpostion;
    cb.url_player_isplaying = url_player_isplaying;
    cb.url_player_ispaused = url_player_ispaused;

    cb.mirroring_player_play = mirroring_player_play;
    cb.mirroring_player_process = mirroring_player_process;
    cb.mirroring_player_stop = mirroring_player_stop;
    cb.mirroring_player_live = mirroring_player_live;

    cb.audio_player_init = audio_player_init;
    cb.audio_player_flush = audio_player_flush;
    cb.audio_player_setcoverart = audio_player_setcoverart;
    cb.audio_player_process = audio_player_process;
    cb.audio_player_setmetadata = audio_player_setmetadata;
    cb.audio_player_destroy = audio_player_destroy;
    cb.audio_player_setvolume = audio_player_setvolume;


    char *pFriendname = (char *) env->GetStringUTFChars(devicesName, NULL);
    char *pSerialNumber = (char *) env->GetStringUTFChars(serialNumber, NULL);
    char *pUuid = (char *) env->GetStringUTFChars(deviceUuid, NULL);
//    int ret = startMediaServer(pFriendname, SCREEN_WIDTH, SCREEN_HEIGHT, &cb);
    dlnaServer = new DLNAServer;
    int dlnaRet = dlnaServer->Start(pFriendname, false, pUuid, (unsigned int) 0,
                                    pSerialNumber, false, &cb);
//    running = true;
//    pthread_create(&airplay_video_handle, NULL, airplay_video_handle_thread, NULL);
//    pthread_create(&airplay_mirror_live_handle, NULL, airplay_mirror_live_handle_thread, NULL);
//    pthread_create(&airplay_rightkey_handle, NULL, airplay_rightkey_handle_thread, NULL);

    env->ReleaseStringUTFChars(serialNumber, pSerialNumber);
    env->ReleaseStringUTFChars(devicesName, pFriendname);
    env->ReleaseStringUTFChars(deviceUuid, pUuid);

    return 0;
}

//extern "C"
JNIEXPORT jint JNICALL
Java_com_ex_unisen_cast_CastServer_stopCastServer( JNIEnv* env, jobject thiz) {
    running = false;
    stopMediaServer();
    if (dlnaServer != NULL)
    {
        dlnaServer->Stop();
    }
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_ex_unisen_cast_CastServer_flushDlna( JNIEnv* env, jobject thiz) {
    if(mVideoStatus == 1){
        if (dlnaServer != NULL)
        {
            dlnaServer->Flush();
        }
        mVideoStatus = 0;
    }
    return 0;
}


static const JNINativeMethod nativeMethods[] = {
        { "startCastServer", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",(void *)Java_com_ex_unisen_cast_CastServer_startCastServer },
        { "stopCastServer", "()I",(void *)Java_com_ex_unisen_cast_CastServer_stopCastServer },
        { "flushDlna", "()I",(void *)Java_com_ex_unisen_cast_CastServer_flushDlna },
};

static int registerNativeMethods(JNIEnv* env) {
    int result = -1;

    /* look up the class */
    jclass clazz = env->FindClass("com/ex/unisen/cast/CastServer");

    if (NULL != clazz) {
        if (env->RegisterNatives(clazz, nativeMethods, sizeof(nativeMethods)
                                                       / sizeof(nativeMethods[0])) == JNI_OK) {
            result = 0;
        }
    }
    return result;
}

jint JNI_OnLoad(JavaVM* vm, void* /* reserved */) {
    JNIEnv* env = NULL;
    jint result = -1;
    g_vm = vm;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK) {
        if (NULL != env && registerNativeMethods(env) == 0) {
            result = JNI_VERSION_1_4;
        }
    }
    return result;
}
