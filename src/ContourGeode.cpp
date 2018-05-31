#include <osg/LineStipple>

#include "ContourGeode.h"

#include <sstream>

ContourGeode::ContourGeode():
	m_id(0),
	m_ctrlPointArray(NULL),
	m_height(0.0f),
	m_ctrlPointNum(0),
	m_contourGeometry(new osg::Geometry()),
	m_contourStyle(""),
	m_textGeode(NULL),
	m_xOffset(0.0f),
	m_yOffset(0.0f)
	{
		//addDrawable(m_contourGeometry);
	}


ContourGeode::~ContourGeode()
{

}

void ContourGeode::init()
{
	//for(unsigned int i = 0; i < m_ctrlPointArray->size(); i++)
	//设置顶点数据
	m_contourGeometry->setVertexArray(m_ctrlPointArray);

	m_ctrlPointNum = m_ctrlPointArray->size();

	//创建颜色数组
/*
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	color->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	//设置颜色数组
	m_contourGeometry->setColorArray(color.get());
	//设置颜色的绑定方式为单个顶点
	m_contourGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//创建法线数组
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
	//添加法线
	nc->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	//设置法线数组
	m_contourGeometry->setNormalArray(nc.get());
	//设置法线的绑定方式为全部顶点
	m_contourGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);*/


	//添加图元，绘图基元为线形
	m_contourGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, m_ctrlPointArray->size()));

	//添加到叶节点
	addDrawable(m_contourGeometry);

	/*osg::ref_ptr<osg::LineStipple> pLineStipple = new osg::LineStipple(1, 0x55FF);
	m_contourGeometry->getOrCreateStateSet()->setAttribute(pLineStipple, osg::StateAttribute::ON);*/

}


void ContourGeode::setID(const unsigned int& pID)
{
	m_id = pID;
}

unsigned int ContourGeode::getID()
{
	return m_id;
}


void ContourGeode::computeLabelPositon()
{
	if(m_ctrlPointNum < 500)
	{
		return;
	}

	std::vector<osg::Vec2d> pvector;

	for(unsigned int i = 0; i < m_ctrlPointNum; i++)
	{
		pvector.push_back(osg::Vec2d((*m_ctrlPointArray)[i].x(),(*m_ctrlPointArray)[i].y()));
	}
	//设置一个误差参数dif来生成等值线的近似多边形,dif越大，近似多边形越粗糙
	double dif = (double)m_ctrlPointNum/50.0;
	unsigned int pointNUM1 = 0;
	unsigned int pointNUM2 = 1;

	//直到不再有点减少，说明近似多边形计算完毕
	while(pointNUM1 != pointNUM2)
	{
		pointNUM1 = pvector.size();
		std::vector<osg::Vec2d>::iterator Iter = pvector.begin() + 1;
		for(; (Iter != pvector.end()-2) && (Iter != pvector.end() -1) && (Iter != pvector.end());)
		{
			double x = Iter->x();
			double y = Iter->y();
			double x1 = (Iter-1)->x();
			double y1 = (Iter-1)->y();
			double x2 = (Iter+1)->x();
			double y2 = (Iter+1)->y();
			double distance = PointsCompute::PointToSegDist(x, y, x1, y1, x2, y2);
			if(distance < dif)
			{
				Iter = pvector.erase(Iter);
				Iter = Iter + 1;
			}
			else
			{
				Iter = Iter + 2;
			}
		}
		pointNUM2 = pvector.size();
	}

	//对计算完的点，以第一个点为标，当如果距离太近的话，则将后一个点剔除掉，这样就避免两个点靠的太近
	std::vector<osg::Vec2d>::iterator Iter = pvector.begin() + 1;
	for(;(Iter != pvector.end());)
	{
		double x = Iter->x();
		double y = Iter->y();
		double x1 = (Iter-1)->x();
		double y1 = (Iter-1)->y();
		double distance = PointsCompute::TwoPointDist(x, y, x1, y1);
		if(distance < 2000.0)
		{
			Iter = pvector.erase(Iter);
		}
		else
		{
			Iter = Iter + 1;
		}
	}

	//近似多边形最后形成的点在陡峭处很密集（并不希望标注在这些地方集中显示），这里采用一种逆近似多变形的方法将陡峭处的点抽稀
/*
	double dif2 = (double)m_ctrlPointNum/500000.0;
	unsigned int pointNUM3 = 0;
	unsigned int pointNUM4 = 1;
	//直到不再有点减少，说明逆近似多边形计算完毕
	while(pointNUM3 != pointNUM4)
	{
		pointNUM3 = pvector.size();
		std::vector<osg::Vec2d>::iterator Iter = pvector.begin() + 1;
		for(; (Iter != pvector.end()-2) && (Iter != pvector.end() -1) && (Iter != pvector.end());)
		{
			double x = Iter->x();
			double y = Iter->y();
			double x1 = (Iter-1)->x();
			double y1 = (Iter-1)->y();
			double x2 = (Iter+1)->x();
			double y2 = (Iter+1)->y();
			double distance = PointsCompute::PointToSegDist(x, y, x1, y1, x2, y2);
			if(distance > dif2)
			{
				Iter = pvector.erase(Iter);
				Iter = Iter + 1;
			}
			else
			{
				Iter = Iter + 2;
			}
		}
		pointNUM4 = pvector.size();
	}*/

	if(pvector.size()>3)
	{
		pvector.pop_back();
		pvector.pop_back();
	}

	m_textPositionVector = pvector;

	m_textGeode = new osg::Geode();
	std::stringstream height;
	height<< m_height;
	std::vector<osg::Vec2d>::iterator Iter2 = pvector.begin();
	for(; Iter2 != pvector.end(); Iter2++)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text();
		//osg::ref_ptr<osgText::Font>  font= new osgText::Font();
		//font = osgText::readFontFile("simhei.ttf");
		//text->setFont(font.get());
		text->setCharacterSize(15);
		text->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
		text->setText(height.str());
		text->setPosition(osg::Vec3(Iter2->x(), Iter2->y(), m_height + 30));
		text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
		m_textGeode->addDrawable(text);
	}
}


