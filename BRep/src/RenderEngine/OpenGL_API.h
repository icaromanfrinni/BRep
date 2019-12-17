#pragma once

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <Windows.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"

#include "Linear_Algebra.h"
#include "Camera.h"
#include "OpenGL_Render.h"
#include "ObjFile.h"
#include "HalfEdge.h"
#include "Primitives.h"
#include "Bezier.h"

#define DEBUG 0

namespace OpenGL_API
{
	const int widthRender = 512, heightRender = 512;

	/* CAMERA */
	Camera cam = Camera({ 10.0f, 5.0f, 10.0f, 1.0f },	// viewpoint
	{ 0.0f, 0.0f, 0.0f, 1.0f },		// lookat
	{ 0.0f, 1.0f, 0.0f, 0.0f },		// view up
	float2{ widthRender*1.0f, heightRender*1.0f }, //resolution
	float2{ 1.0f, 1.0f },			//dimensions
	1.0f //near
	);	

	//LIGHT
	GLfloat posicaoLuz[4] = { 10.0f, 10.0f, 10.0f, 1.0f };

	// mouse event handlers
	Vector4Df last = { 0.0f, 0.0f, 0.0f, 0.0f };
	int TheButtonState = 0;
	int TheKeyState = GLUT_KEY_CTRL_L;
	float panSpeed = 0.05f;

	//class ObjFile
	ObjFile inputObjFile;

	//List of Solids
	std::vector<HED::solid*> solids;

	//Bézier Curve
	CURVE::Bezier exCurve;

	//Menu
	static bool show_app_property_editor = false;
	static bool show_app_modify = false;
	static bool show_mass_window = false;
	static bool show_curve_property_editor = false;

	/* ======================================= MENU FUNCTIONS ======================================= */

	void New_MENU()
	{
		// mouse event handlers
		int TheButtonState = 0;
		int TheKeyState = GLUT_KEY_CTRL_L;
		// keyboard event handlers

		//list of objects
		solids.clear();
	}

	void Import_MENU()
	{
		std::string fileName;
		std::cout << "Enter file name (*.obj): " << std::endl;
		std::cin >> fileName;
		fileName += ".obj";
		if (inputObjFile.ReadObjFile(fileName) == false) {
			MessageBox(NULL, "File could not be opened!", "Info",
				MB_OK | MB_ICONEXCLAMATION);
		}
		else
		{
			solids.clear();
			for (int i = 0; i < inputObjFile.objectList.size(); i++)
				solids.push_back(new HED::solid(i, inputObjFile.objectList[i]));
		}
	}

	void ExportOBJ_MENU()
	{
		if (solids.empty() == false) {
			HED::WriteObjFile(solids);
		}
	}

	void ExportHED_MENU()
	{
		if (solids.empty() == false) {
			HED::WriteHalfEdgeFile(solids);
		}
	}

	/* ======================================= ImGui MAIN MENU ======================================= */

	static void ShowCurvePropertyEditor(bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Curve editor", p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Bezier curve");

		for (int i = 0; i < exCurve.control_points.size(); i++)
		{
			ImGui::PushID(i);
			//ImGui::TreeNodeEx("Control points", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "%s_%u", "P", i);
			ImGui::DragFloat3(" ", (float*)&exCurve.control_points[i], 0.1f, NULL, NULL, "%.2f");
			ImGui::PopID();
		}
		ImGui::DragInt("Segments", (int*)&exCurve.segments, 1.0f, 10, 100, "%d");

