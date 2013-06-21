#include "System_Core.h"
#define ErrorMacro(x) MessageBox(NULL, x, "Error", MB_OK);

/// Default Constructor.  Fills in the window class structure.
SystemCore::SystemCore() {

	// Save instance handle.
	g_SystemCore = this;

	// Get the new instance handle.
	m_hInst = GetModuleHandle(NULL);

	// Set the default name and caption
	strcpy_s(m_WndName, "Skeleton");
	strcpy_s(m_WndCaption, "Skeleton App");

	// Set default style, position, and dimensions
	m_Style  = WS_OVERLAPPEDWINDOW;
	m_XPos   = 200;
	m_YPos   = 200;
	m_Width  = 256;
	m_Height = 256;

//create standard extended window	
m_wcex.cbSize        = sizeof(WNDCLASSEX);
m_wcex.style         = CS_CLASSDC;
m_wcex.lpfnWndProc   = WindowProc;
m_wcex.cbClsExtra    = 0;
m_wcex.cbWndExtra    = 0;
m_wcex.hInstance     = m_hInst;
m_wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION); // standard icon
m_wcex.hCursor       = NULL;
m_wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH); // black background
m_wcex.lpszMenuName  = NULL;
m_wcex.lpszClassName = m_WndName;
m_wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION); // standard icon
}

/// Main SystemCore entry point.
/** This function registers and creates the window, then enters the
	windows message pump.
	\return FALSE if something goes wrong, TRUE if the application successfully exits.
*/
BOOL SystemCore::Run() {
	
	//register class
	if(!RegisterClassEx(&m_wcex)) {
		Error("Couldn't Register Class", TRUE);
		return FALSE;
	}
	
	//Create window, using set variables.
	hWnd = CreateWindow(m_WndName, m_WndCaption, m_Style, m_XPos, m_YPos, m_Width, m_Height, NULL, NULL, m_hInst, NULL);
	
	if(!hWnd) {
		Error("Couldn't Create Window", TRUE);
		return FALSE;
	}

	// show, and update window
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	
	//Entering message pump
	if(Init() == TRUE) { // run application specific Init function.
		
		ZeroMemory(&msg, sizeof(MSG));

		// While the message is not a destroy window message.
		while(msg.message != WM_QUIT) {
			// Handle any Windows messages.
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				// Perform PerFrame() processing, break if PerFrame() returns false.
				if(PerFrame() == FALSE) { break; }
			} // end else
		} // end while
	} // end if

	// Shutdown window
	Shutdown();
	// Unregister window
	UnregisterClass(m_WndName, m_hInst);

	return TRUE;

}

/// Moves the window to Xpos, Ypos
BOOL SystemCore::Move(long XPos, long YPos) {
	RECT ClientRect;

	GetClientRect(hWnd, &ClientRect);
	MoveWindow(hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);

	return TRUE;
}

/// Resizes the window to Width, Height.
BOOL SystemCore::Resize(long Width, long Height) {
	RECT WndRect, ClientRect;
	long WndWidth, WndHeight;

	GetWindowRect(hWnd, &WndRect);
	GetClientRect(hWnd, &ClientRect);

	// Find new width, using the window rectangle and client rectangle.
	WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;

	// Find new height, using the window rectangle and client rectangle.
	WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;

	// Move Window can also resize the window.
	MoveWindow(hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);

	return TRUE;
}

/// Shows or Hides the mouse in the window.
BOOL SystemCore::ShowMouse(BOOL Show) {
	ShowCursor(Show);
	return TRUE;
}

/// Error MessageBox.
/** \param text The text to print to the message box
	\param Fatal if TRUE, the application exits.
*/
void SystemCore::Error(char text[], BOOL Fatal) {
	char error[200];
	strcpy_s(error, text);
	if(Fatal) {
		MessageBox(NULL, error, "Fatal Error", MB_OK);
		PostQuitMessage(0);
	}
	else {
		MessageBox(NULL, error, "Error", MB_OK);
	}
}
/// Returns the window handle.
HWND SystemCore::GetHWnd() { return hWnd; }

/// Returns the Instance handle.
HINSTANCE SystemCore::GetHInst() { return m_hInst; }

/// The standard message procedure.  returns the class message procedure, except for the case of a destroy message.
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
		case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		default: return g_SystemCore->MsgProc(hWnd, uMsg, wParam, lParam);
  }
}

