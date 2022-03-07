#pragma once

#include <Windows.h>
//#include <DirectXMath.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/type_ptr.hpp"




class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

    static int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);




	/*static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius*sinf(phi)*cosf(theta),
			radius*cosf(phi),
			radius*sinf(phi)*sinf(theta),
			1.0f);
	}*/



	static glm::uvec4 SphericalToCartesianglm(float radius, float theta, float phi)
	{
		return glm::vec4(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}






 //   static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	//{
	//	// Inverse-transpose is just applied to normals.  So zero out 
	//	// translation row so that it doesn't get into our inverse-transpose
	//	// calculation--we don't want the inverse-transpose of the translation.
 //       DirectX::XMMATRIX A = M;
 //       A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

 //       DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
 //       return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	//}


    /*static DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }*/

	static glm::mat4x4 Identity4x4glm()
	{
		static glm::mat4x4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		return I;
	}


	static glm::vec4 glmVecZero()
	{
		static glm::vec4  V(0.0f, 0.0f, 0.0f, 0.0f);
		return V;
	}


	/*static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;*/



	static const float Piglm;
	static const float Infinityglm;
	static const float XM_PIDIV4glm;


};

