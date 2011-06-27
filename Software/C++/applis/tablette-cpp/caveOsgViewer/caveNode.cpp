#include "StdAfx.h"

#include "caveNode.h"

CaveNode::CaveNode() :
	_node(0)
{
}

CaveNode::~CaveNode(void)
{
}

void CaveNode::init(double width, double depth, double height)
{
	//_node = new osg::Group();
	
	_node = new osg::PositionAttitudeTransform;
	// besause the main program rotates the scene
	_node->setAttitude(osg::Quat(osg::PI_2, osg::Vec3(1,0,0)));
	//_node->addChild(mainPat);
	osg::StateSet* stateSet = _node->getOrCreateStateSet();
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	const int N_CORNERS = 8;
	osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0, 0, 0), 0.2);
	osg::ref_ptr<osg::Geode> geodes[N_CORNERS];
	osg::ref_ptr<osg::PositionAttitudeTransform> pat[N_CORNERS];
	osg::ref_ptr<osg::ShapeDrawable> sphereSDs[N_CORNERS];
	for (int i = 0; i < N_CORNERS; i++) {
		geodes[i] = new osg::Geode();
		pat[i] = new osg::PositionAttitudeTransform();
		sphereSDs[i] = new osg::ShapeDrawable(sphere);
		geodes[i]->addDrawable(sphereSDs[i]);
		pat[i]->addChild(geodes[i]);
		_node->addChild(pat[i]);
	}

	const osg::Vec4 RED =		osg::Vec4(1, 0, 0, 1);
	const osg::Vec4 BLUE =		osg::Vec4(0, 0, 1, 1);
	const osg::Vec4 GREEN =		osg::Vec4(0, 1, 0, 1);
	const osg::Vec4 CYAN =		osg::Vec4(0, 1, 1, 1);
	const osg::Vec4 MAGENTA =	osg::Vec4(1, 0, 1, 1);
	const osg::Vec4 YELLOW =	osg::Vec4(1, 1, 0, 1);
	const osg::Vec4 BLACK =		osg::Vec4(0, 0, 0, 1);
	const osg::Vec4 WHITE =		osg::Vec4(1, 1, 1, 1);

	enum{
		FRONT_TOP_LEFT,
		FRONT_TOP_RIGHT,
		FRONT_BOTTOM_LEFT,
		FRONT_BOTTOM_RIGHT,
		REAR_TOP_LEFT,
		REAR_TOP_RIGHT,
		REAR_BOTTOM_LEFT,
		REAR_BOTTOM_RIGHT,
	};


	pat[FRONT_TOP_LEFT]->setPosition(osg::Vec3(-width/2.0, height, -depth/2.0));
	sphereSDs[FRONT_TOP_LEFT]->setColor(GREEN);

	pat[FRONT_BOTTOM_LEFT]->setPosition(osg::Vec3(-width/2.0, 0, -depth/2.0));
	sphereSDs[FRONT_BOTTOM_LEFT]->setColor(BLACK);

	pat[FRONT_TOP_RIGHT]->setPosition(osg::Vec3(width/2.0, height, -depth/2.0));
	sphereSDs[FRONT_TOP_RIGHT]->setColor(YELLOW);

	pat[FRONT_BOTTOM_RIGHT]->setPosition(osg::Vec3(width/2.0, 0, -depth/2.0));
	sphereSDs[FRONT_BOTTOM_RIGHT]->setColor(RED);

	pat[REAR_TOP_LEFT]->setPosition(osg::Vec3(-width/2.0, height, depth/2.0));
	sphereSDs[REAR_TOP_LEFT]->setColor(CYAN);

	pat[REAR_BOTTOM_LEFT]->setPosition(osg::Vec3(-width/2.0, 0, depth/2.0));
	sphereSDs[REAR_BOTTOM_LEFT]->setColor(BLUE);

	pat[REAR_TOP_RIGHT]->setPosition(osg::Vec3(width/2.0, height, depth/2.0));
	sphereSDs[REAR_TOP_RIGHT]->setColor(WHITE);

	pat[REAR_BOTTOM_RIGHT]->setPosition(osg::Vec3(width/2.0, 0, depth/2.0));
	sphereSDs[REAR_BOTTOM_RIGHT]->setColor(MAGENTA);

}


osg::ref_ptr<osg::PositionAttitudeTransform> CaveNode::getNode() {
	return _node;
}