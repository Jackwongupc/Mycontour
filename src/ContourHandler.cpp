#include <osg/Camera>

#include "ContourHandler.h"


ContourHandler::ContourHandler(ContourNode* pContourNode):
	m_contourNode(pContourNode),
	m_pushZ(0),
	m_pushT(0),
	m_pushW(0),
	m_pushC(0),
	isLabelGenerated(false),
	m_pushB(0),
	m_pushV(0),
	m_isContourRarefying(true)
	{

	}

ContourHandler::~ContourHandler()
{

}


void ContourHandler::keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if(ea.getKey() == 'z' || ea.getKey() == 'Z')
	{
		m_pushZ++;
		if(m_pushZ%2 == 0)
			m_contourNode->setNodeMask(true);
		else if(m_pushZ%2 == 1)
			m_contourNode->setNodeMask(false);
		else
			return;
	}

	if(ea.getKey() == 't'|| ea.getKey()=='T')
	{
		m_pushT++;
		if(m_pushT%2 == 0)
			m_contourNode->setContourStyle("LINE_STRIP");
		else if(m_pushT%2 == 1)
			m_contourNode->setContourStyle("LINES");
		else
			return;
	}

	if(ea.getKey() == 'w'|| ea.getKey()=='W')
	{
		m_pushW++;
		if(m_pushW%6 == 0)
			m_contourNode->setContourWidth(1.0f);
		else if(m_pushW%6 == 1)
			m_contourNode->setContourWidth(1.5f);
		else if(m_pushW%6 == 2)
			m_contourNode->setContourWidth(2.0f);
		else if(m_pushW%6 == 3)
			m_contourNode->setContourWidth(2.5f);
		else if(m_pushW%6 == 4)
			m_contourNode->setContourWidth(3.0f);
		else if(m_pushW%6 == 5)
			m_contourNode->setContourWidth(3.5f);
		else
			return;
	}

	if(ea.getKey() == 'c' || ea.getKey() == 'C')
	{
		m_pushC++;
		if(m_pushC%7 == 0)
			m_contourNode->setContourColor(osg::Vec4(1.0f, 0.0f, 0.0f,1.0f));
		else if(m_pushC%7 == 1)
			m_contourNode->setContourColor(osg::Vec4(0.0f, 1.0f, 0.0f,1.0f));
		else if(m_pushC%7 == 2)
			m_contourNode->setContourColor(osg::Vec4(0.0f, 0.0f, 1.0f,1.0f));
		else if(m_pushC%7 == 3)
			m_contourNode->setContourColor(osg::Vec4(1.0f, 1.0f, 0.0f,1.0f));
		else if(m_pushC%7 == 4)
			m_contourNode->setContourColor(osg::Vec4(1.0f, 0.0f, 1.0f,1.0f));
		else if(m_pushC%7 == 5)
			m_contourNode->setContourColor(osg::Vec4(0.0f, 1.0f, 1.0f,1.0f));
		else if(m_pushC%7 == 6)
			m_contourNode->setContourColor(osg::Vec4(0.0f, 0.0f, 0.0f,1.0f));
		else
			return;
	}

	if(ea.getKey() == 'b' || ea.getKey() == 'B')
	{
		m_pushB++;
		osg::Group* pContourLabelGroup = m_contourNode->getContourLabelGroup();
		if(m_pushB%2 == 0)
		{
			pContourLabelGroup->setNodeMask(true);
		}
		else if(m_pushB%2 == 1)
		{
			pContourLabelGroup->setNodeMask(false);
		}
		else
			return;

	}

	if(ea.getKey() == 'l' || ea.getKey() == 'L')
	{
		if(isLabelGenerated == false)
		{
			m_contourNode->computeOrReadFileGenerateLabel();
			isLabelGenerated = true;
		}
		else
			return;
	}

	if(ea.getKey() == 'v' || ea.getKey() == 'V')
	{
		m_pushV++;
		if(m_pushV%7 == 0)
			m_contourNode->setLabelColor(osg::Vec4(0.0f, 0.0f, 0.0f,1.0f));
		else if(m_pushV%7 == 1)
			m_contourNode->setLabelColor(osg::Vec4(1.0f, 0.0f, 0.0f,1.0f));
		else if(m_pushV%7 == 2)
			m_contourNode->setLabelColor(osg::Vec4(0.0f, 1.0f, 0.0f,1.0f));
		else if(m_pushV%7 == 3)
			m_contourNode->setLabelColor(osg::Vec4(0.0f, 0.0f, 1.0f,1.0f));
		else if(m_pushV%7 == 4)
			m_contourNode->setLabelColor(osg::Vec4(1.0f, 1.0f, 0.0f,1.0f));
		else if(m_pushV%7 == 5)
			m_contourNode->setLabelColor(osg::Vec4(1.0f, 0.0f, 1.0f,1.0f));
		else if(m_pushV%7 == 6)
			m_contourNode->setLabelColor(osg::Vec4(0.0f, 1.0f, 1.0f,1.0f));
		else
			return;
	}

	if(ea.getKey() == 0xFFE9 || ea.getKey() == 0xFFEA )
	{
		m_pushAlt++;
		if(m_pushAlt%2 == 0)
		{
			m_contourNode->contourRarefying();
			m_isContourRarefying =true;
		}
		else if(m_pushAlt%2 == 1)
		{
			m_contourNode->notContourRarefying();
			m_isContourRarefying = false;
		}

		else
			return;
	}

}


void ContourHandler::keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

}


void ContourHandler::mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if(m_isContourRarefying == true)
		m_contourNode->contourRarefying();
}