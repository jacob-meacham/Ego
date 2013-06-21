#pragma once

#include <string>
#include <windows.h>
#include <assert.h>

#define LEAN_AND_MEAN

#define DX_RELEASE(x) if(x) { x->Release(); x = NULL; }

#define ASSERT(x) assert(x)
#define ASSERT_FAILED assert(0)

#ifdef _DEBUG
static void TRACE(const char * pformat, ...)
{
	char buf[4096], *p = buf;
	
	va_list args;
    va_start(args, pformat);
    int n = vsnprintf_s(p, sizeof(buf) - 1, sizeof(buf) - 1, pformat, args);
    va_end(args);

	if(n < 0)
	{
		p += sizeof(buf) - 1;
		p = '\0';
	}
    
	OutputDebugString(buf);
}
#else
static void TRACE(const char * pformat, ...) { }
#endif