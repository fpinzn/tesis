
#include "stdafx.h"

#include "caveViewer.h"
#include "vrpn_Tracker_Android.h"
#include "utilities.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

#include <winsock2.h>
#include <stdio.h>
#include <math.h>
static const osg::Vec3 X_pos = osg::Vec3(1, 0, 0);
static const osg::Vec3 X_neg = osg::Vec3(-1, 0, 0);
static const osg::Vec3 Y_pos = osg::Vec3(0, 1, 0);
static const osg::Vec3 Y_neg = osg::Vec3(0, -1, 0);
static const osg::Vec3 Z_pos = osg::Vec3(0, 0, 1);
static const osg::Vec3 Z_neg = osg::Vec3(0, 0, -1);

//VRPN CODES
//navigation screen
static const int VRPN_TRANSLATION_JOYSTICK=299;
static const int VRPN_ROTATION_JOYSTICK=298;
static const int VRPN_TAP_SCREEN=297;

//DTrack Codes
static const int VRPN_DTRACK_TABLET_ID=1;
static const int VRPN_DTRACK_CYCLOPS_ID=0;
//annotation screen
static const int VRPN_GOTO_ANNOTATION=399;
static const int VRPN_REMOVE_ANNOTATION=398;
static const int VRPN_NEW_ANNOTATION=397;
static const int VRPN_CANCEL_SELECT_POLYGONS=396;

// Buttons
static const int VrpnButtonRotateLeft = 10;
static const int VrpnButtonRotateRight = 11;
static const int VrpnButtonMoveRelativeToRoom = 12;
static const int VrpnButtonUsePitchAsForward = 13;

// Vrpn Analogs for the Android device
static const int VrpnTranslationSpeed = 96;
static const int VrpnInterEyeDistance = 97;

//Streaming
const int STREAMING_SERVER_PORT=8888;

//Annotating
bool selectingPolygons=false;
double lastTapX=0;
double lastTapY=0;
std::vector<Annotation> annotations;
//TABLET SIZE
int TABLET_HEIGHT=850;
int TABLET_WIDTH=600;

//Tablet tracking info
osg::Vec3 v_tablet;
osg::Quat q_tablet;

#define PI 3.14159265

osg::ref_ptr<osg::Group> root;

std::ostream& operator<<(std::ostream& o, const osg::Vec3& v) {
	o << v[0] << " " << v[1] << " " << v[2];
	return o;
}
//-------------------------------------------------------------------------------------------------------

static float ClampUnity(float x)
{
   if (x >  1.0f) { return  1.0f; }
   if (x < -1.0f) { return -1.0f; }
   return x;
}

static void HprToMatrix( osg::Matrix& rotation, const osg::Vec3& hpr )
{
   // implementation converted from plib's sg.cxx
   // PLIB - A Suite of Portable Game Libraries
   // Copyright (C) 1998,2002  Steve Baker
   // For further information visit http://plib.sourceforge.net


   double ch, sh, cp, sp, cr, sr, srsp, crsp, srcp;

   // this can't be smart for both 32 and 64 bit types.
   ///\todo find a preprocessor way to assign this constant different for the different precision types.
   const osg::Vec3::value_type magic_epsilon = (osg::Vec3::value_type)0.00001;

   if (osg::equivalent(hpr[0], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      ch = 1.0;
      sh = 0.0;
   }
   else
   {
      sh = sinf(osg::DegreesToRadians(hpr[0]));
      ch = cosf(osg::DegreesToRadians(hpr[0]));
   }

   if (osg::equivalent(hpr[1], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      cp = 1.0;
      sp = 0.0;
   }
   else
   {
      sp = sinf(osg::DegreesToRadians(hpr[1]));
      cp = cosf(osg::DegreesToRadians(hpr[1]));
   }

   if (osg::equivalent(hpr[2], (osg::Vec3::value_type)0.0, magic_epsilon))
   {
      cr   = 1.0;
      sr   = 0.0;
      srsp = 0.0;
      srcp = 0.0;
      crsp = sp;
   }
   else
   {
      sr   = sinf(osg::DegreesToRadians(hpr[2]));
      cr   = cosf(osg::DegreesToRadians(hpr[2]));
      srsp = sr * sp;
      crsp = cr * sp;
      srcp = sr * cp;
  } 

   rotation(0, 0) =  ch * cr - sh * srsp;
   rotation(1, 0) = -sh * cp;
   rotation(2, 0) =  sr * ch + sh * crsp;

   rotation(0, 1) =  cr * sh + srsp * ch;
   rotation(1, 1) =  ch * cp;
   rotation(2, 1) =  sr * sh - crsp * ch;

   rotation(0, 2) = -srcp;
   rotation(1, 2) =  sp;
   rotation(2, 2) =  cr * cp;

   rotation(3, 0) =  0.0;  // x trans
   rotation(3, 1) =  0.0;  // y trans
   rotation(3, 2) =  0.0;  // z trans

   rotation(0, 3) =  0.0;
   rotation(1, 3) =  0.0;
   rotation(2, 3) =  0.0;
   rotation(3, 3) =  1.0;
}

static void MatrixToHpr( osg::Vec3& hpr, const osg::Matrix& rotation )
{
   // implementation converted from plib's sg.cxx
   // PLIB - A Suite of Portable Game Libraries
   // Copyright (C) 1998,2002  Steve Baker
   // For further information visit http://plib.sourceforge.net

   osg::Matrix mat;

   osg::Vec3 col1(rotation(0, 0), rotation(0, 1), rotation(0, 2));
   double s = col1.length();

   const double magic_epsilon = 0.00001;
   if (s <= magic_epsilon)
   {
      hpr.set(0.0f, 0.0f, 0.0f);
      return;
   }


   double oneOverS = 1.0f / s;
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         mat(i, j) = rotation(i, j) * oneOverS;
      }
   }


   double sin_pitch = ClampUnity(mat(1, 2));
   double pitch = asin(sin_pitch);
   hpr[1] = osg::RadiansToDegrees(pitch);

   double cp = cos(pitch);

   if (cp > -magic_epsilon && cp < magic_epsilon)
   {
      double cr = ClampUnity(-mat(2,1));
      double sr = ClampUnity(mat(0,1));

      hpr[0] = 0.0f;
      hpr[2] = osg::RadiansToDegrees(atan2(sr,cr));
   }
   else
   {
      double one_over_cp = 1.0 / cp;
      double sr = ClampUnity(-mat(0,2) * one_over_cp);
      double cr = ClampUnity( mat(2,2) * one_over_cp);
      double sh = ClampUnity(-mat(1,0) * one_over_cp);
      double ch = ClampUnity( mat(1,1) * one_over_cp); 

      if ((osg::equivalent(sh,0.0,magic_epsilon) && osg::equivalent(ch,0.0,magic_epsilon)) ||
          (osg::equivalent(sr,0.0,magic_epsilon) && osg::equivalent(cr,0.0,magic_epsilon)) )
      {
         cr = ClampUnity(-mat(2,1));
         sr = ClampUnity(mat(0,1));;

         hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = osg::RadiansToDegrees(atan2(sh, ch));
      }

      hpr[2] = osg::RadiansToDegrees(atan2(sr, cr));
   }
}

