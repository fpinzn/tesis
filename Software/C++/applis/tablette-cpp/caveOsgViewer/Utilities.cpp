#include "StdAfx.h"
#include "Utilities.h"

Utilities::Utilities(void)
{
}

Utilities::~Utilities(void)
{
}
double Utilities::angleToSpeed(double a, double min_, double max_) {
	const double absA = abs(a);
	const double signA = a / absA;

	if (absA < min_ ) {
		return 0.0;
	}

	if (absA > max_) {
		return signA;
	}

	const double x = (absA - min_) / (max_ - min_) ;
	return signA * x * x;
}

void Utilities::quaternionToSpeed(const osg::Quat& quat, double& lateral, double& longitudinal)
{
	// +Rz (roll to the right)-> +X (right)
	// +Rx (pitch up) -> +Z (backwards)
	// get euler angles
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Rotation_matrices
	const osg::Quat& q = quat;
	const double x = q.x();
	const double y = q.y();
	const double z = q.z();
	const double w = q.w();
	const float phiR = atan2(2.0*(x*y+z*w), 1.0 - 2.0*(y*y+z*z));
	const float thetaR = asin(2.0*(x*z-y*w));
	const float psiR = atan2(2.0*(x*w+y*z), 1.0 - 2.0*(z*z+w*w));
	const float RAD_TO_DEG = 180.0 / 3.141592;
	/*
	printf("phi %.0f theta %.0f psi %.0f\n", 
		RAD_TO_DEG * phiR,
		RAD_TO_DEG * thetaR,
		RAD_TO_DEG * psiR
		);
		*/
	float roll = RAD_TO_DEG * phiR;
	float pitch = 180.0 - RAD_TO_DEG * psiR;
	if (roll > 180.0) {
		roll = roll - 360.0;
	} else if (roll < -180.0) {
		roll= 360.0 + roll;
	}

	//printf("pitch %d roll %d\n", (int)pitch, (int) roll);
	if (pitch > 180.0) {
		pitch = pitch - 360.0;
	} else if (pitch < -180.0) {
		pitch= 360.0 + pitch;
	}

	//printf("pitch %d roll %d\n", (int)pitch, (int) roll);
	// Move camera
	// +X is right
	
	const double deadAngle = 10.0;	
	const double maxSpeedAngle = 30.0;
	float rightSpeed = -angleToSpeed(roll, deadAngle, maxSpeedAngle);
	float backSpeed = angleToSpeed(pitch, deadAngle, maxSpeedAngle);

	lateral = rightSpeed;
	longitudinal = backSpeed;

}


