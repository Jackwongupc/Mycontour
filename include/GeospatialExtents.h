#ifndef __GEOSPATIALEXTENTS_H__
#define __GEOSPATIALEXTENTS_H__

#include <osg/Vec2>
#include <osg/Vec2d>
/*#include "osg/CoordinateSystemNode"
#include "osg/Notify"*/
#include <osg/BoundingBox>
#include <osg/BoundingSphere>

#include <float.h>

/** GeospatialExtents contains xmin/max and ymin/max for a spatial boundary.
*/
class GeospatialExtents
{
public:
	/*范围的最小值*/
    osg::Vec2d  _min;

	/*范围的最大值*/
    osg::Vec2d  _max;

	/*标志： 表示是否为大地范围*/
    bool        _isGeographic;
    
	/*默认构造函数*/
    inline GeospatialExtents() : 
        _min(DBL_MAX,DBL_MAX),
        _max(-DBL_MAX,-DBL_MAX),
        _isGeographic(false) {}

	/*拷贝构造函数*/
    inline GeospatialExtents(const GeospatialExtents& rhs):
        _min(rhs._min),
        _max(rhs._max),
        _isGeographic(rhs._isGeographic) {}

  /** 重载构造函数，xmin x为最小值，ymin y最小值， xmax x最大值， ymax y最大值，isGeographic 是否为大地范围
  */
    inline GeospatialExtents(double xmin, double ymin, double xmax, double ymax, bool isGeographic) :
        _min(xmin, ymin),
        _max(xmax, ymax),
        _isGeographic(isGeographic) {}
   
	/*赋值函数*/
    GeospatialExtents& operator = (const GeospatialExtents& rhs)
    {
        _min = rhs._min;
        _max = rhs._max;
        _isGeographic = rhs._isGeographic;
        return *this;
    }

	/*重载运算符！=*/
    bool operator != (const GeospatialExtents& rhs) const
    {
        return (_min != rhs._min) || (_max != rhs._max) || (_isGeographic != rhs._isGeographic);
    }

	/*重载运算符==*/
    bool operator == (const GeospatialExtents& rhs) const
    {
        return (_min == rhs._min) && (_max == rhs._max) && (_isGeographic == rhs._isGeographic);
    }

	/*重载运算符>=*/
	bool operator >= (const GeospatialExtents& rhs) const
	{
		return (_min.x() <= rhs._min.x()) && (_min.y() <= rhs._min.y()) &&(_max.x() >= rhs._max.x()) &&(_max.y() >= rhs._max.y()) && (_isGeographic == rhs._isGeographic);
	}

	/*判断是否相等*/
    bool equivalent(const GeospatialExtents& rhs, double epsilon=1e-6) const
    {
        return osg::equivalent(_min.x(),rhs._min.x(),epsilon) &&
               osg::equivalent(_min.y(),rhs._min.y(),epsilon) && 
               osg::equivalent(_max.x(),rhs._max.x(),epsilon) && 
               osg::equivalent(_max.y(),rhs._max.y(),epsilon) && 
               _isGeographic == rhs._isGeographic;
    }

	/*得到x最小值*/
    inline double& xMin() { return _min.x(); }
    inline double xMin() const { return _min.x(); }

	/*得到y最小值*/
    inline double& yMin() { return _min.y(); }
    inline double yMin() const { return _min.y(); }

	/*得到x最大值*/
    inline double& xMax() { return _max.x(); }
    inline double xMax() const { return _max.x(); }

	/*得到y最大值*/
    inline double& yMax() { return _max.y(); }
    inline double yMax() const { return _max.y(); }

	/*初始化函数*/
    inline void init()
    {
        _min.set(DBL_MAX,DBL_MAX);
        _max.set(-DBL_MAX,-DBL_MAX);
    }

	/*判断对象是否有效*/
    inline bool valid() const
    {
        return _max.x()>=_min.x() &&  _max.y()>=_min.y();
    }

	/*判断包围盒不为空*/
    inline bool nonZeroExtents() const
    {
        return valid() && _max.x()!=_min.x() &&  _max.y()!=_min.y();
    }

	/*得到包围盒半径*/
    inline double radius() const
    {
        return sqrt((radius2()));
    }

	/*包围盒半径的平方*/
    inline double radius2() const
    {
        return 0.25f*((_max-_min).length2());
    }

	/*判断是否相交，GeospatialExtents 为相交的区域， e 判断的范围， xoffset 偏移量*/
    GeospatialExtents intersection(const GeospatialExtents& e, double xoffset) const
    {
        return GeospatialExtents(osg::maximum(xMin(),e.xMin()+xoffset),osg::maximum(yMin(),e.yMin()),
                                 osg::minimum(xMax(),e.xMax()+xoffset),osg::minimum(yMax(),e.yMax()),_isGeographic);
    }

    /** Return true if this bounding box intersects the specified bounding box. */
	/*判断当前包围盒是否与某个区域相交*/
    bool intersects(const GeospatialExtents& bb) const
    {
        if (_isGeographic)
        {
            // first check vertical axis overlap
            if (osg::maximum(yMin(),bb.yMin()) > osg::minimum(yMax(),bb.yMax())) return false;
            
            // next check if overlaps directly without any 360 degree horizontal shifts.
            if (osg::maximum(xMin(),bb.xMin()) <= osg::minimum(xMax(),bb.xMax())) return true;
            
            // next check if a 360 rotation will produce an overlap
            float rotationAngle = (xMin() > bb.xMin()) ? 360.0 : -360;
            return (osg::maximum(xMin(),bb.xMin()+rotationAngle) <= osg::minimum(xMax(),bb.xMax()+rotationAngle));
        }
        else
        {
            return (osg::maximum(xMin(),bb.xMin()) <= osg::minimum(xMax(),bb.xMax()) &&
                    osg::maximum(yMin(),bb.yMin()) <= osg::minimum(yMax(),bb.yMax()));
        }
    }

