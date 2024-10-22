
#include "particlehelper.h"
#include "strutil.h"

#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include <algorithm>

NS_CC_BEGIN

bool CCParticleHelper::IsEqual(float left, float right)
{
	if ((left - right) <= PRECISION && (left - right) >= -PRECISION)
	{
		return true;
	}
	return false;
}

bool CCParticleHelper::IsEqual(int left, int right)
{
	if (left == right)
	{
		return true;
	}
	return false;
}

bool CCParticleHelper::IsEqual(bool left, bool right)
{
	if (left == right)
	{
		return true;
	}
	return false;
}

bool CCParticleHelper::IsEqual(const Vec2& left, const Vec2& right)
{
	if (IsEqual(left.x, right.x) && IsEqual(left.y, right.y))
	{
		return true;
	}
	return false;
}

bool CCParticleHelper::IsEqual(const Vec3& left, const Vec3& right)
{
	if (IsEqual(left.x, right.x) && IsEqual(left.y, right.y) && IsEqual(left.z, right.z))
	{
		return true;
	}
	return false;
}

bool CCParticleHelper::IsEqual(const Color4F& left, const Color4F& right)
{
	if (IsEqual(left.r, right.r) && IsEqual(left.g, right.g) && IsEqual(left.b, right.b) && IsEqual(left.a, right.a))
	{
		return true;
	}
	return false;
}

float CCParticleHelper::ParseFloat(const std::string& val)
{
	float ret = 0.0f;
	sscanf(val.c_str(), "%g", &ret);
	return ret;
}

int CCParticleHelper::ParseInt(const std::string& val)
{
	int ret = 0;
	sscanf(val.c_str(), "%d", &ret);
	return ret;
}

bool CCParticleHelper::ParseBool(const std::string& val)
{
	if (StartsWith(val, "true"))
		return true;
	else if (StartsWith(val, "false"))
		return false;
	else
		return false;
}

Vec2* CCParticleHelper::ParseVec2(const std::string& val, Vec2* pOut)
{
	if (pOut != 0)
	{
		sscanf(val.c_str(), "%g %g", &pOut->x, &pOut->y);
	}
	return pOut;
}

Vec3* CCParticleHelper::ParseVec3(const std::string& val, Vec3* pOut)
{
	if (pOut != 0)
	{
		sscanf(val.c_str(), "%g %g %g", &pOut->x, &pOut->y, &pOut->z);
	}
	return pOut;
}

Color4F* CCParticleHelper::ParseColor(const std::string& val, Color4F* pOut)
{
	if (pOut != 0)
	{
		sscanf(val.c_str(), "%g %g %g %g", &pOut->r, &pOut->g, &pOut->b, &pOut->a);
	}
	return pOut;
}

Rect* CCParticleHelper::ParseRect(const std::string& val, Rect* pOut)
{
	if (pOut != 0)
	{
		float left = -32;
		float top = -32;
		float right = 32;
		float bottom = 32;
		sscanf(val.c_str(), "%g %g %g %g", &left, &top, &right, &bottom);
		pOut->setRect(left, top, right - left, bottom - top);
	}
	return pOut;
}

void CCParticleHelper::Trim( std::string& str, bool left, bool right)
{
	static const std::string delims = " \t\r";
	if(right)
		str.erase(str.find_last_not_of(delims)+1); // trim right
	if(left)
		str.erase(0, str.find_first_not_of(delims)); // trim left
}

std::string CCParticleHelper::ToString(float val)
{
	char tmpbuf[64];
	memset(tmpbuf, 0, 64);
	sprintf( tmpbuf, "%g", val);

	return std::string(tmpbuf);
}

std::string CCParticleHelper::ToString(int val)
{
	char tmpbuf[64];
	memset(tmpbuf, 0, 64);
	sprintf( tmpbuf, "%d", val);

	return std::string(tmpbuf);
}

std::string CCParticleHelper::ToString(bool val)
{
	if (val)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

std::string CCParticleHelper::ToString(const Vec2& val)
{
	char tmpbuf[256];
	memset(tmpbuf, 0, 256);
	sprintf( tmpbuf, "%g %g", val.x, val.y);

	return std::string(tmpbuf);
}

std::string CCParticleHelper::ToString(const Vec3& val)
{
	char tmpbuf[256];
	memset(tmpbuf, 0, 256);
	sprintf( tmpbuf, "%g %g %g", val.x, val.y, val.z);

	return std::string(tmpbuf);
}

std::string CCParticleHelper::ToString(const Color4F& val)
{
	char tmpbuf[256];
	memset(tmpbuf, 0, 256);
	sprintf( tmpbuf, "%g %g %g %g", val.r, val.g, val.b, val.a);

	return std::string(tmpbuf);
}

std::string CCParticleHelper::ToString(const Rect& val)
{
	char tmpbuf[256];
	memset(tmpbuf, 0, 256);
	sprintf( tmpbuf, "%g %g %g %g", val.origin.x, val.origin.y, val.origin.x + val.size.width, val.origin.y + val.size.height);

	return std::string(tmpbuf);
}

void CCParticleHelper::ToLowerCase( std::string& str )
{
	StrUtil::toLowerCase(str);
}

void CCParticleHelper::ToUpperCase( std::string& str )
{
	StrUtil::toUpperCase(str);
}

bool CCParticleHelper::StartsWith(const std::string& str, const std::string& pattern, bool lowerCase )
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string startOfThis = str.substr(0, patternLen);
	if (lowerCase)
		ToLowerCase(startOfThis);

	return (startOfThis == pattern);
}

