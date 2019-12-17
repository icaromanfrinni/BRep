#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <GL/glut.h>

#include "HalfEdge.h"
#include "Bezier.h"

namespace OpenGL_Render
{
	void DrawCurve(const CURVE::Bezier &curve)
	{
		if (!curve.enable)
			return;

		// DRAW POINTS
		glEnable(GL_POINT_SMOOTH);
		glColor3f(1.0f, 0.0f, 0.0f);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < curve.control_points.size(); i++)
				glVertex3f((GLfloat)curve.control_points[i].x, (GLfloat)curve.control_points[i].y, (GLfloat)curve.control_points[i].z);
		glEnd();
		// DRAW CURVE
		float t = 0.0f;
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= curve.segments; i++)
		{
			Vector4Df B = curve.control_points[0] * powf(1.0 - t, 3.0f) + curve.control_points[1] * (3 * t * powf(1 - t, 2.0f)) + curve.control_points[2] * (3 * (1 - t) * powf(t, 2.0f)) + curve.control_points[3] * powf(t, 3.0f);
			glVertex3f((GLfloat)B.x, (GLfloat)B.y, (GLfloat)B.z);
			t += 1.0f / curve.segments;
		}
		glEnd();
	}

	void DrawSolid(const std::vector<HED::solid *> solidList)
	{
		for (int i = 0; i < solidList.size(); i++)
		{
			// DRAW POINTS
			glEnable(GL_POINT_SMOOTH);
			glPointSize(5.0f);
			glBegin(GL_POINTS);
			for (int j = 0; j < solidList[i]->vertices.size(); j++)
			{
				if (solidList[i]->vertices[j]->enable == 1) glColor3f(1.0f, 0.0f, 0.0f);
				else glColor3f(0.0f, 0.0f, 0.0f);
				glVertex3f((GLfloat)solidList[i]->vertices[j]->point.x, (GLfloat)solidList[i]->vertices[j]->point.y, (GLfloat)solidList[i]->vertices[j]->point.z);
			}
			glEnd();

			// DRAW LINES
			glColor3f(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < solidList[i]->halfEdges.size(); j++)
			{
				glBegin(GL_LINES);
				glVertex3f((GLfloat)solidList[i]->halfEdges[j]->vStart->point.x, (GLfloat)solidList[i]->halfEdges[j]->vStart->point.y, (GLfloat)solidList[i]->halfEdges[j]->vStart->point.z);
				glVertex3f((GLfloat)solidList[i]->halfEdges[j]->next->vStart->point.x, (GLfloat)solidList[i]->halfEdges[j]->next->vStart->point.y, (GLfloat)solidList[i]->halfEdges[j]->next->vStart->point.z);
				glEnd();
			}

			//DRAW FACES
			for (int j = 0; j < solidList[i]->faces.size(); j++)
			{
				glColor4f(1.0f, 0.5f, 1.0f, 1.0f);
				glPolygonMode(GL_FRONT, GL_FILL);
				//glPolygonMode(GL_BACK, GL_LINE);

				HED::halfEdge* he = solidList[i]->faces[j]->hEdge;
				glBegin(GL_POLYGON);
				//normal da face
				Vector4Df P1P2 = solidList[i]->faces[j]->hEdge->next->vStart->point - solidList[i]->faces[j]->hEdge->vStart->point;
				Vector4Df P1P3 = solidList[i]->faces[j]->hEdge->next->next->vStart->point - solidList[i]->faces[j]->hEdge->vStart->point;
				Vector4Df normal = cross(P1P2, P1P3).to_unitary();
				glNormal3f((GLfloat)normal.x, (GLfloat)normal.y, (GLfloat)normal.z);
				glVertex3f((GLfloat)he->vStart->point.x, (GLfloat)he->vStart->point.y, (GLfloat)he->vStart->point.z);
				for (he = solidList[i]->faces[j]->hEdge->next; he != solidList[i]->faces[j]->hEdge; he = he->next)
				{
					if (he->next == he->opp) break;
					glVertex3f((GLfloat)he->vStart->point.x, (GLfloat)he->vStart->point.y, (GLfloat)he->vStart->point.z);
				}
				glEnd();
			}
		}
	}
}