std::vector<osg::Vec2d> ContourGeode::getLabelPosition()
{
	return m_textPositionVector;
}


void ContourGeode::setTextGeode(osg::Geode* pTextGeode)
{
	m_textGeode = pTextGeode;
}

osg::ref_ptr<osg::Geode> ContourGeode::getTextGeode()
{
	return m_textGeode;
}


void ContourGeode::setCtrlPointArray(osg::Vec3Array* pCtrlPointArray)
{
	m_ctrlPointArray = pCtrlPointArray;
}

osg::Vec3Array* ContourGeode::getCtrlPointArray()
{
	return m_ctrlPointArray;
}

unsigned int ContourGeode::getCtrlPointNum()
{
	return m_ctrlPointNum;
}


void ContourGeode::setContourHeight(const float& pHeight)
{
	m_height = pHeight;
}


float ContourGeode::getContourHeight()
{
	return m_height;
}


void ContourGeode::setContourStyle(const std::string& pContourStyle)
{
	m_contourStyle = pContourStyle;
	if(m_contourStyle == "LINE_STRIP")
	{
		m_contourGeometry->removePrimitiveSet(0);
		m_contourGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0,m_ctrlPointArray->size()));
		//m_contourGeometry->
	}
	else if(m_contourStyle == "LINES")
	{
		m_contourGeometry->removePrimitiveSet(0);
		m_contourGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,m_ctrlPointArray->size()));
	}
	/*else if(m_contourStyle == "LINES_ADJACENCY")
	{
		m_contourGeometry->removePrimitiveSet(0);
		m_contourGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES_ADJACENCY, 0,m_ctrlPointArray->size()));
	}
	else if(m_contourStyle == "LINE_STRIP_ADJACENCY")
	{
		m_contourGeometry->removePrimitiveSet(0);
		m_contourGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP_ADJACENCY, 0,m_ctrlPointArray->size()));
	}*/
	else
		return;

}


void ContourGeode::setXOffset(const float& pxOffset)
{
	m_xOffset = pxOffset;
}

void ContourGeode::setYOffset(const float& pyOffset)
{
	m_yOffset = pyOffset;
}