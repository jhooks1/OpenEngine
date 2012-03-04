#pragma once

#include "trace.h"

#include "DrawObject.h"

#include "PerFrameFilter.h"

#include <map>
#include "..\..\ESMParser\ESMParser\LAND.h"
extern int isShowCollision;
extern unsigned collisionTri;
extern std::map<CellCoords, CELL* const> ExtCellLookup;
extern std::map<CellCoords, LAND* const> LANDLookup;

#include "GetGroundPosition.h"

#include "GetGroundIntersect.h"

#include "IntersectAABB_Ray.h"

#include "ObjectsIntersectingBox.h"

#include "Scene.h"

#include "..\..\ESMParser\ESMParser\CELL.h"

#include "NewPhysics.h"

void newtrace(traceResults* const results, const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::Vector3& BBHalfExtents, const float rotation)  //Traceobj was a Aedra Object
{
	//if (!traceobj)
	//	return;

	//if (!traceobj->incellptr)
	//	return;

	const Ogre::Vector3 rayDir = end - start;

	// Nudge starting point backwards
	//const Position3D nudgestart = start + (rayDir * -0.1f); // by 10% (isn't that too much?)
	//const Position3D nudgestart = start;

	NewPhysTraceResults out;
	const bool hasHit = NewPhysicsTrace<collisionWorldTrace>(&out, start, end, BBHalfExtents, Ogre::Vector3(0.0f, rotation, 0.0f));

	if (out.fraction < 0.001f)
		results->startsolid = true;
	else
		results->startsolid = false;

	//results->allsolid = out.startSolid;
	const Cell* const traceCell = mCurrentCell->cell->//traceobj->incellptr;  //Get current OpenMW cell

	// If outside and underground, we're solid
	if (traceCell.isInteriror)
	{
		const Ogre::Vector3 height = GetGroundPosition(start, CellCoords(traceCell->data->gridX, traceCell->data->gridY) );
		if (start.yPos - height.yPos < (-2.0f * BBHalfExtents.yPos) )
		{
			results->allsolid = true;
		}
		else
			results->allsolid = false;
	}

	// If inside and out of the tree, we're solid
	else
	{
		results->allsolid = out.startSolid;
	}

	if (!hasHit)
	{
		results->endpos = end;
		results->planenormal = Ogre::Vector3(0.0f, 1.0f, 0.0f);
		results->entityNum = ENTITYNUM_NONE;
		results->fraction = 1.0f;
	}
	else
	{
		results->fraction = out.fraction;
		results->planenormal = out.hitNormal;
		results->endpos = rayDir * results->fraction + start;
		results->entityNum = ENTITYNUM_WORLD;
		/*bprintf("Start: (%f, %f, %f) End: (%f, %f, %f) TraceDir: (%f, %f, %f) HitNormal: (%f, %f, %f) Fraction: %f Hitpos: (%f, %f, %f) CompensatedHitpos: (%f, %f, %f)\n", 
			start.xPos, start.yPos, start.zPos,
			end.xPos, end.yPos, end.zPos,
			rayDir.xPos, rayDir.yPos, rayDir.zPos,
			results->planenormal.xPos, results->planenormal.yPos, results->planenormal.zPos, results->fraction,
			out.endPos.xPos, out.endPos.yPos, out.endPos.zPos,
			results->endpos.xPos, results->endpos.yPos, results->endpos.zPos);*/
	}
}



template <const traceWorldType traceType>
const bool NewPhysicsTrace(NewPhysTraceResults* const out, const Ogre::Vector3& start, const Ogre::Vector3& end, 
	const Ogre::Vector3& BBHalfExtents, const Ogre::Vector3& rotation)
{
	//if (!traceobj->incellptr)
	//	return false;

	

#ifdef USE_CUSTOM_CONVEX_CALLBACK
	customHitsList.clear();
#endif

	const btVector3 btstart(start.x, start.y, start.z);
	const btVector3 btend(end.x, end.y, end.z);
	const btQuaternion btrot(rotation.y, rotation.x, rotation.z);

	const btBoxShape newshape(btVector3(BBHalfExtents.x, BBHalfExtents.y, BBHalfExtents.z) );
	const btTransform from(btrot, btstart);
	const btTransform to(btrot, btend);	

#ifdef USE_CUSTOM_CONVEX_CALLBACK
	CustomConvexResultCallback 
#else
	btCollisionWorld::ClosestConvexResultCallback
#endif
		newTraceCallback(btstart, btend);

	newTraceCallback.m_collisionFilterMask = (traceType == collisionWorld
		Trace) ? Only_Collision : Only_Pickup;

	dynamicsWorld->convexSweepTest(&newshape, from, to, newTraceCallback);
	
#ifdef USE_CUSTOM_CONVEX_CALLBACK
	if (!customHitsList.empty() )
	{
		const unsigned nHits = (const unsigned)customHitsList.size();
		bprintf("nHits: %u: ", nHits);
		for (unsigned x = 0; x < nHits; ++x)
		{
			bprintf("(F: %f, P: %f, %f, %f, N: %f, %f, %f) ", customHitsList[x].m_closestHitFraction,
				customHitsList[x].m_hitPointWorld.x(), customHitsList[x].m_hitPointWorld.y(), customHitsList[x].m_hitPointWorld.z(),
				customHitsList[x].m_hitNormalWorld.x(), customHitsList[x].m_hitNormalWorld.y(), customHitsList[x].m_hitNormalWorld.z() );
		}
		bprintf("\n");
	}
#endif

	// Copy the hit data over to our trace results struct:
	out->fraction = newTraceCallback.m_closestHitFraction;

	Ogre::Vector3& outhitnormal = out->hitNormal;
	const btVector3& tracehitnormal = newTraceCallback.m_hitNormalWorld;

	outhitnormal.xPos = tracehitnormal.x();
	outhitnormal.yPos = tracehitnormal.y();
	outhitnormal.zPos = tracehitnormal.z();

	Ogre::Vector3& outhitpos = out->endPos;
	const btVector3& tracehitpos = newTraceCallback.m_hitPointWorld;

	outhitpos.xPos = tracehitpos.x();
	outhitpos.yPos = tracehitpos.y();
	outhitpos.zPos = tracehitpos.z();

	// StartSolid test:
	{
		out->startSolid = false;
		//btCollisionObject collision;
		//collision.setCollisionShape(const_cast<btBoxShape* const>(&newshape) );

		//CustomContactCallback crb;

		//world.world->contactTest(&collision, crb);
		//out->startSolid = crb.hit;

		// If outside and underground, we're solid
		if (mCurrentCell->cell->data.flags)   //Check if we are interior
		{
		}

		// If inside and out of the tree, we're solid
		else
		{
			btVector3 aabbMin, aabbMax;
			world.sweepBP->getBroadphaseAabb(aabbMin, aabbMax);
			if (!TestPointAgainstAabb2(aabbMin, aabbMax, *(const btVector3* const)&(start) ) )
			{
				out->startSolid = true;
			}
		}
	}

	const bool hasHit = newTraceCallback.hasHit();

	if (hasHit)
	{
		out->hitObj = (const Object* const)(newTraceCallback.m_hitCollisionObject->getUserPointer() );
	}
	else
	{
		out->hitObj = NULL;
	}

#ifdef VIEW_CAST_HITS
	if (hasHit)
	{
		Position3D hit;
		hit = out->endPos;
		hitPos.push_back(hit);
	}
#endif

	
	return hasHit;
}
