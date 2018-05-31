#ifndef __CONTOURHANDLER_H__
#define __CONTOURHANDLER_H__

#include "BaseHandler.h"
#include "ContourNode.h"
#include "ContourGeode.h"

/*!***************************************************
<br>��˵������ֵ�߽�����
*****************************************************/
class ContourHandler: public BaseHandler
{
public:
	/*!Ĭ�Ϲ��캯��*/
	ContourHandler(ContourNode* pContourNode);

	/*!��������*/
	~ContourHandler();

	/*!��Ӧ���̰����¼�*/
	virtual void keydownEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!��Ӧ���̵����¼�*/
	virtual void keyupEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	/*!��Ӧ�������¼�*/
	virtual void mouseScrollEvent(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:

	/*!handler��Ӧ�ĵ�ֵ��*/
	ContourNode* m_contourNode;

	/*!��¼z�������������������ߵ����غ���ʾ��������*/
	unsigned int m_pushZ;

	/*!��¼t�������������������ߵ���ʽ��������*/
	unsigned int m_pushT;

	/*!��¼w�������������������߿��������*/
	unsigned int m_pushW;

	/*!��¼c�������������������ߵ���ɫ��������*/
	unsigned int m_pushC;

	/*!����l,�����������ɻ��߼����ļ����ɱ�ע�ڵ�;�˳�Ա������¼��ע�Ƿ��Ѿ�����*/
	bool isLabelGenerated;

	/*!��¼b���������������л���ע�Ƿ���ʾ*/
	unsigned int m_pushB;

	/*!��¼v�����������������Ե�ֵ�߱�ע����ɫ��������*/
	unsigned int m_pushV;

	/*!�Ƿ�����ϡ��ʾ*/
	bool m_isContourRarefying;

	/*!��¼Alt�����������������Ե�ֵ�߽����Ƿ��ϡ����*/
	unsigned int m_pushAlt;
};

#endif