//-------------------------------------------------------------------------------------------------------

CCaveViewer::CCaveViewer() :
	/*
	 * Tracker base in room CS.
	 * No translation : both originate on the floor approminately in the middle of the bottom face, 
	 * which is not square
	 */
	q_RT(osg::Quat(-osg::PI_2, X_pos))
{
	m_leftView = NULL;
	m_rightView = NULL;

	m_eyeDist = 0.065;
	m_shiftWidth = 60;
	m_caveFaceId = CAVE_CENTER;
	m_startEye = osg::Vec3(0, 0, 0);
	m_caveWidth = m_caveDepth = 3.0;
	m_caveHeight = 2.7;
	m_caveGroundTop = m_caveGroundBottom = 3.0;
	m_sceneScale = osg::Vec3(1, 1, 1);
	m_fullscreen = true;
	m_windowName = "";
	
	/*
	 * Initial Room postion in World CS
	 */
	v_WR = osg::Vec3(0, 0, 0);									
	q_WR = osg::Quat(0, osg::Vec3(0, 1, 0));


}

CCaveViewer::~CCaveViewer()
{
}

void CCaveViewer::init()
{
	sManager.init(tabletIp.c_str());

	// set window parameters
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if(!wsi)
		return;
	unsigned int windowWidth, windowHeight;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), windowWidth, windowHeight);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	if(m_caveFaceId==TABLET){
		traits->width = 600;
		traits->height = 850;
		//traits->pbuffer = true;
		shot = new osg::Image;
		shot->allocateImage(600, 850, 24, GL_RGB, GL_UNSIGNED_BYTE);
	}
	else{
		traits->width = windowWidth;
		traits->height = windowHeight;
	}

	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	const int height = 300;
	const int width = height;
	/*
	 * If not in fullscreen mode, 
	 * display the face in un undecorated window layed out like this :
	 *						L F R
	 *						  B
	 */
	if (! m_fullscreen) {

		const int baseX = 50;					// Top left corner of left face
		const int baseY = 50;
		switch(m_caveFaceId) {
			case CAVE_LEFT : {
				traits->x = baseX;
				traits->y = baseY;
				break;
			}
			case CAVE_CENTER : {
				traits->x = baseX + width;
				traits->y = baseY;
				break;
			}
			case CAVE_RIGHT : {
				traits->x = baseX + 2 * width;
				traits->y = baseY;
				break;
			}
			case CAVE_BOTTOM : {
				traits->x = baseX + width;
				traits->y = baseY + height;
				break;
			}
		}
		traits->windowDecoration = false;
		traits->windowName = m_windowName;
		windowHeight = height;
		// square viewports
		windowWidth = 2 * (windowHeight + 2 * m_shiftWidth);

		traits->width = windowWidth;
		traits->height = windowHeight;
	}
// graphic context
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());


 

	if(!gc.valid())
		return;
	gc->setClearColor(osg::Vec4f(0.0f,0.0f,0.0f,1.0f));
	gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// manipulator
	//osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator;
