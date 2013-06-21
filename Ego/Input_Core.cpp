#include "Input_Core.h"

#define Error(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default constructor.  Sets everything to null.  Must call Init() after constructor.
Input::Input() {
	m_hWnd = NULL;
	m_Input = NULL;
	m_InputDevice = NULL;
	m_Type = NONE;
	m_Windowed = TRUE;
	m_XPos = m_YPos = 0;
	m_MouseState  = (DIMOUSESTATE*)&m_State;
}

/// Default destructor.
Input::~Input() {
	Free();
	Shutdown();
}

/// Returns pointer to the DirectX input interface.
IDirectInput8 *Input::GetDirectInputCOM() {
	return m_Input;
}

/// Initializes the input device.
/** Init() must be called before Create().
	\param hWnd The handle to the main window.
	\param hInst Instance of the main window.
	\return TRUE if the initialization succeeds.
*/
BOOL Input::Init(HWND hWnd, HINSTANCE hInst) {
	// if either are null, the creation will fail.  Return false.
	if((m_hWnd = hWnd) == NULL) { return FALSE; }
	if(hInst == NULL) { return FALSE; }

	// create the device using DirectX
	if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_Input, NULL)))
		return FALSE;
	return TRUE;
}

/// Called by the destructor to shutdown the input device.
BOOL Input::Shutdown() {
	// free a previous device.
	Free();

	if(m_hWnd) {m_hWnd = NULL;}

	// release the COM.
	if(m_Input) {
	m_Input->Release();
	m_Input = NULL;
	}
	return TRUE;
}

/// Creates an input device of type Type.
/** \param Type the type of device.  Use enum InputDevices.
	\return TRUE if the device is created successfully.
*/
BOOL Input::Create(short Type, BOOL Windowed) {
	// free a previous device.
	Free();
	if(m_Input == NULL) { return FALSE; }
	
	// Use DirectX input COM to create the specified device.
	switch(Type) {
		case KEYBOARD: 
			if(FAILED(m_Input->CreateDevice(GUID_SysKeyboard, &m_InputDevice, NULL))) {
				return FALSE; 
			}
			if(FAILED(m_InputDevice->SetDataFormat(&c_dfDIKeyboard))) { 
				return FALSE; 
			}
			break;

		case MOUSE:
			if(FAILED(m_Input->CreateDevice(GUID_SysMouse, &m_InputDevice, NULL))) {
				return FALSE; 
			}
			if(FAILED(m_InputDevice->SetDataFormat(&c_dfDIMouse))) { 
				return FALSE; 
			}
			break;

		default: return FALSE;
	}
	m_Windowed = Windowed;

	if(FAILED(m_InputDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) { return FALSE; }
	
	if(FAILED(m_InputDevice->Acquire())) { return FALSE; }
    
	m_Type = Type;
    
	Clear();
    return TRUE;
}

/// Frees a previously created input device, but does not shutdown the Input Interface.
BOOL Input::Free() {
	if(m_InputDevice != NULL) {
    m_InputDevice->Unacquire();
    m_InputDevice->Release();
	m_InputDevice = NULL;
	}
	m_Type = NONE;
	Clear();
	return TRUE;
}		

/// Reads the status of the input device, regardless of type.
/** This function should be called every game frame, before polling the device.
	\return TRUE if the read is successful.
*/
BOOL Input::Read() {
	HRESULT hr;
	long BufferSize[2] = { 256, sizeof(DIMOUSESTATE) };
	short i;
	
	if(m_InputDevice == NULL) { return FALSE; }

	if(m_Type < 1 || m_Type > 3) { return FALSE; }

	while(TRUE) {
		// Poll the device.
		m_InputDevice->Poll();

		// if the poll is successful, we have the state. break.
		if(SUCCEEDED(hr = m_InputDevice->GetDeviceState(BufferSize[m_Type-1], &m_State))) 
			break;

		// if we have any error besides these two, it is unknown.  Return false.
		if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED) { return FALSE; }

		// if it is one of these errors, reacquire the device and try again.
		if(FAILED(m_InputDevice->Acquire())) { return FALSE; }

	} // end while
	
	// Set variables specific to mouse.
	if(m_Type == MOUSE) {
		if(m_Windowed == TRUE) {
			POINT pointer;
			GetCursorPos(&pointer);
			ScreenToClient(m_hWnd, &pointer);
			m_XPos = pointer.x;
			m_YPos = pointer.y;
		} else {
			m_XPos += m_MouseState->lX;
			m_YPos += m_MouseState->lY;
		}
	}
	// Set locks specific to keyboard
	switch(m_Type) {
		case KEYBOARD:
			for(i=0;i<256;i++) {
				// If the keyboard button was released, then it should be unlocked.
				if(!(m_State[i] & 0x80)) {
					m_Locks[i] = FALSE;
				} // end if
			} // end for
			break;
		// Set locks specific to mouse (using a 4 button mouse).
		case MOUSE:
			for(i=0;i<4;i++) {
				// if the mouse button is not pressed, then there is no lock.
				if(!(m_MouseState->rgbButtons[i])) {
					m_Locks[i] = FALSE;
				} // end if
			} // end for
       break;
	} // end switch

	return TRUE;
}

