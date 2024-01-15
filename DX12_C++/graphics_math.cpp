#include "win32_graphics.h"

//
// NOTE: V2
//

v2 V2(f32 X, f32 Y)
{
    v2 Result;

    Result.x = X;
    Result.y = Y;

    return Result;
}

v2 V2(u32 X, u32 Y)
{
    v2 Result;

    Result.x = (f32)X;
    Result.y = (f32)Y;

    return Result;
}

v2 operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return Result;
}

v2 operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return Result;
}

v2 operator*(f32 B, v2 A)
{
    v2 Result;

    Result.x = A.x * B;
    Result.y = A.y * B;

    return Result;
}

v2 operator*(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x * B.x;
    Result.y = A.y * B.y;

    return Result;
}

v2 operator/(v2 A, f32 B)
{
    v2 Result;

    Result.x = A.x / B;
    Result.y = A.y / B;

    return Result;
}

//
// NOTE: V3 
//

inline v3 V3(f32 X, f32 Y, f32 Z)
{
    v3 Result;

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return Result;
}

v3 operator+(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;

    return Result;
}

v3 operator*(f32 A, v3 B)
{
    v3 Result = {};
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return Result;
}

v3 operator*(v3 B, f32 A)
{
    v3 Result = {};
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return Result;
}


//
// NOTE: V4
//

v4 V4(v3 A, f32 W)
{
    v4 Result = {};
    Result.xyz = A;
    Result.w = W;
    return Result;
}


m4 IdentityM4()
{
    m4 Result = {};
    Result.v[0].x = 1.0f;
    Result.v[1].y = 1.0f;
    Result.v[2].z = 1.0f;
    Result.v[3].w = 1.0f;
    return Result;
}

m4 ScaleMatrix(f32 X, f32 Y, f32 Z)
{
    m4 Result = IdentityM4();
    Result.v[0].x = X;
    Result.v[1].y = Y;
    Result.v[2].z = Z;
    return Result;
}


m4 TranslationMatrix(f32 X, f32 Y, f32 Z)
{
    m4 Result = IdentityM4();
    Result.v[3].xyz = V3(X, Y, Z);
    return Result;
}


v4 operator+(v4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;
    return Result;
} 


v4 operator*(v4 A, f32 B)
{
    v4 Result = {};
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;
    return Result;
}


//
// NOTE: M4
//

v4 operator*(m4 A, v4 B)
{
    v4 Result = A.v[0] * B.x + A.v[1] * B.y + A.v[2] * B.z + A.v[3] * B.w;
    return Result;
}


m4 operator*(m4 A, m4 B)
{
    m4 Result = {};
    Result.v[0] = A * B.v[0];
    Result.v[1] = A * B.v[1];
    Result.v[2] = A * B.v[2];
    Result.v[3] = A * B.v[3];
    return Result;
}


m4 RotationMatrix(f32 X, f32 Y, f32 Z)
{
    m4 Result = {};

    m4 RotateX = IdentityM4();
    RotateX.v[1].y = cos(X);
    RotateX.v[2].y = -sin(X);
    RotateX.v[1].z = sin(X);
    RotateX.v[2].z = cos(X);

    m4 RotateY = IdentityM4();
    RotateY.v[0].x = cos(Y);
    RotateY.v[2].x = -sin(Y);
    RotateY.v[0].z = sin(Y);
    RotateY.v[2].z = cos(Y);

    m4 RotateZ = IdentityM4();
    RotateZ.v[0].x = cos(Z);
    RotateZ.v[1].x = -sin(Z);
    RotateZ.v[0].y = sin(Z);
    RotateZ.v[1].y = cos(Z);


    Result = RotateZ * RotateY * RotateX;
    return Result;
}