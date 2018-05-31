#ifndef __CONTOURGEODE_H__
#define __CONTOURGEODE_H__

#include <osg/Geode>
#include <osgText/Text>

/*!***************************************************
<br>��˵����������ֵ����ά�ڵ���
*****************************************************/
class ContourGeode: public osg::Geode
{
public:
	/*!Ĭ�Ϲ��캯��*/
	ContourGeode();

	/*!��������*/
	~ContourGeode();

	/*!������ֵ�߳�ʼ������*/
	virtual void init();

	/*!���õ�����ֵ�ߵ�ΨһID*/
	virtual void setID(const unsigned int& pID );

	/*!�õ���ֵ�ߵ�ΨһID*/
	unsigned int getID();

	/*!�����ֵ�ߵı�ע��ʾλ��*/
	void computeLabelPositon();

	/*!�õ���ֵ�ߵı�עλ��*/
	std::vector<osg::Vec2d> getLabelPosition();

	/*!���õ�ֵ�ߵı�עGeode*/
	void setTextGeode(osg::Geode* pTextGeode);

	/*!�õ���ֵ�ߵı�ע*/
	osg::ref_ptr<osg::Geode> getTextGeode();

	/*!���õ�ֵ�ߵĿ��Ƶ�*/
	void setCtrlPointArray(osg::Vec3Array* pCtrlPointArray);

	/*!�õ���ֵ�ߵĿ��Ƶ�*/
	osg::Vec3Array* getCtrlPointArray();

	/*!�õ���ֵ�߿����ߵ�����*/
	unsigned int getCtrlPointNum();

	/*!���õ�����ֱ�ߵĸ߳�*/
	void setContourHeight(const float& pHeight);

	/*!�õ�������ֵ�����ڵĸ߳�*/
	float getContourHeight();

	/*!���õ�ֵ�ߵ���ʽ*/
	void setContourStyle(const std::string& pContourStyle);

	/*!����X����ƫ����*/
	void setXOffset(const float& pxOffset);

	/*!����Y����ƫ����*/
	void setYOffset(const float& pyOffset);

private:
	/*!������ֵ�ߵ�Ψһ��ʶ*/
	unsigned int m_id;

	/*!��ֵ�ߵĿ��Ƶ�*/
	osg::Vec3Array* m_ctrlPointArray;

	/*!��ֵ������ʾ�ĸ߳�*/
	float m_height;

	/*!���Ƶ����*/
	unsigned int m_ctrlPointNum;

	/*!��ֵ�߼�����*/
	osg::ref_ptr<osg::Geometry> m_contourGeometry;

	/*!��ֵ�ߵ���ʽ*/
	std::string m_contourStyle;

	/*!��ŵ�ֵ�ߵı�ע��Geode*/
	osg::ref_ptr<osg::Geode> m_textGeode;

	/*!��ŵ�ֵ�߱�עλ�õ�����*/
	std::vector<osg::Vec2d> m_textPositionVector;

	/*!x����ƫ����*/
	float m_xOffset;

	/*!y����ƫ����*/
	float m_yOffset;


};


/*!*********************************************************
<br> ��˵������ļ����ṩ��
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