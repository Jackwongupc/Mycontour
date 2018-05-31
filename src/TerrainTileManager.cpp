
#include <omp.h> 

#include "TerrainTileManager.h"
#include <osg/Vec3f>

#include<iostream>

bool isExistInvalidValue(osg::HeightField* hf, double x, double y)
{
	double r, c;
	c = (x - hf->getOrigin().x()) / hf->getXInterval();
	r = (y - hf->getOrigin().y()) / hf->getYInterval();

	int rowMin = osg::maximum((int)floor(r), 0);
	int rowMax = osg::maximum(osg::minimum((int)ceil(r), (int)(hf->getNumRows()-1)), 0);
	int colMin = osg::maximum((int)floor(c), 0);
	int colMax = osg::maximum(osg::minimum((int)ceil(c), (int)(hf->getNumColumns()-1)), 0);

	if(rowMin > rowMax) rowMin = rowMax;
	if(colMin > colMax) colMin = colMax;

	if(hf->getHeight(colMax, rowMax) == -99999 || hf->getHeight(colMin, rowMax) == -99999 ||
		hf->getHeight(colMax, rowMin) == -999999 || hf->getHeight(colMin, rowMin) == -99999)
		return true;
	else
		return false;
}


TerrainTileManager::TerrainTileManager(osg::Node* rootNode):
		m_root(rootNode),
		m_curPos(0),
		m_paralNum(PARALLELNUM),
		m_xOffset(0.0f),
		m_yOffset(0.0f)
{

}

/**
* 首先清除m_TileMap，然后利用m_memVisitor类，来遍历OSG场景树中，有效的TerrainTile节点。
*/
void TerrainTileManager::gatherCurrentOSGScenceTerrainTile()
{
	m_tileMap.clear();
	m_extentsVect.clear();
	m_tileIDVect.clear();
	TerrainTileVisitor m_memVisitor(this);
	m_root->accept(m_memVisitor);
}


/**
* 得到z值。根据x，y值在当前视图上，通过差值得到对应的高程值
*/
void TerrainTileManager::getValue(float x, float y, float& z)
{
	osg::Vec3f vec;
	vec.x()=x;
	vec.y()=y;

	if ((m_extentsVect.size() > 0) && (m_extentsVect[m_curPos].isContain(vec)))
	{
		//在当前指向缓存中
		osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[m_curPos]]->getElevationLayer());
		/*z=getInterpolatedValue(hf->getHeightField(),x,y);
				return;*/
		if(!isExistInvalidValue(hf->getHeightField(), x, y))
			z = getInterpolatedValue(hf->getHeightField(), x, y);
		else
			z = -99999;
		return;
		
	}

	//差值x，y所在的tile
	for(int i=0;i<m_extentsVect.size();i++)
	{		
		if (m_extentsVect[i].isContain(vec))
		{
			//查找到在此模块范围
			osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[i]]->getElevationLayer());
			/*z=getInterpolatedValue(hf->getHeightField(),x,y);
			m_curPos=i;
			break;*/
			if(!isExistInvalidValue(hf->getHeightField(),x ,y))
				z = getInterpolatedValue(hf->getHeightField(), x, y);
			else
				z = -99999;
			m_curPos = i;
			break;

		}
	}
}


/**
* 得到z值。根据x，y值在当前视图上，通过差值得到对应的高程值
*/
void TerrainTileManager::getValue(float x, float y, osg::Vec3f& value)
{
}

/*
void TerrainTileManager::getValue(std::vector<KLShotPoint*> m_shotpointVect)
{
	osg::Vec3Array* vertexArray = new osg::Vec3Array();
	for(int i=0; i<m_shotpointVect.size(); i++)
	{
	   vertexArray->push_back(m_shotpointVect[i]->getRealPt());
	}
	getValue(vertexArray,vertexArray->size());
	for(int i=0; i<m_shotpointVect.size(); i++)
	{
		m_shotpointVect[i]->setRealPt((*vertexArray)[i]);
	}
}*/



