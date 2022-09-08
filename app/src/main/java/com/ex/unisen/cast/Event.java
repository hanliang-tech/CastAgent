package com.ex.unisen.cast;

public class Event {

    private int operate;
    private String message;
    private Object data;

    public static final int EVENT_MODIFY_IMG = 0;
    public static final int EVENT_MODIFY_TIME = 1;
    public static final int EVENT_NETWORK_MESSAGE = 2;
    public static final int EVENT_DEVICE_NAME_CHANGE = 3;

    public static final int CALLBACK_EVENT_ON_NEXT = 1001;
    public static final int CALLBACK_EVENT_ON_PAUSE = 1003;
    public static final int CALLBACK_EVENT_ON_PLAY = 1002;
    public static final int CALLBACK_EVENT_ON_PREVIOUS = 1004;
    public static final int CALLBACK_EVENT_ON_SEEK = 1005;
    public static final int CALLBACK_EVENT_ON_STOP = 1006;
    public static final int CALLBACK_EVENT_ON_SET_AV_TRANSPORT_URI = 1007;
    public static final int CALLBACK_EVENT_ON_SET_PLAY_MODE = 1008;
    public static final int CALLBACK_EVENT_ON_SET_VOLUME = 1009;
    public static final int CALLBACK_EVENT_ON_SET_MUTE = 1010;


    public static final int CALLBACK_EVENT_ONSEARCH = 2001;

    public static final int EVENT_CONNECTING = 2001;
    public static final int EVENT_CONNECT_RESULT= 2002;

    public Event() {

    }

    public Event(int operate, String message) {
        this.operate = operate;
        this.message = message;
    }

    public int getOperate() {
        return operate;
    }

    public void setOperate(int operate) {
        this.operate = operate;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public Object getData() {
        return data;
    }

    public void setData(Object data) {
        this.data = data;
    }
}
