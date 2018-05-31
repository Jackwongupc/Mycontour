#ifndef __CONTOURGEODE_H__
#define __CONTOURGEODE_H__

#include <osg/Geode>
#include <osgText/Text>

/*!***************************************************
<br>类说明：单条等值线三维节点类
*****************************************************/
class ContourGeode: public osg::Geode
{
public:
	/*!默认构造函数*/
	ContourGeode();

	/*!析构函数*/
	~ContourGeode();

	/*!单条等值线初始化函数*/
	virtual void init();

	/*!设置单条等值线的唯一ID*/
	virtual void setID(const unsigned int& pID );

	/*!得到等值线的唯一ID*/
	unsigned int getID();

	/*!计算等值线的标注显示位置*/
	void computeLabelPositon();

	/*!得到等值线的标注位置*/
	std::vector<osg::Vec2d> getLabelPosition();

	/*!设置等值线的标注Geode*/
	void setTextGeode(osg::Geode* pTextGeode);

	/*!得到等值线的标注*/
	osg::ref_ptr<osg::Geode> getTextGeode();

	/*!设置等值线的控制点*/
	void setCtrlPointArray(osg::Vec3Array* pCtrlPointArray);

	/*!得到等值线的控制点*/
	osg::Vec3Array* getCtrlPointArray();

	/*!得到等值线控制线的数量*/
	unsigned int getCtrlPointNum();

	/*!设置单条等直线的高程*/
	void setContourHeight(const float& pHeight);

	/*!得到此条等值线所在的高程*/
	float getContourHeight();

	/*!设置等值线的样式*/
	void setContourStyle(const std::string& pContourStyle);

	/*!设置X方向偏移量*/
	void setXOffset(const float& pxOffset);

	/*!设置Y方向偏移量*/
	void setYOffset(const float& pyOffset);

private:
	/*!单条等值线的唯一标识*/
	unsigned int m_id;

	/*!等值线的控制点*/
	osg::Vec3Array* m_ctrlPointArray;

	/*!等值线所表示的高程*/
	float m_height;

	/*!控制点个数*/
	unsigned int m_ctrlPointNum;

	/*!等值线几何体*/
	osg::ref_ptr<osg::Geometry> m_contourGeometry;

	/*!等值线的样式*/
	std::string m_contourStyle;

	/*!存放等值线的标注的Geode*/
	osg::ref_ptr<osg::Geode> m_textGeode;

	/*!存放等值线标注位置的数组*/
	std::vector<osg::Vec2d> m_textPositionVector;

	/*!x坐标偏移量*/
	float m_xOffset;

	/*!y坐标偏移量*/
	float m_yOffset;


};


/*!*********************************************************
<br> 类说明：点的计算提供类
*************************************************************/
class PointsCompute
{
public:
	static double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
	{
		double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
		if (cross <= 0) return sqrtf((x - x1) * (x - x1) + (y - y1) * (y - y1));

		double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
		if (cross >= d2) return sqrtf((x - x2) * (x - x2) + (y - y2) * (y - y2));

		double r = cross / d2;
		double px = x1 + (x2 - x1) * r;
		double py = y1 + (y2 - y1) * r;
		return sqrtf((x - px) * (x - px) + (py - y) * (py - y));
	}


	static double TwoPointDist(double x, double y, double x1, double y1)
	{
		return sqrtf((x - x1)*(x - x1) + (y - y1)*(y - y1));
	}
};

#endif