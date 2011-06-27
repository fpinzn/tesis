#pragma once

#include "StdAfx.h"

/**
 * A drawable node with a color sphere in each corner of the cave
 *
 * For debugging purposes
 */
class CaveNode 
{
public:
	CaveNode();
	~CaveNode(void);
	void init(double width, double depth, double height);
	osg::ref_ptr<osg::PositionAttitudeTransform> getNode();
private:
	osg::ref_ptr<osg::PositionAttitudeTransform> _node;
};


