/*
This source file is a *modified* version of various header files from the Quake 3 Arena source code,
which was released under the GNU GPL (v2) in 2005.
Quake 3 Arena is copyright (C) 1999-2005 Id Software, Inc.
*/

#pragma once

#include "GameMath.h"
#include "GameTime.h"

// Forwards-declare it!
struct Object;

/*#ifndef COMPILING_PMOVE
#include "Scene.h"
extern SceneInstance* global_lastscene;
#endif*/

static const Position3D halfExtents(14.64f * 2.0f, 33.25f * 2.0f, 14.24f * 2.0f);

#define	MAX_CLIP_PLANES	5
#define	OVERCLIP 1.001f
//#define	STEPSIZE 18 // 18 is way too much
#define STEPSIZE (18 / 2)
#ifndef M_PI
	#define M_PI 3.14159265358979323846f
#endif
#define YAW 0
#define PITCH /*1*/2
#define ROLL /*2*/1
#define	SHORT2ANGLE(x) ( (x) * (360.0f / 65536.0f) )
#define	ANGLE2SHORT(x) ( (const short)( (x) / (360.0f / 65536.0f) ) )
#define	GENTITYNUM_BITS 10 // don't need to send any more
#define	MAX_GENTITIES (1 << GENTITYNUM_BITS)
#define	ENTITYNUM_NONE (MAX_GENTITIES - 1)
#define ENTITYNUM_WORLD (MAX_GENTITIES - 2)
#define	MIN_WALK_NORMAL 0.7f // can't walk on very steep slopes
#define	JUMP_VELOCITY (270 * 1)
#define PS_PMOVEFRAMECOUNTBITS 6
#define	MINS_Z -24
#define	DEFAULT_VIEWHEIGHT 26
#define CROUCH_VIEWHEIGHT 12
#define	DEAD_VIEWHEIGHT (-16)
#define	CONTENTS_SOLID			1		// an eye is never valid in a solid
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_FOG			64
static const float	pm_accelerate = 10.0f;
static const float	pm_stopspeed = 100.0f;
static const float	pm_friction = 6.0f;
static const float  pm_flightfriction = 3.0f;
static const float	pm_waterfriction = 1.0f;
static const float	pm_airaccelerate = 1.0f;
static const float	pm_swimScale = 0.50f;
static const float	pm_duckScale = 0.25f;
static const float  pm_flyaccelerate = 8.0f;
static const float	pm_wateraccelerate = 4.0f;

enum pmtype_t : unsigned char
{
	PM_NORMAL,		// can accelerate and turn
	PM_NOCLIP,		// noclip movement
	PM_SPECTATOR,	// still run into walls
	PM_DEAD,		// no acceleration or turning, but free falling
	PM_FREEZE,		// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
	PM_SPINTERMISSION	// no movement or status bar
};

enum waterlevel_t : unsigned char
{
	WL_DRYLAND = 0,
	WL_ANKLE,
	WL_WAIST,
	WL_UNDERWATER
};

#include "bprintf.h"

struct playerMove
{
	struct playerStruct
	{
		playerStruct() : gravity(800.0f), speed(320.0f), pmove_framecount(20), groundEntityNum(ENTITYNUM_NONE), commandTime(40), move_type(PM_NORMAL), pm_time(0)
		{
			origin = Position3D(0.0f, 2500.0f, 0.0f);
			velocity = Position3D(0.0f, 0.0f, 0.0f);

			viewangles = Position3D(0.0f, 0.0f, 0.0f);

			delta_angles[0] = delta_angles[1] = delta_angles[2] = 0;

			lastframe_origin.clear();
			lerp_multiplier.clear();
		}

		inline void SpeedUp(void)
		{
			bprintf("speed up to: %f\n", speed);
			speed *= 1.25f;
		}

		inline void SpeedDown(void)
		{
			bprintf("speed down to %f\n", speed);
			speed /= 1.25f;
		}

		Position3D velocity;
		Position3D origin;
		float gravity; // default = 800
		float speed; // default = 320

		int commandTime; // the time at which this command was issued (in milliseconds)

		int pm_time;

		Position3D viewangles;

		int groundEntityNum;

		int pmove_framecount;

		int watertype;
		waterlevel_t waterlevel;

		signed short delta_angles[3];

		pmtype_t move_type;

		TimeTicks last_compute_time;
		Position3D lastframe_origin;
		Position3D lerp_multiplier;
	} ps;

	struct playercmd
	{
		enum CMDstateChange
		{
			NO_CHANGE,
			KEYDOWN,
			KEYUP
		};

		playercmd() : forwardmove(0), rightmove(0), upmove(0), serverTime(50), ducking(false), 
			activating(false), lastActivatingState(false), procActivating(NO_CHANGE),
			dropping(false), lastDroppingState(false), procDropping(NO_CHANGE)
		{
			angles[0] = angles[1] = angles[2] = 0;
		}

		int serverTime;

		short angles[3];

		signed char forwardmove;
		signed char rightmove;
		signed char upmove;

		bool ducking;
		bool activating; // if the user is holding down the activate button
		bool dropping; // if the user is dropping an item

		bool lastActivatingState;
		bool lastDroppingState;

		CMDstateChange procActivating;
		CMDstateChange procDropping;
	} cmd;

	playerMove() : msec(50), pmove_fixed(false), pmove_msec(50)
	{
	}

	int msec;
	int pmove_msec;
	bool pmove_fixed;

	Object* traceObj;
};

void Pmove (playerMove* const pmove);
void Ext_UpdateViewAngles(playerMove* const pm);
void AngleVectors( const Position3D& angles, Position3D* const forward, Position3D* const right, Position3D* const up) ;