bool CCParticleHelper::EndsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
	if (lowerCase)
		ToLowerCase(endOfThis);

	return (endOfThis == pattern);
}

void CCParticleHelper::Split(std::vector<std::string>& ret,  const std::string& str, const std::string& delims, unsigned int maxSplits, bool preserveDelims)
{
	ret.clear();
	// Pre-allocate some space for performance
	ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do 
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the string
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			// Copy up to delimiter
			ret.push_back( str.substr(start, pos - start) );

			if(preserveDelims)
			{
				// Sometimes there could be more than one delimiter in a row.
				// Loop until we don't find any more delims
				size_t delimStart = pos, delimPos;
				delimPos = str.find_first_not_of(delims, delimStart);
				if (delimPos == std::string::npos)
				{
					// Copy the rest of the string
					ret.push_back( str.substr(delimStart) );
				}
				else
				{
					ret.push_back( str.substr(delimStart, delimPos - delimStart) );
				}
			}

			start = pos + 1;
		}
		// parse up to next real data
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != std::string::npos);
}

float CCParticleHelper::UnitRandom ()
{
	return (float)rand() / RAND_MAX;
}

float CCParticleHelper::RangeRandom(float fmin, float fmax)
{
	return (fmax - fmin)*UnitRandom() + fmin;
}

Vec3* CCParticleHelper::Perpendicular(Vec3* pOut, Vec3* pIn)
{
	if (pOut == 0 || pIn == 0)
	{
		return 0;
	}

	static const float fSquareZero = (float)(1e-06 * 1e-06);
	Vec3 unit_x (1,0,0);

	Vec3::cross(*pIn,unit_x,pOut);
	
	float l = pOut->length();

    if( l < fSquareZero )
    {
		Vec3 unit_y (0,1,0);
		Vec3::cross(*pIn,unit_y,pOut);
    }
	pOut->normalize();

    return pOut;
}

 Vec3* CCParticleHelper::RandomDeviant(float angle, Vec3* pUp, Vec3* pOut, Vec3* pIn)
 {
 	if (pUp == 0 || pOut == 0 || pIn == 0)
 	{
 		return 0;
 	}

	Vec3 newUp (0, 1, 0);
	newUp=*pUp;
 	Quaternion q (0,0,0,1);
 	float temp = UnitRandom() * kmPI * 2.0f;

 	Quaternion::QuaternionRotationAxis(&q, pIn, temp);

	Quaternion::QuaternionMultiplyVec3(&newUp, &q, &newUp);
 
	Quaternion::QuaternionRotationAxis(&q, &newUp, angle * kmPIOver180);

	return Quaternion::QuaternionMultiplyVec3(pOut, &q, pIn);
 }

 Color4F* CCParticleHelper::SaturateColor(Color4F* color)
 {
	 if (color->r < 0)
		 color->r = 0;
	 else if (color->r > 1)
		 color->r = 1;

	 if (color->g < 0)
		 color->g = 0;
	 else if (color->g > 1)
		 color->g = 1;

	 if (color->b < 0)
		 color->b = 0;
	 else if (color->b > 1)
		 color->b = 1;

	 if (color->a < 0)
		 color->a = 0;
	 else if (color->a > 1)
		 color->a = 1;

	 return color;
 }

 Color4F*  CCParticleHelper::SubColour(Color4F* pOut, Color4F* p1, Color4F* p2)
 {
	 pOut->a = p1->a - p2->a;
	 pOut->r = p1->r - p2->r;
	 pOut->g = p1->g - p2->g;
	 pOut->b = p1->b - p2->b;
	 return pOut;
 }

 Color4F* CCParticleHelper::AddColour(Color4F* pOut, Color4F* p1, Color4F* p2)
 {
	 pOut->a = p1->a + p2->a;
	 pOut->r = p1->r + p2->r;
	 pOut->g = p1->g + p2->g;
	 pOut->b = p1->b + p2->b;
	 return pOut;
 }

 Color4F* CCParticleHelper::ScaleColour(Color4F* pOut, float scale)
 {
	 pOut->a *= scale;
	 pOut->r*= scale;
	 pOut->g *= scale;
	 pOut->b *= scale;
	 return pOut;
 }

NS_CC_END