/**
* 根据vec3f中的x,y得到z ,此vec中的x,y最好是有序的，这样速度会比较快
*/
void TerrainTileManager::getValue(std::vector<osg::Vec3f>& vecs)
{
	if (vecs.size()<m_paralNum)
	{ //如果数量较少，不用启用并行
		for (int i=0;i<vecs.size();i++)
		{
			getValue(vecs[i].x(),vecs[i].y(),vecs[i].z());
		}
	}
	else
	{
		//使用并行代码
		//设置并行线程数量
		int prossNum=omp_get_num_procs();
		omp_set_num_threads(prossNum);
		//得到数组并行的块数
		int blockNum=vecs.size()/prossNum;
		int j,pos,curPos;
#pragma omp parallel for private(j,pos,curPos)
		for (int i=0;i<prossNum;i++)
		{
			pos=i*blockNum;
			curPos=0; //标识当前heightfieldlayer位置
			for (j=0;j<blockNum;j++,pos++)
			{
				
				if (m_extentsVect[curPos].isContain(vecs[pos]))
				{
					//在当前指向缓存中
					osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[curPos]]->getElevationLayer());
					vecs[pos].z()=getInterpolatedValue(hf->getHeightField(),vecs[pos].x(),vecs[pos].y());
				}else{
					//差值x，y所在的tile
					for(int d=0;d<m_extentsVect.size();d++)
					{		
						if (m_extentsVect[d].isContain(vecs[pos]))
						{
							//查找到在此模块范围
							osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[d]]->getElevationLayer());
							vecs[pos].z()=getInterpolatedValue(hf->getHeightField(),vecs[pos].x(),vecs[pos].y());
							curPos=d;						
						}else
						{
							continue;
						}
					}
				}
			}			
		}//end  parallel for 
		//计算剩余的数值

		for (int pos=prossNum*blockNum;pos<vecs.size();pos++)
		{
			getValue(vecs[pos].x(),vecs[pos].y(),vecs[pos].z());
		}
	}

}

void TerrainTileManager::getValue(osg::Vec3Array* vecs, int size)
{
	int jingshen = false;
	if (size<m_paralNum)
	{ //如果数量较少，不用启用并行
		for (int i=0;i<size;i++)
		{
			getValue((*vecs)[i].x(),(*vecs)[i].y(),(*vecs)[i].z());
		}
	}
	else
	{
		////使用并行代码
		//设置并行线程数量
		int prossNum=omp_get_num_procs();
		//omp_set_num_threads(prossNum);
		//得到数组并行的块数
		int blockNum=size/prossNum;
		int j,pos,curPos;
		//#pragma omp parallel for private(j,pos,curPos)
		for (int i=0;i<prossNum;i++)
		{
			pos=i*blockNum;
			curPos=0;                   //标识当前heightfieldlayer位置
			for (j=0;j<blockNum;j++,pos++)
			{

				if ((m_extentsVect.size() > 0) && (m_extentsVect[curPos].isContain( (*vecs)[pos] )))
				{
					//在当前指向缓存中
					osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[curPos]]->getElevationLayer());
					/*(*vecs)[pos].z()=getInterpolatedValue(hf->getHeightField(),(*vecs)[pos].x(),(*vecs)[pos].y());*/
					if(!isExistInvalidValue(hf->getHeightField(),(*vecs)[pos].x(),(*vecs)[pos].y()))
					{
						float heightValue = getInterpolatedValue(hf->getHeightField(),(*vecs)[pos].x(),(*vecs)[pos].y());
						if(heightValue != -99999.0f)
						{
							(*vecs)[pos].z() = heightValue;
						}
						else
						{
							jingshen = true;
						}
					}
					else
					{
						jingshen = true;
					}
				}else{
					//差值x，y所在的tile
					for(int d=0;d<m_extentsVect.size();d++)
					{		
						if (m_extentsVect[d].isContain((*vecs)[pos]))
						{
							//查找到在此模块范围
							osgTerrain::HeightFieldLayer* hf=dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[d]]->getElevationLayer());
							/*(*vecs)[pos].z()=getInterpolatedValue(hf->getHeightField(),(*vecs)[pos].x(),(*vecs)[pos].y());
							curPos=d;*/
							if(!isExistInvalidValue(hf->getHeightField(),(*vecs)[pos].x(),(*vecs)[pos].y()))
							{
								float heightValue = getInterpolatedValue(hf->getHeightField(), (*vecs)[pos].x(),(*vecs)[pos].y());
								if(heightValue != -99999.0f)
								{
									(*vecs)[pos].z() = heightValue;
								}
								else
								{
									jingshen = true;
								}
							}
							else
							{
								jingshen = true;
							}
							curPos = d;
						}else
						{
							continue;
						}
					}
				}
			}			
		}//end  parallel for 
		//计算剩余的数值

		for (int pos=prossNum*blockNum;pos<size;pos++)
		{
			getValue((*vecs)[pos].x(),(*vecs)[pos].y(),(*vecs)[pos].z());
		}
	}
}



