#include "stdafx.h"
#include "Global.h"
#include "CGlobal.h"

CGlobal::CGlobal()
{
	player_pos[0]=0.0f;
	player_pos[1]=0.0f;
	player_pos[2]=0.0f;
	yaw=0.0f;
	pitch=0.0f;
	crit_PlayerPos=NULL;

	if(!QueryPerformanceFrequency(&freq)) throw L"Couldn't get Performance Frequency...";

	crit_PlayerPos=new CRITICAL_SECTION;
	if(!InitializeCriticalSectionAndSpinCount(crit_PlayerPos,4000))
	{
		CLEAN_DELETE(crit_PlayerPos);
		throw L"Couldn't init crit_PlayerPos...";
	}
}

CGlobal::~CGlobal()
{
	if(crit_PlayerPos)
	{
		DeleteCriticalSection(crit_PlayerPos);
		CLEAN_DELETE(crit_PlayerPos);
	}
}

void CGlobal::SetPlayerPosition(float *_player_pos)
{
	EnterCriticalSection(crit_PlayerPos);
	player_pos[0]=_player_pos[0];
	player_pos[1]=_player_pos[1];
	player_pos[2]=_player_pos[2];
	LeaveCriticalSection(crit_PlayerPos);
}

void CGlobal::SetPlayerPositionHavok(float *_player_pos)
{
	EnterCriticalSection(crit_PlayerPos);
	player_pos[0]=_player_pos[0]*SCALE_HAVOK;
	player_pos[1]=_player_pos[1]*SCALE_HAVOK;
	player_pos[2]=_player_pos[2]*SCALE_HAVOK;
	LeaveCriticalSection(crit_PlayerPos);
}

void CGlobal::GetPlayerPosition(float *_player_pos)
{
	EnterCriticalSection(crit_PlayerPos);
	_player_pos[0]=player_pos[0];
	_player_pos[1]=player_pos[1];
	_player_pos[2]=player_pos[2];
	LeaveCriticalSection(crit_PlayerPos);
}

void CGlobal::GetPlayerPositionHavok(float *_player_pos)
{
	EnterCriticalSection(crit_PlayerPos);
	_player_pos[0]=player_pos[0]/SCALE_HAVOK;
	_player_pos[1]=player_pos[1]/SCALE_HAVOK;
	_player_pos[2]=player_pos[2]/SCALE_HAVOK;
	LeaveCriticalSection(crit_PlayerPos);
}

void CGlobal::SetYaw(float _yaw)
{
	yaw=_yaw;
}

float CGlobal::GetYaw()
{
	return yaw;
}

void CGlobal::SetPitch(float _pitch)
{
	pitch=_pitch;
}

float CGlobal::GetPitch()
{
	return pitch;
}

void CGlobal::UpdateTimeKeeper()
{
	LARGE_INTEGER diff;

	previous_time=current_time;
	QueryPerformanceCounter(&current_time);

	diff.QuadPart=current_time.QuadPart-previous_time.QuadPart;

	frame_time=(float)((double)diff.QuadPart/(double)freq.QuadPart);

	//current_time=GetTickCount();
	//frame_time=current_time-previous_time;
}

float CGlobal::GetFrameTime()
{
	return frame_time;
}