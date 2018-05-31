#include <osg/ComputeBoundsVisitor>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <osgText/Text>
#include <osgText/Text3D>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>

#include "ContourNode.h"
#include "TextureManager.h"
#include "TerrainTileManager.h"
#include "FileReadFunc.h"

const std::string m_contourVShaderPath = "shaders/LineRender.vert";
const std::string m_contourFShaderPath = "shaders/LineRender.frag";

const std::string m_contourLabelVShaderPath = "shaders/LabelRender.vert";
const std::string m_contourLabelFShaderpath = "shaders/LabelRender.frag";

ContourNode::ContourNode():
	m_id(""),
	m_viewer(NULL),
	m_stateSet(NULL),
	m_contourLabelGroup(new osg::Group()),
	m_width(1.0f),
	m_color(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)),
	m_contourStyle(""),
	m_allContourPointNum(0),
	m_terrainNode(NULL),
	m_demTextureManager(NULL),
	m_demTextureIndex(0),
	m_xOffset(0.0f),
	m_yOffset(0.0f),
	m_xBaseNum(100000.0f),
	m_yBaseNum(100000.0f),
	m_terrainScaleZ(1.0f),
	m_labelSize(100.0f)
	{

	}

ContourNode::~ContourNode()
{

}

void ContourNode::init()
{
	//获取状态集
	m_stateSet = getOrCreateStateSet();

	//创建program对象加载shader
	osg::ref_ptr<osg::Program> pProgram = new osg::Program();
	osg::ref_ptr<osg::Shader> pVShader = osg::Shader::readShaderFile(osg::Shader::VERTEX, m_sourcePath + m_contourVShaderPath);
	osg::ref_ptr<osg::Shader> pFShader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, m_sourcePath + m_contourFShaderPath);
	pProgram->addShader(pVShader);
	pProgram->addShader(pFShader);
	m_stateSet->setAttribute(pProgram);

	//判断是否存在地形
	bool isTerrainExist = false;
	if(m_terrainNode)
	{
		isTerrainExist = true;
		m_demTextureManager = new TextureManager(m_terrainNode);
		m_demTextureManager->setSourcePath(m_sourcePath);
		m_demTextureManager->init();
		addChild(m_demTextureManager);

		osg::ref_ptr<osg::Texture2D> pDemTexture = m_demTextureManager->getOrCreateDemTexture();
		osg::ref_ptr<osg::Uniform> pUniformTexture = new osg::Uniform("terrainTexture", m_demTextureIndex);
		m_stateSet->addUniform(pUniformTexture);
		m_stateSet->setTextureAttributeAndModes(m_demTextureIndex, pDemTexture);

		osg::Vec3 minCoord(m_demTextureManager->getTerrainBoundingBox()._min);
		osg::Vec3 maxCoord(m_demTextureManager->getTerrainBoundingBox()._max);
		minCoord.x() -= m_xOffset;
		maxCoord.x() -= m_xOffset;
		minCoord.y() -= m_yOffset;
		maxCoord.y() -= m_yOffset;

		osg::ref_ptr<osg::Uniform> pUniformMinCoord = new osg::Uniform("minCoord", minCoord);
		osg::ref_ptr<osg::Uniform> pUniformMaxCoord = new osg::Uniform("maxCoord", maxCoord);
		m_stateSet->addUniform(pUniformMinCoord);
		m_stateSet->addUniform(pUniformMaxCoord);

		osg::ref_ptr<osg::Uniform> pTerrainScaleUniform = new osg::Uniform("terrainScale", m_terrainScaleZ);
		m_stateSet->addUniform(pTerrainScaleUniform);

	}

	osg::ref_ptr<osg::Uniform> pUniformColor = new osg::Uniform("color", m_color);
	m_stateSet->addUniform(pUniformColor);

	osg::ref_ptr<osg::LineWidth> pLineWidth = new osg::LineWidth();
	pLineWidth->setWidth(m_width);
	m_stateSet->setAttribute(pLineWidth, osg::StateAttribute::ON);

	m_stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

	m_stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	m_stateSet->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	m_stateSet->setRenderingHint(GL_LINE_SMOOTH_HINT);
	//反走样的fastest效果设置
	m_stateSet->setRenderingHint(GL_FASTEST);
	//允许设置线的样式
	m_stateSet->setMode(GL_LINE_STIPPLE, osg::StateAttribute::ON);

	//线样式的设置
	osg::ref_ptr<osg::LineStipple> pLineStipple = new osg::LineStipple(1, 0xFFFF/*0x27FF*/);
	m_stateSet->setAttribute(pLineStipple, osg::StateAttribute::ON);
	
	//根据x、y的坐标基数为变换矩阵的偏移量
	/*osg::ref_ptr<osg::MatrixTransform> pMatrixTransform = new osg::MatrixTransform();
	pMatrixTransform->setMatrix(osg::Matrix::translate(m_xOffset, m_yOffset, 0.0f));
	pMatrixTransform->addChild(this);
	addChild(pMatrixTransform);*/


	//addChild(m_contourLabelGroup);

	computeContourBoundingBox();
	computeContourInfo();
	computeAllContourPointNum();
}

