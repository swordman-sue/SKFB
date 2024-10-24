#ifndef SIMPLESPLINE_H
#define SIMPLESPLINE_H

#include "cocos2d.h"
#include <vector>

NS_CC_BEGIN

class CCSimpleSpline
{
public:
    CCSimpleSpline();
    ~CCSimpleSpline();

    /** Adds a control point to the end of the spline. */
    void AddPoint(const Vec3& p);

    /** Gets the detail of one of the control points of the spline. */
    const Vec3& GetPoint(unsigned short index) const;

    /** Gets the number of control points in the spline. */
    unsigned short GetNumPoints(void) const;

    /** Clears all the points in the spline. */
    void Clear(void);

    /** Updates a single point in the spline. 
    @remarks
        This point must already exist in the spline.
    */
    void UpdatePoint(unsigned short index, const Vec3& value);

    /** Returns an interpolated point based on a parametric value over the whole series.
    @remarks
        Given a t value between 0 and 1 representing the parametric distance along the
        whole length of the spline, this method returns an interpolated point.
    @param t Parametric value.
    */
    Vec3 Interpolate(float t) const;

    /** Interpolates a single segment of the spline given a parametric value.
    @param fromIndex The point index to treat as t=0. fromIndex + 1 is deemed to be t=1
    @param t Parametric value
    */
    Vec3 Interpolate(unsigned int fromIndex, float t) const;


    /** Tells the spline whether it should automatically calculate tangents on demand
        as points are added.
    @remarks
        The spline calculates tangents at each point automatically based on the input points.
        Normally it does this every time a point changes. However, if you have a lot of points
        to add in one go, you probably don't want to incur this overhead and would prefer to 
        defer the calculation until you are finished setting all the points. You can do this
        by calling this method with a parameter of 'false'. Just remember to manually call 
        the recalcTangents method when you are done.
    @param autoCalc If true, tangents are calculated for you whenever a point changes. If false, 
        you must call reclacTangents to recalculate them when it best suits.
    */
    void SetAutoCalculate(bool autoCalc);

    /** Recalculates the tangents associated with this spline. 
    @remarks
        If you tell the spline not to update on demand by calling setAutoCalculate(false)
        then you must call this after completing your updates to the spline points.
    */
    void RecalcTangents(void);

	void SetDynamicPoint(std::vector<Vec2>& points);

protected:

    bool mAutoCalc;

	std::vector<Vec3> mPoints;
	std::vector<Vec3> mTangents;

    /// Matrix of coefficients 
    Mat4 mCoeffs;
};

NS_CC_END
#endif // DreamSimpleSpline_h__