/// Attempts to reacquire or unacquire a device.
/** \param Active if TRUE, Acquire() attempts to reacquire.  If FALSE, Acquire() attempts to Unacquire.
	\return TRUE if the function is successful.
*/
BOOL Input::Acquire(BOOL Active) {
	if(m_InputDevice == NULL) { return FALSE; }
	if(Active) {
		if(FAILED(m_InputDevice->Acquire())) { return FALSE; }
	}
	else { m_InputDevice->Unacquire();	}
	
	return TRUE;
}

/// Clears the status of the device.  Resets all locks and toggles.
BOOL Input::Clear() {
	short i;

	ZeroMemory(&m_State, 256);
  
	for(i=0;i<256;i++) {
		m_Locks[i]  = FALSE;
		m_Toggle[i] = FALSE;
	}

	m_XPos = 0;
	m_YPos = 0;

	return TRUE;
}

/// Returns the status of a lock on the button Num.
/** See SetLock() for informatio on locks. */
BOOL Input::GetLock(char Num) { return m_Locks[Num]; }

/// Sets the lock on Num to State.
/** Locks are used to stop the input device from seeing the locked key.
	Without locks, if a key is pressed, the input device will register
	a key press every frame (good for movement keys).  However, if we want
	only one key press per time the key is pressed, then we must set a lock.
*/
BOOL Input::SetLock(char Num, BOOL State) {
	m_Locks[Num] = State;
	return TRUE;
}

/// Returns the X-Coordinate of the mouse.
long Input::GetXPos() { return m_XPos; }
	
/// Sets the X-Coordinate of the mouse.
BOOL Input::SetXPos(long XPos) {
	m_XPos = XPos;
	return TRUE;
}

/// Returns the Y-Coordinate of the mouse.
long Input::GetYPos() { return m_YPos; }
	
/// Sets the Y-Coordinate of the mouse.
BOOL Input::SetYPos(long YPos) {
	m_YPos = YPos;
	return TRUE;
}

/// Returns the change in mouse position from last poll in the x direction.
/** DirectX Mice do not store absolute position.  They stor delta, which can
	be used to determine absolute screen position.
*/
long Input::GetXDelta() { return m_MouseState->lX; }

/// Returns the change in mouse position from last poll in the y direction.
/** DirectX Mice do not store absolute position.  They stor delta, which can
	be used to determine absolute screen position.
*/
long Input::GetYDelta() { return m_MouseState->lY; }

// Begin keyboard specific functions

/// Returns the state of key Num.  If True, the key is pressed.
BOOL Input::GetKeyState(char Num) {
	if(m_State[Num] & 0x80 && m_Locks[Num] == FALSE)
		return TRUE;
	return FALSE;
}

/// Sets the state of key Num to State.
BOOL Input::SetKeyState(char Num, BOOL State) {
	m_State[Num] = State;
	return TRUE;
}

/// Returns the state of the key, disregarding any locks.
BOOL Input::GetPureKeyState(char Num) { return ((m_State[Num] & 0x80) ? TRUE : FALSE); }

