package com.kuyue.sdklib;

import com.kuyue.queue.MessageData;

class MsgType
{
	public static final int LOGIN = 1;
	public static final int PAY =2; 
	public static final int COMMON =3; 
}

public class MsgSdkData extends MessageData
{
	int code;
	String data;
	int msgType;
	
	public MsgSdkData(int code, String data,int msgType) {
		super();
		this.code = code;
		this.data = data;
		this.msgType = msgType;
	}
	public int getCode() {
		return code;
	}
	public void setCode(int code) {
		this.code = code;
	}
	public String getData() {
		return data;
	}
	public void setData(String data) {
		this.data = data;
	}
	public int getMsgType() {
		return msgType;
	}
	public void setMsgType(int msgType) {
		this.msgType = msgType;
	}
	
	 
}