/// Default Constructor.
DataPackage::DataPackage() { if(m_Buf) m_Buf = NULL; m_Size = 0; }

/// Default Destructor.
DataPackage::~DataPackage() { Free(); }

/// Creates a new DataPackage of size Size.
/** Example usage, for a char* data package:
	DataPackage DP;
	char* output = (char *)DP.Create(dataSize);
	output = "Foo";
	DP.Save("output.dat");
	\return A void* to the data buffer
*/
void *DataPackage::Create(unsigned long Size) {
	// free a previous buffer.
	Free();
	
	// set internal size
	m_Size = Size;

	// create a new char array, cast to a void*
	m_Buf = (void*)new char[m_Size];
	return m_Buf;
}

/// Frees the data package's allocated memory
void DataPackage::Free() {
	delete m_Buf;
	m_Buf = NULL;
	m_Size = 0;
}

/// Saves the data package to the specified filename.
BOOL DataPackage::Save(char *Filename) {
	// File pointer
	FILE *fp;
	// as long as both the buffer and a size exist, we can write.
	if(m_Buf != NULL && m_Size) {
		fopen_s(&fp, Filename, "wb");
		if(fp != NULL) {
			// first write the size of the data.
			fwrite(&m_Size, 1, 4, fp);
			// then write the data.
			fwrite(m_Buf, sizeof(char), m_Size, fp);
			fclose(fp);
			return TRUE;
		} // end if(fp...)
	} // end if(m_Buf...)
	return FALSE;
}

/// Loads a data package from the specified file name.
/** Load can load any data package save by DataPackage.
	\param Filename The name of the file to load
	\param Size A pass-by-reference variable that, when Load
	completes, holds the size of the data package.
	\return a void* to the data package (cast it to the correct type)
*/
void *DataPackage::Load(char *Filename, unsigned long *Size) {
	FILE *fp;

	// Free a prior buffer
	Free();

	fopen_s(&fp, Filename, "rb");
	if(fp != NULL) {
		// Read in size and data
		fread(&m_Size, 1, 4, fp);
		// create buffer of size m_Size
		if((m_Buf = (void*)new char[m_Size]) != NULL) {
			fread(m_Buf, sizeof(char), m_Size, fp);
		}
		fclose(fp);

		// Store size to return
		if(Size != NULL) {
			*Size = m_Size;
		}

		// return pointer
		return m_Buf;
	}
	// if the file cannot be opened, return a null package.
	ErrorMacro("DataPackage is NULL!");
	return NULL;
}

/// Return the void* to the data buffer.
void *DataPackage::GetPtr()
{
return m_Buf;
}

/// Return the size of the data buffer.
unsigned long DataPackage::GetSize()
{
return m_Size;
}

/*
StateManager::StateManager() { 
	m_StateParent = NULL; 
}

StateManager::~StateManager() {
	PopAll();
}

// Push a function on to the stack
void StateManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr) {
	sState *StatePtr;

	// Don't push a NULL value
	if(Function != NULL) {
		// Allocate a new state and push it on stack
		StatePtr = new sState();

		StatePtr->Function = Function;
		StatePtr->Next = m_StateParent;

		m_StateParent = StatePtr;

		// Call state with init purpose
		StatePtr->Function(DataPtr, INITPURPOSE);
	}
}

BOOL StateManager::Pop(void *DataPtr) {
	sState *StatePtr;

	// Remove the head of stack (if any)
	if((StatePtr = m_StateParent) != NULL) {
		// First call with shutdown purpose
		m_StateParent->Function(DataPtr, SHUTDOWNPURPOSE);

		m_StateParent = StatePtr->Next;
		StatePtr->Next = NULL;
		delete StatePtr;
	}

	// return TRUE if more states exist, FALSE otherwise
	if(m_StateParent == NULL) { return FALSE; }
	return TRUE;
}

void StateManager::PopAll(void *DataPtr) {
	while(Pop(DataPtr) == TRUE);
}

BOOL StateManager::Process(void *DataPtr) { 
	// return an error if no more states
	if(m_StateParent == NULL) { return FALSE; }

	// Process the top-most state
	m_StateParent->Function(DataPtr, FRAMEPURPOSE); 

	return TRUE;
}
*/