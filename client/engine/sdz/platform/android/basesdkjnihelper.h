#ifndef BASESDK_JNI_HELPER_H_
#define BASESDK_JNI_HELPER_H_

#include "zqcommon/typedef.h"

#define CPP_INVOKE_JAVA_PLATFORMUTIL "com/kuyue/sdklib/PlatformUtil"
#define CPP_INVOKE_JAVA_SDKMANAGER   "com/kuyue/sdklib/SdkManager"
#define CPP_INVOKE_JAVA_BASESDK   "com/kuyue/sdklib/BaseSdk"
#define CPP_INVOKE_JAVA_PUSHMANAGER   "com/kuyue/pushsdk/MpushManager"

void* GetSdkPropertie(const char* pro_name);

std::string GetCSdkStringPropertie(const char* class_name,const char* method_name,const char* meta_name = "");

bool GetCSdkBooleanPropertie(const char* class_name,const char* method_name);

std::string GetSdkStringPropertie(const char* pro_name);

bool  GetSdkBooleanPropertie(const char* pro_name);

#endif
