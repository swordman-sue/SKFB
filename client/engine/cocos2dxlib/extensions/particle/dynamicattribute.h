#ifndef DYNAMICATTRIBUTE_H
#define DYNAMICATTRIBUTE_H

#include "simplespline.h"

#include "math/vec2.h"
#include <string>
#include <vector>

namespace tinyxml2
{
	class XMLElement;
	class XMLDocument;
}


NS_CC_BEGIN

struct ControlPointSorter
{
	inline bool operator()(const Vec2& a, const Vec2& b)
	{
		return a.x < b.x;
	}
};

class  DynamicAttribute
{
	public:
		enum DynamicAttributeType
		{
			DAT_NONE,
			DAT_FIXED,
			DAT_RANDOM,
			DAT_CURVED,
		};

		/** Constructor
		*/
		DynamicAttribute (void) : m_type(DAT_NONE)
		{}

		/** Destructor
		*/
		virtual ~DynamicAttribute (void)
		{}

		/** Virtual function that needs to be implemented by its childs.
		*/
		virtual float GetValue (float x = 0) = 0;

		/** Todo
		*/
		DynamicAttributeType GetType (void) const
		{
			return m_type;
		}

		/** Todo
		*/
		void SetType (DynamicAttributeType type)
		{
			m_type = type;
		}

		/** Todo
		*/
		virtual void CopyAttributesTo(DynamicAttribute* dynamicAttribute) = 0;

		virtual bool SetAttribute(const char* type, const char* value) = 0;

		// 解决特效不精确问题 add by zhuyi 2014-09-11
		virtual float GetMaxValue()=0;
		virtual float GetMinValue()=0;
		// end
		virtual float GetCalculusValue()
		{
			return 0.0f;
		}

		virtual bool GetAttribute(const char* type, char* value, size_t size) = 0;

		virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element) = 0;

	protected:
		DynamicAttributeType m_type;
};

/*	This class is an implementation of the DynamicAttribute class in its most simple form. It just returns a value
	that has previously been set.
@remarks
	Although use of a regular attribute within the class that needs it is preferred, its benefit is that it makes
	use of the generic 'getValue' mechanism of a DynamicAttribute.
*/
class  DynamicAttributeFixed : public DynamicAttribute
{
	public:
		/** Constructor
		*/
		DynamicAttributeFixed (void) : m_value(0.0)
		{
			m_type = DAT_FIXED;
		}

		/** Copy constructor
		*/
		DynamicAttributeFixed (const DynamicAttributeFixed& dynamicAttributeFixed);

		/** Destructor
		*/
		~DynamicAttributeFixed (void)
		{}

		/** Todo
		*/
		virtual float GetValue (float x = 0)
		{
			return m_value;
		}

		/** Todo
		*/
		virtual void SetValue (float value)
		{
			m_value = value;
		}

		// 解决特效不精确问题 add by zhuyi 2014-09-11
		virtual float GetMaxValue()
		{
			return m_value;
		}

		virtual float GetMinValue()
		{
			return m_value;
		}
		// end

		/** Todo
		*/
		virtual void CopyAttributesTo(DynamicAttribute* dynamicAttribute);

		virtual bool SetAttribute(const char* type, const char* value);

		virtual bool GetAttribute(const char* type, char* value, size_t size);

		virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

	protected:
		float m_value;
};

/* This class generates random values within a given minimum and maximum interval.
*/
class  DynamicAttributeRandom : public DynamicAttribute
{
	public:
		/** Constructor
		*/
		DynamicAttributeRandom (void) : m_min(0.0), m_max(0.0)
		{
			m_type = DAT_RANDOM;
		}

		/** Copy constructor
		*/
		DynamicAttributeRandom (const DynamicAttributeRandom& dynamicAttributeRandom);

		/** Destructor
		*/
		~DynamicAttributeRandom (void)
		{}

		/** Todo
		*/
		virtual float GetValue (float x = 0);

		float GetMin (void) const
		{
			return m_min;
		}

		float GetMax (void) const
		{
			return m_max;
		}

		void SetMinMax (float min, float max)
		{
			m_min = min;
			m_max = max;
		}

		// 解决特效不精确问题 add by zhuyi 2014-09-11
		virtual float GetMaxValue()
		{
			return m_max;
		}

		virtual float GetMinValue()
		{
			return m_min;
		}
		// end

		/** Todo
		*/
		virtual void CopyAttributesTo(DynamicAttribute* dynamicAttribute);

		virtual bool SetAttribute(const char* type, const char* value);

		virtual bool GetAttribute(const char* type, char* value, size_t size);

		virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);

protected:
		float m_min, m_max;
};

class DynamicAttributeCurved : public DynamicAttribute
{
	public:
		static bool m_force_linear;

		enum InterpolationType
		{
			IT_LINEAR,
			IT_SPLINE
		};

		typedef std::vector<Vec2> ControlPointList;
		typedef ControlPointList::iterator ControlIterotor;

		/** Constructor
		*/
		DynamicAttributeCurved (void);
		DynamicAttributeCurved (InterpolationType interpolationType);

		/** Copy constructor
		*/
		DynamicAttributeCurved (const DynamicAttributeCurved& dynamicAttributeCurved);

		/** Destructor
		*/
		~DynamicAttributeCurved (void)
		{
		}

		/** Get and set the curve type
		*/
		void SetInterpolationType (InterpolationType interpolationType);

		InterpolationType GetInterpolationType (void) const;

		/** Todo
		*/
		virtual float GetValue (float x = 0);

		/** Todo
		*/
		virtual void AddControlPoint (float x, float y);

		virtual void AddControlPoint (const Vec2& point);

		/** Todo
		*/
		const ControlPointList& GetControlPoints (void) const;

		/** Todo
		*/
		void ProcessControlPoints (void);

		/** Todo
		*/
		size_t GetNumControlPoints(void) const;

		// 解决特效不精确问题 add by zhuyi 2014-09-11
		virtual float GetMaxValue();
		virtual float GetMinValue();
		// end
		virtual float GetCalculusValue();

		/** Todo
		*/
		void RemoveAllControlPoints(void);

		/** Todo
		*/
		virtual void CopyAttributesTo(DynamicAttribute* dynamicAttribute);

		virtual bool SetAttribute(const char* type, const char* value);

		virtual bool GetAttribute(const char* type, char* value, size_t size);

		static void SetForceLinear(bool is_linear) { m_force_linear = is_linear; }

		virtual void SaveScript(tinyxml2::XMLDocument *my_document, tinyxml2::XMLElement *element);
	protected:
		size_t m_last_index;

		float m_last_time;
		/** Todo
		*/
		float m_range;

		/** Todo
		*/
		CCSimpleSpline m_spline;

		/** Todo
		*/
		InterpolationType m_interpolation_type;

		/** Todo
		*/
		ControlPointList m_control_points;

		/** Find an iterator that forms the low (left) value of the interval where x lies in.
		*/
		inline size_t FindNearestControlPointIterator(float x);

		/** Helper functions 
		*/
		inline ControlIterotor GetFirstValidIterator(void);
		inline ControlIterotor GetLastValidIterator(void);
};

float   AttributeCalculate(DynamicAttribute* dyn, float x, float defaultValue = 0.0f);

DynamicAttribute* CloneDynamicAttribute(DynamicAttribute* dynamicAttribute);

DynamicAttribute* CreateDynamicAttribute(const char* type);

NS_CC_END

#endif