/**
* 根据高程域，x，y坐标（实际坐标值）,得到高程值
*/
float TerrainTileManager::getInterpolatedValue(osg::HeightField* hf, double x, double y)
{
	double r, c;
	c = (x - hf->getOrigin().x()) / hf->getXInterval();
	r = (y - hf->getOrigin().y()) / hf->getYInterval();

	int rowMin = osg::maximum((int)floor(r), 0);
	int rowMax = osg::maximum(osg::minimum((int)ceil(r), (int)(hf->getNumRows()-1)), 0);
	int colMin = osg::maximum((int)floor(c), 0);
	int colMax = osg::maximum(osg::minimum((int)ceil(c), (int)(hf->getNumColumns()-1)), 0);

	if (rowMin > rowMax) rowMin = rowMax;
	if (colMin > colMax) colMin = colMax;

	float urHeight = hf->getHeight(colMax, rowMax);
	float llHeight = hf->getHeight(colMin, rowMin);
	float ulHeight = hf->getHeight(colMin, rowMax);
	float lrHeight = hf->getHeight(colMax, rowMin);

	double x_rem = c - (int)c;
	double y_rem = r - (int)r;

	double w00 = (1.0 - y_rem) * (1.0 - x_rem) * (double)llHeight;
	double w01 = (1.0 - y_rem) * x_rem * (double)lrHeight;
	double w10 = y_rem * (1.0 - x_rem) * (double)ulHeight;
	double w11 = y_rem * x_rem * (double)urHeight;

	float result = (float)(w00 + w01 + w10 + w11);

	return result;
}

/**
* 测试使用，为根据指定的顶点值，获得其周围四个顶点的数值                                                                      
*/
void TerrainTileManager::printSroundVec(std::vector<osg::Vec3f>& vecs)
{
	for (int i=0;i<vecs.size();i++)
	{
		for(int j=0;j<m_extentsVect.size();j++)
		{		
			if (m_extentsVect[j].isContain(vecs[i]))
			{
				//查找到在此模块范围
				osg::HeightField* hf=(dynamic_cast<osgTerrain::HeightFieldLayer*>(m_tileMap[m_tileIDVect[j]]->getElevationLayer()))->getHeightField();
				double r, c;
				c = (vecs[i].x() - hf->getOrigin().x()) / hf->getXInterval();
				r = (vecs[i].y() - hf->getOrigin().y()) / hf->getYInterval();

				int rowMin = osg::maximum((int)floor(r), 0);
				int rowMax = osg::maximum(osg::minimum((int)ceil(r), (int)(hf->getNumRows()-1)), 0);
				int colMin = osg::maximum((int)floor(c), 0);
				int colMax = osg::maximum(osg::minimum((int)ceil(c), (int)(hf->getNumColumns()-1)), 0);

				if (rowMin > rowMax) rowMin = rowMax;
				if (colMin > colMax) colMin = colMax;

				/*float urHeight = hf->getHeight(colMax, rowMax);
				float llHeight = hf->getHeight(colMin, rowMin);
				float ulHeight = hf->getHeight(colMin, rowMax);
				float lrHeight = hf->getHeight(colMax, rowMin);*/
				osg::Vec3f urvec3f=hf->getVertex(colMax,rowMax);
				osg::Vec3f llvec3f=hf->getVertex(colMin,rowMin);
				osg::Vec3f ulvec3f=hf->getVertex(colMin,rowMax);
				osg::Vec3f lrvec3f=hf->getVertex(colMax,rowMin);
				//print sround vertex
				std::cout<< "----------sround vertex ----------------" <<std::endl;
				std::cout << llvec3f.x() <<" " <<llvec3f.y()<< " "<<llvec3f.z() <<std::endl;
				std::cout << ulvec3f.x() << " "<< ulvec3f.y()<< " " << ulvec3f.z() << std::endl;
				std::cout<< lrvec3f.x() << " "<< lrvec3f.y()<< " " << lrvec3f.z() << std::endl;
				std::cout<< urvec3f.x() << " "<< urvec3f.y() << " "<< urvec3f.z() << std::endl;
				std::cout<< vecs[i].x()<< " "<<vecs[i].y()<< " "<< vecs[i].z()<<std::endl;

			}
		}
	}
}


void TerrainTileManager::setXOffset(const float& xOffset)
{
	m_xOffset = xOffset;
}


void TerrainTileManager::setYOffset(const float& yOffset)
{
	m_yOffset = yOffset;
}


float TerrainTileManager::getXOffset() const
{
	return m_xOffset;
}



float TerrainTileManager::getYOffset() const
{
	return m_yOffset;
}