// set viewer parameters
		m_windowSize = osg::Vec2(windowWidth, windowHeight);
		unsigned int viewportWidth = windowWidth / 2;

	if(m_caveFaceId==TABLET){
	// set viewer parameters
		m_windowSize = osg::Vec2(windowWidth, windowHeight);
		//unsigned int viewportWidth = windowWidth / 2;


		// left view
		m_leftView = new osgViewer::View;
		m_leftView->setName("left view");
		//m_leftView->setCameraManipulator(manipulator);
		addView(m_leftView);
		
		m_leftCamera = m_leftView->getCamera();

		//m_leftCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		m_leftCamera->attach(osg::Camera::COLOR_BUFFER, shot);

		m_leftCamera->setName("left cam");
		m_leftCamera->setViewport(new osg::Viewport(0, 0, TABLET_WIDTH, TABLET_HEIGHT));
		m_leftCamera->setGraphicsContext(gc.get());
   
	
	

		consecutivo=0;
		writer = osgDB::Registry::instance()->getReaderWriterForExtension("jpg"); 
		if( !writer.valid() ) 
		std::cout<<"Invalid writer";
		options = new osgDB::ReaderWriter::Options(); 
		options->setOptionString("JPEG_QUALITY 95"); 
	}
	else{
		


		// left view
		m_leftView = new osgViewer::View;
		m_leftView->setName("left view");
		//m_leftView->setCameraManipulator(manipulator);
		addView(m_leftView);
		
		m_leftCamera = m_leftView->getCamera();
		m_leftCamera->setName("left cam");
		m_leftCamera->setViewport(new osg::Viewport(m_shiftWidth, 0, viewportWidth - 2 * m_shiftWidth, windowHeight));
		m_leftCamera->setGraphicsContext(gc.get());

		// right view
		m_rightView = new osgViewer::View;
		m_rightView->setName("right view");
		//m_rightView->setCameraManipulator(manipulator);

		addView(m_rightView);
		
		m_rightCamera = m_rightView->getCamera();
		m_rightCamera->setName("right cam");
		m_rightCamera->setViewport(new osg::Viewport(viewportWidth + m_shiftWidth, 0, viewportWidth - 2 * m_shiftWidth, windowHeight));
		m_rightCamera->setGraphicsContext(gc.get());
	}

	

	// init vrpn
const int TRACKER_ID = 2;
//#define MOVE
#ifdef MOVE
	m_VRPNClientManager.init("DTrack@192.168.1.10");
	
	std::cout<<"Connected: "<<m_vrpnClient.init(TRACKER_ID,"127.0.0.1", 8888)<<"\n";
	m_VRPNClientManagerAndroid.init("GGG@127.0.0.1");
	std::cout<<"\n MOVE";
#else
	/*tricks the VRPN to make it believe its the Tablet to update the positions
	of the other views*/
	std::cout<<"Connected: "<<m_vrpnClient.init(TRACKER_ID,"193.48.141.126", 6666)<<"\n";
	//the Android App running in the tablet
	m_VRPNClientManagerAndroid.init("GGG@193.48.141.126");
	//DTrack system
	m_VRPNClientDTrack.init("DTrack@193.48.141.126");
#endif



}

void CCaveViewer::setScene(osg::Node* scene)
{



	if((m_leftView && m_rightView) || (m_leftView && m_caveFaceId==TABLET))
	{
//#define SEB_WAY
#ifdef SEB_WAY
		osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
		pat->setAttitude(osg::Quat(-osg::PI_2, osg::Vec3(1,0,0)));
		//pat->setPosition(osg::Vec3(0, -1, 0));
		
		osg::Matrix m;
		m.makeScale(m_sceneScale);
		osg::ref_ptr<osg::MatrixTransform> tr = new osg::MatrixTransform;
		tr->setMatrix(m);

		pat->addChild(tr);
		tr->addChild(scene);	
#else
		root = new osg::Group;
		// Keep this as a member and update according to tracker data
		osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
		osg::Quat baseQ = osg::Quat(-osg::PI_2, osg::Vec3(1,0,0));

		pat->setAttitude( baseQ * (-q_WR) );
		// Normal scaling required see http://www.vis-sim.com/osg/osg_faq_1.htm#f19
		//pat->setScale(m_sceneScale);
		// Translate la scène

		pat->addChild(scene);
		//tr->addChild(scene);
		root->addChild(pat);
	
#endif

		m_leftView->setSceneData(root);
		if(m_caveFaceId!=TABLET)m_rightView->setSceneData(root);


		// add light
		osg::Light *light = new osg::Light();
		// each light must have a unique number
		light->setLightNum(0);
		// we set the light's position via a PositionAttitudeTransform object
		light->setPosition(osg::Vec4(0.0, 2.0, 0.0, 1.0));
		light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
		light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
		light->setAmbient(osg::Vec4(0.5, 0.5, 0.5, 1.0));

		osg::StateSet* rootStateSet = pat->getOrCreateStateSet();
		rootStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		rootStateSet->setMode(GL_LIGHT0, osg::StateAttribute::ON);

		osg::Material* mat = new osg::Material;
		//mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1,1,1,1));
		//mat->setAmbient(osg::Material::FRONT, osg::Vec4(0.2,0.2,0.2,1));
		mat->setSpecular(osg::Material::FRONT, osg::Vec4(1,1,1,1));
		mat->setShininess(osg::Material::FRONT, 95.0f);
		mat->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
		rootStateSet->setAttribute(mat, osg::StateAttribute::ON);

		osg::LightSource* lightSource = new osg::LightSource();
		lightSource->setLight(light);
		lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
		lightSource->setStateSetModes(*rootStateSet, osg::StateAttribute::ON);
		pat->addChild(lightSource);

	}
}

