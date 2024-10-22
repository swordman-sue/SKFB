#ifndef XMLREADER_H
#define XMLREADER_H

#include "tinyxml2/tinyxml2.h"
#include "cocos2d.h"


NS_CC_BEGIN

class MyParticleSystem;
class CCParticleTechnique;
class CCParticleAttribute;
class DynamicAttribute;


class PSReader
{
public:
	bool ParseFile(const char *file_name, MyParticleSystem *ps);
	bool ParseBuffer(const char *buffer, int len, MyParticleSystem* ps);
	void SaveScript(MyParticleSystem *ps, const char *file_name);

private:
	bool ParseScript(tinyxml2::XMLDocument *my_document, MyParticleSystem* ps);
	bool ParseTechnique(CCParticleTechnique* technique, tinyxml2::XMLElement *element);
	bool ParseClipper(MyParticleSystem* ps, tinyxml2::XMLElement *element);
	bool ParseParticleAttribute(CCParticleAttribute* attribute, tinyxml2::XMLElement *element);
	bool ParseDynAttribute(DynamicAttribute* attribute, tinyxml2::XMLElement *element);
};

NS_CC_END
#endif
