#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

struct Annotation{
	std::string id;
	std::string author;
	std::string content;
	std::string date;
	//polygons
	//pov
	std::string priority;
	osg::Vec3 v_WR;
	osg::Quat q_WR;
	osg::Vec3 v_tablet;
	osg::Quat q_tablet;

};