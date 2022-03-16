#pragma once

struct FVector
{
	float x;
	float y;
	float z;
};

struct FVector2
{
	float u;
	float v;
};

struct FVector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Location
{
	float x;
	float y;
	float z;
};

struct Rotation
{
	float pitch;
	float yaw;
	float roll;
	float w;
};

struct Scale
{
	float x;
	float y;
	float z;
};

struct FTransform
{
	Rotation rotation;
	Location location;
	Scale scale;
};