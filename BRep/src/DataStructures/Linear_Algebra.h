#pragma once
#include <math.h>
#include <immintrin.h>

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

#ifndef SMALL_NUMBER
#  define SMALL_NUMBER  0.0001f
#endif

struct Vector4Df
{
	//float x, y, z, w;
	union {//union btw array, floats to use the names and _m128 to use vetorial instructions
		struct { float x, y, z, w; };
		float _v[4];
		__m128 v128;
	};

	inline float lengthsq() const { return x * x + y * y + z * z; }
	inline float length() const { return sqrtf(lengthsq()); }
	inline void normalize() { float norm = length();  x /= norm; y /= norm; z /= norm; };
	inline Vector4Df to_unitary() const { float norm = 1.0f / length(); return Vector4Df{ x *norm, y * norm, z * norm, w }; }

	inline Vector4Df operator*(float a) const { return Vector4Df{ x*a, y*a, z*a, w }; }
	inline Vector4Df operator/(float a) const { return Vector4Df{ x/a, y/a, z/a, w }; }
	__forceinline Vector4Df operator*(const Vector4Df& v) const { return *(Vector4Df*)&_mm_mul_ps(v128, v.v128); }
	inline Vector4Df operator+(const Vector4Df& v) const { return Vector4Df{ x + v.x, y + v.y, z + v.z, w + v.w }; }
	inline Vector4Df operator-(const Vector4Df& v) const { return Vector4Df{ x - v.x, y - v.y, z - v.z, w - v.w }; }

	inline void operator+=(const Vector4Df& v) { x += v.x; y += v.y; z += v.z; }
	inline void operator-=(const Vector4Df& v) { x -= v.x; y -= v.y; z -= v.z; }

	bool operator==(const Vector4Df& v) const { return fabsf(x - v.x) < SMALL_NUMBER && fabsf(y - v.y) < SMALL_NUMBER && fabsf(z - v.z) < SMALL_NUMBER; }

	inline operator __m128() const { return v128; }
};

//dot PRODUCT
inline float dot(const Vector4Df& v1, const Vector4Df& v2)
{
	float d = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return d;
}
//dot product using vetorial instructions
__forceinline float dot_simd(const __m128& v1, const __m128& v2) { return _mm_cvtss_f32(_mm_dp_ps(v1, v2, 0xff)); }
__forceinline Vector4Df dot_simd_Vec(const __m128& v1, const __m128& v2) { return *(Vector4Df*)&_mm_dp_ps(v1, v2, 0xff); }

//reflection vector
inline Vector4Df reflection(const Vector4Df& v, const Vector4Df& mirror) { return (mirror * dot(mirror, v))*(2.0f) - v; }

//cross PRODUCT
inline Vector4Df cross(const Vector4Df& v1, const Vector4Df& v2)
{
	Vector4Df c = { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x, 0.0f };
	return c;
}

//cross product using vetorial instructions
//__forceinline __m128 cross_simd_mm128(const __m128& v1, const __m128& v2) {
//	const __m128 &v1_yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
//	const __m128 &v2_yzx = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
//	const __m128 &mul1 = _mm_mul_ps(v1, v2_yzx);
//	const __m128 &mul2 = _mm_mul_ps(v1_yzx, v2);
//	const __m128 &c = _mm_sub_ps(mul1, mul2);
//	return _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));
//}
//__forceinline Vector4Df cross_simd(const __m128& v1, const __m128& v2) {
//	return *(Vector4Df*)&cross_simd_mm128(v1, v2);
//}





struct Matrix4
{
	//Vector4Df row[4];
	union
	{
		float m[16];
		float _m[4][4];
		__m128 row[4];
	};

	__forceinline Vector4Df operator *(const Vector4Df &v) const { return Vector4Df{ dot_simd(v, row[0]), dot_simd(v, row[1]), dot_simd(v, row[2]), dot_simd(v, row[3]) }; }
};

inline Matrix4 translate(const Vector4Df &v)
{
	return Matrix4{
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1 };
}

inline Matrix4 rotateX(float ang)
{
	ang = (ang* M_PI) / 180.0f;

	return Matrix4{
	1,	0,	0,	0,
	0,	cosf(ang),	-sinf(ang),	0,
	0,	sinf(ang),	cosf(ang),	0,
	0,	0,	0,	1 };
}

inline Matrix4 rotateY(float ang)
{
	ang = (ang* M_PI) / 180.0f;

	return Matrix4{
		cosf(ang),	0,	sinf(ang),	0,
		0,	1,	0,	0,
		-sinf(ang),	0,	cosf(ang),	0,
		0,	0,	0,	1 };
}

inline Matrix4 rotateZ(float ang)
{
	ang = (ang* M_PI) / 180.0f;

	return Matrix4{
		cosf(ang),	-sinf(ang),	0,	0,
		sinf(ang),	cosf(ang),	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1 };

}

inline Matrix4 resizeMatrix(float x, float y, float z)
{
	return Matrix4{
	x,	0,	0,	0,
	0,	y,	0,	0,
	0,	0,	z,	0,
	0,	0,	0,	1 };
}

inline Matrix4 resizeMatrix(const Vector4Df& s)
{
	return Matrix4{
	s.x,0,	0,	0,
	0,	s.y,0,	0,
	0,	0,	s.z,0,
	0,	0,	0,	1 };
}

//ROTATION USING QUATERNIONS
inline Matrix4 rotateMatrix(Vector4Df u, float ang)
{
	u.normalize();
	ang = (ang* M_PI) / 180.0f;
	Vector4Df vq = u * sin(ang / 2.0f);
	float wq = cos(ang / 2.0f);

	return Matrix4{
	powf(vq.x, 2.0f) - powf(vq.y, 2.0f) - powf(vq.z, 2.0f) + powf(wq, 2.0f),	2.0f*vq.x*vq.y - 2.0f*vq.z*wq,	2.0f*vq.x*vq.z + 2.0f*vq.y*wq,	0,
	2.0f*vq.x*vq.y + 2.0f*vq.z*wq,	-powf(vq.x, 2.0f) + powf(vq.y, 2.0f) - powf(vq.z, 2.0f) + powf(wq, 2.0f),	2.0f*vq.y*vq.z - 2.0f*vq.x*wq,	0,
	2.0f*vq.x*vq.z - 2.0f*vq.y*wq,	2.0f*vq.y*vq.z + 2.0f*vq.x*wq,	-powf(vq.x, 2.0f) - powf(vq.y, 2.0f) + powf(vq.z, 2.0f) + powf(wq, 2.0f),	0,
								0,								0,																			0,	1 };
}