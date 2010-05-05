#ifndef CLASSWEATHERMANAGER
#define CLASSWEATHERMANAGER

class CWeatherManager
{
public:
	CWeatherManager();
	~CWeatherManager();
	void Init();
protected:
	CMesh *dasky;
};

#endif