void ContourNode::initLabel()
{
	//获取状态集
	osg::ref_ptr<osg::StateSet> pstateSet = m_contourLabelGroup->getOrCreateStateSet();

	//创建program对象加载shader
	osg::ref_ptr<osg::Program> pProgram = new osg::Program();
	osg::ref_ptr<osg::Shader> pVShader = osg::Shader::readShaderFile(osg::Shader::VERTEX, m_sourcePath + m_contourLabelVShaderPath);
	osg::ref_ptr<osg::Shader> pFShader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, m_sourcePath + m_contourLabelFShaderpath);
	pProgram->addShader(pVShader);
	pProgram->addShader(pFShader);
	pstateSet->setAttribute(pProgram);

	//判断是否存在地形
	bool isTerrainExist = false;
	if(m_terrainNode)
	{
		isTerrainExist = true;
		osg::ref_ptr<TextureManager> pdemTextureManager = m_demTextureManager; /*new TextureManager(m_terrainNode)*/;
		//pdemTextureManager->setSourcePath(m_sourcePath);
		//pdemTextureManager->init();
		m_contourLabelGroup->addChild(pdemTextureManager);

		osg::ref_ptr<osg::Texture2D> pDemTexture = pdemTextureManager->getOrCreateDemTexture();
		osg::ref_ptr<osg::Uniform> pUniformTexture = new osg::Uniform("terrainTexture", 1);
		pstateSet->addUniform(pUniformTexture);
		pstateSet->setTextureAttributeAndModes(1, pDemTexture);

		osg::Vec3 minCoord(pdemTextureManager->getTerrainBoundingBox()._min);
		osg::Vec3 maxCoord(pdemTextureManager->getTerrainBoundingBox()._max);
		minCoord.x() -= m_xOffset;
		maxCoord.x() -= m_xOffset;
		minCoord.y() -= m_yOffset;
		maxCoord.y() -= m_yOffset;

		osg::ref_ptr<osg::Uniform> pUniformMinCoord = new osg::Uniform("minCoord", minCoord);
		osg::ref_ptr<osg::Uniform> pUniformMaxCoord = new osg::Uniform("maxCoord", maxCoord);
		pstateSet->addUniform(pUniformMinCoord);
		pstateSet->addUniform(pUniformMaxCoord);

		/*osg::ref_ptr<osg::Uniform> pTerrainScaleUniform = new osg::Uniform("terrainScale", m_terrainScaleZ);
		pstateSet->addUniform(pTerrainScaleUniform);*/
		

	}
	osg::Vec4 labelColor = osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	osg::ref_ptr<osg::Uniform> pUniformColor = new osg::Uniform("color", labelColor);
	pstateSet->addUniform(pUniformColor);

	//根据x、y的坐标基数为变换矩阵的偏移量
	/*osg::ref_ptr<osg::MatrixTransform> pMatrixTransform = new osg::MatrixTransform();
	pMatrixTransform->setMatrix(osg::Matrix::translate(m_xOffset, m_yOffset, 0.0f));
	pMatrixTransform->addChild(m_contourLabelGroup);
	addChild(pMatrixTransform);*/
	addChild(m_contourLabelGroup);

}


void ContourNode::setID(const std::string& id)
{
	m_id = id;
}

std::string ContourNode::getID() const
{
	return m_id;
}


void ContourNode::setContourLabelGroup(osg::Group* pContourLabelGroup)
{
	m_contourLabelGroup = pContourLabelGroup;
}

osg::ref_ptr<osg::Group> ContourNode::getContourLabelGroup()
{
	return m_contourLabelGroup;
}

void ContourNode::setContourWidth(const float& width)
{
	m_width = width;
	osg::ref_ptr<osg::LineWidth> pLineWidth = new osg::LineWidth();
	pLineWidth->setWidth(m_width);
	m_stateSet->setAttribute(pLineWidth, osg::StateAttribute::ON);
}

