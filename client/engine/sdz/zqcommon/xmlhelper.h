
#ifndef XMLHELPER_H
#define XMLHELPER_H

#include "common/tinyxml/tinyxml.h"
#include "zqcommon/strutil.h"

// 读写element的子节点name的值，不适用多个同名子节点
// <element>
//		……
//		<name>s_val</name>
//		……
// </element>
template<class T>
static bool GetSubNodeValue(TiXmlElement *element, const std::string& name, T& val)
{
	if (!element) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name.c_str());
	if (!TmpElement) return false;
	return GetNodeValue(TmpElement, val);
}
template<class T>
static bool SetSubNodeValue(TiXmlElement *element, const std::string& name, T& val)
{
	if (!element) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name.c_str());
	if (!TmpElement) return false;
	return SetNodeValue(TmpElement, val);
}

// 读写如下节点element的属性attr->a_val
//	<name attr="a_val">s_val</name>
template<class T>
static bool GetNodeAttr(const TiXmlElement *element, const std::string& name, T& val)
{
	if (0 == element)
	{
		return false;
	}
	const std::string* tmp_str = element->Attribute(name);
	if (0 == tmp_str)
	{
		return false;
	}
	if (!ZQ::StrUtil::StringToBaseType(*tmp_str, val))
	{
		return false;
	}
	return true;
}

// 读写如下节点element的值,不支持形如<element/>的空值
// <element>s_val</element>
template<class T>
static bool GetNodeValue(const TiXmlElement *element, T& val)
{
	if (!element || !element->FirstChild() || !ZQ::StrUtil::StringToBaseType(element->FirstChild()->ValueStr(), val))
	{
		return false;
	}
	return true;
}
template<class T>
static bool SetNodeValue(TiXmlElement *element, T& val)
{
	std::string value;
	if (!element || !element->FirstChild() || !BaseTypeToString(val,value))
	{
		return false;
	}
	element->FirstChild()->SetValue(value);
	return true;
}

// 对应至少一个子节点的xml结构
// <Element>
//		<node_0>value</node_0>
//		<node_1>value</node_1>
//		<node_2>value</node_2>
//		……
//		<Element>……</Element>
// </Element>
class ConfigElementBase
{
public:
	ConfigElementBase():m_element(0) {}
	virtual ~ConfigElementBase() {}

	void SetElement(TiXmlElement *element) { m_element = element; }

	// 供子类列表中获取偏移地址
	virtual ConfigElementBase* offset() = 0;
	// 将子节点读到相应成员数据中
	virtual bool read() = 0;
	// 将成员数据写入文件
	virtual bool write() = 0;

protected:
	// 获取子节点的值,不适用有多个同名子节点的情况
	template<class T>
	bool GetValue(const char *name, T* value)
	{
		if (m_element == 0) return false;
		return GetSubNodeValue(m_element, name, *value);
	}
	bool GetValue(const char *name, ConfigElementBase* value)
	{
		if (m_element == 0) return false;
		value->SetElement(m_element->FirstChildElement(name));
		return value->read();
	}

	// 获取子节点的值,适用有多个同名子节点的情况,与数组对应
	template<class T>
	bool GetListValue(const char *name, T* list_value, int& list_len, int list_max_len)
	{
		list_len = 0;
		if (m_element == 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for ( ;list_len < list_max_len && tmp_element != 0; ++list_len, ++list_value, tmp_element = tmp_element->NextSiblingElement(name))
		{
			if (!GetNodeValue(tmp_element, *list_value)) return false;
		}
		return true;
	}
	bool GetListValue(const char *name, ConfigElementBase* list_value, int& list_len, int list_max_len)
	{
		list_len = 0;
		if (m_element == 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for ( ;list_len < list_max_len && tmp_element != 0; ++list_len)
		{
			list_value->SetElement(tmp_element);
			if (!list_value->read())return false;
			list_value = list_value->offset();
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		return true;
	}

	// 设置子节点的值,不适用有多个同名子节点的情况
	template<class T>
	bool SetValue(const char *name, T* value)
	{
		if (m_element == 0) return false;

		TiXmlElement * tmp_element = m_element->FirstChildElement(name);
		if (!tmp_element)
		{
			TiXmlElement insert_node(name);
			tmp_element = (TiXmlElement *)m_element->InsertEndChild(insert_node);
		}
		if (!tmp_element->FirstChild())
		{
			std::string tmp_value;
			BaseTypeToString(*value, tmp_value);
			TiXmlText sub_node(tmp_value);
			tmp_element->InsertEndChild(sub_node);
			return true;
		}
		return SetSubNodeValue(m_element, name, *value);
	}
	bool SetValue(const char *name, ConfigElementBase* value)
	{
		if (m_element == 0) return false;
		TiXmlElement * tmp_element = m_element->FirstChildElement(name);
		if (!tmp_element)
		{
			TiXmlElement insert_node(name);
			m_element->InsertEndChild(insert_node);
			tmp_element = m_element->FirstChildElement(name);
		}
		value->SetElement(tmp_element);
		return value->write();
	}

	// 设置子节点的值,适用有多个同名子节点的情况,与数组对应
	template<class T>
	bool SetListValue(const char *name, T* list_value, int list_len)
	{
		if (m_element == 0 || list_len < 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for(int i=0;i<list_len;++i,++list_value)
		{
			if (!tmp_element)
			{
				TiXmlElement insert_node(name);
				tmp_element = (TiXmlElement *)m_element->InsertEndChild(insert_node);
			}
			if (!tmp_element->FirstChild())
			{
				std::string tmp_value;
				BaseTypeToString(*list_value, tmp_value);
				TiXmlText sub_node(tmp_value);
				tmp_element->InsertEndChild(sub_node);
			}
			else
			{
				if (!SetNodeValue(tmp_element, *list_value)) return false;
			}
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		while(tmp_element)
		{
			TiXmlElement *to_remove = tmp_element;
			tmp_element = tmp_element->NextSiblingElement(name);
			m_element->RemoveChild(to_remove);
		};
		return true;
	}
	bool SetListValue(const char *name, ConfigElementBase* list_value, int list_len)
	{
		if (m_element == 0 || list_len < 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for(int i=0;i<list_len;++i)
		{
			if (!tmp_element)
			{
				TiXmlElement insert_node(name);
				tmp_element = (TiXmlElement *)m_element->InsertEndChild(insert_node);
			}

			list_value->SetElement(tmp_element);
			if (!list_value->write()) return false;

			list_value= list_value->offset();
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		while(tmp_element)
		{
			TiXmlElement *to_remove = tmp_element;
			tmp_element = tmp_element->NextSiblingElement(name);
			m_element->RemoveChild(to_remove);
		};
		return true;
	}

	// 至少有一个节点的xml结构
	TiXmlElement *m_element;

private:
};

#endif

