#ifndef __BASEHANDLE_H__
#define __BASEHANDLE_H__

#include <osgViewer/Viewer>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osg/Geometry>
#include <osg/Geode>

/*!***************************************
<br>��˵���� ��ά������������
*****************************************/

class BaseHandler : public osgGA::GUIEventHandler
{
public:

	/*!Ĭ�Ϲ��캯��*/
	BaseHandler();

	/*!��д�����handle����*/
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!��Ӧ���̰����¼�*/
	virtual void keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!��Ӧ���̵����¼�*/
	virtual void keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!��Ӧ�������¼�*/
	virtual void mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	virtual ~BaseHandler();
protected:
	/*!��ǰ��������*/
	osgViewer::Viewer* m_viewer;

	/*!��־�������Ƿ񰸼�*/
	bool m_isKeyDown;
};



#endif