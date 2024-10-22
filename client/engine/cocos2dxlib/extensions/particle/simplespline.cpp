
#include "simplespline.h"
#include "base/ccTypes.h"
#include "math/CCMath.h"
#include "math/vec4.h"

NS_CC_BEGIN

//---------------------------------------------------------------------
CCSimpleSpline::CCSimpleSpline()
{
    // Set up matrix
    // Hermite polynomial
    mCoeffs.m[0] = 2;
    mCoeffs.m[1] = -2;
    mCoeffs.m[2] = 1;
    mCoeffs.m[3] = 1;
    mCoeffs.m[4] = -3;
    mCoeffs.m[5] = 3;
    mCoeffs.m[6] = -2;
    mCoeffs.m[7] = -1;
    mCoeffs.m[8] = 0;
	mCoeffs.m[9] = 0;
	mCoeffs.m[10] = 1;
    mCoeffs.m[11] = 0;
    mCoeffs.m[12] = 1;
    mCoeffs.m[13] = 0;
    mCoeffs.m[14] = 0;
    mCoeffs.m[15] = 0;

    mAutoCalc = true;
}
//---------------------------------------------------------------------
CCSimpleSpline::~CCSimpleSpline()
{
}
//---------------------------------------------------------------------
void CCSimpleSpline::AddPoint(const Vec3& p)
{
    mPoints.push_back(p);
    if (mAutoCalc)
    {
        RecalcTangents();
    }
}
//---------------------------------------------------------------------
Vec3 CCSimpleSpline::Interpolate(float t) const
{
    // Currently assumes points are evenly spaced, will cause velocity
    // change where this is not the case
    // TODO: base on arclength?

        
    // Work out which segment this is in
    float fSeg = t * (mPoints.size() - 1);
    unsigned int segIdx = (unsigned int)fSeg;
    // Apportion t 
    t = fSeg - segIdx;

    return Interpolate(segIdx, t);

}
//---------------------------------------------------------------------
Vec3 CCSimpleSpline::Interpolate(unsigned int fromIndex, float t) const
{
    // Bounds check
    assert (fromIndex < mPoints.size() &&
        "fromIndex out of bounds");

    if ((fromIndex + 1) == mPoints.size())
    {
        // Duff request, cannot blend to nothing
        // Just return source
        return mPoints[fromIndex];

    }

    // Fast special cases
    if (t == 0.0f)
    {
        return mPoints[fromIndex];
    }
    else if(t == 1.0f)
    {
        return mPoints[fromIndex + 1];
    }

    // Real interpolation
    // Form a vector of powers of t
    float t2, t3;
    t2 = t * t;
    t3 = t2 * t;
	Vec4 powers (t3, t2, t, 1);


    // Algorithm is ret = powers * mCoeffs * Matrix4(point1, point2, tangent1, tangent2)
    const Vec3& point1 = mPoints[fromIndex];
    const Vec3& point2 = mPoints[fromIndex+1];
    const Vec3& tan1 = mTangents[fromIndex];
    const Vec3& tan2 = mTangents[fromIndex+1];
    Mat4 pt;

    pt.m[0] = point1.x;
    pt.m[1] = point1.y;
    pt.m[2] = point1.z;
    pt.m[3] = 1.0f;
    pt.m[4] = point2.x;
    pt.m[5] = point2.y;
    pt.m[6] = point2.z;
    pt.m[7] = 1.0f;
    pt.m[8] = tan1.x;
    pt.m[9] = tan1.y;
    pt.m[10] = tan1.z;
    pt.m[11] = 1.0f;
    pt.m[12] = tan2.x;
    pt.m[13] = tan2.y;
    pt.m[14] = tan2.z;
    pt.m[15] = 1.0f;

	Vec4 ret;
	mCoeffs.transformVector(powers,&ret);
	pt.transformVector(ret,&powers);
	Vec3 result(powers.x, powers.y, powers.z);

    return result;
}
//---------------------------------------------------------------------
void CCSimpleSpline::RecalcTangents(void)
{
    // Catmull-Rom approach
    // 
    // tangent[i] = 0.5 * (point[i+1] - point[i-1])
    //
    // Assume endpoint tangents are parallel with line with neighbour

    size_t i, numPoints;
    bool isClosed;

    numPoints = mPoints.size();
    if (numPoints < 2)
    {
        // Can't do anything yet
        return;
    }

    // Closed or open?
    if (mPoints[0]==mPoints[numPoints-1])
    {
        isClosed = true;
    }
    else
    {
        isClosed = false;
    }

    mTangents.resize(numPoints);

    for(i = 0; i < numPoints; ++i)
    {
        if (i ==0)
        {
            // Special case start
            if (isClosed)
            {
                // Use numPoints-2 since numPoints-1 is the last point and == [0]
				Vec3::subtract(mPoints[1],mPoints[numPoints-2],&mTangents[i]);
				mTangents[i] = mTangents[i] * 0.5;
            }
            else
            {
				Vec3::subtract(mPoints[1],mPoints[0],&mTangents[i]);
				mTangents[i] = mTangents[i] * 0.5;
            }
        }
        else if (i == numPoints-1)
        {
            // Special case end
            if (isClosed)
            {
                // Use same tangent as already calculated for [0]
				mTangents[i] = mTangents[0];
			}
            else
            {
				Vec3::subtract(mPoints[i],mPoints[i-1],&mTangents[i]);
				mTangents[i] = mTangents[i] * 0.5;
            }
        }
        else
        {
			Vec3::subtract(mPoints[i + 1],mPoints[i - 1],&mTangents[i]);
			mTangents[i] = mTangents[i] * 0.5;
        }
            
    }
}
//---------------------------------------------------------------------
const Vec3& CCSimpleSpline::GetPoint(unsigned short index) const
{
    assert (index < mPoints.size() && "Point index is out of bounds!!");

    return mPoints[index];
}
//---------------------------------------------------------------------
unsigned short CCSimpleSpline::GetNumPoints(void) const
{
    return (unsigned short)mPoints.size();
}
//---------------------------------------------------------------------
void CCSimpleSpline::Clear(void)
{
    mPoints.clear();
    mTangents.clear();
}
//---------------------------------------------------------------------
void CCSimpleSpline::UpdatePoint(unsigned short index, const Vec3& value)
{
    assert (index < mPoints.size() && "Point index is out of bounds!!");

    mPoints[index] = value;
    if (mAutoCalc)
    {
        RecalcTangents();
    }
}
//---------------------------------------------------------------------
void CCSimpleSpline::SetAutoCalculate(bool autoCalc)
{
    mAutoCalc = autoCalc;
}

void CCSimpleSpline::SetDynamicPoint(std::vector<Vec2>& points)
{
	mPoints.clear();
	int size = points.size();
	mPoints.reserve(size);
	mPoints.resize(size);
	for(int i = 0; i < size; ++i)
	{
		mPoints[i].x = points[i].x;
		mPoints[i].y = points[i].y;
		mPoints[i].z = 0.0f;
	}
	RecalcTangents();
}

NS_CC_END