/*
 * Update the room postion and attitude according to the tracker that delivers incremental updates
 */
void CCaveViewer::computeRoomPosition() {
	/*
	 * Get Android tracker : defines the room position and the simulated head position
	 */
	if  (m_VRPNClientManagerAndroid.getTrackerCount())
	{
		/*
		 * This part is just for one ""master"" that computes the absolute room position
		 */

		CVRPNClientManager::Tracker* pGoToannotation = m_VRPNClientManagerAndroid.getTrackerFromId(VRPN_GOTO_ANNOTATION);
		if(pGoToannotation){
			
		
		}

		CVRPNClientManager::Tracker* pPositionTracker = m_VRPNClientManagerAndroid.getTrackerFromId(VRPN_TRANSLATION_JOYSTICK);
		CVRPNClientManager::Tracker* pRotationTracker = m_VRPNClientManagerAndroid.getTrackerFromId(VRPN_ROTATION_JOYSTICK);

	

			/*
			 * Compute the room rotation around the vertical axis (Z_W) according to Vrpn buttons
			 */
			// TODO : add roll
			static float angularSpeed = 0.0;
			const float maxAngularSpeed = 0.4f;										// degrees per frame
			const float framesToMaxSpeed = 3 * 60;									// frames to reach max speed (3 seconds)
			const float angularAcceleration = maxAngularSpeed / framesToMaxSpeed;
			bool doRotateLeft = false;
			bool doRotateRight= false;
			m_VRPNClientManagerAndroid.getButtonFromId(VrpnButtonRotateLeft, doRotateLeft);
			m_VRPNClientManagerAndroid.getButtonFromId(VrpnButtonRotateRight, doRotateRight);



			if (pRotationTracker) 
			{
			q_WR = osg::Quat(osg::DegreesToRadians(pRotationTracker->crtQuat.x()), Y_pos) * q_WR;
			q_WR = osg::Quat(osg::DegreesToRadians(pRotationTracker->crtQuat.x()), Z_pos) * q_WR;
			q_WR = osg::Quat(osg::DegreesToRadians(pRotationTracker->crtQuat.z()), X_pos) * q_WR;
		
			}
			/*if ((doRotateLeft) && (angularSpeed < maxAngularSpeed)) {
					angularSpeed += angularAcceleration;
					q_WR = osg::Quat(osg::DegreesToRadians(angularSpeed), Y_pos) * q_WR;
			} else if ((doRotateRight) && (angularSpeed > -maxAngularSpeed)) {
					angularSpeed -= angularAcceleration;
					q_WR = osg::Quat(osg::DegreesToRadians(angularSpeed), Y_pos) * q_WR;
			} else {
				if (abs(angularSpeed) <= 2.0 * angularAcceleration) {
					angularSpeed = 0.0f;
				} else  {
					if (angularSpeed < 0.0f) {
						angularSpeed +=  angularAcceleration;
					} else {
						angularSpeed -=  angularAcceleration;
					}
					q_WR = osg::Quat(osg::DegreesToRadians(angularSpeed), Y_pos) * q_WR;
				}
			}*/

			/*
			 * Compute translation vectors for the room, in World CS
			 
			
			 */
			if (pPositionTracker) 
			{
			double speed = 0.0;						// Scene Units / sec
			speed = 5.0;//m_VRPNClientManagerAndroid.getAnalogFromId(VrpnTranslationSpeed);

			// Room translation can be aligned on Room CS axis (doMoveRelativeToRoom)
			// or on Head XY
			// TODO constrain on horizontal movements
			bool doMoveRelativeToRoom = false;
			//m_VRPNClientManagerAndroid.getButtonFromId(VrpnButtonMoveRelativeToRoom, doMoveRelativeToRoom);
			
			// Does pitch move up/down or forward/backward ?
			bool moveForward = true;
			//m_VRPNClientManagerAndroid.getButtonFromId(VrpnButtonUsePitchAsForward, moveForward);

			// Convert pitch & roll into speed in range 0..1
			double xSpeed = 0;
			double ySpeed = 0;
			double zSpeed=0;
	
			

	
			//Utilities::quaternionToSpeed(pPositionTracker->crtQuat, xSpeed, ySpeed);
			xSpeed=pPositionTracker->crtQuat.x();
			ySpeed=pPositionTracker->crtQuat.y();
			zSpeed=pPositionTracker->crtQuat.z();
		
			//		BEWARE :(q2 * q1) * v != q2 * (q1 * v)
			//		see http://article.gmane.org/gmane.comp.graphics.openscenegraph.user/43425
			osg::Vec3 dirY_W;										// Front/back direction in World CS
			osg::Vec3 dirX_W;	
			osg::Vec3 dirZ_W;	
	
				/*
				 * Move relative to view 
				 */
				// lateral room translation along +- X_H
				dirX_W = (q_WR * (q_RT * (q_TH * X_neg)));
				dirZ_W = (q_WR * Y_pos);
				if (!moveForward) {
					dirY_W = Y_pos;
				} else {
					// Longitudinal room translation along view vector (-Y_H)
					dirY_W = (q_WR * (q_RT * (q_TH * Y_neg)));
				}
			

			// Update the room position
			const double assumedFps = 60.0;
			double baseSpeed = speed / assumedFps;
			v_WR += dirY_W * (ySpeed * baseSpeed) ;
			v_WR += dirX_W * (-xSpeed * baseSpeed);
			v_WR += dirZ_W * (zSpeed * baseSpeed);


		} // if (pPositionTracker)
	} // if  (m_VRPNClientManagerAndroid.getTrackerCount())
} // computeRoomPosition

