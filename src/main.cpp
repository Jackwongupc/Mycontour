
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
	std::cout<<"���ɵ�ֵ�߽ڵ�����ʱ�䣺"<<(endTime1 - startTime1)/1000 <<"��!"<<std::endl;

	osg::ComputeBoundsVisitor cbVisitorTerrain;
	pTerrain->accept(cbVisitorTerrain);
	osg::BoundingBox terrainBoundingBoox = cbVisitorTerrain.getBoundingBox();

	long startTime3 = clock();
	//pContourNode->setContourLabelGroup(pContourLabelGroup);
	pContourNode->setSourcePath("../MyContour/");
	pContourNode->init();
	//pContourNode->initLabel();
	long endTime3 = clock();
	std::cout<<"��ֵ����Ⱦ����ʱ�䣺"<<endTime3 - startTime3 <<"����!"<<std::endl;

	return pContourNode;
}

int main()
{
	//����Viewer���󣬳��������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

	//����������ڵ�
	osg::ref_ptr<osg::Group> root = new osg::Group();

	//��ȡ���νڵ�
	//osg::Node* node = osgDB::readNodeFile("D:\\KLDisplay3D\\KLDisplay3D\\data1\\puget.ive");
	osg::Node* node = osgDB::readNodeFile("../MyContour/terrains/dabei/��_xyz54.ive");
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
	std::cout<<"���е�ֵ�߿��Ƶ�������ǣ�"<< sumOfContourPoint <<std::endl;

	//��ӵ�����
	root->addChild(terrain);

	osg::ref_ptr<osg::MatrixTransform> pMatrixTransform = new osg::MatrixTransform();
	pMatrixTransform->setMatrix(osg::Matrix::translate(pContourNode->getXOffset(), pContourNode->getYOffset(), 0.0f));
	pMatrixTransform->addChild(pContourNode);
	root->addChild(pMatrixTransform);

	osg::ref_ptr<ContourHandler> pContourHandler = new ContourHandler(pContourNode);
	viewer->getCamera()->setEventCallback(pContourHandler);
	//pContourNode->setNodeMask(false);

	//�Ż���������
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;

	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->setUpViewInWindow(100, 100, 800, 600);
	//���ó�������
	viewer->setSceneData(root.get());

	//��ʼ������������
	viewer->realize();

	//��ʼ��Ⱦ
	viewer->run();

	/*osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;
	camera->getViewMatrixAsLookAt(eye,center,up);*/
	system("pause");

	return 0 ;
}