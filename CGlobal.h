#ifndef CLASSGLOBAL
#define CLASSGLOBAL

class CGlobal
{
public:
	CGlobal();
	~CGlobal();
	void SetPlayerPosition(float *);
	void GetPlayerPosition(float *);
	void SetPlayerPositionHavok(float *);
	void GetPlayerPositionHavok(float *);

	void SetYaw(float);
	float GetYaw();
	void SetPitch(float);
	float GetPitch();

	void UpdateTimeKeeper();
	float GetFrameTime();
protected:
	//Info on the player
	float player_pos[3];
	float yaw,pitch;

	//TimeKeeper stuff
	LARGE_INTEGER freq;
	LARGE_INTEGER current_time,previous_time;
	float frame_time;

	//Multithreading stuff
	CRITICAL_SECTION *crit_PlayerPos;
};

#endif