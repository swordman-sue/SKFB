#ifndef PARTICLEHELPER_H
#define PARTICLEHELPER_H

#include "cocos2d.h"
#include <string>
#include <vector>
#include "base/ccTypes.h"

#define  PRECISION    0.00001
#define  kmPI         3.141592f
#define  EMPTY_PARTICLE "empty_ps"

NS_CC_BEGIN

struct Color4F;
class Rect;

enum MaterialType
{
	MAT_ALPHA_BLEND = 0,
	MAT_ADD,
};

class CCParticleHelper
{
public:
	static bool IsEqual(float left, float right);

	static bool IsEqual(int left, int right);

	static bool IsEqual(bool left, bool right);

	static bool IsEqual(const Vec2& left, const Vec2& right);

	static bool IsEqual(const Vec3& left, const Vec3& right);

	static bool IsEqual(const Color4F& left, const Color4F& right);

	static float ParseFloat(const std::string& val);

	static int ParseInt(const std::string& val);

	static bool ParseBool(const std::string& val);

	static Vec2* ParseVec2(const std::string& val, Vec2* pOut);

	static Vec3* ParseVec3(const std::string& val, Vec3* pOut);

	static Color4F* ParseColor(const std::string& val, Color4F* pOut);

	static Rect* ParseRect(const std::string& val, Rect* pOut);

	static std::string ToString(float val);

	static std::string ToString(int val);

	static std::string ToString(bool val);

	static std::string ToString(const Vec2& val);

	static std::string ToString(const Vec3& val);

	static std::string ToString(const Color4F& val);

	static std::string ToString(const Rect& val);

	static void Trim( std::string& str, bool left = true, bool right = true );

	static void ToLowerCase( std::string& str );

	static void ToUpperCase( std::string& str );

	static bool StartsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);

	static bool EndsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);

	static void Split(std::vector<std::string>& ret, const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);

	static float UnitRandom ();
	
	static float RangeRandom(float fmin, float fmax);

	static Vec3* Perpendicular(Vec3* pOut, Vec3* pIn);

	static Vec3* RandomDeviant(float angle, Vec3* pUp, Vec3* pOut, Vec3* pIn);

	static Color4F* SaturateColor(Color4F* color);

	static Color4F* SubColour(Color4F* pOut, Color4F* p1, Color4F* p2);

	static Color4F* AddColour(Color4F* pOut, Color4F* p1, Color4F* p2);

	static Color4F* ScaleColour(Color4F* pOut, float scale);
};

NS_CC_END

#endif