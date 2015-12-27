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
		ID3DXSprite	*			pSprite; ///< Sprite Interface.
		IDirect3D9	*			pd3d; ///< Main DirectX Interface.
		IDirect3DDevice9 *		pd3dDevice; ///< Hardware device Interface.

		// Determines the format is supported by the hardware.
		bool checkFormat(const D3DFORMAT & format, bool windowed) const;
		bool initialize();

		// Clear backbuffer.
		bool clear(Color color = 0x00000000, float zbuffer = 1.0f);

		// Blit to screen.
		bool display();
	public:
		GraphicsDevice();
		
		// Sets the hardware graphics mode.
		bool setGraphics(HWND hwnd, bool windowed, u32 width, u32 height);
		void shutdown();
		
		bool beginScene();
		bool endScene();

		IDirect3DDevice9 * getDevice() { return pd3dDevice; }
		ID3DXSprite * getSpriteRenderer() { return pSprite; }
};

extern GraphicsDevice gGraphics;