float ContourNode::getContourWidth() const
{
	return m_width;
}

void ContourNode::setContourColor(const osg::Vec4& color)
{
	m_color = color;
	osg::ref_ptr<osg::Uniform> pUniformColor = new osg::Uniform("color", m_color);
	m_stateSet->addUniform(pUniformColor);
}

osg::Vec4 ContourNode::getContourColor() const
{
	return m_color;
}


void ContourNode::setLabelColor(const osg::Vec4& pColor)
{
	osg::ref_ptr<osg::Uniform> pUniformColor = new osg::Uniform("color", pColor);
	osg::ref_ptr<osg::StateSet> pLabelStateSet = m_contourLabelGroup->getOrCreateStateSet();
	pLabelStateSet->addUniform(pUniformColor);
}


void ContourNode::setContourStyle(const std::string& pContourStyle)
{
	m_contourStyle = pContourStyle;
	std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
	for(; Iter != m_contourGeodeMap.end(); Iter++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
		pContourGeode->setContourStyle(pContourStyle);
	}
}


std::string ContourNode::getContourStyle() const
{
	return m_contourStyle;
}


void ContourNode::computeAllContourPointNum()
{
	std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
	for(; Iter != m_contourGeodeMap.end(); Iter++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
		m_allContourPointNum = m_allContourPointNum + (long)pContourGeode->getCtrlPointNum();
	}
}


unsigned int ContourNode::getAllContourPointNum()
{
	return m_allContourPointNum;
}


osg::BoundingBox ContourNode::computeContourBoundingBox()
{
	osg::ComputeBoundsVisitor cbVisitor;
	accept(cbVisitor);
	m_box = cbVisitor.getBoundingBox();

	return m_box;
}

void ContourNode::setTerrainNode(osg::Node* pTerrainNode)
{
	m_terrainNode = pTerrainNode;
}

osg::Node* ContourNode::getTerrainNode()
{
	return m_terrainNode;
}

void ContourNode::setSourcePath(const std::string& sourcePath)
{
	m_sourcePath = sourcePath;
}


void ContourNode::setContourViewer(osgViewer::Viewer* pViewer)
{
	m_viewer = pViewer;
}


void ContourNode::computeContourInfo()
{
	size_t dotPosition = m_id.find_last_of('.');
	if(dotPosition == std::string::npos)
	{
		std::cerr<<"文件名字有错误"<<std::endl;
		return;
	}
	std::string extensionName = m_id.substr(dotPosition + 1);
	std::transform(extensionName.begin(), extensionName.end(),extensionName.begin(), toupper);
	if(extensionName == "SHP")
		computeContourInfoFromSHP();
	else if(extensionName == "CON")
		computeContourInfoFromCON();
/*
	m_info.m_xMin = m_box.xMin();
	m_info.m_yMin = m_box.yMin();
	m_info.m_xMax = m_box.xMax();
	m_info.m_yMax = m_box.yMax();

	std::sort(m_values.begin(),m_values.end());
	m_info.m_minElevation = *(m_values.begin());
	m_info.m_maxElevation = *(m_values.end() - 1);

	float value = *(m_values.begin());
	float currentValue = *(m_values.begin());

	std::vector<float>::iterator Iter = m_values.begin();
	for(; Iter != m_values.end(); Iter++)
	{
		currentValue = *Iter;
		if(currentValue != value)
			break;
	}

	m_info.m_step = fabs(currentValue - value);*/




 }


void ContourNode::computeContourInfoFromSHP()
{
	FILE* file = NULL;
	char* fileBuf;
	if((file = fopen(m_id.c_str(), "rb")) == NULL)
	{
		std::cout<<"SHP文件无法打开！"<<std::endl;
		return ;
	}

	long fileSize = KLValueOfFile::sizeOfFile(file);
	fileBuf = new char[fileSize];
	fread(fileBuf, fileSize, 1 ,file);
	
	int ShapeType;
	ShapeType = KLValueOfFile::littleEndianIntRead(fileBuf, 32);
	if(ShapeType!=3 && ShapeType != 13)
	{
		std::cout<<"此文件内容非等值线"<<std::endl;
		return;
	}
	m_info.m_xMin = KLValueOfFile::littleEndianDoubleRead(fileBuf, 36);
	m_info.m_yMin = KLValueOfFile::littleEndianDoubleRead(fileBuf, 44);
	m_info.m_xMax = KLValueOfFile::littleEndianDoubleRead(fileBuf, 52);
	m_info.m_yMax = KLValueOfFile::littleEndianDoubleRead(fileBuf, 60);
	double minHeight = KLValueOfFile::littleEndianDoubleRead(fileBuf, 68);
	double MaxHeight = KLValueOfFile::littleEndianDoubleRead(fileBuf, 76);

	delete []fileBuf;
	fclose(file);


	std::vector<float> pValues;
	std::vector<float>::iterator Iter = m_values.begin();
	for(; Iter != m_values.end(); Iter++)
	{
		pValues.push_back(*Iter);
	}

	std::sort(pValues.begin(),pValues.end());
	m_info.m_minElevation = *(pValues.begin());
	m_info.m_maxElevation = *(pValues.end() - 1);

	float value = *(pValues.begin());
	float currentValue = *(pValues.begin());

	std::vector<float>::iterator Iter2 = pValues.begin();
	for(; Iter2 != pValues.end(); Iter2++)
	{
		currentValue = *Iter2;
		if(currentValue != value)
			break;
	}

	m_info.m_step = fabs(currentValue - value);

}


