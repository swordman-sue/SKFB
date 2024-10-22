#include "platform/android/fileasynchandler.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "zqcommon/logutil.h"

#define FILE_ASYNC_CLASS  "com/kuyue/file/FileAsyncHanderManager"

extern "C"
{
	int GetIntValue(JNIEnv * env,jclass clazz,jobject obj,const char * method_name)
	{
		jmethodID op_type_method = env->GetMethodID(clazz,method_name,"()I");
		jint int_value = env->CallIntMethod(obj,op_type_method);
		return int_value;
	}

}

ListenersMap FileAsyncHandler::map_listener;

FileAsync::FileInfoList FileAsyncHandler::Get()
{
	FileAsync::FileInfoList fileInfos;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,
			"Get", "()Ljava/util/Vector;");

	if (is_get)
	{
		jobject vector_obj = method_info.env->CallStaticObjectMethod(method_info.classID, method_info.methodID);
		if(vector_obj)
		{
			jclass vector_class =  method_info.env->GetObjectClass(vector_obj);
			//获取第几个数据
			jmethodID vector_list_get_method =  method_info.env->GetMethodID(vector_class,"get","(I)Ljava/lang/Object;");
			//获取size
			jmethodID vector_list_size_method =  method_info.env->GetMethodID(vector_class,"size","()I");

			jint len = method_info.env->CallIntMethod(vector_obj,vector_list_size_method);

			for(int i = 0;i < len; ++i)
			{
				//得到FileInfo对象
				jobject fileInfo_obj = method_info.env->CallObjectMethod(vector_obj,vector_list_get_method,i);
				jclass fileinfo_class = method_info.env->GetObjectClass(fileInfo_obj);
				//获取操作类型
				int op_type = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getOpType");
				//获取操作结果
				int op_result = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getOpResult");
				//获取async_id
				int async_id = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getAsyncId");
				//获取path
				jmethodID path_method = method_info.env->GetMethodID(fileinfo_class,"getFilePath","()Ljava/lang/String;");
				jstring path = (jstring)method_info.env->CallObjectMethod(fileInfo_obj,path_method);
				std::string path_str = cocos2d::JniHelper::jstring2string(path);

				//获取dest_path
				jmethodID dest_path_method = method_info.env->GetMethodID(fileinfo_class,"getDestPath","()Ljava/lang/String;");
				jstring dest_path = (jstring)method_info.env->CallObjectMethod(fileInfo_obj,dest_path_method);
				std::string dest_path_str = cocos2d::JniHelper::jstring2string(dest_path);

				//获取dest_path
				jmethodID zip_path_method = method_info.env->GetMethodID(fileinfo_class,"getZipPath","()Ljava/lang/String;");
				jstring zip_path = (jstring)method_info.env->CallObjectMethod(fileInfo_obj, zip_path_method);
				std::string zip_path_str = cocos2d::JniHelper::jstring2string(zip_path);

				//获取byte 长度
				int byte_len = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getLen");

				FileAsync::FileInfo _fileInfo((FileAsync::FileOpType)op_type,path_str,0);
				_fileInfo.len = byte_len;
				_fileInfo.op_result = op_result;
				_fileInfo.async_id = async_id;
				_fileInfo.dest_path = dest_path_str;
				_fileInfo.expansion_path = zip_path_str;

				ListenersMap::iterator iter = map_listener.find(async_id);
				if(iter != map_listener.end())
				{
					_fileInfo.listener = iter->second;
					map_listener.erase(async_id);
				}
				fileInfos.push_back(_fileInfo);
				if (byte_len > 0)
				{
					//used_mem_size  reduce  byte_len。
					FileAsyncHandler::ReleasedMemSize(byte_len);
				}
				method_info.env->DeleteLocalRef(path);
				method_info.env->DeleteLocalRef(dest_path);
				method_info.env->DeleteLocalRef(fileInfo_obj);
				method_info.env->DeleteLocalRef(fileinfo_class);
			}
			method_info.env->DeleteLocalRef(vector_class);
		}
		method_info.env->DeleteLocalRef(vector_obj);
		method_info.env->DeleteLocalRef(method_info.classID);
	}

   return fileInfos;
}

bool FileAsyncHandler::Add(FileAsync::FileInfo fileInfo)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,
				"Add", "(Lcom/kuyue/file/FileInfo;)Z");
	if (is_get)
	{
		jclass    m_cls   = method_info.env->FindClass("com/kuyue/file/FileInfo");
		jmethodID m_mid   = method_info.env->GetMethodID(m_cls,"<init>","()V");
		jfieldID  m_fid_1 = method_info.env->GetFieldID(m_cls,"asyncId","I");
		jfieldID  m_fid_2 = method_info.env->GetFieldID(m_cls,"opType","I");
		jfieldID  m_fid_3 = method_info.env->GetFieldID(m_cls,"filePath","Ljava/lang/String;");
		jfieldID  m_fid_4 = method_info.env->GetFieldID(m_cls,"destPath","Ljava/lang/String;");
		jfieldID  m_fid_5 = method_info.env->GetFieldID(m_cls,"zipPath","Ljava/lang/String;");

		jobject   m_obj   = method_info.env->NewObject(m_cls,m_mid);
		method_info.env->SetIntField(m_obj,m_fid_1,fileInfo.async_id);
		method_info.env->SetIntField(m_obj,m_fid_2,fileInfo.op_type);

		jstring j_file_path = method_info.env->NewStringUTF(fileInfo.path.c_str());
		jstring j_dest_file_path = method_info.env->NewStringUTF(fileInfo.dest_path.c_str());
		jstring j_zip_file_path = method_info.env->NewStringUTF(fileInfo.expansion_path.c_str());
		method_info.env->SetObjectField(m_obj,m_fid_3,j_file_path);
		method_info.env->SetObjectField(m_obj,m_fid_4,j_dest_file_path);
		method_info.env->SetObjectField(m_obj,m_fid_5,j_zip_file_path);

		method_info.env->DeleteLocalRef(j_file_path);
		method_info.env->DeleteLocalRef(j_dest_file_path);
		method_info.env->DeleteLocalRef(j_zip_file_path);

		ret_val  = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID,m_obj);

		if(ret_val)
		{
			map_listener.insert(std::make_pair(fileInfo.async_id,fileInfo.listener));
		}
		method_info.env->DeleteLocalRef(m_obj);
		method_info.env->DeleteLocalRef(m_cls);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
	return ret_val;
}

