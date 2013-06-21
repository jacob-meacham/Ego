#ifndef _inputCore_h_included_
#define _inputCore_h_included_
#include "DXUtil.h"

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

enum InputDevices {
  NONE = 0,
  KEYBOARD,
  MOUSE
};

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
	  
		short                    m_Type; ///< Type of device (keyboard, or mouse).
		BOOL                     m_Windowed; ///< TRUE if m_hWnd is windowed.
		char                     m_State[256]; ///< State of all possible buttons.
		BOOL                     m_Locks[256]; ///< State of the locks on all buttons (see SetLock()).
		BOOL					 m_Toggle[256]; ///< State of the toggles on all buttons.

		DIMOUSESTATE            *m_MouseState;	///< DirectX mouse state structure.
		long                     m_XPos, m_YPos; ///< Coordinates of the mouse.

	public:
		// Constructor/Destructor.
		Input();
		~Input();

		// System Getters.
		IDirectInput8 *GetDirectInputCOM();
		HWND GetHWnd(); 

		// Initializes the input device.
		BOOL Init(HWND hWnd, HINSTANCE hInst);

		// Called by the destructor to shutdown the input device.
		BOOL Shutdown();

		// Creates an input device of type Type.
		BOOL Create(short Type, BOOL Windowed = TRUE);

		// Frees a previously created input device.
		BOOL Free();
		
		// Reads the status of the input device, regardless of type.
		BOOL Read();

		// Attempts to reacquire or unacquire a device.
		BOOL Acquire(BOOL Active = TRUE);

		// Clears the status of the device.
		BOOL Clear();

		// Universal Getters/Setters.
		BOOL GetLock(char Num);
		BOOL SetLock(char Num, BOOL State = TRUE);

		long GetXPos();
		BOOL SetXPos(long XPos);

		long GetYPos();
		BOOL SetYPos(long YPos);
		
		long GetXDelta();
		long GetYDelta();

		// Keyboard specific Getters/Setters.
		BOOL  GetKeyState(char Num);
		BOOL  SetKeyState(char Num, BOOL State);
		BOOL  GetPureKeyState(char Num);
		BOOL  GetToggle(char Num);
		BOOL  SetToggle(char Num);
		short GetKeypress(long TimeOut = 0);
		long  GetNumKeyPresses();
		long  GetNumPureKeyPresses();

		// Mouse specific Getters/Setters.
		BOOL  GetButtonState(char Num);
		BOOL  SetButtonState(char Num, BOOL State);
		BOOL  GetPureButtonState(char Num);
		long  GetNumButtonPresses();
		long  GetNumPureButtonPresses();
};
#endif
		
		

