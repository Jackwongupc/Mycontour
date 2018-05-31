#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <osg/Camera>
#include <osg/Texture2D>
#include <osgTerrain/TerrainTile>

#define TEXTURESIZE 2048

class TextureManager: public osg::Camera
{
public:
	TextureManager(osg::Node*pTerrainNode);

	virtual void init();


	void setID(osg::Node* id);

	osg::Node* getID();

	osg::Node* getTerrainNode();


	osg::BoundingBox getTerrainBoundingBox() const;

	//得到或者创建高程纹理
	osg::ref_ptr<osg::Texture2D> getOrCreateDemTexture();

	void setSourcePath(const std::string& sourcePath);

protected:
	virtual ~TextureManager();

private:
	osg::Node* m_id;

	osg::StateSet* m_stateSet;

	osg::ref_ptr<osg::Texture2D> m_demTexture;

	osg::Node* m_terrainNode;

	/*!地形包围盒大小*/
	osg::BoundingBox m_terrainBounding;

	std::string m_sourcePath;





};

#define  RTTCOUNT 10

class RttCallBack: public osg::NodeCallback
{
public:
	RttCallBack(osg::Node* pRttCamera);

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

protected:
	virtual ~RttCallBack();

private:
	unsigned int m_count;

	osg::Node* m_rttCamera;
};

#endif