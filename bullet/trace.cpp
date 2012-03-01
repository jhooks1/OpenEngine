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

void newtrace(traceResults* const results, const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::Vector3& BBHalfExtents, const float rotation, const Ogre::Vector3* const traceobj)
{
	if (!traceobj)
		return;

	if (!traceobj->incellptr)
		return;

	const Position3D rayDir = end - start;

	// Nudge starting point backwards
	//const Position3D nudgestart = start + (rayDir * -0.1f); // by 10% (isn't that too much?)
	//const Position3D nudgestart = start;

	NewPhysTraceResults out;
	const bool hasHit = NewPhysicsTrace<collisionWorldTrace>(&out, start, end, BBHalfExtents, Rotation3D(0.0f, rotation, 0.0f), traceobj);

	if (out.fraction < 0.001f)
		results->startsolid = true;
	else
		results->startsolid = false;

	//results->allsolid = out.startSolid;
	const CELL* const traceCell = traceobj->incellptr;

	// If outside and underground, we're solid
	if (!traceobj->incellptr->IsCellInterior() )
	{
		const Position3D height = GetGroundPosition(start, CellCoords(traceCell->data->gridX, traceCell->data->gridY) );
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
		results->planenormal = Position3D(0.0f, 1.0f, 0.0f);
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

