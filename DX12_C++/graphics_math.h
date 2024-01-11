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

#define GRAPHICS_MATH_H
#endif