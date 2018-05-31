#ifndef __TERRAINTILEMANAGER_H__
#define __TERRAINTILEMANAGER_H__

#include <osgTerrain/TerrainTile>
#include "GeospatialExtents.h"

#define PARALLELNUM 10

class GeospatialExtents;

class TerrainTileManager
{
public:
	TerrainTileManager(osg::Node* rootNode);

	//先清除m_TileMap,然后利用节点遍历类，来遍历OSG场景树，有效的TerrainTile节点
	virtual void gatherCurrentOSGScenceTerrainTile();

	virtual void getValue(float x, float y, float& z);

	virtual void getValue(float x, float y, osg::Vec3f& value);

	virtual void getValue(std::vector<osg::Vec3f>& vecs);

	virtual void getValue(osg::Vec3Array* vecs, int size);

	//根据指定的顶点值，获得其周围四个顶点的数值
	virtual void printSroundVec(std::vector<osg::Vec3f>& vecs);

	/*!
	\brief 根据高程域，x、y坐标（实际坐标值），得到高程值
	\return float 插值结果
	\param hf 高程对象
	\param x,y二维坐标
	*/
	float getInterpolatedValue(osg::HeightField* hf, double x, double y);

	//获取瓦片ID
	std::vector<osgTerrain::TileID> getTileIDs(){return m_tileIDVect;}

	//得到瓦片集合
	std::map<osgTerrain::TileID,osgTerrain::TerrainTile*> getTileID2TerrainMap()
	{
		return m_tileMap;
	}

	void setXOffset(const float& xOffset);

	void setYOffset(const float& yOffset);

	float getXOffset() const;

	float getYOffset() const;

	//地形访问器
	friend class TerrainTileVisitor;

protected:
	
	//保存地形瓦片序列
	std::map<osgTerrain::TileID, osgTerrain::TerrainTile*> m_tileMap;

	//保存地形瓦片extents序列
	std::vector<GeospatialExtents> m_extentsVect;
	std::vector<osgTerrain::TileID> m_tileIDVect;

	//缓冲当前操作瓦片，为了提高速度，保存缓冲的TileID位置
	int m_curPos;

	//保存当前场景中的根节点
	osg::Node* m_root;

	//并行数
	int m_paralNum;

	float m_xOffset;

	float m_yOffset;


};


/************************便利当前OSG场景树中，有效的TerrainTile*************************/
class TerrainTileVisitor: public osg::NodeVisitor
{
public:
	TerrainTileVisitor(TerrainTileManager* map):osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
	{
		m_terrainTileMap = map;
	}

	//重写apply方法
	virtual void apply(osg::Node& node)
	{
		osgTerrain::TerrainTile* result = dynamic_cast<osgTerrain::TerrainTile*>(&node);
		if(result)
		{
			apply(*result);
		}

		traverse(node);
	}

	//重写apply方法，node 待处理的OSG地形瓦片节点
	virtual void apply(osgTerrain::TerrainTile& node)
	{
		//遍历当前场景的根节点，然后构造相应的数据缓存
		m_terrainTileMap->m_tileMap.insert(std::pair<osgTerrain::TileID, osgTerrain::TerrainTile*>(node.getTileID(),&node));

		osgTerrain::HeightFieldLayer* heightLayer = dynamic_cast<osgTerrain::HeightFieldLayer*>(node.getElevationLayer());
		if(heightLayer)
		{
			//构建范围
			osg::HeightField* hf = heightLayer->getHeightField();
			if(hf != NULL)
			{
				float xOffset = m_terrainTileMap->getXOffset();
				float yOffset = m_terrainTileMap->getYOffset();

				GeospatialExtents extents;
				extents.xMin() = hf->getOrigin().x();
				extents.yMin() = hf->getOrigin().y();
				extents.xMax() = extents.xMin() + (hf->getNumColumns() - 1)* hf->getXInterval();
				extents.yMax() = extents.yMin() + (hf->getNumRows() - 1)* hf->getYInterval();
				m_terrainTileMap->m_extentsVect.push_back(extents);
				m_terrainTileMap->m_tileIDVect.push_back(node.getTileID());

			}
		}
	}

private:
	//地形瓦片序列
	TerrainTileManager* m_terrainTileMap;
};

#endif