void FileAsyncHandler::Cancel(unsigned int async_id)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,"Cancel", "(I)Z");
	if (is_get)
	{
		ret_val  = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID,async_id);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
	if(ret_val)
	{
		ListenersMap::iterator iter = map_listener.find(async_id);
		if(iter != map_listener.end())
		{
			if (iter->second != 0)
			{
				iter->second->Free();
			}
			map_listener.erase(async_id);
		}
	}
}

void FileAsyncHandler::SetMemLimit(unsigned int mem_limit)
{
	if(mem_limit <= 0)
	{
		LogUtil::LogInfo("mem_limit is less than zero when call SetMemLimit menthod");
		return;
	}
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,"SetMemLimit", "(I)V");
	if (is_get)
	{
		method_info.env->CallStaticVoidMethod(method_info.classID, method_info.methodID,mem_limit);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
}
void FileAsyncHandler::ReleasedMemSize(unsigned int mem_size)
{
	if(mem_size <= 0)
	{
		LogUtil::LogInfo("mem_size is less than zero when call ReleasedMemSize menthod");
		return;
	}
	cocos2d::JniMethodInfo method_info;
	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,"ReleasedMemSize", "(I)V");
	if (is_get)
	{
		method_info.env->CallStaticVoidMethod(method_info.classID, method_info.methodID,mem_size);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
}

bool FileAsyncHandler::ReadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len)
{

	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;

	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,
			"ReadFile", "(Ljava/lang/String;)Lcom/kuyue/file/FileInfo;");

	if (is_get)
	{
		jstring j_file_name = method_info.env->NewStringUTF(file_full_path.c_str());

		jobject fileInfo_obj = method_info.env->CallStaticObjectMethod(method_info.classID, method_info.methodID, j_file_name);
		jclass fileinfo_class = method_info.env->GetObjectClass(fileInfo_obj);

		//获取操作结果
		int op_result = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getOpResult");
		if(op_result == FileAsync::FOR_SUCC)
		{
			//获取byte 长度
			int byte_len = GetIntValue(method_info.env,fileinfo_class,fileInfo_obj,"getLen");
			*data_len = byte_len;
			//获取返回的byte[] 数据
			jmethodID get_byte_method = method_info.env->GetMethodID(fileinfo_class,"getData","()[B");
			jbyteArray byteArray = (jbyteArray)method_info.env->CallObjectMethod(fileInfo_obj,get_byte_method);
			jbyte      *jBuf     = method_info.env->GetByteArrayElements(byteArray, JNI_FALSE);
			if (jBuf)
			{
				*data_ptr = (char*)malloc(byte_len + 1);
				if (*data_ptr)
				{
					memcpy(*data_ptr, jBuf, byte_len);
					//补零
					(*data_ptr)[byte_len] = 0;
				}else
				{
					LogUtil::LogError("create data_ptr is fail in FileAsyncHandler::ReadFile method");
				}
			}
			method_info.env->ReleaseByteArrayElements(byteArray,jBuf, 0);
			method_info.env->DeleteLocalRef(byteArray);
			ret_val = true;
		}
		method_info.env->DeleteLocalRef(j_file_name);
		method_info.env->DeleteLocalRef(fileInfo_obj);
		method_info.env->DeleteLocalRef(fileinfo_class);
		method_info.env->DeleteLocalRef(method_info.classID);
	}

	return ret_val;
}

bool FileAsyncHandler::Copy(const char *src_full_path, const char *dest_full_path)
{
	bool ret_val = false;
	cocos2d::JniMethodInfo method_info;

	bool is_get = cocos2d::JniHelper::getStaticMethodInfo(method_info, FILE_ASYNC_CLASS,
			"Copy", "(Ljava/lang/String;Ljava/lang/String;)Z");

	if (is_get)
	{
		jstring j_file_path = method_info.env->NewStringUTF(src_full_path);
		jstring j_dest_file_path = method_info.env->NewStringUTF(dest_full_path);

		ret_val = (bool)method_info.env->CallStaticBooleanMethod(method_info.classID, method_info.methodID, j_file_path, j_dest_file_path);

		method_info.env->DeleteLocalRef(j_file_path);
		method_info.env->DeleteLocalRef(j_dest_file_path);
		method_info.env->DeleteLocalRef(method_info.classID);
	}
	return ret_val;
}