/// Returns a keypress from the keyboard.
/** \param TimeOut Lenght of time to poll the keyboard.
	\return The number of the first found keypress, 0 if no key is found.
*/
short Input::GetKeypress(long TimeOut) {
	static HKL KeyboardLayout = GetKeyboardLayout(0); // layout of the keyboard.
	unsigned char WinKeyStates[256], DIKeyStates[256]; // arrays for the state of the keys.
	unsigned short i, ScanCode, VirtualKey, Keys, Num;
	unsigned long EndTime;
  
	// Make sure it's a keyboard and its initialized
	if((m_Type != KEYBOARD) || (m_InputDevice == NULL)) {
		return 0;
	}

	// Calculate end time for TimeOut
	EndTime = GetTickCount() + TimeOut;

	// Loop until timeout or key pressed
	while(1) {
		// Get Windows keyboard state
		GetKeyboardState(WinKeyStates);

		// Get DirectInput keyboard state
		m_InputDevice->GetDeviceState(256, DIKeyStates);

		// Scan through looking for key presses
		for(i=0;i<256;i++) {
			// If one found, try to convert it
			if(DIKeyStates[i] & 0x80) {
				// Get virtual key code
				if((VirtualKey = MapVirtualKeyEx((ScanCode = i), 1, KeyboardLayout))) {
					// Get ASCII code of key and return it
					Num = ToAsciiEx(VirtualKey, ScanCode, WinKeyStates, &Keys, 0, KeyboardLayout);
					if(Num) { return Keys; }
				} // end if((VirtualKey...)
			} // end if((DIKey...)

			// Check for TimeOut
			if(TimeOut) {
				if(GetTickCount() > EndTime) { return 0; }
			}
		}
	}
	return 0;
}

/// Returns the number of keypresses, not including locks.
long Input::GetNumKeyPresses() {
	short i;
	long Num = 0;
	for (i=0; i<256; i++) {
		if(m_State[i] & 0x80 && m_Locks[i] == FALSE)
			Num++;
	}
	return Num;
}

/// Returns the number of keypresses, including locks.
long Input::GetNumPureKeyPresses() {
	short i;
	long Num = 0;
	for(i=0; i<256; i++) {
		if(m_State[i] & 0x80)
			Num++;
	}
	return Num;
}

/// Returns the toggle state of key Num.
/** See SetToggle() for more information on toggles. */
BOOL Input::GetToggle(char Num) {
	if(m_Toggle[Num] == TRUE)
		return TRUE;
	return FALSE;
}

/// Sets the toggle stat of key Num.
/** Toggles are like locks (see SetLock() ) which stay until the key is pressed again.
*/
BOOL Input::SetToggle(char Num) {
	if(m_Toggle[Num] == FALSE)
		m_Toggle[Num] = TRUE;
	else m_Toggle[Num] = FALSE;
	return TRUE;
}

// Begin mouse specific functions
/// Returns the state of mouse button Num, not including locks.
BOOL Input::GetButtonState(char Num) { 
	if(m_MouseState->rgbButtons[Num] & 0x80 && m_Locks[Num] == FALSE)
		return TRUE;
    return FALSE;
}

/// Sets the state of mouse button Num to State.
BOOL Input::SetButtonState(char Num, BOOL State) {
	m_MouseState->rgbButtons[Num] = State;
	return TRUE;
}

/// Returns the state of mouse button Num, including any locks.
BOOL Input::GetPureButtonState(char Num) {
	if(m_MouseState->rgbButtons[Num] & 0x80)
		return TRUE;
	return FALSE;
}

/// Returns the number of mouse buttons pressed, not including locked buttons.
long Input::GetNumButtonPresses() {
	short i;
	long Num = 0;
	for(i=0; i<4; i++) {
		if(m_MouseState->rgbButtons[Num] & 0x80 && m_Locks[Num] == FALSE)
			Num++;
	}
	return Num;
}

/// Returns the number of mouse buttons pressed, including locked buttons.
long Input::GetNumPureButtonPresses() {
	short i;
	long Num = 0;
	for(i=0; i<4; i++) {
		if(m_MouseState->rgbButtons[Num] & 0x80)
			Num++;
	}
	return Num;
}