
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/ComputeBoundsVisitor>
#include <osg/MatrixTransform>

#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>

#include "ContourNode.h"
#include "ContourHandler.h"

osg::ref_ptr<ContourNode> creatContourNode(osgTerrain::Terrain* pTerrain)
{
	osg::ref_ptr<ContourNode> pContourNode = new ContourNode();
	pContourNode->setTerrainNode(pTerrain);

	long startTime1 = clock();
	//pContourNode->generateContourNodeFromSHP("D:/MyProgram/MyTerrain/MyTerrain/contours/dabei_20m.shp");
	pContourNode->generateContourNodeFromShpByGDAL("../MyContour/contours/dabei_20m.shp");
	long endTime1 = clock();
	std::cout<<"生成等值线节点所用时间："<<(endTime1 - startTime1)/1000 <<"秒!"<<std::endl;

	osg::ComputeBoundsVisitor cbVisitorTerrain;
	pTerrain->accept(cbVisitorTerrain);
	osg::BoundingBox terrainBoundingBoox = cbVisitorTerrain.getBoundingBox();

	long startTime3 = clock();
	//pContourNode->setContourLabelGroup(pContourLabelGroup);
	pContourNode->setSourcePath("../MyContour/");
	pContourNode->init();
	//pContourNode->initLabel();
	long endTime3 = clock();
	std::cout<<"等值线渲染所用时间："<<endTime3 - startTime3 <<"毫秒!"<<std::endl;

	return pContourNode;
}

int main()
{
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

	//创建场景组节点
	osg::ref_ptr<osg::Group> root = new osg::Group();

	//读取地形节点
	//osg::Node* node = osgDB::readNodeFile("D:\\KLDisplay3D\\KLDisplay3D\\data1\\puget.ive");
	osg::Node* node = osgDB::readNodeFile("../MyContour/terrains/dabei/大北_xyz54.ive");
	osgTerrain::Terrain* terrain =dynamic_cast<osgTerrain::Terrain*>(node);
	/*
	TerrainSub* terrainSub = static_cast<TerrainSub*>(node);
		terrainSub->initNodeVisitor();
		if(terrainSub)
		{
			TerrainTileMap terraintileMap = terrainSub->getTerrainTileMap();
		}*/	

/*
	terrain->setVerticalScale(10.0);
	TerrainSub* terrainSub = new TerrainSub();
	terrainSub->setterrain(terrain);*/

	osg::ref_ptr<ContourNode> pContourNode = creatContourNode(terrain);
	pContourNode->setContourViewer(viewer);

	long sumOfContourPoint = pContourNode->getAllContourPointNum();
	std::cout<<"所有等值线控制点的数量是："<< sumOfContourPoint <<std::endl;

	//添加到场景
	root->addChild(terrain);

	osg::ref_ptr<osg::MatrixTransform> pMatrixTransform = new osg::MatrixTransform();
	pMatrixTransform->setMatrix(osg::Matrix::translate(pContourNode->getXOffset(), pContourNode->getYOffset(), 0.0f));
	pMatrixTransform->addChild(pContourNode);
	root->addChild(pMatrixTransform);

	osg::ref_ptr<ContourHandler> pContourHandler = new ContourHandler(pContourNode);
	viewer->getCamera()->setEventCallback(pContourHandler);
	//pContourNode->setNodeMask(false);

	//优化场景数据
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;

	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->setUpViewInWindow(100, 100, 800, 600);
	//设置场景数据
	viewer->setSceneData(root.get());

	//初始化并创建窗口
	viewer->realize();

	//开始渲染
	viewer->run();

	/*osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;
	camera->getViewMatrixAsLookAt(eye,center,up);*/
	system("pause");

	return 0 ;
}