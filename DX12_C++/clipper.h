#pragma once
#if !defined(CLIPPER_H)


#define CLIP_MAX_NUM_VERTICES 128
#define W_CLIPPING_VALUE 0.0001f

enum clip_axis
{
	ClipAxis_None,
	ClipAxis_Left,
	ClipAxis_Right,
	ClipAxis_Top,
	ClipAxis_Bottom,
	ClipAxis_Far,
	ClipAxis_Near,
	ClipAxis_W,
};

enum NumBehindPlane
{
	Triangle_Is_Visible = 0,
	Smaler_Part_Triangle_Invisible = 1,
	Most_Of_Triangle_Invisible = 2,
	Triangle_Is_Invisible = 3,
};

struct clip_vertex
{
	v4 Pos;
	v2 Uv;
};

struct clip_result
{
	u32 NumTriangles;
	clip_vertex Vertices[CLIP_MAX_NUM_VERTICES];
};

#define CLIPPER_H
#endif