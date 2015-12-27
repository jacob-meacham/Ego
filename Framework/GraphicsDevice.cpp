#include "GraphicsDevice.h"
#include <stdio.h>
#include <sstream>
//////////////////////////////////////////////////////////////////////////////////
//																				//
// ------------------------------ GraphicsDevice ------------------------------ //
//																				//
//////////////////////////////////////////////////////////////////////////////////
GraphicsDevice gGraphics;

GraphicsDevice::GraphicsDevice() : pd3d(NULL), pd3dDevice(NULL)
{
	initialize();
}
//////////////////////////////////////////////////////////////////////////////////
bool GraphicsDevice::initialize() 
{
	pd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(!pd3d) 
	{ 
		OutputDebugString("Could not initialize GraphicsDevice.");
		return false; 
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
bool GraphicsDevice::setGraphics(HWND hwnd, bool windowed, u32 width, u32 height) 
{	
	if(!hwnd) return false;
	if(!pd3d) return false;

	D3DDISPLAYMODE d3dDisplay;
	if(FAILED(pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplay)))
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	D3DFORMAT BPP;

	if(windowed) //If it is windowed, use the current resolution settings.
		BPP = d3dDisplay.Format;
	else //otherwise use the typical resolution format (ie 32 bit).
	{ 
		BPP = D3DFMT_X8R8G8B8;
		if(!checkFormat(BPP, windowed)) 
			return false;
	}
	
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	// These are standard D3DPRESENT_PARAMETERS settings.
	if(windowed) 
	{
		d3dpp.Windowed = true;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = BPP;
	}
	else
	{
		d3dpp.Windowed = false;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.BackBufferWidth  = width;
		d3dpp.BackBufferHeight = height;
		d3dpp.BackBufferFormat = BPP;
		d3dpp.hDeviceWindow = hwnd;
	}
	
	//Create device, using software vertex processing and HAL.
	HRESULT res = pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &pd3dDevice);

	if(FAILED(res)) { return false; }

	//Create Sprite interface, using newly minted device.
	if(FAILED(D3DXCreateSprite(pd3dDevice, &pSprite))) { return false; }

	// if all creations are successful, return true.
	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Function used to check whether a particular format is supported by the hardwore and/or software emulation.
bool GraphicsDevice::checkFormat(const D3DFORMAT & format, bool windowed) const
{
  if(FAILED(pd3d->CheckDeviceType(D3DADAPTER_DEFAULT, 
              D3DDEVTYPE_HAL, format, format, windowed)))
    return false;

  return true;
}
//////////////////////////////////////////////////////////////////////////////////
void GraphicsDevice::shutdown() 
{
	DX_RELEASE(pSprite); 
	DX_RELEASE(pd3dDevice);
	DX_RELEASE(pd3d);
}
//////////////////////////////////////////////////////////////////////////////////
/// Sets the graphic device up so that it is ready to render a frame.
/** NOTE: Should be coupled with a call to EndScene() after any rendering is complete.
*/
bool GraphicsDevice::beginScene() 
{
	if (!pd3dDevice) { return false; }

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	if (FAILED(pd3dDevice->BeginScene())) { return false; }

	if (!pSprite) { return false; }

	if (FAILED(pSprite->Begin(D3DXSPRITE_ALPHABLEND))) { return false; }

	clear();

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Returns the graphic device to its former state after a call to BeginScene()
/** NOTE: Should be coupled with a call to BeginScene().
*/
bool GraphicsDevice::endScene() 
{
	bool result = true;
	// Sprite couldn't have been initialized if this is false
	if (!pd3dDevice) { return false; }

	if (pSprite) {
		if (FAILED(pSprite->End())) {
			result = false;
		}
	}

	if (FAILED(pd3dDevice->EndScene())) {
		result = false;
	}
	
	if (!display()) {
		result = false;
	}

	return result;
}
//////////////////////////////////////////////////////////////////////////////////
/// Presents the backbuffer, and anything rendered to it.
/** \return TRUE if the backbuffer is flipped successfully.
*/
bool GraphicsDevice::display()
{
	if(!pd3dDevice) 
		return false;

	if(FAILED(pd3dDevice->Present(NULL, NULL, NULL, NULL))) 
		return false;

  return true;
}
//////////////////////////////////////////////////////////////////////////////////
/// Clears the backbuffer
bool GraphicsDevice::clear(Color color, float zbuffer)
{
	if(!pd3dDevice)
		return false;

	if(FAILED(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, color, zbuffer, 0))) 
		return false;

  return true; 
}