package com.kuyue.pushsdk;

import com.kuyue.queue.MessageData;


public class MsgPushData extends MessageData {

	private int code;
	private int luafunid;
	private String data;
	
	public int getCode() {
		return code;
	}
	public void setCode(int code) {
		this.code = code;
	}
	public int getLuafunid() {
		return luafunid;
	}
	public void setLuafunid(int luafunid) {
		this.luafunid = luafunid;
	}
	public String getData() {
		return data;
	}
	public void setData(String data) {
		this.data = data;
	}
}

