#ifndef CCUDIM_H__
#define CCUDIM_H__

#include "cocos/math/CCGeometry.h"
//#include "support/CCPointExtension.h"

NS_CC_BEGIN

class CCUDim
{
public:
    CCUDim() {}
    CCUDim(float scale, float offset) : d_scale(scale), d_offset(offset) {}
    CCUDim(const CCUDim& v): d_scale(v.d_scale), d_offset(v.d_offset) {}
    ~CCUDim() {}

    float asAbsolute(float base) const
    {
        return base * d_scale + d_offset;
    }
    float asRelative(float base) const
    {
        return (base != 0.0f) ? d_offset / base + d_scale : 0.0f;
    }

    CCUDim operator+(const CCUDim& other) const
    {
        return CCUDim(d_scale + other.d_scale, d_offset + other.d_offset);
    }
    CCUDim operator-(const CCUDim& other) const
    {
        return CCUDim(d_scale - other.d_scale, d_offset - other.d_offset);
    }
    CCUDim operator*(const CCUDim& other) const
    {
        return CCUDim(d_scale * other.d_scale, d_offset * other.d_offset);
    }
    CCUDim operator/(const CCUDim& other) const
    {
        return CCUDim(other.d_scale == 0.0f ? 0.0f : d_scale / other.d_scale,
                    other.d_offset == 0.0f ? 0.0f : d_offset / other.d_offset);
    }

    const CCUDim& operator+=(const CCUDim& other)
    {
        d_scale += other.d_scale;
        d_offset += other.d_offset;
        return *this;
    }
    const CCUDim& operator-=(const CCUDim& other)
    {
        d_scale -= other.d_scale;
        d_offset -= other.d_offset;
        return *this;
    }
    const CCUDim& operator*=(const CCUDim& other)
    {
        d_scale *= other.d_scale;
        d_offset *= other.d_offset;
        return *this;
    }
    const CCUDim& operator/=(const CCUDim& other)
    {
        // division by zero sets component to zero.  Not technically correct
        // but probably better than exceptions and/or NaN values.
        d_scale = (other.d_scale == 0.0f ? 0.0f : d_scale / other.d_scale);
        d_offset = (other.d_offset == 0.0f ? 0.0f : d_offset / other.d_offset);
        return *this;
    }

    bool operator==(const CCUDim& other) const
    {
        return d_scale == other.d_scale && d_offset == other.d_offset;
    }
    bool operator!=(const CCUDim& other) const
    {
        return !operator==(other);
    }

    float d_scale, d_offset;
};

/*!
\brief
    Two dimensional vector class built using unified dimensions (UDims).
    The UVector2 class is used for representing both positions and sizes.
*/
class CCUVector
{
public:
    CCUVector() {}
    CCUVector(const CCUDim& x, const CCUDim& y) : d_x(x), d_y(y) {}
    CCUVector(const CCUVector& v): d_x(v.d_x), d_y(v.d_y) {}
    ~CCUVector() {}

    Vec2 asAbsolutePoint(const Size& base) const
    {
        return ccp(d_x.asAbsolute(base.width), d_y.asAbsolute(base.height));
    }
    Vec2 asRelativePoint(const Size& base) const
    {
        return ccp(d_x.asRelative(base.width), d_y.asRelative(base.height));
    }

	Size asAbsoluteSize(const Size& base) const
	{
		return CCSizeMake(d_x.asAbsolute(base.width), d_y.asAbsolute(base.height));
	}
	Size asRelativeSize(const Size& base) const
	{
		return CCSizeMake(d_x.asRelative(base.width), d_y.asRelative(base.height));
	}

	bool isZero()
	{
		return d_x.d_scale == 0 && d_x.d_offset == 0&&
			d_y.d_scale == 0 &&d_y.d_offset == 0;
	}

    CCUVector operator+(const CCUVector& other) const
    {
        return CCUVector(d_x + other.d_x, d_y + other.d_y);
    }
    CCUVector operator-(const CCUVector& other) const
    {
        return CCUVector(d_x - other.d_x, d_y - other.d_y);
    }
    CCUVector operator/(const CCUVector& other) const
    {
        return CCUVector(d_x / other.d_x, d_y / other.d_y);
    }
    CCUVector operator*(const CCUVector& other) const
    {
        return CCUVector(d_x * other.d_x, d_y * other.d_y);
    }

