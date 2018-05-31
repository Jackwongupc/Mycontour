
#include<osg/ComputeBoundsVisitor>

#include "TextureManager.h"

TextureManager::TextureManager(osg::Node* pTerrainNode):
	m_id(pTerrainNode),
	m_terrainNode(pTerrainNode),
	m_demTexture(NULL),
	m_sourcePath("")
{
	osg::ComputeBoundsVisitor cbVisitor;
	m_terrainNode->accept(cbVisitor);
	m_terrainBounding = cbVisitor.getBoundingBox();
}


TextureManager::~TextureManager()
{

}

void TextureManager::init()
{
	//������ά����������� �洢���θ߳�
	m_demTexture  = getOrCreateDemTexture();

	//������Ⱦ���״̬
	setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f,1.0f));
	setViewport(0, 0, TEXTURESIZE, TEXTURESIZE);
	setRenderOrder(osg::Camera::PRE_RENDER);
	setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

	//ͶӰ�任��ͨ����ͶӰ��ʽ����ģ�ͷ�����һ���������У����������ҿ��right�����¿��top�͵��ΰ�Χ��һ�£����Ϊ100~10000
	double right  = (m_terrainBounding._max.x() - m_terrainBounding._min.x())/2.0;
	double top    = (m_terrainBounding._max.y() - m_terrainBounding._min.y())/2.0;
	setProjectionMatrixAsOrtho(-right, right, -top, top, 1, 10000);

	//ͨ�������ӵ�λ��eyePoint���ο���λ��center���ӵ��Ϸ���up��ȷ�������λ�ú���̬
	//�Ӷ����������������������ϵWCSת��Ϊ�ӵ�����ϵVCS
	osg::Vec3d center((m_terrainBounding._min.x()+ m_terrainBounding._max.x())/2,
		(m_terrainBounding._min.y()+ m_terrainBounding._max.y())/2,
		/*(m_terrainBounding._min.z()+ m_terrainBounding._max.z())/2*/m_terrainBounding._max.z()*1.2);
	osg::Vec3d upDirection(0.0, 1.0, 0.0);
	osg::Vec3d eyePoint = center + osg::Vec3d(0.0, 0.0, 10);
	setViewMatrixAsLookAt(eyePoint, center, upDirection);

	// ���������
	addChild(m_terrainNode);
	attach(osg::Camera::COLOR_BUFFER, m_demTexture);
	

	// ����״̬��
	m_stateSet = getOrCreateStateSet();
	osg::ref_ptr<osg::Program> m_program = new osg::Program();
	m_program->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX,m_sourcePath + "shaders/DesignDepthRender.vert"));
	m_program->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT,m_sourcePath + "shaders/DesignDepthRender.frag"));
	m_stateSet->setAttribute(m_program);


	//���ø��»ص�
	osg::ref_ptr<RttCallBack> pRttCallback = new RttCallBack(this);
	setUpdateCallback(pRttCallback);
}

void TextureManager::setID(osg::Node* id)
{
	m_id = id;
}

osg::Node* TextureManager::getID()
{
	return m_id;
}

osg::Node* TextureManager::getTerrainNode()
{
	return m_terrainNode;
}

osg::BoundingBox TextureManager::getTerrainBoundingBox() const
{
	return m_terrainBounding;
}

osg::ref_ptr<osg::Texture2D> TextureManager::getOrCreateDemTexture()
{
	if(!m_demTexture)
	{
		m_demTexture = new osg::Texture2D();
		m_demTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		m_demTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

		m_demTexture->setTextureSize(TEXTURESIZE,TEXTURESIZE);
		m_demTexture->setInternalFormat(GL_RGBA32F_ARB);
		m_demTexture->setSourceFormat(GL_LUMINANCE);

	}

	return m_demTexture;
}

void TextureManager::setSourcePath(const std::string& sourcePath)
{
	m_sourcePath = sourcePath;
}

//***********************************************RttCallback***********************************//
RttCallBack::RttCallBack(osg::Node* pRttCamera):
	m_rttCamera(pRttCamera),
	m_count(0)
{

}

RttCallBack::~RttCallBack()
{


}

void RttCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
{

	if(m_count == RTTCOUNT)
	{
		m_rttCamera->setNodeMask(false);
	}

	if(m_count!=RTTCOUNT)
	{

		m_count++;
	}

	traverse(node, nv);
}