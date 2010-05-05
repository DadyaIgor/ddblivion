#ifndef INPUTMANAGER
#define INPUTMANAGER

class CInputManager
{
public:
	//Constructor and Destructor
	CInputManager();
	~CInputManager();
	//Init the keyboard and the mouse
	void Init(HWND);
	//Update the status of input
	void UpdateInput();
	//Get the status of a key
	unsigned long IsKeyPressed(unsigned char);
	//Get Mouse Variance
	void Get_MouseXY(long &,long &);
protected:
	LPDIRECTINPUT8  pDI;
	LPDIRECTINPUTDEVICE8 pDI_Keyboard;
	LPDIRECTINPUTDEVICE8 pDI_Mouse;

	unsigned char bKeyboard[256];
	long mouseX,mouseY;
	unsigned long buttons[3];
};

#endif