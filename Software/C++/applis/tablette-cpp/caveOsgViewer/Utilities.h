#pragma once

class Utilities
{
private:
	Utilities(void);
	~Utilities(void);
public:
	static void quaternionToSpeed(const osg::Quat& quat, double& lateral, double& longitudinal);
	static double angleToSpeed(double a, double min_, double max_);
};
