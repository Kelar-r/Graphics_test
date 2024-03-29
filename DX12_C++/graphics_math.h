#pragma once
#if !defined(GRAPHICS_MATH_H)

global f32 Pi32 = 3.141592653589793238462643383279502884f;

union v2 
{
	struct 
	{
		f32 x, y;
	};


	f32 e[2];
};


union v2i
{
	struct
	{
		i32 x, y;
	};


	i32 e[2];
};

union v3
{
	struct
	{
		f32 x, y, z;
	};

	struct
	{
		f32 r, g, b;
	};

	struct
	{
		v2 xy;
		f32 ignored0;
	};

	struct
	{
		f32 ignored1;
		v2 yz;
	};


	f32 e[3];
};

union v4
{
	struct
	{
		f32 x, y, z, w;
	};

	struct
	{
		f32 r, g, b, a;
	};

	struct
	{
		v3 xyz;
		f32 ignored0;
	};

	struct
	{
		v2 xy;
		f32 ignored1;
	};

	f32 e[4];
};


union m4
{
	v4 v[4];
	f32 e[16];
};

#define GRAPHICS_MATH_H
#endif