void CCaveViewer::frame(double simulationTime)
{


	// run vrpn
	m_VRPNClientManager.run();
	m_VRPNClientManagerAndroid.run();

	/*
	 * The coordinate systems (CS) are :
	 *
	 *						X				Y				Z
	 * Head	CS				right			forward			up
	 * Tracker CS			right			forward			up
	 * Room	CS				right			up				backward (standard OpenGL)
	 * World CS				right			up				backward (standard OpenGL)
	 */

	osg::Matrix mRot_TH;										// Head rotation in room Tracker CS

	osg::Vec3 v_TH = osg::Vec3f(0, 0, 1.8);						// head position in Tracker CS, was v_TH
	osg::Vec3 v_RH;												// head Position in Room CS , was v_TH
	osg::Vec3 v_WH;												// head position in World CS , was v_WH

	mRot_TH = osg::Matrix::identity();							// 

	double theta = 0.0;

	/*
	 * Retrieve head position and attitude from Dtrack, is available
	 */
	bool foundRealHeadTracker = false;
	/*if(m_VRPNClientManager.getTrackerCount())
	{
		CVRPNClientManager::Tracker* tracker =m_VRPNClientDTrack.getTrackerFromId(VRPN_DTRACK_CYCLOPS_ID);
		if(tracker)
		{
			v_TH = tracker->crtPos;
			q_TH = tracker->crtQuat;
			foundRealHeadTracker = true;
		}
	}*/

	/*
	 * One face computes the absolute room position from the incremental tracker updates and
	 * then the absolute room "virtual" vrpn tracker.
	 * Other faces get the room position from the virtual tracker
	 */
	if (m_caveFaceId == CAVE_CENTER) {

		if(m_VRPNClientManagerAndroid.theresNewGoToAnnotationCommand()){
			std::cout<<"New Go To Annotation Command!\n";
			CVRPNClientManager::Tracker* pGoToannotation = m_VRPNClientManagerAndroid.getTrackerFromId(VRPN_GOTO_ANNOTATION);
			int i=0;
			for(i;i<annotations.size();i++){
				Annotation actual=annotations[i];
				std::stringstream goToAnnotationId;;
				goToAnnotationId<<(int)(pGoToannotation->crtQuat[0]);
				std::cout<<"\tAnnotation Id: "<<actual.id<<"/("<<(int)pGoToannotation->crtQuat[0]<<"/"<<goToAnnotationId.str()<<")\n";
				if(actual.id==goToAnnotationId.str()){
					std::cout<<"Annotation "<<i<<":"<<actual.content+"\n"<<"\t"<<actual.v_WR[0]<<","<<actual.v_WR[1]<<","<<actual.v_WR[2]<<"\n\t"<<actual.q_WR[0]<<","<<actual.q_WR[1]<<","<<actual.q_WR[2]<<","<<actual.q_WR[3]<<"\n";
					v_WR=actual.v_WR;
					q_WR=actual.q_WR;
				}
			}
		
		}
		else{
			computeRoomPosition();
		}
		m_vrpnClient.sendTrackerUpdate(v_WR, q_WR);
		//std::cerr << "send update " << v_WR << std::endl;
	} else {
		CVRPNClientManager::Tracker* pRoomPositionTracker = m_VRPNClientManagerAndroid.getTrackerFromId(2);
		if (pRoomPositionTracker) {
			q_WR = pRoomPositionTracker->crtQuat;
			v_WR = pRoomPositionTracker->crtPos;
			//std::cerr << "recv update " << v_WR << std::endl;
		}
	}

	/*
	 * Compute Head position in World CS
	 * v_WR = T_WR . T_RT . v_TH
	 * v_RH = vT_RT + q_RT * v_TH, but vT_RT == 0
	 */
	v_RH = (q_RT * v_TH);
	v_WH = v_WR + (q_WR * v_RH);
	
	/*
	 * Update the Head rotation in Tracker space to keep only the rotation around the Z axis in TS
	 */
	osg::Vec3 euler;
	mRot_TH = osg::Matrix(q_TH);
	MatrixToHpr(euler, mRot_TH);
	theta = euler.x();
	const osg::Quat qZ_TH = osg::Quat(osg::DegreesToRadians(theta), Z_pos);		// Head rotation constrained to Z in Tracker CS

	/*
	 * Inter-eye distance
	 */
	m_eyeDist = m_VRPNClientManagerAndroid.getAnalogFromId(VrpnInterEyeDistance) * 0.01;

	// Eyes in head position: on each side of the head tracker
	osg::Vec3 leftEye_H = -osg::Vec3(m_eyeDist / 2.0, 0, 0);
	osg::Vec3 rightEye_H = osg::Vec3(m_eyeDist / 2.0, 0, 0);

	osg::Vec3 leftEye_R;				// Left eye in Room CS
	osg::Vec3 rightEye_R;				// Right eye in Room CS
	osg::Vec3 leftEye_W;				// Left eye in World CS
	osg::Vec3 rightEye_W;				// Right eye in World CS
	
	/*
	 * Eye in room CS is required to compute the frustum planes
	 *
	 * e_R = T_RT . T_TH . e_H
	 * e_R = v_RT + q_RT * (v_TH + q_TH * e_H)
	 * but :
	 *		v_RT = (0) : origins of tracker_CS and room_CS are identical
	 *
	 * e_R = v_RT + q_RT * (v_TH + q_TH * e_H)
	 */
	leftEye_R = (q_RT * (v_TH + qZ_TH * leftEye_H));
	rightEye_R = (q_RT * (v_TH + qZ_TH * rightEye_H));
	
	/*
	 * Eye in World CS is required to compute the View matrix
	 * e_W = T_WR . e_R
	 * e_W = v_WR + (q_WR * e_R)
	 */
	leftEye_W = v_WR + (q_WR * leftEye_R);
	rightEye_W = v_WR + (q_WR * rightEye_R);


	


	/*
	 * Now setup the camaras (frustrum and lookAt) depending on which face
	 * we are displaying
	 */
	const double w = m_caveWidth;
	const double h = m_caveHeight;
	const double d = m_caveDepth;

	osg::Vec3 viewUp_W = osg::Vec3(0,1,0);				// View Up in World CS
	osg::Vec3 viewDir_R;								// View direction in Room CS
	osg::Vec3 viewDir_W;								// View direction in World CS

	double left, right, bottom, top, znear;
	double zfar = 10000.0;
	left = right = bottom = top = znear = 0.0;



	//Para todos menos la tablet!
	if(m_caveFaceId!=TABLET){
	//Is there a new annotation?
		if(m_VRPNClientManagerAndroid.theresNewAnnotation()){

			Annotation an;
			m_VRPNClientManagerAndroid.copyLastAnnotation(&an);
			an.q_WR=q_WR;
			an.v_WR=v_WR;
			annotations.push_back(an);
			
			int i=0;
			for(i;i<annotations.size();i++){
				Annotation actual=annotations[i];
				std::cout<<"Annotation "<<i<<":"<<actual.content+"\n"<<"\t"<<an.v_WR[0]<<","<<an.v_WR[1]<<","<<an.v_WR[2]<<"\n\t"<<an.q_WR[0]<<","<<an.q_WR[1]<<","<<an.q_WR[2]<<","<<an.q_WR[3]<<"\n";
			}
		}
	}

	switch(m_caveFaceId)
	{
		case TABLET:
		{
			if(selectingPolygons){

				//Is there a new annotation?
				if(m_VRPNClientManagerAndroid.theresNewAnnotation()){

					Annotation an;
					m_VRPNClientManagerAndroid.copyLastAnnotation(&an);
					an.q_WR=q_WR;
					an.v_WR=v_WR;
					
					annotations.push_back(an);
					
					int i=0;
					for(i;i<annotations.size();i++){
						Annotation actual=annotations[i];
						std::cout<<"Annotation "<<i<<":"<<actual.content+"\n";
						std::cout<<"Annotation "<<i<<":"<<actual.content+"\n"<<"\t"<<an.v_WR[0]<<","<<an.v_WR[1]<<","<<an.v_WR[2]<<"\n\t"<<an.q_WR[0]<<","<<an.q_WR[1]<<","<<an.q_WR[2]<<","<<an.q_WR[3]<<"\n";
					}
					selectingPolygons=false;
				}
			}
			


					
			//Get Tap VRPN
			CVRPNClientManager::Tracker* pTapScreen= m_VRPNClientManagerAndroid.getTrackerFromId(VRPN_TAP_SCREEN);
			//Get DTrack Info			
			CVRPNClientManager::Tracker* pTablet = m_VRPNClientDTrack.getTrackerFromId(VRPN_DTRACK_TABLET_ID);
			CVRPNClientManager::Tracker* pCyclops = m_VRPNClientDTrack.getTrackerFromId(VRPN_DTRACK_CYCLOPS_ID);

			/**Has anyboady tocuhed the screen?**/
			if(pTapScreen){
				double x,y;
				x=pTapScreen->crtQuat.x();
				y=pTapScreen->crtQuat.y();	
				if(lastTapX!=x && lastTapY!=y){
					addPlane();
					lastTapX=x;
					lastTapY=y;
					std::cout<<"Screen Coordinates: "<<x<<","<<y<<"\n";
					if(!selectingPolygons){
						std::cout<<"not selecting polygons";
						selectingPolygons=true;
					}
	
				}
			

			}

			if(!selectingPolygons && pTablet){
				v_tablet=pTablet->crtPos;
				q_tablet=pTablet->crtQuat;
			}


			
			viewDir_R = Z_neg;
			viewDir_W = q_WR * viewDir_R;
			



			//Turns tablet quaternion into rotations
			double sqw = q_tablet.w()*q_tablet.w();
			double sqx = q_tablet.x()*q_tablet.x();
			double sqy = q_tablet.y()*q_tablet.y();
			double sqz = q_tablet.z()*q_tablet.z();
			double x, y, z;
			double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
			double test = q_tablet.x()*q_tablet.y() + q_tablet.z()*q_tablet.w();
			if (test > 0.499*unit) { // singularity at north pole
				x = 2 * atan2(q_tablet.x(),q_tablet.w());
				y = PI/2;
				z = 0;
				return;
			}
			if (test < -0.499*unit) { // singularity at south pole
				x = -2 * atan2(q_tablet.x(),q_tablet.w());
				y = PI/2;
				z = 0;
				return;
			}
			x = atan2(2*q_tablet.y()*q_tablet.w()-2*q_tablet.x()*q_tablet.z() , sqx - sqy - sqz + sqw);
			y = asin(2*test/unit);
			z = atan2(2*q_tablet.x()*q_tablet.w()-2*q_tablet.y()*q_tablet.z() , -sqx + sqy - sqz + sqw);

			
			osg::Vec3 tablet_attitude=osg::Vec3(sin(x+PI/2),sin(-y),sin(z+PI/2));

			//tablet_attitude*=180/PI;
			std::cout <<"\nAttitude: \n\t X: "<<tablet_attitude.x()<<"\n\t Y: "<<tablet_attitude.y()<<"\n\t Z: "<<tablet_attitude.z();
			//std::cout << "Tracker Tablet " << ": " << v_tablet[0] << "," <<  v_tablet[1] << "," << v_tablet[2] << "//" << q_tablet[0] << "," <<  q_tablet[1] << "," << q_tablet[2] << "," << q_tablet[3] << "\n";
			//std::cout << "Room " << ": " << v_WH[0] << "," <<  v_WH[1] << "," << v_WH[2] << "//" << "\n";
			//leftEye_R += v_tablet;

			osg::Vec3& e2 = leftEye_R;
			left =		-w/2 - e2.x()+v_tablet.x();
			right =		 w/2 - e2.x()+v_tablet.x();
			bottom =	-e2.y()+v_tablet.z();
			top =		h - e2.y()+v_tablet.z();
			znear=		d/2 + e2.z()+v_tablet.y();	// will be >= 0 since the wall is at -d/2
			m_leftCamera->setViewMatrixAsLookAt(leftEye_W, leftEye_W+tablet_attitude, viewUp_W);
			m_leftCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			
			if(pCyclops){
				//std::cout << "Tracker Cyclops" << pCyclops->id << " : " << pCyclops->crtPos[0] << "," <<  pCyclops->crtPos[1] << "," << pCyclops->crtPos[2] << "//" << pCyclops->crtQuat[0] << "," <<  pCyclops->crtQuat[1] << "," << pCyclops->crtQuat[2] << "," << pCyclops->crtQuat[3] <<pTablet->crtQuat[4] << "\n";
			}
			//q_WR = pRoomPositionTracker->crtQuat;
			//v_WR = pRoomPositionTracker->crtPos;

			



			


			std::stringstream renderSS; 
			try{
				osgDB::ReaderWriter::WriteResult res = writer->writeImage(*shot, renderSS, options); 

				std::string test=renderSS.str();
			
				sManager.sendDatagram(&test);
			}
			catch(int e){
				std::cout<<"error creating jpg writer";
			}

	


	


			break;
		} // CENTER

		case CAVE_CENTER:
		{
	
			viewDir_R = Z_neg;
			viewDir_W = q_WR * viewDir_R;
			
			// Left-right = width = X
			// top-bottom = height = Y
			osg::Vec3& e = leftEye_R;
			left =		-w/2 - e.x();
			right =		 w/2 - e.x();
			bottom =	-e.y();
			top =		h - e.y();
			znear=		d/2 + e.z();	// will be >= 0 since the wall is at -d/2
			m_leftCamera->setViewMatrixAsLookAt(leftEye_W, leftEye_W + viewDir_W, viewUp_W);
			// Osg will recompute the near anf far planes
			// see http://www.openscenegraph.org/projects/osg/wiki/Support/FAQ#WhydoesOSGseemtoignoremynearfarclippingplanes
			m_leftCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			e = rightEye_R;
			left =		-w/2 - e.x();
			right =		 w/2 - e.x();
			bottom =	-e.y();
			top =		h - e.y();
			znear =		d/2 + e.z();
			m_rightCamera->setViewMatrixAsLookAt(rightEye_W, rightEye_W  + viewDir_W, viewUp_W);
			m_rightCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);
			break;
		} // CENTER
		case CAVE_RIGHT:
		{
			viewDir_R = X_pos;
			viewDir_W = q_WR * viewDir_R;

			// Left-right = depth = Z
			// top-bottom = height = Y
			osg::Vec3& e = leftEye_R;
			left =		-d/2 - e.z();
			right =		 d/2 - e.z();
			bottom =	-e.y();
			top =		h - e.y();
			znear=		w/2 - e.x();	// will be >= 0 since the wall is at -d/2
			m_leftCamera->setViewMatrixAsLookAt(leftEye_W, leftEye_W + viewDir_W, viewUp_W);
			m_leftCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			e = rightEye_R;
			left =		-d/2 - e.z();
			right =		 d/2 - e.z();
			bottom =	-e.y();
			top =		h - e.y();
			znear=		w/2 - e.x();	// will be >= 0 since the wall is at -d/2
			m_rightCamera->setViewMatrixAsLookAt(rightEye_W, rightEye_W + viewDir_W, viewUp_W);
			m_rightCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			break;
		} // RIGHT
		case CAVE_LEFT:
		{
			viewDir_R = X_neg;
			viewDir_W = q_WR * viewDir_R;

			// Left-right = depth = Z
			// top-bottom = height = Y
			osg::Vec3& e = leftEye_R;
			left =		-d/2 + e.z();
			right =		 d/2 + e.z();
			bottom =	-e.y();
			top =		h - e.y();
			znear=		w/2 + e.x();	// will be >= 0 since the wall is at -d/2
			m_leftCamera->setViewMatrixAsLookAt(leftEye_W, leftEye_W + viewDir_W, viewUp_W);
			m_leftCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			e = rightEye_R;
			left =		-d/2 + e.z();
			right =		 d/2 + e.z();
			bottom =	-e.y();
			top =		h - e.y();
			znear=		w/2 + e.x();	// will be >= 0 since the wall is at -d/2
			m_rightCamera->setViewMatrixAsLookAt(rightEye_W, rightEye_W + viewDir_W, viewUp_W);
			m_rightCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);
			break;
		} // LEFT
		case CAVE_BOTTOM:
		{
			// CLEANUP
			const float bottomTopAdjust = 0.3f;
			viewUp_W = q_WR * Z_pos;
			
			viewDir_R = Y_neg;
			/*
			 * viewDir_W = T_WR * viewDir_R
			 * viewDir_W = (q_WR * viewDir_R)
			 * but q_WR rotates around Y only
			 */
			viewDir_W = viewDir_R;

			// Left-right = width = X
			// top-bottom = depth = Y
			osg::Vec3& e = leftEye_R;
			left =		-w/2 + e.x();
			right =		 w/2 + e.x();
			bottom =	-d/2 - e.z();
			top =		d/2 - bottomTopAdjust - e.z();
			znear=		e.y();		// will be >= 0 since the eye is above the floor
			
			m_leftCamera->setViewMatrixAsLookAt(leftEye_W, leftEye_W + viewDir_W, viewUp_W);
			m_leftCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);

			e = rightEye_R;
			left =		-w/2 + e.x();
			right =		 w/2 + e.x();
			bottom =	-d/2 - e.z();
			top =		d/2 - bottomTopAdjust - e.z();
			znear=		e.y();		// will be >= 0 since the eye is above the floor

			m_rightCamera->setViewMatrixAsLookAt(rightEye_W, rightEye_W + viewDir_W, viewUp_W);		
			m_rightCamera->setProjectionMatrixAsFrustum( left, right, bottom, top, znear, zfar);
			break;
		} // BOTTOM
	}

	__super::frame(simulationTime);
}


