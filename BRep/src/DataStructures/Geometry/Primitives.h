#pragma once
#include "HalfEdge.h"
#include "EulerOp.h"

namespace Primitive
{
	void Plane(std::vector<HED::solid*>& model)
	{
		//Step 1 - First vertex
		EulerOp::mvfs(model, { 1.0f, 0.0f, 1.0f, 1.0f });
		model.back()->name = "Plane";
		//Step 2 - First edge / Second vertex
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { -1.0f, 0.0f, 1.0f, 1.0f });
		//Step 3 - Another edge and vertex
		EulerOp::mev(model.back()->halfEdges[0], NULL, 1, { -1.0f, 0.0f, -1.0f, 1.0f });
		//Step 4 - Another edge and vertex
		EulerOp::mev(model.back()->halfEdges[2], NULL, 2, { 1.0f, 0.0f, -1.0f, 1.0f });
		//Step 5 - First face and another edge
		EulerOp::mef(model.back()->halfEdges[5], model.back()->halfEdges[0], 0);
	}

	void Circle(std::vector<HED::solid*>& model)
	{
		int ratio = 24;
		float delta = -M_PI * (360.0f / ratio) / 180.0f;
		float angle = 0.0f;

		/* TOP FACE */
		//Step 1 - First vertex
		EulerOp::mvfs(model, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		model.back()->name = "Circle";
		//Step 2 - First edge / Second vertex
		angle += delta;
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		//Step 3 - 10x MEV
		for (int vtx = 1; vtx < ratio - 1; vtx++)
		{
			angle += delta;
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 2], NULL, vtx, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		}
		//Step 4 - MEF
		EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges[0], 0);
	}

	void Cube(std::vector<HED::solid*> &model)
	{
		/* FIRST FACE */
		//Step 1 - First vertex
		EulerOp::mvfs(model, { 1.0f, 1.0f, 1.0f, 1.0f });
		model.back()->name = "Cube";
		//Step 2 - First edge / Second vertex
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { -1.0f, 1.0f, 1.0f, 1.0f });
		//Step 3 - Another edge and vertex
		EulerOp::mev(model.back()->halfEdges[0], NULL, 1, { -1.0f, 1.0f, -1.0f, 1.0f });
		//Step 4 - Another edge and vertex
		EulerOp::mev(model.back()->halfEdges[2], NULL, 2, { 1.0f, 1.0f, -1.0f, 1.0f });
		////Step 5 - First face and another edge
		EulerOp::mef(model.back()->halfEdges[5], model.back()->halfEdges[0], 0);

		/* EDGES */
		//Step 6 - 4x MEV
		EulerOp::mev(model.back()->halfEdges[7], NULL, 0, { 1.0f, -1.0f, 1.0f, 1.0f });
		EulerOp::mev(model.back()->halfEdges[0], NULL, 1, { -1.0f, -1.0f, 1.0f, 1.0f });
		EulerOp::mev(model.back()->halfEdges[2], NULL, 2, { -1.0f, -1.0f, -1.0f, 1.0f });
		EulerOp::mev(model.back()->halfEdges[4], NULL, 3, { 1.0f, -1.0f, -1.0f, 1.0f });

		/* FACES */
		//Step 7 - 4x MEF
		EulerOp::mef(model.back()->halfEdges[9], model.back()->halfEdges[11], 0);
		EulerOp::mef(model.back()->halfEdges[11], model.back()->halfEdges[13], 0);
		EulerOp::mef(model.back()->halfEdges[13], model.back()->halfEdges[15], 0);
		EulerOp::mef(model.back()->halfEdges[15], model.back()->halfEdges[17], 0);
	}

	void Cylinder(std::vector<HED::solid*>& model)
	{
		int ratio = 24;
		float delta = -M_PI * (360.0f / ratio) / 180.0f;
		float angle = 0.0f;

		/* TOP FACE */
		//Step 1 - First vertex
		EulerOp::mvfs(model, { sinf(angle), 1.0f, cosf(angle), 1.0f });
		model.back()->name = "Cylinder";
		//Step 2 - First edge / Second vertex
		angle += delta;
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { sinf(angle), 1.0f, cosf(angle), 1.0f });
		//Step 3 - 10x MEV
		for (int vtx = 1; vtx < ratio - 1; vtx++)
		{
			angle += delta;
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 2], NULL, vtx, { sinf(angle), 1.0f, cosf(angle), 1.0f });
		}
		//Step 4 - MEF
		EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges[0], 0);

		/* EDGES AND FACES */
		//Step 5 - 1x MEV
		angle = 0.0f;
		EulerOp::mev(model.back()->halfEdges.back(), NULL, 0, { sinf(angle), -1.0f, cosf(angle), 1.0f });
		//Step 6 - 11x MEV + 11x MEF
		for (int vtx = 1; vtx < ratio; vtx++)
		{
			angle += delta;
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 2], NULL, vtx, { sinf(angle), -1.0f, cosf(angle), 1.0f });
			EulerOp::mef(model.back()->halfEdges.back()->prev->prev->prev, model.back()->halfEdges.back(), 0);
		}
		////Step 7 - Last face
		EulerOp::mef(model.back()->halfEdges.back()->next, model.back()->halfEdges.back()->next->next->next->next, 0);
	}

	void Sphere(std::vector<HED::solid*>& model)
	{
		int ratio = 12;
		float delta = -M_PI * (360.0f / ratio) / 180.0f;
		float angle = 0.0f;

		/* EQUADOR */
		//Step 1 - First vertex
		EulerOp::mvfs(model, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		model.back()->name = "Sphere";
		//Step 2 - First edge / Second vertex
		angle += delta;
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		//Step 3 - 10x MEV
		for (int vtx = 1; vtx < ratio - 1; vtx++)
		{
			angle += delta;
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 2], NULL, vtx, { sinf(angle), 0.0f, cosf(angle), 1.0f });
		}
		//Step 4 - MEF
		EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges[0], 0);

		/* HEMISFÉRIO NORTE */
		//Step 5 - MEV
		EulerOp::mev(model.back()->halfEdges[2 * ratio - 2], NULL, ratio - 1, { sinf(angle) * cosf(delta), sinf(delta) * (-1.0f), cosf(angle) * cosf(delta), 1.0f });
		angle = 0.0f;
		for (int vtx = 1; vtx < ratio; vtx++)
		{
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 1], NULL, vtx - 1, { sinf(angle) * cosf(delta), sinf(delta) * (-1.0f), cosf(angle) * cosf(delta), 1.0f });
			EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges.back()->next->next->next, 1);
			angle += delta;
		}
		//Last face
		EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges.back()->prev->prev->prev, 1);
	}

	void Cone(std::vector<HED::solid*>& model)
	{
		int ratio = 24;
		float delta = -M_PI * (360.0f / ratio) / 180.0f;
		float angle = 0.0f;

		/* BOTTOM FACE */
		//Step 1 - First vertex
		EulerOp::mvfs(model, { sinf(angle), -1.0f, cosf(angle), 1.0f });
		model.back()->name = "Cone";
		//Step 2 - First edge / Second vertex
		angle += delta;
		EulerOp::mev(model.back()->faces[0]->hEdge, NULL, 0, { sinf(angle), -1.0f, cosf(angle), 1.0f });
		//Step 3 - 10x MEV
		for (int vtx = 1; vtx < ratio - 1; vtx++)
		{
			angle += delta;
			EulerOp::mev(model.back()->halfEdges[2 * vtx - 2], NULL, vtx, { sinf(angle), -1.0f, cosf(angle), 1.0f });
		}
		//Step 4 - MEF
		EulerOp::mef(model.back()->halfEdges.back(), model.back()->halfEdges[0], 0);

		/* EDGE */
		//Step 5 - MEV
		EulerOp::mev(model.back()->halfEdges[1], NULL, 0, { 0.0f, 1.0f, 0.0f, 1.0f });

		/* FACES */
		//Step 6 - MEF	
		for (int i = 1; i < 2 * ratio - 2; i += 2)
			EulerOp::mef(model.back()->halfEdges[i], model.back()->halfEdges[i]->next->next, 1);
	}
}
