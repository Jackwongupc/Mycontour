#ifndef __CONTOURHANDLER_H__
#define __CONTOURHANDLER_H__

#include "BaseHandler.h"
#include "ContourNode.h"
#include "ContourGeode.h"

/*!***************************************************
<br>类说明：等值线交互类
*****************************************************/
class ContourHandler: public BaseHandler
{
public:
	/*!默认构造函数*/
	ContourHandler(ContourNode* pContourNode);

	/*!析构函数*/
	~ContourHandler();

	/*!响应键盘按下事件*/
	virtual void keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!响应键盘弹起事件*/
	virtual void keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!响应鼠标滚轮事件*/
	virtual void mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:

	/*!handler对应的等值线*/
	ContourNode* m_contourNode;

	/*!记录z键按键次数，用来对线的隐藏和显示进行设置*/
	unsigned int m_pushZ;

	/*!记录t键按键次数，用来对线的样式进行设置*/
	unsigned int m_pushT;

	/*!记录w键按键次数，用来对线宽进行设置*/
	unsigned int m_pushW;

	/*!记录c键按键次数，用来对线的颜色进行设置*/
	unsigned int m_pushC;

	/*!按键l,用来计算生成或者加载文件生成标注节点;此成员用来记录标注是否已经生成*/
	bool isLabelGenerated;

	/*!记录b按键次数，用来切换标注是否显示*/
	unsigned int m_pushB;

	/*!记录v键按键次数，用来对等值线标注的颜色进行设置*/
	unsigned int m_pushV;

	/*!是否坐抽稀显示*/
	bool m_isContourRarefying;

	/*!记录Alt键按键次数，用来对等值线进行是否抽稀设置*/
	unsigned int m_pushAlt;
};

#endif