	/*根据包围盒扩展区域范围*/
    void expandBy(const osg::BoundingSphere& sh)
    {
        if (!sh.valid()) return;

        if(sh._center.x()-sh._radius<_min.x()) _min.x() = sh._center.x()-sh._radius;
        if(sh._center.x()+sh._radius>_max.x()) _max.x() = sh._center.x()+sh._radius;

        if(sh._center.y()-sh._radius<_min.y()) _min.y() = sh._center.y()-sh._radius;
        if(sh._center.y()+sh._radius>_max.y()) _max.y() = sh._center.y()+sh._radius;
    }

	/*根据某点扩展区域范围*/
    inline void expandBy(const osg::Vec3& v)
    {
        if(v.x()<_min.x()) _min.x() = v.x();
        if(v.x()>_max.x()) _max.x() = v.x();

        if(v.y()<_min.y()) _min.y() = v.y();
        if(v.y()>_max.y()) _max.y() = v.y();
    }

	/*根据 extents扩展区域范围*/
    void expandBy(const GeospatialExtents& e)
    {
        if (!e.valid()) return;

        if(e._min.x()<_min.x()) _min.x() = e._min.x();
        if(e._max.x()>_max.x()) _max.x() = e._max.x();

        if(e._min.y()<_min.y()) _min.y() = e._min.y();
        if(e._max.y()>_max.y()) _max.y() = e._max.y();
    }

	/*判断extents中是否包含点v*/
	bool isContain(const osg::Vec3& v)
	{
		if (v.x()>=_min.x() &&v.y()>=_min.y()&&v.x()<=_max.x()&&v.y()<=_max.y())
		{
			return true;
		}
		return false;
	}

	/*判断区域是否包含extents对象*/
	bool isContain(const GeospatialExtents& e)
	{
		if(e._min.x() < _min.x()) return false;
		if(e._max.x() > _max.x()) return false;

		if(e._min.y() < _min.y()) return false;
		if(e._max.y() > _max.y()) return false;

		return true;
	}
};

/*
class KLGeoPolySelectExtents
{
public:
	osg::ref_ptr<osg::Vec2Array> m_allVertexArray;
	osg::ref_ptr<osg::Vec2Array> m_boundVertexArray;

	inline KLGeoPolySelectExtents()
	{
		m_allVertexArray = new osg::Vec2Array;
		m_boundVertexArray = new osg::Vec2Array;
	}

	inline KLGeoPolySelectExtents(osg::ref_ptr<osg::Vec2Array> pointArray)
	{
		m_allVertexArray = pointArray;
		m_boundVertexArray = new osg::Vec2Array;
	}

	inline KLGeoPolySelectExtents(const KLGeoPolySelectExtents& rhs)
	{
		m_allVertexArray = rhs.m_allVertexArray;
		m_boundVertexArray = rhs.m_boundVertexArray;
	}

	KLGeoPolySelectExtents& operator = (const KLGeoPolySelectExtents& rhs)
	{
		m_allVertexArray->insert(m_allVertexArray->begin(),rhs.m_allVertexArray->begin(),rhs.m_allVertexArray->end());
		m_boundVertexArray->insert(m_boundVertexArray->begin(),rhs.m_boundVertexArray->begin(),rhs.m_boundVertexArray->end());
		return *this;
	}

	void move(float dx,float dy)
	{
		if(m_boundVertexArray.valid())
		{
			for(int i=0;i<m_boundVertexArray->size();i++)
			{
				(*m_boundVertexArray)[i].x() += dx;
				(*m_boundVertexArray)[i].y() += dy;
			}
		}
	}

	void init();

	float multiply(osg::Vec2 p1,osg::Vec2 p2,osg::Vec2 p0)
	{
		return (p1.x()-p0.x())*(p2.y()-p0.y())-(p2.x()-p0.x())*(p1.y()-p0.y());
	}

	float dis(osg::Vec2 p1,osg::Vec2 p2)
	{
		return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y()));
	}

	void vertexSort();

	bool intersects(const GeospatialExtents& bb)const;

    void expandBy(const osg::Vec3& v)
	{
		m_allVertexArray->push_back(osg::Vec2(v.x(),v.y()));
	}

	void expandBy(const osg::Vec2& v)
	{
		m_allVertexArray->push_back(v);
	}

	void expandBy(const GeospatialExtents& e)
	{
		if(!e.valid())
			return;

		m_allVertexArray->push_back(osg::Vec2(e.xMin(),e.yMin()));
		m_allVertexArray->push_back(osg::Vec2(e.xMax(),e.yMin()));
		m_allVertexArray->push_back(osg::Vec2(e.xMin(),e.yMax()));
		m_allVertexArray->push_back(osg::Vec2(e.xMax(),e.yMax()));
	} 

	bool isContain(const osg::Vec3& v);
	bool isContain(const osg::Vec2& v);
};*/

#endif