#pragma once

#include <string>

#include <osgViewer/CompositeViewer>

#include "VRPNClientManager.h"
#include "vrpnClient.h"
#include "StreamingManager.h"

class CCaveViewer :	public osgViewer::CompositeViewer
{
public:
	enum CaveFaceId
	{
		TABLET=-1,
		CAVE_CENTER,
		CAVE_RIGHT,
		CAVE_LEFT,
		CAVE_BOTTOM,

	};

	CCaveViewer();
	virtual ~CCaveViewer();

	void setCaveFaceId(CaveFaceId id);
	void setEyeDistance(double dist);
	void setViewportShift(unsigned int dist);
	void setStartEye(const osg::Vec3& eye);
	void setCaveDimensions(double width, double height, double depth);
	void setGroundDimensions(double top, double bottom);
	void setSceneScale(double x, double y, double z);
	void setFullscreen(bool fullscreen);
	void setStereo(bool stereo);
	void setDebug(bool debug);
	void setWindowName(const std::string& windowName);
	void init();
	void setScene(osg::Node* scene);
	void setTabletIp(std::string ip);
	void addPlane();
	virtual void frame(double simulationTime = USE_REFERENCE_TIME);
	StreamingManager sManager;

private:
	void computeRoomPosition();
	osg::Vec2 m_windowSize;
	unsigned int m_shiftWidth;
	
	double m_caveWidth;
	double m_caveHeight;
	double m_caveDepth;
	double m_caveGroundTop;
	double m_caveGroundBottom;
	osg::Vec3 m_sceneScale;
	bool m_fullscreen;
	std::string m_windowName;
	const osg::Quat q_RT;											// Tracker to Room
	osg::Quat q_TH;											// Head to Tracker
	osg::Vec3 v_WR;											// Room position in World CS
	osg::Quat q_WR;											// Room attitude in world CS
	std::string tabletIp;
	double m_eyeDist;


	osg::Vec3 m_startEye;
	
	osg::ref_ptr<osg::Camera> m_leftCamera;
	osg::ref_ptr<osg::Camera> m_rightCamera;

	osgViewer::View* m_leftView;
	osgViewer::View* m_rightView;

	CaveFaceId m_caveFaceId;

	CVRPNClientManager m_VRPNClientManager;
	CVRPNClientManager m_VRPNClientManagerAndroid;
	CVRPNClientManager m_VRPNClientDTrack;

	VrpnClient m_vrpnClient;

	osg::Image *shot;
	
	int consecutivo;
	osg::ref_ptr<osgDB::ReaderWriter> writer;
	osg::ref_ptr<osgDB::ReaderWriter::Options> options;
};

inline void CCaveViewer::setEyeDistance(double dist)
{
	m_eyeDist = dist;
}

inline void CCaveViewer::setViewportShift(unsigned int dist)
{
	m_shiftWidth = dist;
}

inline void CCaveViewer::setStartEye(const osg::Vec3& eye)
{
	m_startEye = eye;
}

inline void CCaveViewer::setCaveDimensions(double width, double height, double depth)
{
	m_caveWidth = width;
	m_caveHeight = height;
	m_caveDepth = depth;
}

inline void CCaveViewer::setGroundDimensions(double top, double bottom)
{
	m_caveGroundTop = top;
	m_caveGroundBottom = bottom;
}

inline void CCaveViewer::setSceneScale(double x, double y, double z)
{
	m_sceneScale = osg::Vec3(x, y, z);
}

inline void CCaveViewer::setCaveFaceId(CaveFaceId id)
{
	m_caveFaceId = id;
}

inline void CCaveViewer::setFullscreen(bool fullscreen) {
	m_fullscreen = fullscreen;
}

inline void CCaveViewer::setWindowName(const std::string& windowName) {
	m_windowName = windowName;
}

inline void CCaveViewer::setTabletIp(std::string ip){
	
	tabletIp=ip;
	std::cout << "Tablet IP: "<<tabletIp;
}