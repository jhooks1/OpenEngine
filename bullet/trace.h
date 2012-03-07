


#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <components/nifbullet/bullet_nif_loader.hpp>
//#include <apps\openmw\mwworld\world.hpp>

#include "physic.hpp"

OEngine::Physic::PhysicEngine* engine;
enum traceWorldType
{
	collisionWorldTrace = 1,
	pickWorldTrace = 2,
	bothWorldTrace = collisionWorldTrace | pickWorldTrace
};

struct NewPhysTraceResults
{
	Ogre::Vector3 endPos;
	Ogre::Vector3 hitNormal;
	float fraction;
	bool startSolid;
	//const Object* hitObj;
}; 
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

bool TestPointAgainstAabb2(const btVector3 &aabbMin1, const btVector3 &aabbMax1,
								const btVector3 &point);

template <const traceWorldType traceType>
const bool NewPhysicsTrace(NewPhysTraceResults* const out, const Ogre::Vector3& start, const Ogre::Vector3& end, 
	const Ogre::Vector3& BBExtents, const Ogre::Vector3& rotation);
template const bool NewPhysicsTrace<collisionWorldTrace>(NewPhysTraceResults* const out, const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::Vector3& BBExtents, const Ogre::Vector3& rotation);
template const bool NewPhysicsTrace<pickWorldTrace>(NewPhysTraceResults* const out, const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::Vector3& BBExtents, const Ogre::Vector3& rotation);

void newtrace(traceResults* const results, const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::Vector3& BBExtents, const float rotation);


