#ifndef _systemCore_h_included_
#define _systemCore_h_included_
#include <windows.h>
#include <stdio.h>

/// Core window handling class.
/** This class is the interface between Windows and the game.  It handles creation of the window
	as well as the message pump.  The class is based off of code found in Jim Adams'
	Programming Role Playing Games with DirectX.
*/
class SystemCore {
	
	private:
		HINSTANCE		m_hInst; ///< Instance handle.
		HWND			hWnd;	///< Window handle.
		MSG				msg;    ///< Message handle.

	protected:
		char			m_WndName[MAX_PATH]; ///< Window name.
		char			m_WndCaption[MAX_PATH]; ///< Window caption.

		WNDCLASSEX		m_wcex; ///< Extended window structure.

		DWORD			m_Style; ///< Window Style.
		DWORD			m_XPos; ///< X-Coordinate of the top-left corner of the window.
		DWORD			m_YPos; ///< Y-Coordinate of the top-left corner of the window.
		DWORD			m_Width; ///< Width of window.
		DWORD			m_Height; ///< Height of window.

	public:
		
		//Constructor
		SystemCore(); 
				
		//Main function
		BOOL Run();
		
		//Moves window to {XPos, YPos}
		BOOL Move(long XPos, long YPos); 
		
		 // Resizes Window
		BOOL Resize(long Width, long Height);
		
		// Show/hide Mouse
		BOOL ShowMouse(BOOL Show = TRUE); 
		
		//Error messagebox
		void Error(char	text[], BOOL Fatal); 

		//Getters.
		HWND GetHWnd();
		HINSTANCE GetHInst();

		
		// Virtual functions written for each class that utilizes the SystemCore

		/// Virtual function called when a message needs translation.
		virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hWnd, Msg, wParam, lParam); }
		
		/// Virtual function called when the application initializes.
		virtual BOOL Init() { return TRUE; }

		/// Virtual function called each frame of the application.
		virtual BOOL PerFrame() { return TRUE; }

		/// Virtual function called when the application is shutdown.
		virtual BOOL Shutdown() { return TRUE; }
		
};

/// A pointer to the main SystemCore.
static SystemCore *g_SystemCore = NULL;
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/// Data packaging manager, for saving game data.
/** This class creates a void buffer and can save any array or custom structure.
	It can then load this same data back in, including the size of the data.
*/
class DataPackage {
	protected:
		void          *m_Buf; ///< buffer for loaded/saved data.
		unsigned long  m_Size; ///< size of the data buffer.

	public:
		DataPackage();
		~DataPackage();

		void *Create(unsigned long Size);
		void Free();

		BOOL Save(char *Filename);
		void *Load(char *Filename, unsigned long *Size);

		void *GetPtr();
		unsigned long  GetSize();
}; 

/*
enum Purposes {
    NOPURPOSE = 0,
    INITPURPOSE,
    SHUTDOWNPURPOSE,
    FRAMEPURPOSE
  };
  
class StateManager
{
  typedef struct sState {
    void (*Function)(void *Ptr, long Purpose);
    sState *Next;

    sState()
    {
      Function = NULL;
      Next = NULL;
    }

    ~sState()
    {
      delete Next;
    }
  } sState;

  protected:
    sState *m_StateParent;

  public:
    StateManager();
    ~StateManager();

    void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
    BOOL Pop(void *DataPtr = NULL);
    void PopAll(void *DataPtr = NULL);
    BOOL Process(void *DataPtr = NULL);
};
*/
#endif