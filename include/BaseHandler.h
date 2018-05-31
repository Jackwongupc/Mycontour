#ifndef __BASEHANDLE_H__
#define __BASEHANDLE_H__

#include <osgViewer/Viewer>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osg/Geometry>
#include <osg/Geode>

/*!***************************************
<br>类说明： 三维场景交互基类
*****************************************/

class BaseHandler : public osgGA::GUIEventHandler
{
public:

	/*!默认构造函数*/
	BaseHandler();

	/*!重写父类的handle方法*/
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!响应键盘按下事件*/
	virtual void keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!响应键盘弹起事件*/
	virtual void keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!响应鼠标滚轮事件*/
	virtual void mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	virtual ~BaseHandler();
protected:
	/*!当前场景对象*/
	osgViewer::Viewer* m_viewer;

	/*!标志：键盘是否案件*/
	bool m_isKeyDown;
};



#endif