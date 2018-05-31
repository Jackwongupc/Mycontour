#ifndef __CONTOURNODE_H__
#define __CONTOURNODE_H__

#include <osg/Geode>
#include <osg/Group>

#include <osgViewer/Viewer>

#include "TextureManager.h"
#include "ContourGeode.h"

/*!***************************************************
<br>类说明：等值线三维节点类
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
	/*！默认构造函数*/
	ContourNode();

	/*!初始化函数*/
	virtual void init();

	/*!初始化标注的状态集*/
	void initLabel();

	/*!设置ID*/
	virtual void setID(const std::string& id);

	/*!得到ID*/
	virtual std::string getID() const;

	/*!设置等值线标注显示叶节点*/
	virtual void setContourLabelGroup(osg::Group* pContourLabelGroup);

	/*!得到等值线标注显示叶节点*/
	virtual osg::ref_ptr<osg::Group> getContourLabelGroup();

	/*!设置线宽*/
	virtual void setContourWidth(const float& pwidth);

	/*!得到线宽*/
	virtual float getContourWidth() const;

	/*!设置等值线颜色*/
	virtual void setContourColor(const osg::Vec4& color);

	/*!得到等值线颜色*/
	virtual osg::Vec4 getContourColor() const;

	/*!设置标注的颜色*/
	virtual void setLabelColor(const osg::Vec4& pColor);

	/*!设置等值线的样式*/
	void setContourStyle(const std::string& pContourStyle);

	/*!得到等值线的样式*/
	std::string getContourStyle() const;

	/*!计算等值线所有的控制点数量*/
	void computeAllContourPointNum();

	/*!得到等值线所有控制点的数量*/
	unsigned int getAllContourPointNum();

	/*!计算等值线节点包围盒*/
	virtual osg::BoundingBox computeContourBoundingBox();

	/*!设置地形节点*/
	virtual void setTerrainNode(osg::Node* pTerrainNode);

	/*!得到地形节点*/
	virtual osg::Node* getTerrainNode();

	/*!设置资源路径*/
	virtual void setSourcePath(const std::string& sourcePath);

	/*!设置等值线的视景器*/
	void setContourViewer(osgViewer::Viewer* pViewer);

	/*!计算等值线信息*/
	void computeContourInfo();

	/*!从shp文件计算等值线信息*/
	void computeContourInfoFromSHP();

	/*!从con文件计算等值线信息*/
	void computeContourInfoFromCON();

	/*!获取等值线信息*/
	ContourInfo getContourInfo();

	/*!得到X方向偏移量*/
	float getXOffset();

	/*!得到Y方向偏移量*/
	float getYOffset();
	
	/*!设置地形拉伸系数*/
	void setTerrainScale(osg::MatrixTransform* mt);

	/*!从SHP文件生成等值线节点*/
	void generateContourNodeFromSHP(const std::string& fileName);

	/*!利用GDAL生成等值线节点*/
	void generateContourNodeFromShpByGDAL(const std::string& fileName);

	/*!从con文件生成等直线节点*/
	void generateContourNodeFromCon(const std::string& fileName);

	/*!得到ContourGeodeMap*/
	ContourGeodeMap getContourGeodeMap();

	/*!等值线的抽稀实现*/
	void contourRarefying();

	/*!不对等值线抽稀*/
	void notContourRarefying();

	/*!计算生成或者读取生成等值线标注*/
	void computeOrReadFileGenerateLabel();

	/*!生成等值线标注节点*/
	osg::ref_ptr<osg::Group> generateContourLabelNode(std::vector<osg::ref_ptr<osg::Vec3Array>> pcurrentCoords);

	/*!判断此位置是否被占据*/
	bool isOccupied(osg::Vec3 position, std::string::size_type size);

	/*!获取等值线的顶点数组的矢量数组*/
	std::vector<osg::ref_ptr<osg::Vec3Array>> getCurrentCoords();
protected:
	/*！默认析构函数*/
	virtual ~ContourNode();

private:
	/*!等值线节点的唯一标识*/
	std::string m_id;

	/*!等值线所在的视景器*/
	osgViewer::Viewer* m_viewer;

	/*!状态集*/
	osg::ref_ptr<osg::StateSet> m_stateSet;

	/*!三维等值线Map*/
	ContourGeodeMap m_contourGeodeMap;

	/*!三维标注显示节点*/
	osg::ref_ptr<osg::Group> m_contourLabelGroup;

	/*!等值线线宽*/
	float m_width;

	/*!等值线颜色*/
	osg::Vec4 m_color;

	/*!等值线的样式*/
	std::string m_contourStyle;

	/*!所有等值线的总的控制线的数量*/
	unsigned int m_allContourPointNum;

	/*!获取地形节点包围盒*/
	osg::BoundingBox m_box;

	/*!地形节点*/
	osg::Node* m_terrainNode;

	/*!地形高程纹理管理器*/
	osg::ref_ptr<TextureManager> m_demTextureManager;

	/*!高程纹理索引*/
	unsigned int m_demTextureIndex;

	/*!资源路径*/
	std::string m_sourcePath;

	/*!等值线的基本信息*/
	ContourInfo m_info;

	/*!x坐标偏移量*/
	float m_xOffset;

	/*!y坐标偏移量*/
	float m_yOffset;

	/*!控制x坐标偏移基数*/
	float m_xBaseNum;

	/*!控制y坐标偏移基数*/
	float m_yBaseNum;

	/*!地形Z值拉伸系数*/
	float m_terrainScaleZ;

	/*！标注的大小*/
	float m_labelSize;

	/*!等值线的顶点数组的矢量数组*/
	std::vector<osg::ref_ptr<osg::Vec3Array>> m_currentCoords;

	/*!判断标注位置的矩形类*/
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

	/*!判断标注位置的矩形类的矢量数组指针*/
	std::vector<OccupyingRectangle>* m_occupied;

	/*!等值线的高程值矢量数组，一条线对应一个值*/
	std::vector<float> m_values;

};

#endif