    const CCUVector& operator+=(const CCUVector& other)
    {
        d_x += other.d_x;
        d_y += other.d_y;
        return *this;
    }
    const CCUVector& operator-=(const CCUVector& other)
    {
        d_x -= other.d_x;
        d_y -= other.d_y;
        return *this;
    }
    const CCUVector& operator/=(const CCUVector& other)
    {
        d_x /= other.d_x;
        d_y /= other.d_y;
        return *this;
    }
    const CCUVector& operator*=(const CCUVector& other)
    {
        d_x *= other.d_x;
        d_y *= other.d_y;
        return *this;
    }

    CCUVector operator+(const CCUDim& dim) const
    {
        return CCUVector(d_x + dim, d_y + dim);
    }
    CCUVector operator-(const CCUDim& dim) const
    {
        return CCUVector(d_x - dim, d_y - dim);
    }
    CCUVector operator/(const CCUDim& dim) const
    {
        return CCUVector(d_x / dim, d_y / dim);
    }
    CCUVector operator*(const CCUDim& dim) const
    {
        return CCUVector(d_x * dim, d_y * dim);
    }

    const CCUVector& operator+=(const CCUDim& dim)
    {
        d_x += dim;
        d_y += dim;
        return *this;
    }
    const CCUVector& operator-=(const CCUDim& dim)
    {
        d_x -= dim;
        d_y -= dim;
        return *this;
    }
    const CCUVector& operator/=(const CCUDim& dim)
    {
        d_x /= dim;
        d_y /= dim;
        return *this;
    }
    const CCUVector& operator*=(const CCUDim& dim)
    {
        d_x *= dim;
        d_y *= dim;
        return *this;
    }

    bool operator==(const CCUVector& other) const
    {
        return d_x == other.d_x && d_y == other.d_y;
    }
    bool operator!=(const CCUVector& other) const
    {
        return !operator==(other);
    }

    CCUDim d_x, d_y;
};

class CCURect
{
public:
    CCURect() {}

    CCURect(const CCUVector& min, const CCUVector& max) : d_min(min), d_max(max) {}

    CCURect(const CCUDim& left, const CCUDim& top, const CCUDim& right, const CCUDim& bottom)
    {
        d_min.d_x = left;
        d_min.d_y = top;
        d_max.d_x = right;
        d_max.d_y = bottom;
    }

    CCURect(const CCURect& v): d_min(v.d_min), d_max(v.d_max) {}

    ~CCURect() {}

    Rect asAbsolute(const Size& base) const
    {
        return CCRectMake(
                   d_min.d_x.asAbsolute(base.width),
                   d_min.d_y.asAbsolute(base.height),
                   d_max.d_x.asAbsolute(base.width),
                   d_max.d_y.asAbsolute(base.height)
               );
    }

    Rect asRelative(const Size& base) const
    {
        return CCRectMake(
                   d_min.d_x.asRelative(base.width),
                   d_min.d_y.asRelative(base.height),
                   d_max.d_x.asRelative(base.width),
                   d_max.d_y.asRelative(base.height)
               );
    }

    const CCUVector& getPosition() const
    {
        return d_min;
    }
    CCUVector getSize() const
    {
        return d_max - d_min;
    }
    CCUDim getWidth() const
    {
        return d_max.d_x - d_min.d_x;
    }
    CCUDim getHeight() const
    {
        return d_max.d_y - d_min.d_y;
    }

    void setPosition(const CCUVector& pos)
    {
        CCUVector sz(d_max - d_min);
        d_min = pos;
        d_max = d_min + sz;
    }

    void setSize(const CCUVector& sz)
    {
        d_max = d_min + sz;
    }

    void setWidth(const CCUDim& w)
    {
        d_max.d_x = d_min.d_x + w;
    }
    void setHeight(const CCUDim& h)
    {
        d_max.d_y = d_min.d_y + h;
    }

    void offset(const CCUVector& sz)
    {
        d_min += sz;
        d_max += sz;
    }

    CCURect operator*(const CCUDim& dim) const
    {
        return CCURect(d_min * dim, d_max * dim);
    }

    CCURect operator+(const CCURect& r) const
    {
        return CCURect(d_min + r.d_min, d_max + r.d_max);
    }

    CCUVector d_min, d_max;
};

NS_CC_END

#endif