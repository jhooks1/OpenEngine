#pragma once

#include "GameMath.h"

#include "Object.h"

struct traceResults
{
	Ogre::Vector3 endpos;
	Ogre::Vector3 planenormal;

	float fraction;

	int surfaceFlags;
	int contents;
	int entityNum;

	bool allsolid;
	bool startsolid;
};

void tracefunc(traceResults* const results, Ogre::Vector3 start, const Ogre::Vector3& end, Ogre::Vector3 rayDir, int cliphandle, const Object* const traceobj);