void ContourNode::computeContourInfoFromCON()
{

}


ContourInfo ContourNode::getContourInfo()
{
	return m_info;
}


float ContourNode::getXOffset()
{
	return m_xOffset;
}


float ContourNode::getYOffset()
{
	return m_yOffset;
}


void ContourNode::setTerrainScale(osg::MatrixTransform* mt)
{
	osg::Matrix invertMatrix = mt->getInverseMatrix();
	m_terrainScaleZ = (invertMatrix.getScale()).z();
}

/*
void ContourNode::generateContourNodeFromSHP(const std::string& fileName)
{
	long startTime = clock();
	osg::ref_ptr<osg::Geode> pGeode = dynamic_cast<osg::Geode*>(osgDB::readNodeFile(fileName));
	long endTime = clock();
	std::cout<<"OSG插件读取SHP文件所用时间："<< (endTime - startTime)/1000 <<"秒!"<<std::endl;

	TerrainTileManager* pTerrainTileManager = new TerrainTileManager(m_terrainNode);
	pTerrainTileManager->gatherCurrentOSGScenceTerrainTile();

	osg::ref_ptr<osg::Geometry> p_Geometry = pGeode->getDrawable(0)->asGeometry();
	osg::ref_ptr<osg::Array> p_Array = p_Geometry->getVertexArray();
	unsigned int num = p_Array->getType();
	osg::ref_ptr<osg::Vec3Array> p_VecArray = dynamic_cast<osg::Vec3Array*>(p_Geometry->getVertexArray());
	float xBase = 0.0f;
	float yBase = 0.0f;
	float sampleXCoord = (*p_VecArray)[0].x();
	float sampleYCoord = (*p_VecArray)[0].y();
	xBase = floorf(sampleXCoord / m_xBaseNum);
	yBase = floorf(sampleYCoord / m_yBaseNum);
	(xBase >= 1.0) ? m_xOffset = xBase * m_xBaseNum : m_xOffset = 0.0f;
	(yBase >= 1.0) ? m_yOffset = yBase * m_yBaseNum : m_yOffset = 0.0f;
 
	unsigned int NumOfGeo = pGeode->getNumDrawables();
	for(unsigned int i = 0; i < NumOfGeo; i++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = new ContourGeode();

		osg::ref_ptr<osg::Vec3Array> coor = new osg::Vec3Array();
		m_currentCoords.push_back(coor);

		osg::ref_ptr<osg::Geometry> pGeometry = pGeode->getDrawable(i)->asGeometry();
		osg::ref_ptr<osg::Vec3Array> pVec3Array = dynamic_cast<osg::Vec3Array*>(pGeometry->getVertexArray());
		osg::ref_ptr<osg::Vec3Array> pCtrlArray = new osg::Vec3Array();
		unsigned int NumOfArr = pVec3Array->size();
		bool signal = false;

		for(unsigned int j = 0; j < NumOfArr; j++)
		{
			pTerrainTileManager->getValue((*pVec3Array)[j].x(),(*pVec3Array)[j].y(),(*pVec3Array)[j].z());
			if((*pVec3Array)[j].z() > 0 && signal == false)
			{
				m_values.push_back(ceil((*pVec3Array)[j].z()));
				signal = true;
			}
			m_currentCoords[i]->push_back(osg::Vec3((*pVec3Array)[j].x(),(*pVec3Array)[j].y(),(*pVec3Array)[j].z()));

			(*pVec3Array)[j].x() = (*pVec3Array)[j].x() - m_xOffset;
			(*pVec3Array)[j].y() = (*pVec3Array)[j].y() - m_yOffset;
			pCtrlArray->push_back(osg::Vec3((*pVec3Array)[j].x(),(*pVec3Array)[j].y(),(*pVec3Array)[j].z()));
		}
		if(signal == false)
			m_values.push_back(0.0f);

		pContourGeode->setID(i+1);
		pContourGeode->setCtrlPointArray(pCtrlArray);
		pContourGeode->setXOffset(m_xOffset);
		pContourGeode->setYOffset(m_yOffset);
		pContourGeode->init();

		addChild(pContourGeode);
		m_contourGeodeMap.insert(std::pair<unsigned int,osg::ref_ptr<ContourGeode>>(i+1, pContourGeode));

		//std::cout<<"生成第"<< i + 1<<"条线"<<std::endl;

	}
	delete pTerrainTileManager;
	pTerrainTileManager = NULL;
}
*/


