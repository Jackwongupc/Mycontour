
#include "BaseHandler.h"

BaseHandler::BaseHandler():
	m_viewer(NULL)
{

}


BaseHandler::~BaseHandler()
{

}


bool BaseHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	//得到场景的视景器对象
	m_viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			keydownEvent(ea, aa);
			return false;
		}

	case(osgGA::GUIEventAdapter::KEYUP):
		{
			keyupEvent(ea, aa);
			return false;
		}

	case(osgGA::GUIEventAdapter::SCROLL):
		{
			mouseScrollEvent(ea, aa);
			return false;
		}
	}

	return false;
}



void BaseHandler::keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

}


void BaseHandler::keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

}


void BaseHandler::mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

}
