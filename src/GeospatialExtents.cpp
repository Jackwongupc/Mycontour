
#include "GeospatialExtents.h"

#include <algorithm>

double _cross(const osg::Vec2 &p1, const osg::Vec2 &p2, const osg::Vec2 &q1, const osg::Vec2 &q2)
{
	return (q2.y()-q1.y()) * (p2.x()-p1.x()) - (q2.x()-q1.x()) * (p2.y()-p1.y());
}

bool _cmp(const osg::Vec2 &a, const osg::Vec2 &b)
{
	osg::Vec2 origin;
	origin.x() = origin.y() = 0;
	return _cross(origin, b, origin, a) < 0;
}

/*
void KLGeoPolySelectExtents::init()
{
	if(m_allVertexArray.valid())
		m_allVertexArray->clear();
	else
		m_allVertexArray = new osg::Vec2Array;

	if(m_boundVertexArray.valid())
		m_boundVertexArray->clear();
	else
		m_boundVertexArray = new osg::Vec2Array;
}

void KLGeoPolySelectExtents::vertexSort()
{
	if(!m_allVertexArray.valid()) return;
	if(m_allVertexArray->size() < 4) return;
	if(m_allVertexArray->size() ==  5)
	{
		m_boundVertexArray = m_allVertexArray;
		return;
	}
	m_boundVertexArray->clear();
	unsigned int i, j, k=0, top = 2;
	osg::Vec2 tmp;
	//找到最左下的点
	for(i=1; i<m_allVertexArray->size(); i++)
	{
		if( ((*m_allVertexArray)[i].y() < (*m_allVertexArray)[k].y()) ||( ((*m_allVertexArray)[i].y() == (*m_allVertexArray)[k].y()) 
			&& (*m_allVertexArray)[i].y() < (*m_allVertexArray)[k].y())	)
			k = i;
	}
	tmp = (*m_allVertexArray)[0];
	(*m_allVertexArray)[0] = (*m_allVertexArray)[k];
	(*m_allVertexArray)[k] = tmp;

	//按极角从小到大，距离偏短进行排序
	for(i=1; i<m_allVertexArray->size()-2; i++)
	{
		k = i;
		for(j = i+1; j<m_allVertexArray->size()-1; j++)
		{
			if( (multiply( (*m_allVertexArray)[j], (*m_allVertexArray)[k], (*m_allVertexArray)[0] ) > 0)
				||((multiply( (*m_allVertexArray)[j], (*m_allVertexArray)[k], (*m_allVertexArray)[0] ) == 0)
				&&(dis( (*m_allVertexArray)[0], (*m_allVertexArray)[j]) < dis( (*m_allVertexArray)[0], (*m_allVertexArray)[k]))))
				k = j;
			tmp = (*m_allVertexArray)[i];
			(*m_allVertexArray)[i] = (*m_allVertexArray)[k];
			(*m_allVertexArray)[k] = tmp;
		}
		if(m_allVertexArray->size() < 3)
		{
			m_boundVertexArray->push_back((*m_allVertexArray)[0]);
			m_boundVertexArray->push_back((*m_allVertexArray)[1]);
			m_boundVertexArray->push_back((*m_allVertexArray)[2]);
		}
		else
		{
			(*m_boundVertexArray)[0] = (*m_allVertexArray)[0];
			(*m_boundVertexArray)[1] = (*m_allVertexArray)[1];
			(*m_boundVertexArray)[2] = (*m_allVertexArray)[2];
		}

		//判断与其余所有点的关系
		for(i=3; i<m_allVertexArray->size(); i++)
		{
			while(top>=1 && multiply( (*m_allVertexArray)[i], (*m_boundVertexArray)[top],(*m_boundVertexArray)[top-1] ) >= 0) top--;

			if(m_boundVertexArray->size() > top+2)
			{
				(*m_boundVertexArray)[++top] = (*m_allVertexArray)[i];
			}
			else
			{
				m_boundVertexArray->push_back((*m_allVertexArray)[i]);
				++top;
			}
		}
	}
	std::sort(m_boundVertexArray->begin(), m_boundVertexArray->end(), _cmp);
	m_boundVertexArray->push_back((*m_boundVertexArray)[0]);
}

bool KLGeoPolySelectExtents::intersects(const KLGeospatialExtents& bb) const
{
	osg::ref_ptr<osg::Geode> _polygon;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geometry->setVertexArray(m_boundVertexArray);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,m_boundVertexArray->size()));
	osg::BoundingBox _polyBox = geometry->computeBound();

	osg::BoundingBox klge;
	klge.expandBy(osg::Vec3(bb.xMin(), bb.yMin(), 0.0));
	klge.expandBy(osg::Vec3(bb.xMin(), bb.yMax(), 0.0));
	klge.expandBy(osg::Vec3(bb.xMax(), bb.yMin(), 0.0));
	klge.expandBy(osg::Vec3(bb.xMax(), bb.yMax(), 0.0));

	return (_polyBox.intersects(klge));
}

bool KLGeoPolySelectExtents::isContain(const osg::Vec3& rtPoint)
{
	int Count = m_boundVertexArray->size();
	if(Count < 3) return false;
	int* alpha = new int[Count];

	int i;
	for(i=0; i<Count; i++)
	{
		osg::Vec2 pt;
		pt.y() = (*m_boundVertexArray)[i].y() - rtPoint.y();
		pt.x() = (*m_boundVertexArray)[i].x() - rtPoint.x();
		if(pt.y() >= 0 && pt.x() >= 0) alpha[i] = 0;
		if(pt.y() >= 0 && pt.x() < 0) alpha[i] = 3;
		if(pt.y() <= 0 && pt.x() > 0) alpha[i] = 1;
		if(pt.y() < 0 && pt.x() <=0) alpha[i] = 2;
	}

	int total = 0;
	for(i=1; i<Count; i++)
	{
		int delta = alpha[i] - alpha[i-1];
		if(delta >= -1 && delta <=1) total+=delta;
		if(delta > 2) total += delta - 4;
		if(delta < -2) total += delta + 4;
		if(delta == 2 || delta == -2)
		{
			osg::Vec2 pt, pt1, pt2, pt3, pt4;
			pt1.y() = (*m_boundVertexArray)[i].y() - rtPoint.y();
			pt1.x() = (*m_boundVertexArray)[i].x() - rtPoint.x();
			pt2.y() = (*m_boundVertexArray)[i-1].y() - rtPoint.y();
			pt2.x() = (*m_boundVertexArray)[i-1].x() - rtPoint.x();
			double lamta;
			bool bNeedPart = true;
			pt3.y() = 0;
			if(pt2.y() != 0)
			{
				lamta =-double(pt1.y()/pt2.y());
				pt3.x() = (pt1.x() + lamta * pt2.x()) /  (1.0 + lamta);
			}
			else if(pt1.y() != 0)
			{
				lamta =-double(pt2.y()/pt1.y());
				pt3.x() = (pt2.x() + lamta * pt1.x()) /  (1.0 + lamta);
			}
			else bNeedPart = false;

			pt4.x() = 0;
			if(pt2.x() != 0)
			{
				lamta =-double(pt1.x() / pt2.x());
				pt4.y() = (pt1.y() + lamta* pt2.y()) / (1.0+lamta);
			}
			else if(pt1.x() !=0)
			{
				lamta =- double(pt2.x() / pt1.x());
				pt4.y() = (pt2.y() + lamta * pt1.y()) / (1+lamta);
			}
			else bNeedPart = false;

			if(!bNeedPart) total += delta;
			else
			{
				pt3.y() = (pt3.y() + pt4.y()) / 2;
				pt3.x() = (pt3.x() + pt4.x()) / 2;
				int talpha = 0;
				if(pt3.y()>=0 && pt3.x() >=0)  talpha = 0;
				if(pt3.y()>=0 &&pt3.x() < 0)  talpha = 3;
				if(pt3.x() <=0 && pt3.x() > 0) talpha = 1;
				if(pt3.y()<0 && pt3.x() <=0)  talpha = 2;
				int delta1 = 0;
				delta1 = alpha[i]-talpha;
				if(delta1 >= -1 && delta1 <= 1) total += delta1;
				if(delta1 > 2) total += delta1 - 4;
				if(delta1 < -2) total += delta1 + 4;
				delta1 = talpha - alpha[i-1];
				if(delta1 >= -1 && delta1 <=1) total += delta1;
				if(delta1 > 2) total += delta1 - 4;
				if(delta1 < -2) total += delta1 + 4;
			}
		}
	}
	delete[]alpha;
	return !(!total);
}


bool KLGeoPolySelectExtents::isContain(const osg::Vec2& rtPoint)
{
	int Count = m_boundVertexArray->size();
	if(Count < 3) return false;
	int* alpha = new int[Count];

	int i;
	for(i=0; i<Count; i++)
	{
		osg::Vec2 pt;
		pt.y() = (*m_boundVertexArray)[i].y() - rtPoint.y();
		pt.x() = (*m_boundVertexArray)[i].x() - rtPoint.x();
		if(pt.y() >= 0 && pt.x() >= 0) alpha[i] = 0;
		if(pt.y() >= 0 && pt.x() < 0) alpha[i] = 3;
		if(pt.y() <= 0 && pt.x() > 0) alpha[i] = 1;
		if(pt.y() < 0 && pt.x() <=0) alpha[i] = 2;
	}

	int total = 0;
	for(i=1; i<Count; i++)
	{
		int delta = alpha[i] - alpha[i-1];
		if(delta >= -1 && delta <=1) total+=delta;
		if(delta > 2) total += delta - 4;
		if(delta < -2) total += delta + 4;
		if(delta == 2 || delta == -2)
		{
			osg::Vec2 pt, pt1, pt2, pt3, pt4;
			pt1.y() = (*m_boundVertexArray)[i].y() - rtPoint.y();
			pt1.x() = (*m_boundVertexArray)[i].x() - rtPoint.x();
			pt2.y() = (*m_boundVertexArray)[i-1].y() - rtPoint.y();
			pt2.x() = (*m_boundVertexArray)[i-1].x() - rtPoint.x();
			double lamta;
			bool bNeedPart = true;
			pt3.y() = 0;
			if(pt2.y() != 0)
			{
				lamta =-double(pt1.y()/pt2.y());
				pt3.x() = (pt1.x() + lamta * pt2.x()) /  (1.0 + lamta);
			}
			else if(pt1.y() != 0)
			{
				lamta =-double(pt2.y()/pt1.y());
				pt3.x() = (pt2.x() + lamta * pt1.x()) /  (1.0 + lamta);
			}
			else bNeedPart = false;

			pt4.x() = 0;
			if(pt2.x() != 0)
			{
				lamta =-double(pt1.x() / pt2.x());
				pt4.y() = (pt1.y() + lamta* pt2.y()) / (1.0+lamta);
			}
			else if(pt1.x() !=0)
			{
				lamta =- double(pt2.x() / pt1.x());
				pt4.y() = (pt2.y() + lamta * pt1.y()) / (1+lamta);
			}
			else bNeedPart = false;

			if(!bNeedPart) total += delta;
			else
			{
				pt3.y() = (pt3.y() + pt4.y()) / 2;
				pt3.x() = (pt3.x() + pt4.x()) / 2;
				int talpha;
				if(pt3.y()>=0 && pt3.x() >=0)  talpha = 0;
				if(pt3.y()>=0 &&pt3.x() < 0)  talpha = 3;
				if(pt3.x() <=0 && pt3.x() > 0) talpha = 1;
				if(pt3.y()<0 && pt3.x() <=0)  talpha = 2;
				int delta1;
				delta1 = alpha[i]-talpha;
				if(delta1 >= -1 && delta1 <= 1) total += delta1;
				if(delta1 > 2) total += delta1 - 4;
				if(delta1 < -2) total += delta1 + 4;
				delta1 = talpha - alpha[i-1];
				if(delta1 >= -1 && delta1 <=1) total += delta1;
				if(delta1 > 2) total += delta1 - 4;
				if(delta1 < -2) total += delta1 + 4;
			}
		}
	}
	delete[]alpha;
	return !(!total);
}*/