void CCaveViewer::addPlane(){

	    // create the Geode (Geometry Node) to contain all our osg::Geometry objects.
    osg::Geode* geode = new osg::Geode();


	osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
    shared_colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
	    // same trick for shared normal.
    osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
    shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

	 // create Geometry object to store all the vertices and lines primitive.
    osg::Geometry* polyGeom = new osg::Geometry();
    
    // this time we'll a C arrays to initialize the vertices.
    // note, anticlockwise ordering.
    // note II, OpenGL polygons must be convex plan polygons, otherwise 
    // undefined results will occur.  If you have concave polygons or ones
    // that cross over themselves then use the osgUtil::Tessellator to fix
    // the polygons into a set of valid polygons.
    osg::Vec3 myCoords[] =
    {
        osg::Vec3(-1.0464, 0.0f, -0.193626),
        osg::Vec3(-1.0258, 0.0f, -0.26778),
        osg::Vec3(-0.807461, 0.0f, -0.181267),
        osg::Vec3(-0.766264, 0.0f, -0.0576758),
        osg::Vec3(-0.980488, 0.0f, -0.094753)
    };
    
    int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
    
    osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
   
    // pass the created vertex array to the points geometry object.
    polyGeom->setVertexArray(vertices);
    
    // use the shared color array.
    polyGeom->setColorArray(shared_colors.get());
    polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    

    // use the shared normal array.
    polyGeom->setNormalArray(shared_normals.get());
    polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    

    // This time we simply use primitive, and hardwire the number of coords to use 
    // since we know up front,
    polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

//    printTriangles("Polygon",*polyGeom);
    
    // add the points geometry to the geode.
    geode->addDrawable(polyGeom);
	root->addChild(geode);

}
