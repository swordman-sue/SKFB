package com.kuyue.voice;

import com.kuyue.queue.MessageData;


public class MsgPlayFinishData extends MessageData {

	private int luafunid;
	private int code;
	private int voiceid;
	public int getLuafunid() {
		return luafunid;
	}
	public void setLuafunid(int luafunid) {
		this.luafunid = luafunid;
	}
	public int getCode() {
		return code;
	}
	public void setCode(int code) {
		this.code = code;
	}
	public int getVoiceid() {
		return voiceid;
	}
	public void setVoiceid(int voiceid) {
		this.voiceid = voiceid;
	}

}