void ContourNode::generateContourNodeFromSHP(const std::string& fileName)
{
	m_id = fileName;

	//读取Shp文件的高程，暂时将dbf文件转化成txt文件，读取txt文件
	KLValueOfFile::readContourHeightFromTxt(fileName, m_values);

	long startTime = clock();
	osg::ref_ptr<osg::Geode> pGeode = dynamic_cast<osg::Geode*>(osgDB::readNodeFile(fileName));
	long endTime = clock();
	std::cout<<"OSG插件读取SHP文件所用时间："<< (endTime - startTime)/1000 <<"秒!"<<std::endl;

	TerrainTileManager* pTerrainTileManager = new TerrainTileManager(m_terrainNode);
	pTerrainTileManager->gatherCurrentOSGScenceTerrainTile();

	osg::ref_ptr<osg::Geometry> p_Geometry = pGeode->getDrawable(0)->asGeometry();
	osg::ref_ptr<osg::Array> p_Array = p_Geometry->getVertexArray();
	unsigned int num = p_Array->getType();
	osg::ref_ptr<osg::Vec3Array> p_VecArray = dynamic_cast<osg::Vec3Array*>(p_Geometry->getVertexArray());
	float xBase = 0.0f;
	float yBase = 0.0f;
	float sampleXCoord = (*p_VecArray)[0].x();
	float sampleYCoord = (*p_VecArray)[0].y();
	xBase = floorf(sampleXCoord / m_xBaseNum);
	yBase = floorf(sampleYCoord / m_yBaseNum);
	(xBase >= 1.0) ? m_xOffset = xBase * m_xBaseNum : m_xOffset = 0.0f;
	(yBase >= 1.0) ? m_yOffset = yBase * m_yBaseNum : m_yOffset = 0.0f;

	unsigned int NumOfGeo = pGeode->getNumDrawables();
	for(unsigned int i = 0; i < NumOfGeo; i++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = new ContourGeode();

		osg::ref_ptr<osg::Geometry> pGeometry = pGeode->getDrawable(i)->asGeometry();
		osg::ref_ptr<osg::Vec3Array> pCtrlArray = dynamic_cast<osg::Vec3Array*>(pGeometry->getVertexArray());
		unsigned int NumOfArr = pCtrlArray->size();

		for(unsigned int j = 0; j < NumOfArr; j++)
		{
			pTerrainTileManager->getValue((*pCtrlArray)[j].x(),(*pCtrlArray)[j].y(),(*pCtrlArray)[j].z());

			(*pCtrlArray)[j].x() = (*pCtrlArray)[j].x() - m_xOffset;
			(*pCtrlArray)[j].y() = (*pCtrlArray)[j].y() - m_yOffset;
		}

		pContourGeode->setID(i+1);
		pContourGeode->setCtrlPointArray(pCtrlArray);
		pContourGeode->setContourHeight(m_values[i]);
		pContourGeode->setXOffset(m_xOffset);
		pContourGeode->setYOffset(m_yOffset);
		pContourGeode->init();

		addChild(pContourGeode);
		m_contourGeodeMap.insert(std::pair<unsigned int,osg::ref_ptr<ContourGeode>>(i+1, pContourGeode));

		//std::cout<<"生成第"<< i + 1<<"条线"<<std::endl;

	}

	std::cout<<"等值线总共的条数是："<<NumOfGeo<<std::endl;

	delete pTerrainTileManager;
	pTerrainTileManager = NULL;
}


