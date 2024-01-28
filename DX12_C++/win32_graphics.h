#if !defined(WIN32_GRAPHICS_H)

#include <cmath>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <stdio.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t mm;
typedef uintptr_t umm;

typedef int32_t b32;

#define global static
#define local_global static;

#define snpintf _snpintf_s
#define Assert(Expression) if (!(Expression)) {__debugbreak();}
#define InvalideCodePatch Assert(!"Invalide Code Path")
#define ArrayCount(Array) (sizeof(Array)) / sizeof((Array)[0])

#define KiloBytes(Val) ((Val)*1024LL)
#define MegaBytes(Val) (KiloBytes(Val)*1024LL)
#define GigaBytes(Val) (MegaBytes(Val)*1024LL)
#define TeraBytes(Val) (GigaBytes(Val)*1024LL)

#include "graphics_math.h"

struct texture
{
	u32 Width;
	u32 Height;
	u32* Texels;
};


enum sampler_type
{
	Sampler_Type_None,
	Sampler_Type_Nearest,
	Sampler_Type_Bilinear,
};


struct sampler 
{
	sampler_type Type;
	u32 BorderColor;
};

struct camera 
{
	v3 Pos;

	f32 Yaw;
	f32 Pitch;

	b32 PrevMouseDown;
	v2 PrevMousePos;
};

struct global_state
{
	b32 isRunnig;
	HWND WindowHandle;
	HDC DeviceContext;
	u32 FrameBufferWidth;
	u32 FrameBufferHeight;
	u32* FrameBufferPixels;
	f32* DepthBuffer;

	f32 CurrTime;
	f32 CurrAnimation;

	b32 WDown;
	b32 ADown;
	b32 SDown;
	b32 DDown;
	b32 ShiftDown;
	b32 SpaceDown;
	camera Camera;
};

#define WIN32_GRAPHICS_H
#endif



