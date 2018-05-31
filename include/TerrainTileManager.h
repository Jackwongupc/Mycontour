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

	//�����m_TileMap,Ȼ�����ýڵ�����࣬������OSG����������Ч��TerrainTile�ڵ�
	virtual void gatherCurrentOSGScenceTerrainTile();

	virtual void getValue(float x, float y, float& z);

	virtual void getValue(float x, float y, osg::Vec3f& value);

	virtual void getValue(std::vector<osg::Vec3f>& vecs);

	virtual void getValue(osg::Vec3Array* vecs, int size);

	//����ָ���Ķ���ֵ���������Χ�ĸ��������ֵ
	virtual void printSroundVec(std::vector<osg::Vec3f>& vecs);

	/*!
	\brief ���ݸ߳���x��y���꣨ʵ������ֵ�����õ��߳�ֵ
	\return float ��ֵ���
	\param hf �̶߳���
	\param x,y��ά����
	*/
	float getInterpolatedValue(osg::HeightField* hf, double x, double y);

	//��ȡ��ƬID
	std::vector<osgTerrain::TileID> getTileIDs(){return m_tileIDVect;}

	//�õ���Ƭ����
	std::map<osgTerrain::TileID,osgTerrain::TerrainTile*> getTileID2TerrainMap()
	{
		return m_tileMap;
	}

	void setXOffset(const float& xOffset);

	void setYOffset(const float& yOffset);

	float getXOffset() const;

	float getYOffset() const;

	//���η�����
	friend class TerrainTileVisitor;

protected:
	
	//���������Ƭ����
	std::map<osgTerrain::TileID, osgTerrain::TerrainTile*> m_tileMap;

	//���������Ƭextents����
	std::vector<GeospatialExtents> m_extentsVect;
	std::vector<osgTerrain::TileID> m_tileIDVect;

	//���嵱ǰ������Ƭ��Ϊ������ٶȣ����滺���TileIDλ��
	int m_curPos;

	//���浱ǰ�����еĸ��ڵ�
	osg::Node* m_root;

	//������
	int m_paralNum;

	float m_xOffset;

	float m_yOffset;


};


/************************������ǰOSG�������У���Ч��TerrainTile*************************/
class TerrainTileVisitor: public osg::NodeVisitor
{
public:
	TerrainTileVisitor(TerrainTileManager* map):osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
	{
		m_terrainTileMap = map;
	}

	//��дapply����
	virtual void apply(osg::Node& node)
	{
		osgTerrain::TerrainTile* result = dynamic_cast<osgTerrain::TerrainTile*>(&node);
		if(result)
		{
			apply(*result);
		}

		traverse(node);
	}

	//��дapply������node �������OSG������Ƭ�ڵ�
	virtual void apply(osgTerrain::TerrainTile& node)
	{
		//������ǰ�����ĸ��ڵ㣬Ȼ������Ӧ�����ݻ���
		m_terrainTileMap->m_tileMap.insert(std::pair<osgTerrain::TileID, osgTerrain::TerrainTile*>(node.getTileID(),&node));

		osgTerrain::HeightFieldLayer* heightLayer = dynamic_cast<osgTerrain::HeightFieldLayer*>(node.getElevationLayer());
		if(heightLayer)
		{
			//������Χ
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
	//������Ƭ����
	TerrainTileManager* m_terrainTileMap;
};

#endif