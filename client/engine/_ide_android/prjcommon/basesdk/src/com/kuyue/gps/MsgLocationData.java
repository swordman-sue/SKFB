package com.kuyue.gps;

import com.kuyue.queue.MessageData;

class LocationOpType
{
	public static final int START = 1;
	public static final int STOP =2; 
}

public class MsgLocationData extends MessageData 
{
	 int code;
	 String msg;
	 int timeout;
	 int opType;
	
	public int getCode() {
		return code;
	}
	public void setCode(int code) {
		this.code = code;
	}
	public String getMsg() {
		return msg;
	}
	public void setMsg(String msg) {
		this.msg = msg;
	}
	public int getTimeout() {
		return timeout;
	}
	public void setTimeout(int timeout) {
		this.timeout = timeout;
	}
	public int getOpType() {
		return opType;
	}
	public void setOpType(int opType) {
		this.opType = opType;
	}
	
}
