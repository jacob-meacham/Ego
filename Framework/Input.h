#pragma once

#include "DXUtil.h"
#include "Define.h"

#define KEY_RELEASED       FALSE
#define KEY_PRESSED         TRUE

#define KEY_UNLOCKED       FALSE
#define KEY_LOCKED		    TRUE

#define BUTTON_RELEASED    FALSE
#define BUTTON_PRESSED      TRUE

#define BUTTON_UNLOCKED    FALSE
#define BUTTON_LOCKED       TRUE

#define MOUSE_LBUTTON          0
#define MOUSE_RBUTTON          1
#define MOUSE_MBUTTON          2

/// Main Input class.  Supports mouse and keyboard.
/** This class is used to set the directX devices, as well as to poll
the hardware. The class is based off of code found in Jim Adams'
Programming Role Playing Games with DirectX.
*/
class Input {	
protected:
	HWND					 m_hWnd; ///< Handle to the main window.
	IDirectInput8			*m_Input; ///< DirectX input interface.
	IDirectInputDevice8     *m_InputDevice; ///< DirectX input device interface.
	bool                     m_Windowed; ///< TRUE if m_hWnd is windowed.
	bool                    *m_Locks; ///< State of the locks on all buttons (see SetLock()).
	virtual bool CreateDevice() = 0;
	
public:
	Input();
	~Input();

	// Initializes the input device.
	bool Init(HWND hWnd, HINSTANCE hInst, bool Windowed = true);

	// Clears the status of the device.
	virtual void Clear() = 0;
		
	// Reads the status of the input device, regardless of type.
	virtual bool Read() = 0;

	// Attempts to reacquire or unacquire a device.
	bool Acquire(bool Active = true);

	bool GetLock(char Num) const;
	bool SetLock(char Num, bool State = true);

	IDirectInput8 *GetDirectInputCOM();
};

class Keyboard : public Input
{
protected:
	char                     m_State[256]; ///< State of all possible buttons.
	bool					 m_Toggle[256]; ///< State of the toggles on all buttons.
	bool					 m_bKeyDown;

	virtual bool CreateDevice();
public:
	virtual ~Keyboard();

	virtual void Clear();
	virtual bool Read();

	bool  GetKeyState(char Num) const;
	void  SetKeyState(char Num, bool State);
	bool  GetPureKeyState(char Num) const;
	bool  GetToggle(char Num) const;
	void  SetToggle(char Num);
	short GetKeypress(long TimeOut = 0) const;
	long  GetNumKeyPresses() const;
	long  GetNumPureKeyPresses() const;
};

class Mouse : public Input 
{
protected:

	bool					 m_bLMouseDown;
	bool					 m_bRMouseDown;

	DIMOUSESTATE			 m_MouseState;	///< DirectX mouse state structure.
	long                     m_XPos, m_YPos; ///< Coordinates of the mouse.
	
	virtual bool CreateDevice();
public:
	Mouse();
	virtual ~Mouse();

	virtual void Clear();
	virtual bool Read();

	bool  GetButtonState(char Num) const;
	void  SetButtonState(char Num, bool State);

	bool  GetPureButtonState(char Num) const;
	long  GetNumButtonPresses() const;
	long  GetNumPureButtonPresses() const;

	long GetXPos() const;
	void SetXPos(long XPos);

	long GetYPos() const;
	void SetYPos(long YPos);
		
	long GetXDelta() const;
	long GetYDelta() const;
};