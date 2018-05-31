#ifndef __CONTOURNODE_H__
#define __CONTOURNODE_H__

#include <osg/Geode>
#include <osg/Group>

#include <osgViewer/Viewer>

#include "TextureManager.h"
#include "ContourGeode.h"

/*!***************************************************
<br>��˵������ֵ����ά�ڵ���
*****************************************************/
typedef struct 
{
	double m_xMin;
	double m_yMin;
	double m_xMax;
	double m_yMax;
	float m_minElevation;
	float m_maxElevation;
	float m_step;
} ContourInfo;

typedef std::map<unsigned int,osg::ref_ptr<ContourGeode>> ContourGeodeMap;

class ContourNode: public osg::Group
{
public:
	/*��Ĭ�Ϲ��캯��*/
	ContourNode();

	/*!��ʼ������*/
	virtual void init();

	/*!��ʼ����ע��״̬��*/
	void initLabel();

	/*!����ID*/
	virtual void setID(const std::string& id);

	/*!�õ�ID*/
	virtual std::string getID() const;

	/*!���õ�ֵ�߱�ע��ʾҶ�ڵ�*/
	virtual void setContourLabelGroup(osg::Group* pContourLabelGroup);

	/*!�õ���ֵ�߱�ע��ʾҶ�ڵ�*/
	virtual osg::ref_ptr<osg::Group> getContourLabelGroup();

	/*!�����߿�*/
	virtual void setContourWidth(const float& pwidth);

	/*!�õ��߿�*/
	virtual float getContourWidth() const;

	/*!���õ�ֵ����ɫ*/
	virtual void setContourColor(const osg::Vec4& color);

	/*!�õ���ֵ����ɫ*/
	virtual osg::Vec4 getContourColor() const;

	/*!���ñ�ע����ɫ*/
	virtual void setLabelColor(const osg::Vec4& pColor);

	/*!���õ�ֵ�ߵ���ʽ*/
	void setContourStyle(const std::string& pContourStyle);

	/*!�õ���ֵ�ߵ���ʽ*/
	std::string getContourStyle() const;

	/*!�����ֵ�����еĿ��Ƶ�����*/
	void computeAllContourPointNum();

	/*!�õ���ֵ�����п��Ƶ������*/
	unsigned int getAllContourPointNum();

	/*!�����ֵ�߽ڵ��Χ��*/
	virtual osg::BoundingBox computeContourBoundingBox();

	/*!���õ��νڵ�*/
	virtual void setTerrainNode(osg::Node* pTerrainNode);

	/*!�õ����νڵ�*/
	virtual osg::Node* getTerrainNode();

	/*!������Դ·��*/
	virtual void setSourcePath(const std::string& sourcePath);

	/*!���õ�ֵ�ߵ��Ӿ���*/
	void setContourViewer(osgViewer::Viewer* pViewer);

	/*!�����ֵ����Ϣ*/
	void computeContourInfo();

	/*!��shp�ļ������ֵ����Ϣ*/
	void computeContourInfoFromSHP();

	/*!��con�ļ������ֵ����Ϣ*/
	void computeContourInfoFromCON();

	/*!��ȡ��ֵ����Ϣ*/
	ContourInfo getContourInfo();

	/*!�õ�X����ƫ����*/
	float getXOffset();

	/*!�õ�Y����ƫ����*/
	float getYOffset();
	
	/*!���õ�������ϵ��*/
	void setTerrainScale(osg::MatrixTransform* mt);

	/*!��SHP�ļ����ɵ�ֵ�߽ڵ�*/
	void generateContourNodeFromSHP(const std::string& fileName);

	/*!����GDAL���ɵ�ֵ�߽ڵ�*/
	void generateContourNodeFromShpByGDAL(const std::string& fileName);

	/*!��con�ļ����ɵ�ֱ�߽ڵ�*/
	void generateContourNodeFromCon(const std::string& fileName);

	/*!�õ�ContourGeodeMap*/
	ContourGeodeMap getContourGeodeMap();

	/*!��ֵ�ߵĳ�ϡʵ��*/
	void contourRarefying();

	/*!���Ե�ֵ�߳�ϡ*/
	void notContourRarefying();

	/*!�������ɻ��߶�ȡ���ɵ�ֵ�߱�ע*/
	void computeOrReadFileGenerateLabel();

	/*!���ɵ�ֵ�߱�ע�ڵ�*/
	osg::ref_ptr<osg::Group> generateContourLabelNode(std::vector<osg::ref_ptr<osg::Vec3Array>> pcurrentCoords);

	/*!�жϴ�λ���Ƿ�ռ��*/
	bool isOccupied(osg::Vec3 position, std::string::size_type size);

	/*!��ȡ��ֵ�ߵĶ��������ʸ������*/
	std::vector<osg::ref_ptr<osg::Vec3Array>> getCurrentCoords();
protected:
	/*��Ĭ����������*/
	virtual ~ContourNode();

private:
	/*!��ֵ�߽ڵ��Ψһ��ʶ*/
	std::string m_id;

	/*!��ֵ�����ڵ��Ӿ���*/
	osgViewer::Viewer* m_viewer;

	/*!״̬��*/
	osg::ref_ptr<osg::StateSet> m_stateSet;

	/*!��ά��ֵ��Map*/
	ContourGeodeMap m_contourGeodeMap;

	/*!��ά��ע��ʾ�ڵ�*/
	osg::ref_ptr<osg::Group> m_contourLabelGroup;

	/*!��ֵ���߿�*/
	float m_width;

	/*!��ֵ����ɫ*/
	osg::Vec4 m_color;

	/*!��ֵ�ߵ���ʽ*/
	std::string m_contourStyle;

	/*!���е�ֵ�ߵ��ܵĿ����ߵ�����*/
	unsigned int m_allContourPointNum;

	/*!��ȡ���νڵ��Χ��*/
	osg::BoundingBox m_box;

	/*!���νڵ�*/
	osg::Node* m_terrainNode;

	/*!���θ߳����������*/
	osg::ref_ptr<TextureManager> m_demTextureManager;

	/*!�߳���������*/
	unsigned int m_demTextureIndex;

	/*!��Դ·��*/
	std::string m_sourcePath;

	/*!��ֵ�ߵĻ�����Ϣ*/
	ContourInfo m_info;

	/*!x����ƫ����*/
	float m_xOffset;

	/*!y����ƫ����*/
	float m_yOffset;

	/*!����x����ƫ�ƻ���*/
	float m_xBaseNum;

	/*!����y����ƫ�ƻ���*/
	float m_yBaseNum;

	/*!����Zֵ����ϵ��*/
	float m_terrainScaleZ;

	/*����ע�Ĵ�С*/
	float m_labelSize;

	/*!��ֵ�ߵĶ��������ʸ������*/
	std::vector<osg::ref_ptr<osg::Vec3Array>> m_currentCoords;

	/*!�жϱ�עλ�õľ�����*/
	class OccupyingRectangle{
	public:
		float x1;
		float x2;
		float y1;
		float y2;

		OccupyingRectangle(float x01, float x02, float y01, float y02):
			x1(x01), x2(x02), y1(y01), y2(y02)
			{

			}
	};

	/*!�жϱ�עλ�õľ������ʸ������ָ��*/
	std::vector<OccupyingRectangle>* m_occupied;

	/*!��ֵ�ߵĸ߳�ֵʸ�����飬һ���߶�Ӧһ��ֵ*/
	std::vector<float> m_values;

};

#endif