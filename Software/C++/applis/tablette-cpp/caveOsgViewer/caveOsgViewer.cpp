// caveOsgViewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "CaveViewer.h"

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{




	std::string scenePath,tabletIp;
	CCaveViewer::CaveFaceId caveFaceId = CCaveViewer::CAVE_CENTER;
	double eyeDistance = 0.065;
	int viewportShift = 60;
	osg::Vec3 startEye(0,1.8,0);
	double groundTop, groundBottom;
	osg::Vec3 caveDimension(0,1,0);
	osg::Vec3 sceneScale(1,1,1);
	double caveWidth = 3;
	double caveHeight = 3;
	double caveDepth = 3;
	bool fullscreen = true;


	// read config file
	char* configFilename = "moveConfig.txt";
	if (argc > 1) {
		configFilename = argv[1];
	}
	
	std::ifstream stream;
	stream.open(configFilename);
	std::string tmp;	
	do
	{
		stream >> tmp;

		if(tmp == "scene")
			stream >> scenePath;
		else if(tmp == "face")
		{
			int i;
			stream >> i;
			caveFaceId = static_cast<CCaveViewer::CaveFaceId>(i);		
		}
		else if(tmp == "eyeDistance")
			stream >> eyeDistance;
		else if(tmp == "viewportShift")
			stream >> viewportShift;
		else if(tmp == "startEye")
		{
			float x, y, z;
			stream >> x >> y >> z;
			startEye = osg::Vec3(x, y, z);
		}
		else if(tmp == "sceneScale")
		{
			float x, y, z;
			stream >> x >> y >> z;
			sceneScale = osg::Vec3(x, y, z);
		}
		else if(tmp == "groundDimension")
		{
//			float x, y, z;
			stream >> groundTop >> groundBottom;
		}
		else if(tmp == "fullscreen") {
			int doFullscreen;
			stream >> doFullscreen;
			fullscreen = (doFullscreen!= 0);
		}
		else if(tmp == "caveDimension"){
			stream >> caveWidth >> caveHeight >> caveDepth;
		}
		else if(tmp=="tabletIp"){
			stream >> tabletIp;
	

		}
	}
		while(!stream.eof());
	stream.close();
	
	
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile(scenePath);

	// create viewer
	CCaveViewer viewer;
	viewer.setCaveFaceId(caveFaceId);
	viewer.setEyeDistance(eyeDistance);
	viewer.setStartEye(startEye);
//	viewer.setVirtualEye(groundDimension);
	viewer.setViewportShift(viewportShift);
	viewer.setCaveDimensions(caveWidth, caveHeight, caveDepth);
	viewer.setGroundDimensions(groundTop, groundBottom);
	viewer.setSceneScale(sceneScale.x(), sceneScale.y(), sceneScale.z());
	viewer.setFullscreen(fullscreen);
	viewer.setWindowName(configFilename);
	viewer.setTabletIp(tabletIp);
	viewer.init();
	viewer.setScene(scene.get());

	viewer.realize();

	while(!viewer.done())
	{
	
		viewer.frame();		
	}

	return 0;
}