void ContourNode::generateContourNodeFromShpByGDAL(const std::string& fileName)
{
	m_id = fileName;

	//读取Shp文件的高程，暂时将dbf文件转化成txt文件，读取txt文件
	//KLValueOfFile::readContourHeightFromTxt(fileName, m_values);
	KLValueOfFile::readDbfFunc(fileName, m_values);

	TerrainTileManager* pTerrainTileManager = new TerrainTileManager(m_terrainNode);
	pTerrainTileManager->gatherCurrentOSGScenceTerrainTile();

	bool signal = false;

	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO" );
	OGRRegisterAll();

	OGRDataSource* poDS = (OGRDataSource*)OGRSFDriverRegistrar::Open(fileName.c_str(),FALSE);
	if(poDS == NULL)
	{
		std::cout<<"文件无法打开"<<std::endl;
		return ;
	}

	int mmm=0;
	mmm= poDS->GetLayerCount();

	OGRLayer* poLayer = poDS->GetLayer(0);
	poLayer->ResetReading();
	OGRFeature *poFeature;

	unsigned int num = 0;

	while((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		OGRGeometry *poGeometry=poFeature->GetGeometryRef();
		if (poGeometry!=NULL)
		{
			switch (wkbFlatten(poGeometry->getGeometryType()))
			{
			case wkbLineString:
				num++;
				OGRLineString *poLine = (OGRLineString*)poGeometry;
				osg::ref_ptr<ContourGeode> pContourGeode = new ContourGeode();
				osg::ref_ptr<osg::Vec3Array> pCtrlArray = new osg::Vec3Array();
				for(int i = 0;i<poLine->getNumPoints();i++)
				{
					float x = poLine->getX(i);
					float y = poLine->getY(i);
					float z ;//= poLine->getZ(i);
					pTerrainTileManager->getValue(x, y, z);
					if(i==0 && signal == false)
					{
						float xBase = 0.0f;
						float yBase = 0.0f;
						float sampleXCoord = x;
						float sampleYCoord = y;
						xBase = floorf(sampleXCoord / m_xBaseNum);
						yBase = floorf(sampleYCoord / m_yBaseNum);
						(xBase >= 1.0) ? m_xOffset = xBase * m_xBaseNum : m_xOffset = 0.0f;
						(yBase >= 1.0) ? m_yOffset = yBase * m_yBaseNum : m_yOffset = 0.0f;

						signal = true;
					}

					x = x - m_xOffset;
					y = y - m_yOffset;

					pCtrlArray->push_back(osg::Vec3(x, y, z));
				}

				pContourGeode->setID(num);
				pContourGeode->setCtrlPointArray(pCtrlArray);
				pContourGeode->setContourHeight(m_values[num-1]);
				pContourGeode->setXOffset(m_xOffset);
				pContourGeode->setYOffset(m_yOffset);
				pContourGeode->init();

				addChild(pContourGeode);
				m_contourGeodeMap.insert(std::pair<unsigned int,osg::ref_ptr<ContourGeode>>(num, pContourGeode));

				OGRFeature::DestroyFeature( poFeature );
				break;
			}
		}
	}

	std::cout<<"等值线总共的条数是："<<num<<std::endl;
	delete pTerrainTileManager;
	pTerrainTileManager = NULL;
}


void ContourNode::generateContourNodeFromCon(const std::string& fileName)
{
	m_id = fileName;

	std::ifstream file(fileName.c_str());
	std::string line;

	bool readValueBegun = false;

	float x = 0.0f;
	float y = 0.0f;
	float height = 0.0f;
	int number = 0;
	int currentNumber = 0;
	bool signal = false;

	long beginTime = clock();

	while(!file.eof())
	{
		std::getline(file,line);
		if(readValueBegun)
		{
			std::istringstream iss(line);
			iss >> x >> y >> height >> number;
			if(number != currentNumber)
			{
				osg::ref_ptr<osg::Vec3Array> pCtrlArray = new osg::Vec3Array;
				m_values.push_back(height);
				m_currentCoords.push_back(pCtrlArray);
				currentNumber = number;
			}
			if(signal == false)
			{
				float xBase = 0.0f;
				float yBase = 0.0f;
				float sampleXCoord = x;
				float sampleYCoord = y;
				xBase = floorf(sampleXCoord / m_xBaseNum);
				yBase = floorf(sampleYCoord / m_yBaseNum);
				(xBase >= 1.0) ? m_xOffset = xBase * m_xBaseNum : m_xOffset = 0.0f;
				(yBase >= 1.0) ? m_yOffset = yBase * m_yBaseNum : m_yOffset = 0.0f;

				signal = true;
			}

			x = x - m_xOffset;
			y = y - m_yOffset;

			m_currentCoords[currentNumber - 1]->push_back(osg::Vec3(x, y ,m_values[currentNumber - 1]));
		}
		else
		{
			if(line.size() && line[0] == '#')
			{
				readValueBegun = true;
			}
		}
	}

	file.close();

	long endTime = clock();
	std::cout<<"读取con文件的时间是："<<(endTime-beginTime)/1000<<"秒!"<<std::endl;

	for(unsigned int i = 0; i < m_currentCoords.size(); i++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = new ContourGeode();
		osg::ref_ptr<osg::Vec3Array> pCtrlArray = m_currentCoords[i];

		pContourGeode->setID(i + 1);
		pContourGeode->setCtrlPointArray(pCtrlArray);
		pContourGeode->setContourHeight(m_values[i]);
		pContourGeode->setXOffset(m_xOffset);
		pContourGeode->setYOffset(m_yOffset);
		pContourGeode->init();

		addChild(pContourGeode);
		m_contourGeodeMap.insert(std::pair<unsigned int,osg::ref_ptr<ContourGeode>>(i + 1, pContourGeode));
	}
}


ContourGeodeMap ContourNode::getContourGeodeMap()
{
	return m_contourGeodeMap;
}


void ContourNode::contourRarefying()
{
	//当等值线所有的控制点的数量低于1000000的时候，不再做抽稀处理
	if(m_allContourPointNum <= 1000000)
	{
		//this->setNodeMask(true);
		return;
	}	

	//this->setNodeMask(true);
	osg::Camera* pCamera = m_viewer->getCamera();

	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;

	pCamera->getViewMatrixAsLookAt(eye, center, up);
	double distance = fabs(eye.z());
	/*if(distance < 30000)
		this->setNodeMask(true);
	else
		this->setNodeMask(false);*/
	float pContourStep = m_info.m_step;
	float pMinHeight = m_info.m_minElevation;
	float pMaxHeight = m_info.m_maxElevation;
	unsigned int Num = ceil(distance/10000.0f) + 1;
	//场景中显示的等值线总共的点数量
	unsigned int disPlayPointNum = 0;

	if(Num < 3)
	{
		//pContourStep = pContourStep * Num;
		std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
		for(; Iter != m_contourGeodeMap.end(); Iter++)
		{
			osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
			pContourGeode->setNodeMask(true);
			osg::ref_ptr<osg::Geode> pTextGeode = pContourGeode->getTextGeode();
			if(pTextGeode)
				pTextGeode->setNodeMask(true);
			disPlayPointNum += pContourGeode->getCtrlPointNum();
		}

	}
	else
	{
		//首先将所有的等值线(标注)隐藏
		std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
		for(; Iter != m_contourGeodeMap.end(); Iter++)
		{
			osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
			pContourGeode->setNodeMask(false);
			osg::ref_ptr<osg::Geode> pTextGeode = pContourGeode->getTextGeode();
			if(pTextGeode)
				pTextGeode->setNodeMask(false);
		}

		pContourStep = pContourStep * Num;
		float contourExtension = pMaxHeight - pMinHeight;
		if(pContourStep > contourExtension)
			return;
		unsigned int circulateNum = (unsigned int)(contourExtension/pContourStep);
		for(unsigned int i = 0; i <= circulateNum; i++)
		{
			unsigned int contourNum = 0;
			std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
			for(; Iter != m_contourGeodeMap.end(); Iter++)
			{
				osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
				if(pContourGeode->getContourHeight() == (pMinHeight + pContourStep*i))
				{
					pContourGeode->setNodeMask(true);
					osg::ref_ptr<osg::Geode> pTextGeode = pContourGeode->getTextGeode();
					if(pTextGeode)
						pTextGeode->setNodeMask(true);
					contourNum++;
					disPlayPointNum += pContourGeode->getCtrlPointNum();
				}
			}

			std::cout<<"高程为"<< (pMinHeight + pContourStep*i) <<"m的等值线找到"<<contourNum<<"条"<<std::endl;
		}
		std::cout<<std::endl;
	} 
	std::cout<<"场景中现有等值线的点的数量是："<<disPlayPointNum<<std::endl;
	std::cout<<std::endl;
}


void ContourNode::notContourRarefying()
{
	std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = m_contourGeodeMap.begin();
	for(; Iter != m_contourGeodeMap.end(); Iter++)
	{
		osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
		pContourGeode->setNodeMask(true);
		osg::ref_ptr<osg::Geode> pTextGeode = pContourGeode->getTextGeode();
		if(pTextGeode)
			pTextGeode->setNodeMask(true);
	}
}


void ContourNode::computeOrReadFileGenerateLabel()
{
	size_t dotPosition = m_id.find_last_of('.');
	if(dotPosition == std::string::npos)
	{
		std::cerr<<"文件名字有错误"<<std::endl;
		return;
	}
	std::string fileBaseName = m_id.substr(0,dotPosition);
	std::string fileNameOfLabelTxt = fileBaseName + "_label.txt";

	std::fstream file(fileNameOfLabelTxt);
	//如果文件为空需要计算生成标注文件
	if(!file)
	{
		std::cout<<"标注文件不存在，需要生成标注文件，请耐心等待......"<<std::endl;
		unsigned int lineNum = 0;
		long startTime = clock();
		ContourGeodeMap pcontourGeodeMap = this->getContourGeodeMap();
		std::map<unsigned int,osg::ref_ptr<ContourGeode>>::iterator Iter = pcontourGeodeMap.begin();
		for(; Iter != pcontourGeodeMap.end(); Iter++)
		{
			lineNum++;
			osg::ref_ptr<ContourGeode> pContourGeode = Iter->second;
			pContourGeode->computeLabelPositon();
			std::cout<<"计算完第"<<lineNum<<"条!"<<std::endl;
			osg::ref_ptr<osg::Geode> ptextGeode = pContourGeode->getTextGeode();

			m_contourLabelGroup->addChild(ptextGeode);
			
		    std::vector<osg::Vec2d> ptextPositionVector = pContourGeode->getLabelPosition();
			
			//向文件写数据
		}

		initLabel();

		long endTime = clock();
		std::cout<<"标注文件已生成，耗时为："<< (endTime - startTime)/1000 <<"秒!"<<std::endl;
	}
	else
	{
		std::cout<<"标注文件存在，需要读取生成标注节点，请耐心等待......"<<std::endl;
	}

	file.close();

}


osg::ref_ptr<osg::Group> ContourNode::generateContourLabelNode(std::vector<osg::ref_ptr<osg::Vec3Array>> pcurrentCoords)
{
	m_occupied = new std::vector<OccupyingRectangle>();
	osg::ref_ptr<osg::Group> pGroup = new osg::Group();
	for(std::vector<int>::size_type i = 0; i < pcurrentCoords.size(); i++)
	{
		if(i % (0 + 1) == 0)
		{
			std::stringstream value;
			value << m_values[i];

			bool found = false;
			osg::Vec3 position;

			for(osg::Vec3Array::size_type j = 0; j < pcurrentCoords[i]->size(); ++j)
			{
				position = pcurrentCoords[i]->at(j);
				position.x() -= m_xOffset;
				position.y() -= m_yOffset;
				if(!isOccupied(position, value.str().size()))
				{
					found = true;
					break;
				}
			}

			if(found)
			{
				//osgText::Text3D *text = new osgText::Text3D();
				osgText::Text* text = new osgText::Text();
				osg::ref_ptr<osgText::Font>  font= new osgText::Font();
				font = osgText::readFontFile("simhei.ttf");
				text->setFont(font.get());
				text->setCharacterSize(m_labelSize);
				text->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
				text->setText(value.str());
				//用shader时显示标注的位置
				text->setPosition(osg::Vec3(position.x(), position.y(), m_values[i] + 30));
				//非shader时显示标注的位置
				//text->setPosition(osg::Vec3(position.x() + m_xOffset, position.y() + m_yOffset, m_values[i] + 30));
				osg::ref_ptr<osg::Geode> geode = new osg::Geode();
				geode->addDrawable(text);
				pGroup->addChild(geode);
			}
		}
	}

	delete m_occupied;
	return pGroup;
}


bool ContourNode::isOccupied(osg::Vec3 position, std::string::size_type size)
{
	float labelSize = m_labelSize * 1.2f;
	float x01 = position.x();
	float y01 = position.y();
	float x02 = x01 + labelSize * static_cast<float>(size)/1.6f;
	float y02 = y01 + labelSize;

	std::vector<OccupyingRectangle>::iterator Iter;
	for(Iter = m_occupied->begin(); Iter != m_occupied->end(); Iter++)
	{
		if(x01 < Iter->x2 && Iter->x1 < x02 && y01 < Iter->y2 && Iter->y1 < y02)
		{
			return true;
		}
	}

	m_occupied->push_back(OccupyingRectangle(x01, x02, y01, y02));
	return false;
}


std::vector<osg::ref_ptr<osg::Vec3Array>> ContourNode::getCurrentCoords()
{
	return m_currentCoords;
}