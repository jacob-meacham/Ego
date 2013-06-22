#pragma once

#include "DXUtil.h"
#include "Define.h"
#include "Math.h"
//////////////////////////////////////////////////////////////////////////////////
//																				//
// ------------------------------ GraphicsDevice ------------------------------ //
//																				//
//////////////////////////////////////////////////////////////////////////////////
/// Main DirectX graphic class.
/** This class is used to set the directX devices, as well as to render
frames.  The class is based off of code found in Jim Adams'
Programming Role Playing Games with DirectX.
*/

const DWORD D3DFVF_VERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;
struct FVFVERTEX {
			FLOAT x, y, z;
			FLOAT rhw;
			FLOAT u, v;
};

class GraphicsDevice {
	protected:
		HWND					hWnd; ///< Handle to rendering window.
		ID3DXSprite	*			pSprite; ///< Sprite Interface.
		IDirect3D9	*			pd3d; ///< Main DirectX Interface.
		IDirect3DDevice9 *		pd3dDevice; ///< Hardware device Interface.
		D3DDISPLAYMODE			d3dDisplay; ///< DirectX Display structure.
		D3DPRESENT_PARAMETERS	d3dpp; ///< DirectX Paramaters structure.
		bool					bWindowed; ///< boolean for windowed mode.
		RECT					ClientRect; ///< Rectangle which defines the window.
		u32						nWidth; ///< Window width.
		u32						nHeight; ///< Window height.
		D3DFORMAT				BPP; ///< DirectX Format structure.

		// Determines the format is supported by the hardware.
		bool checkFormat(const D3DFORMAT & format, bool windowed) const;
		bool initialize();

	public:
		GraphicsDevice();
		
		// Sets the hardware graphics mode.
		bool setGraphics(HWND hwnd, bool windowed, u32 width, u32 height);
		void shutdown();
		
		bool beginScene();
		bool endScene();

		bool beginSprite();
		bool endSprite();

		// Blit to screen.
		bool display();

		// Clear backbuffer.
		bool clear(Color color = D3DCOLOR_RGBA(0, 0, 0, 0), float zbuffer = 1.0f);

		IDirect3DDevice9 * getDeviceCOM() { return pd3dDevice; }
		ID3DXSprite * getSpriteCOM() { return pSprite; }
};

extern GraphicsDevice gGraphics;