		ImGui::End();
	}

	// create a mass property window.
	static void ShowMassPropertyWindow(bool* p_open)
	{
		if (!ImGui::Begin("Mass_Properties", p_open))
		{
			ImGui::End();
			return;
		}

		struct funcs
		{
			static void ShowObjectProperties(const char* prefix, HED::solid &solid, int uid)
			{
				ImGui::PushID(uid);
				ImGui::TreeNodeEx(solid.name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				ImGui::Text("Area = %.1f", solid.Area());
				ImGui::PopID();
			}
		};

		for (int obj_i = 0; obj_i < solids.size(); obj_i++)
			funcs::ShowObjectProperties("Solid", *solids[obj_i], obj_i);

		if (ImGui::Button("Ok"))
		{
			show_mass_window = false;
		}

		ImGui::End();
	}

	// create a modify editor.
	static void ShowAppModify(bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Delta Transform", p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		
		static bool* selected = new bool[solids.size()];
		static std::string CurrentSolidName = " ";
		static int CurrentSolid;

		if (ImGui::BeginCombo("Solid", CurrentSolidName.c_str()))
		{
			for (int i = 0; i < solids.size(); i++)
			{
				if (ImGui::Selectable(solids[i]->name.c_str(), &selected[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_None)) {
					CurrentSolidName = solids[i]->name.c_str();
					CurrentSolid = i;
				}
			}
			ImGui::EndCombo();
		}
		if (!solids.empty())
		{
			ImGui::DragFloat3("Location", (float*)&solids[CurrentSolid]->location, 0.01f, NULL, NULL, "%.2f");
			solids[CurrentSolid]->transform(translate(solids[CurrentSolid]->location));
			solids[CurrentSolid]->location = { 0.0f, 0.0f, 0.0f, 1.0f };

			ImGui::DragFloat3("Scale", (float*)&solids[CurrentSolid]->scale, 0.01f, NULL, NULL, "%.2f");
			solids[CurrentSolid]->transform(resizeMatrix(solids[CurrentSolid]->scale));
			solids[CurrentSolid]->scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		}
		

		ImGui::PopStyleVar();
		ImGui::End();
	}

	// create a property editor.
	static void ShowAppPropertyEditor(bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Property editor", p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		struct funcs
		{
			static void ShowObjectProperties(const char* prefix, HED::solid &solid, int uid)
			{
				ImGui::PushID(uid);
				ImGui::AlignTextToFramePadding();
				bool node_open = ImGui::TreeNode("Solid", "%s_%u", prefix, uid);
				ImGui::NextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(solid.name.c_str());
				ImGui::NextColumn();
				if (node_open)
				{
					for (int vtx = 0; vtx < solid.vertices.size(); vtx++)
					{
						ImGui::PushID(vtx);
						ImGui::AlignTextToFramePadding();
						ImGui::TreeNodeEx("Vertex", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "%s %u", "Vertex", vtx);
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat3(" ", (float*)&solid.vertices[vtx]->point, 0.1f, NULL, NULL, "%.2f");
						ImGui::RadioButton("On", &solid.vertices[vtx]->enable, 1); ImGui::SameLine();
						ImGui::RadioButton("Off", &solid.vertices[vtx]->enable, 0);
						ImGui::Separator();
						ImGui::NextColumn();
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		};

		for (int obj_i = 0; obj_i < solids.size(); obj_i++)
			funcs::ShowObjectProperties("Solid", *solids[obj_i], obj_i);

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	// Main Menu Bar
	static void Main_Menu_Bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				{
					if (ImGui::MenuItem("New", "N")) { New_MENU(); }
					/*if (ImGui::MenuItem("Open", NULL, false, false)) {}*/
					/*if (ImGui::MenuItem("Save", NULL, false, false)) {}*/
					if (ImGui::MenuItem("Import", "I")) { Import_MENU(); }
					if (ImGui::BeginMenu("Export"))
					{
						if (ImGui::MenuItem("Wavefront (.obj)", "W")) { ExportOBJ_MENU(); }
						if (ImGui::MenuItem("Half-edge (.hed)", "H")) { ExportHED_MENU(); }
						ImGui::EndMenu();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Properties", "P"))
					{
						if (show_app_property_editor)
							show_app_property_editor = false;
						else show_app_property_editor = true;
					}
					if (ImGui::MenuItem("Quit")) { glutDestroyWindow(1); }
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Modeling"))
			{
				if (ImGui::BeginMenu("2D Primitive"))
				{
					if (ImGui::MenuItem("Plane")) { Primitive::Plane(solids); }
					if (ImGui::MenuItem("Circle")) { Primitive::Circle(solids); }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("3D Primitive"))
				{
					if (ImGui::MenuItem("Cube")) { Primitive::Cube(solids); }
					if (ImGui::MenuItem("Sphere")) { Primitive::Sphere(solids); }
					if (ImGui::MenuItem("Cylinder")) { Primitive::Cylinder(solids); }
					if (ImGui::MenuItem("Cone")) { Primitive::Cone(solids); }
					//if (ImGui::MenuItem("Icosphere", NULL, false, false)) {}
					//if (ImGui::MenuItem("Torus")) {}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Curve"))
				{
					if (exCurve.enable)
					{
						exCurve.enable = false;
						show_curve_property_editor = false;
					}
					else
					{
						exCurve.enable = true;
						show_curve_property_editor = true;
					}
				}
				if (ImGui::MenuItem("Surface", NULL, false, false))
				{

				}
				/*ImGui::Separator();
				if (ImGui::MenuItem("Extrude", NULL, false, false))
				{
					
				}
				if (ImGui::MenuItem("Sweep", NULL, false, false))
				{

				}*/
				ImGui::EndMenu();
			}
			/*if (ImGui::BeginMenu("Modify"))
			{
				if (ImGui::MenuItem("Move", NULL, false, false)) {}
				if (ImGui::MenuItem("Scale", NULL, false, false)) {}
				ImGui::EndMenu();
			}*/
			if (ImGui::MenuItem("Modify"))
			{
				if (show_app_modify)
					show_app_modify = false;
				else show_app_modify = true;
			}
			if (ImGui::BeginMenu("Analysis"))
			{
				if (ImGui::MenuItem("Area"))
					if (show_mass_window)
						show_mass_window = false;
					else show_mass_window = true;
				if (ImGui::MenuItem("Volume", NULL, false, false)) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	/* ======================================= INITIALIZE ======================================= */

	void init(void)
	{
		/* LIGHT */

		GLfloat luzAmbiente[4] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Ambient Light
		GLfloat luzDifusa[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // "cor" 
		GLfloat luzEspecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // "brilho" 
		GLfloat especularidade[4] = { 0.1f, 0.1f, 0.1f, 1.0f }; // brilho do material
		GLint especMaterial = 100;

		glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade); // refletância do material 
		glMateriali(GL_FRONT, GL_SHININESS, especMaterial); // concentração do brilho

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente); // Ambient Light ENABLE
		// light0 Parameters
		glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
		glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
		glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

		glEnable(GL_COLOR_MATERIAL); // definição da cor do material a partir da cor corrente
		glEnable(GL_LIGHTING); // lighting ENABLE
		glEnable(GL_LIGHT0); // light0 ENABLE
		//glEnable(GL_DEPTH_TEST); // depth-buffering ENABLE

		/* LIGHT END */

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.3, 0.3, 0.3, 0.0); // Especifica a cor de fundo da janela
		glShadeModel(GL_SMOOTH); // Habilita o modelo de colorização de Gouraud
		glEnable(GL_LINE_SMOOTH);
		/*glEnable(GL_DEPTH_TEST);*/
		//glEnable(GL_CULL_FACE); //  remove as faces cujas normais apóntam para o semiespaço oposto ao do observador
	}

	/* ======================================= glutDisplayFunc ======================================= */

	void display(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z, cam.lookat.x, cam.lookat.y, cam.lookat.z, cam.up.x, cam.up.y, cam.up.z);

		// grid
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (float i = -50; i <= 50; ++i)
		{
			glColor4f(0.3f, 0.3f, 0.3f, 0.5f * (1 - abs(i) / 50));
			if (i == 0) glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f(i, 0.0f, -50.0f);
			glVertex3f(i, 0.0f, 50.0f);
			if (i == 0) glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f(50.0f, 0.0f, i);
			glVertex3f(-50.0f, 0.0f, i);
		}
		glEnd();

		// Y-axis
		glBegin(GL_LINES);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glEnd();

		//draw
		OpenGL_Render::DrawSolid(solids);
		OpenGL_Render::DrawCurve(exCurve);

		// light
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glPushMatrix();
		glTranslatef(posicaoLuz[0], posicaoLuz[1], posicaoLuz[2]);
		glutWireSphere(0.2, 20, 16); //radius, slices, stacks
		glPopMatrix();

		// ImGUI
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplGLUT_NewFrame();

			Main_Menu_Bar();
			if (show_app_property_editor) ShowAppPropertyEditor(&show_app_property_editor);
			if (show_app_modify) ShowAppModify(&show_app_modify);
			if (show_mass_window) ShowMassPropertyWindow(&show_mass_window);
			if (show_curve_property_editor) ShowCurvePropertyEditor(&show_curve_property_editor);

			// Rendering
			ImGui::Render();
			ImGuiIO& io = ImGui::GetIO();

			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		}

		//glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
	}

	/* ======================================= glutReshapeFunc ======================================= */

	void reshape(int w, int h)
	{
		ImGui_ImplGLUT_ReshapeFunc(w, h);

		glViewport(0, 0, (GLsizei)w, (GLsizei)h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1f, 500.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	/* ======================================= glutMouseFunc ======================================= */

	/* Get mouse position on screen */
	void mouse(int button, int state, int x, int y)
	{
		ImGui_ImplGLUT_MouseFunc(button, state, x, y);

		if (!(ImGui::GetIO().WantCaptureMouse))
		{
			if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
			{
				last = { (float)x, (float)y, 0.0f, 0.0f };
				TheButtonState = GLUT_MIDDLE_BUTTON;
			}
			if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
			{
				TheButtonState = 0;
			}
			if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
			{

			}
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
			{
				last = { (float)x, (float)y, 0.0f, 1.0f };
				TheButtonState = GLUT_LEFT_BUTTON;
			}
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
			{
				TheButtonState = 0;
			}
			glutPostRedisplay();
		}
	}

	/* ======================================= glutMotionFunc ======================================= */

	void mouseMotion(int x, int y)
	{
		ImGui_ImplGLUT_MotionFunc(x, y);

		Vector4Df delta = { last.x - (float)x, (last.y - (float)y)*(-1.0f), 0.0f, 0.0f };
		Matrix4 mToCamera = ToCamera(cam);
		Matrix4 mToWorld = ToWorld(cam);

		if (!(ImGui::GetIO().WantCaptureMouse))
		{
			if (TheButtonState == GLUT_MIDDLE_BUTTON)
			{
				/* 3D ORBIT: Rotates the view in 3D space */
				if (TheKeyState == GLUT_KEY_CTRL_L)
				{
					Vector4Df eyeToCam = mToCamera * cam.eye; // change to camera
					eyeToCam += delta * (0.2f); // motion
					cam.eye = mToWorld * eyeToCam; // change back to world
				}
				/* PAN: Shifts the view without changing the viewing direction or magnification */
				if (TheKeyState == GLUT_KEY_SHIFT_L)
				{
					// change to camera
					Vector4Df eyeToCam = mToCamera * cam.eye;
					Vector4Df lookatToCam = mToCamera * cam.lookat;
					// motion
					eyeToCam += delta * panSpeed;
					lookatToCam += delta * panSpeed;
					// change back to world
					cam.eye = mToWorld * eyeToCam;
					cam.lookat = mToWorld * lookatToCam;
				}
			}

			if (TheButtonState == GLUT_LEFT_BUTTON)
			{

			}

			last = { (float)x, (float)y, 0.0f, 1.0f };
			glutPostRedisplay();
		}
	}

	/* ======================================= glutMouseWheelFunc ======================================= */

	/* ZOOM: Increases or decreases the magnification of the view in the current viewport */
	void mouseWheel(int button, int dir, int x, int y)
	{
		Vector4Df lookat = cam.lookat - cam.eye;
		//lookat.normalize();
		cam.eye += lookat * (dir * 0.1f);
		//cam.lookat += lookat*dir;
		glutPostRedisplay();
	}

	/* ======================================= glutSpecialFunc ======================================= */

	void specialKey(int key, int x, int y)
	{
		switch (key) {
		case GLUT_KEY_F5: // iso view
			cam.eye = { 10.0f, 10.0f, 10.0f, 1.0f };
			cam.lookat = { 0.0f, 0.0f, 0.0f, 1.0f };
			cam.up = { -1.0f, 1.0f, -1.0f, 0.0f };
			std::cout << "ISO VIEW" << std::endl;
			break;
		case GLUT_KEY_F6: // front view
			cam.eye = { 0.0f, 0.0f, 10.0f, 1.0f };
			cam.lookat = { 0.0f, 0.0f, 0.0f, 1.0f };
			cam.up = { 0.0f, 1.0f, 0.0f, 0.0f };
			std::cout << "FRONT VIEW" << std::endl;
			break;
		case GLUT_KEY_F7: // top view
			cam.eye = { 0.0f, 10.0f, 0.1f, 1.0f };
			cam.lookat = { 0.0f, 0.0f, 0.0f, 1.0f };
			cam.up = { 0.0f, 0.0f, -1.0f, 0.0f };
			std::cout << "TOP VIEW" << std::endl;
			break;
		case GLUT_KEY_CTRL_L: // active ROTATE
			TheKeyState = key;
			std::cout << "3D Orbit ON" << std::endl;
			break;
		case GLUT_KEY_SHIFT_L: // active PAN
			TheKeyState = key;
			std::cout << "Pan ON" << std::endl;
			break;
		case GLUT_KEY_UP: 

			break;
		case GLUT_KEY_DOWN: 

			break;
		case GLUT_KEY_HOME:
			break;
		default:
			break;
		}
		glutPostRedisplay();
	}

	/* ======================================= glutKeyboardFunc ======================================= */

	void keyboard(unsigned char key, int x, int y)
	{
		switch (key) {
		case 'I':
		case 'i':
			Import_MENU();
			break;
		case 'H':
		case 'h':
			ExportHED_MENU();
		break;
		case 'W':
		case 'w':
			ExportOBJ_MENU();
		break;
		case 'M':
		case 'm':

			break;
		case 'N':
		case 'n':
			New_MENU();
			break;
		case 'P':
		case 'p':
			// Properties
			if (show_app_property_editor)
				show_app_property_editor = false;
			else show_app_property_editor = true;
			break;
		case 'R':
		case 'r':

			break;
		case 'S':
		case 's':

			break;
		}
		glutPostRedisplay();
	}

	/* ======================================= MAIN ======================================= */

	int Start_Window(int argc, char** argv)
	{
		glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(600, 600);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("B-Rep");
		init();

		// ImGUI
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

			// Setup Dear ImGui style
			//ImGui::StyleColorsDark();
			ImGui::StyleColorsClassic();

			// Setup Platform/Renderer bindings
			ImGui_ImplGLUT_Init();
			ImGui_ImplGLUT_InstallFuncs();
			ImGui_ImplOpenGL2_Init();
		}

		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutMouseFunc(mouse);
		glutMotionFunc(mouseMotion);
		glutMouseWheelFunc(mouseWheel);
		glutSpecialFunc(specialKey);
		glutKeyboardFunc(keyboard);
		glEnable(GL_DEPTH_TEST);

		glutMainLoop();

		// ImGUI
		{
			// Cleanup
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplGLUT_Shutdown();
			ImGui::DestroyContext();
		}

		return 0;
	}
}
