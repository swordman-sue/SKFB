#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "tinyxml2/tinyxml2.h"
#include <string>

NS_CC_BEGIN

class DynamicAttribute;

class CCParticleAttribute
{
public:
	CCParticleAttribute() {}
	virtual ~CCParticleAttribute() {}

	virtual bool SetAttribute(const char* type, const char* value) = 0;
	virtual bool GetAttribute(const char* type, char* value, size_t size) = 0;

	virtual bool SetDynAttribute(const char* type, DynamicAttribute *dyn) = 0;
	void GetDynAttribute(DynamicAttribute* dyn, std::string& value);

	DynamicAttribute* ParseDynamicAttribute(const char* data);
	virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element) = 0;
};

NS_CC_END

#endif