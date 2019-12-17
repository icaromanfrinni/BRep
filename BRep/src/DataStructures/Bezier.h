#pragma once
#include <vector>
#include <iostream>

#include "Linear_Algebra.h"

namespace CURVE
{
	struct Bezier
	{
		int segments;
		std::vector<Vector4Df> control_points;
		bool enable;

		Bezier()
		{
			enable = false;
			segments = 10;
			control_points.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
			control_points.push_back({ 2.0f, 2.0f, 1.0f, 1.0f });
			control_points.push_back({ 1.0f, 2.0f, 3.0f, 1.0f });
			control_points.push_back({ 4.0f, 1.0f, 4.0f, 1.0f });
		}
	};
}
