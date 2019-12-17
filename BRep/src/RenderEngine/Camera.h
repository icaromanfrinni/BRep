#pragma once
#include "Linear_Algebra.h"

struct float2
{
	float x, y;
};

struct Camera
{
	Vector4Df eye, lookat, up;
	float2 resolution, dimensions;
	float n; //near plane

	Camera()
	{
	}

	Camera(const Vector4Df &_position, const Vector4Df &_view, const Vector4Df &_up) : eye(_position), lookat(_view), up(_up)
	{
		resolution = { 512.0f, 512.0f };
		dimensions = float2{ 1.0f, 1.0f };
		n = 1.0f;
		up.normalize();
	}

	Camera(const Vector4Df &_position, const Vector4Df &_view, const Vector4Df &_up, const float2 &_resolution,
		const float2 &_dimensions, const float _n) : eye(_position), lookat(_view), up(_up), resolution(_resolution), dimensions(_dimensions), n(_n)
	{
		up.normalize();
	}

	void Transform(const Matrix4 &m)
	{
		eye = m * eye;
		lookat = m * lookat;
		up = m * up;
	}

	void Transform(const Matrix4 &m, const Vector4Df reference)
	{
		Transform(translate(Vector4Df{ -reference.x, -reference.y, -reference.z, 1.0f }));
		Transform(m);
		Transform(translate(Vector4Df{ reference.x, reference.y, reference.z, 1.0f }));
	}

	void newUp()
	{
		Vector4Df sagittalNormal = cross(lookat - eye, up).to_unitary();
		up = cross(sagittalNormal, lookat - eye).to_unitary();
	}
};

inline Matrix4 ToWorld(const Vector4Df &position, const Vector4Df &lookat, const Vector4Df &vup) {
	const Vector4Df& k = (position - lookat).to_unitary();
	const Vector4Df& i = cross(vup, k).to_unitary();
	const Vector4Df& j = cross(k, i).to_unitary();

	Matrix4 m;
	m.row[0] = { i.x, j.x, k.x, position.x };
	m.row[1] = { i.y, j.y, k.y, position.y };
	m.row[2] = { i.z, j.z, k.z, position.z };
	m.row[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	return m;
}
inline Matrix4 ToWorld(const Camera &cam) {
	return ToWorld(cam.eye, cam.lookat, cam.up);
}

inline Matrix4 ToCamera(const Vector4Df &position, const Vector4Df &lookat, const Vector4Df &vup) {
	const Vector4Df& k = (position - lookat).to_unitary();
	const Vector4Df& i = cross(vup, k).to_unitary();
	const Vector4Df& j = cross(k, i).to_unitary();;

	Matrix4 m;
	m.row[0] = { i.x, i.y, i.z, -dot(i, position) };
	m.row[1] = { j.x, j.y, j.z, -dot(j, position) };
	m.row[2] = { k.x, k.y, k.z, -dot(k, position) };
	m.row[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	return m;
}
inline Matrix4 ToCamera(const Camera &cam) {
	return ToCamera(cam.eye, cam.lookat, cam.up);
}