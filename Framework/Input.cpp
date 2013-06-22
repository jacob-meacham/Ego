#include "Input.h"
//////////////////////////////////////////////////////////////////////////////////
//																				//
// ----------------------------------- Input ---------------------------------- //
//																				//
//////////////////////////////////////////////////////////////////////////////////
Input::Input() : m_Locks(NULL), m_hWnd(NULL), m_Input(NULL), m_InputDevice(NULL) {

}
//////////////////////////////////////////////////////////////////////////////////
/// Default destructor.
Input::~Input() {
	if(m_InputDevice != NULL) {
		m_InputDevice->Unacquire();
		m_InputDevice->Release();
		m_InputDevice = NULL;
	}

	// release the COM.
	if(m_Input) {
		m_Input->Release();
		m_Input = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns pointer to the DirectX input interface.
IDirectInput8 *Input::GetDirectInputCOM() {
	return m_Input;
}
//////////////////////////////////////////////////////////////////////////////////
/// Initializes the input device.
/** Init() must be called before Create().
	\param hWnd The handle to the main window.
	\param hInst Instance of the main window.
	\return TRUE if the initialization succeeds.
*/
bool Input::Init(HWND hWnd, HINSTANCE hInst, bool Windowed) {
	// if either are null, the creation will fail.  Return false.
	if((m_hWnd = hWnd) == NULL) { return FALSE; }
	if(hInst == NULL) { return FALSE; }

	m_Windowed = Windowed;

	// create the device using DirectX
	if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_Input, NULL)))
		return false;

	if (!CreateDevice())
		return false;

	if(FAILED(m_InputDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) { return FALSE; }
	if(FAILED(m_InputDevice->Acquire())) { return FALSE; }
    
	Clear();
    return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Attempts to reacquire a device.
/** \param Active if true, Acquire() attempts to reacquire.  If false, Acquire() attempts to Unacquire.
	\return true if the function is successful.
*/
bool Input::Acquire(bool Active) {
	if(m_InputDevice == NULL) { return false; }
	if(Active) {
		if(FAILED(m_InputDevice->Acquire())) { return false; }
	}
	else { m_InputDevice->Unacquire();	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the status of a lock on the button Num.
/** See SetLock() for informatio on locks. */
bool Input::GetLock(char Num) const { return m_Locks[Num]; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the lock on Num to State.
/** Locks are used to stop the input device from seeing the locked key.
	Without locks, if a key is pressed, the input device will register
	a key press every frame (good for movement keys).  However, if we want
	only one key press per time the key is pressed, then we must set a lock.
*/
bool Input::SetLock(char Num, bool State) {
	m_Locks[Num] = State;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////
//																				//
// --------------------------------- Keyboard --------------------------------- //
//																				//
//////////////////////////////////////////////////////////////////////////////////
Keyboard::~Keyboard() {
	delete [] m_Locks;
}
//////////////////////////////////////////////////////////////////////////////////
bool Keyboard::CreateDevice() {
	m_Locks = new bool[256];
	if(FAILED(m_Input->CreateDevice(GUID_SysKeyboard, &m_InputDevice, NULL))) {
		return false; 
	}
	if(FAILED(m_InputDevice->SetDataFormat(&c_dfDIKeyboard))) { 
		return false; 
	}
	
    return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Clears the status of the device.  Resets all locks and toggles.
void Keyboard::Clear() {
	ZeroMemory(&m_State, 256);
  
	for(int i=0;i<256;i++) {
		m_Locks[i]  = FALSE;
		m_Toggle[i] = FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////////
bool Keyboard::Read() {
	if(m_InputDevice == NULL) { return FALSE; }
	while(true) {
		// Poll the device.
		m_InputDevice->Poll();

		// if the poll is successful, we have the state. break.
		HRESULT hr;
		if(SUCCEEDED(hr = m_InputDevice->GetDeviceState(256, &m_State))) 
			break;

		if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED) { return false; }
		if(FAILED(m_InputDevice->Acquire())) { return false; }

	}

	for(int i=0;i<256;i++) {
		// If the keyboard button was released, then it should be unlocked.
		if(!(m_State[i] & 0x80)) {
			m_Locks[i] = FALSE;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the state of key Num.  If True, the key is pressed.
bool Keyboard::GetKeyState(char Num) const {
	if(m_State[Num] & 0x80 && m_Locks[Num] == false)
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the state of key Num to State.
void Keyboard::SetKeyState(char Num, bool State) {
	m_State[Num] = State;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the state of the key, disregarding any locks.
bool Keyboard::GetPureKeyState(char Num) const { return ((m_State[Num] & 0x80) ? true : false); }
//////////////////////////////////////////////////////////////////////////////////
/// Returns a keypress from the keyboard.
/** \param TimeOut Length of time to poll the keyboard.
	\return The number of the first found keypress, 0 if no key is found.
*/
short Keyboard::GetKeypress(long TimeOut) const {
	static HKL KeyboardLayout = GetKeyboardLayout(0); // layout of the keyboard.
	unsigned char WinKeyStates[256], DIKeyStates[256]; // arrays for the state of the keys.
	unsigned short i, ScanCode, VirtualKey, Keys, Num;
	unsigned long EndTime;
  
	// Make sure it's a keyboard and its initialized
	if(m_InputDevice == NULL) {
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
				}
			}

			// Check for TimeOut
			if(TimeOut) {
				if(GetTickCount() > EndTime) { return 0; }
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the number of keypresses, not including locks.
long Keyboard::GetNumKeyPresses() const {
	short i;
	long Num = 0;
	for (i=0; i<256; i++) {
		if(m_State[i] & 0x80 && m_Locks[i] == FALSE)
			Num++;
	}
	return Num;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the number of keypresses, including locks.
long Keyboard::GetNumPureKeyPresses() const {
	short i;
	long Num = 0;
	for(i=0; i<256; i++) {
		if(m_State[i] & 0x80)
			Num++;
	}
	return Num;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the toggle state of key Num.
/** See SetToggle() for more information on toggles. */
bool Keyboard::GetToggle(char Num) const {
	return m_Toggle[Num];
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the toggle stat of key Num.
/** Toggles are like locks (see SetLock() ) which stay until the key is pressed again.
*/
void Keyboard::SetToggle(char Num) {
	if(!m_Toggle[Num])
		m_Toggle[Num] = true;
	else 
		m_Toggle[Num] = false;
}
//////////////////////////////////////////////////////////////////////////////////
//																				//
// ----------------------------------- Mouse ---------------------------------- //
//																				//
//////////////////////////////////////////////////////////////////////////////////
Mouse::Mouse() : m_XPos(0), m_YPos(0) {
	
}
//////////////////////////////////////////////////////////////////////////////////
Mouse::~Mouse() {
	delete [] m_Locks;
}
//////////////////////////////////////////////////////////////////////////////////
bool Mouse::CreateDevice() {
	m_Locks = new bool[4];
	if(FAILED(m_Input->CreateDevice(GUID_SysMouse, &m_InputDevice, NULL))) {
		return false; 
	}
	if(FAILED(m_InputDevice->SetDataFormat(&c_dfDIMouse))) { 
		return false; 
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Clears the status of the device.  Resets all locks and toggles.
void Mouse::Clear() {
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
  
	for(int i=0;i<4;i++) {
		m_Locks[i]  = FALSE;
	}

	m_XPos = 0;
	m_YPos = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Reads the status of the input device, regardless of type.
/** This function should be called every game frame, before polling the device.
	\return TRUE if the read is successful.
*/
bool Mouse::Read() {
	if(m_InputDevice == NULL) { return FALSE; }
	while(true) {
		// Poll the device.
		m_InputDevice->Poll();

		// if the poll is successful, we have the state. break.
		HRESULT hr;
		if(SUCCEEDED(hr = m_InputDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState))) 
			break;

		if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED) { return false; }
		if(FAILED(m_InputDevice->Acquire())) { return false; }

	}
	
	if(m_Windowed) {
		POINT pointer;
		GetCursorPos(&pointer);
		ScreenToClient(m_hWnd, &pointer);
		m_XPos = pointer.x;
		m_YPos = pointer.y;
	} else {
		m_XPos += m_MouseState.lX;
		m_YPos += m_MouseState.lY;
	}

	for(int i=0;i<4;i++) {
		// if the mouse button is not pressed, then there is no lock.
		if(!(m_MouseState.rgbButtons[i])) {
			m_Locks[i] = false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the state of mouse button Num, not including locks.
bool Mouse::GetButtonState(char Num) const {
	return (m_MouseState.rgbButtons[Num] & 0x80 && !m_Locks[Num]);
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the state of mouse button Num to State.
void Mouse::SetButtonState(char Num, bool State) {
	m_MouseState.rgbButtons[Num] = State;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the state of mouse button Num, including any locks.
bool Mouse::GetPureButtonState(char Num) const {
	if(m_MouseState.rgbButtons[Num] & 0x80)
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the number of mouse buttons pressed, not including locked buttons.
long Mouse::GetNumButtonPresses() const {
	short i;
	long Num = 0;
	for(i=0; i<4; i++) {
		if(m_MouseState.rgbButtons[Num] & 0x80 && !m_Locks[Num])
			Num++;
	}
	return Num;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the number of mouse buttons pressed, including locked buttons.
long Mouse::GetNumPureButtonPresses() const {
	short i;
	long Num = 0;
	for(i=0; i<4; i++) {
		if(m_MouseState.rgbButtons[Num] & 0x80)
			Num++;
	}
	return Num;
}

/// Returns the X-Coordinate of the mouse.
long Mouse::GetXPos() const { return m_XPos; }
//////////////////////////////////////////////////////////////////////////////////	
/// Sets the X-Coordinate of the mouse.
void Mouse::SetXPos(long XPos) {
	m_XPos = XPos;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the Y-Coordinate of the mouse.
long Mouse::GetYPos() const { return m_YPos; }
//////////////////////////////////////////////////////////////////////////////////
/// Sets the Y-Coordinate of the mouse.
void Mouse::SetYPos(long YPos) {
	m_YPos = YPos;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the change in mouse position from last poll in the x direction.
/** DirectX Mice do not store absolute position.  They stor delta, which can
	be used to determine absolute screen position.
*/
long Mouse::GetXDelta() const { return m_MouseState.lX; }
//////////////////////////////////////////////////////////////////////////////////
/// Returns the change in mouse position from last poll in the y direction.
/** DirectX Mice do not store absolute position.  They stor delta, which can
	be used to determine absolute screen position.
*/
long Mouse::GetYDelta() const { return m_MouseState.lY; }
//////////////////////////////////////////////////////////////////////////////////