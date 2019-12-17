#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Linear_Algebra.h"
#include "ObjFile.h"

namespace HED
{
	typedef struct vertex Vertex;
	//typedef struct edge Edge;
	typedef struct halfEdge HalfEdge;
	typedef struct face Face;
	typedef struct solid Solid;

	struct vertex
	{
		int id;
		//halfEdge *hEdge;
		Vector4Df point;
		int enable;
	};

	//struct edge
	//{
	//	int id;
	//	halfEdge *he1, *he2;
	//};

	struct halfEdge
	{
		int id;
		halfEdge *opp, *next, *prev;
		vertex *vStart;
		//Edge *edge;
		face *leftFace;
	};

	struct face
	{
		int id;
		halfEdge *hEdge;
		solid *HedSolid;
	};

	struct solid
	{
		int id;
		std::string name;
		std::vector<vertex*> vertices;
		//std::vector<edge> edges;
		std::vector<halfEdge*> halfEdges;
		std::vector<face*> faces;

		//transform
		Vector4Df location, scale;

		//Constructor
		solid()	{
			location = { 0.0f, 0.0f, 0.0f, 1.0f };
			scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		}

		//Overload Constructor (from OBJ File)
		solid(const int &index, const obj &OBJ) : name(OBJ.Name)
		{
			//Initialize
			id = index;
			vertices.resize(OBJ.Vertices.size());
			//edges.resize(OBJ.Lines.size());
			faces.resize(OBJ.Faces.size());
			location = { 0.0f, 0.0f, 0.0f, 1.0f };
			scale = { 1.0f, 1.0f, 1.0f, 0.0f };
			
			
			for (int i = 0; i < OBJ.Faces.size(); i++)
				for (int j = 0; j < OBJ.Faces[i].vertices.size(); j++)
					halfEdges.push_back(new HED::halfEdge);

			//Vertices
			for (int i = 0; i < OBJ.Vertices.size(); i++)
			{
				//PEGA CADA VÉRTICE DO *.OBJ
				vertices[i] = new HED::vertex;
				vertices[i]->id = i;
				vertices[i]->point = OBJ.Vertices[i];
				vertices[i]->enable = 0;
			}

			//Faces
			int heCount = 0; //contador de 'half-edges'
			for (int i = 0; i < OBJ.Faces.size(); i++)
			{
				//PARA CADA FACE DO *.OBJ
				faces[i] = new HED::face;
				faces[i]->id = i; //atribui um 'id'
				faces[i]->hEdge = halfEdges[heCount]; //primeira 'half-edge' da face fica sendo a referência
				faces[i]->HedSolid = this;

				for (int j = 0; j < OBJ.Faces[i].vertices.size(); j++)
				{
					halfEdges[heCount]->id = heCount; //atribui um 'id'
					halfEdges[heCount]->leftFace = faces[i]; //todas as 'half-edges' da face recebem como referência a face corrente
					halfEdges[heCount]->vStart = vertices[OBJ.Faces[i].vertices[j] - 1];
					halfEdges[heCount]->opp = NULL; //por enquanto

					if (j == 0) //se for a primeira 'half-edge' do loop
					{
						halfEdges[heCount]->next = halfEdges[heCount + 1];
						halfEdges[heCount]->prev = halfEdges[heCount + OBJ.Faces[i].vertices.size() - 1];
					}
					else if (j == OBJ.Faces[i].vertices.size() - 1) //se for a última 'half-edge' do loop
					{
						halfEdges[heCount]->next = halfEdges[heCount - OBJ.Faces[i].vertices.size() + 1];
						halfEdges[heCount]->prev = halfEdges[heCount - 1];
					}
					else //se for uma 'half-edge' intermediária
					{
						halfEdges[heCount]->next = halfEdges[heCount + 1];
						halfEdges[heCount]->prev = halfEdges[heCount - 1];
					}

					heCount++; //'id' da próxima 'half-edge'
				}
			}

			//find the opposite half-edge
			for (int i = 0; i < halfEdges.size(); i++)
				for (int j = 0; j < halfEdges.size(); j++)
				{
					if (halfEdges[j]->vStart == halfEdges[i]->next->vStart &&
						halfEdges[i]->vStart == halfEdges[j]->next->vStart)
						halfEdges[i]->opp = halfEdges[j];
				}
		}
	
		void transform(Matrix4 m)
		{
			for (int i = 0; i < vertices.size(); i++)
				vertices[i]->point = m * vertices[i]->point;
		}

		float Area()
		{
			float area = 0;

			for (int j = 0; j < faces.size(); j++)
			{
				HED::halfEdge* he = faces[j]->hEdge->next;

				while (he != faces[j]->hEdge)
				{
					Vector4Df P1P2 = he->vStart->point - faces[j]->hEdge->vStart->point;
					Vector4Df P1P3 = he->next->vStart->point - faces[j]->hEdge->vStart->point;
					area += cross(P1P2, P1P3).length();
					he = he->next;
				}
			}

			return area / 2.0f;
		}
	};

	//WRITE Half-Edge FILE
	void WriteHalfEdgeFile(const std::vector<solid *> &he_List)
	{
		std::cout << "Enter file name (*.hed): " << std::endl;
		std::string outName;
		std::cin >> outName;
		outName += ".hed";

		std::ofstream outFile(outName);
		if (!outFile.is_open())
		{
			std::cerr << "\n\t!!! FILE COULD NOT BE OPENED !!!\n" << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}

		/*-------------------* HEADER *-------------------*/

		outFile << "# ICARO 2019 Half-Edge Structure File" << std::endl;
		outFile << "# icaro@lia.ufc.br" << std::endl;

		/*------------------* OBJECTS *-------------------*/

		//int vCount = 0;
		for (int i = 0; i < he_List.size(); i++)
		{
			//group name (o)
			outFile << "o " << he_List[i]->name << std::endl;

			//geometric vertices
			//EX.: vertices[0].id = 0; vertices[0].point = { 1.0f, 1.0f, 1.0f, 1.0f };
			outFile << "/* VERTICES */" << std::endl;
			for (int j = 0; j < he_List[i]->vertices.size(); j++)
				outFile << "vertices[" << j << "].id = " << he_List[i]->vertices[j]->id << "; vertices[" << j << "].point = { " << he_List[i]->vertices[j]->point.x << ", " << he_List[i]->vertices[j]->point.y << ", " << he_List[i]->vertices[j]->point.z << ", 1.0 };" << std::endl;
			//outFile << "# " << he_List[i]->vertices.size() << " vertices" << std::endl;

			//loop faces
			//EX.: faces[0].id = 0; faces[0].hEdge = &halfEdges[0];
			outFile << "/* FACES */" << std::endl;
			for (int j = 0; j < he_List[i]->faces.size(); j++)
				outFile << "faces[" << j << "].id = " << he_List[i]->faces[j]->id << "; faces[" << j << "].hEdge = &halfEdges[" << he_List[i]->faces[j]->hEdge->id << "];" << std::endl;
			//outFile << "# " << he_List[i]->faces.size() << " faces" << std::endl;

			//half-edges
			//EX.:
			/*halfEdges[0].id = 0;
			halfEdges[0].leftFace = &faces[0];
			halfEdges[0].next = &halfEdges[1];
			halfEdges[0].opp = &halfEdges[13];
			halfEdges[0].prev = &halfEdges[3];
			halfEdges[0].vStart = &vertices[0];*/
			for (int j = 0; j < he_List[i]->halfEdges.size(); j++)
			{
				outFile << "//Half-edge " << j << std::endl;

				outFile << "halfEdges[" << j << "].id = " << he_List[i]->halfEdges[j]->id << ";" << std::endl;
				outFile << "halfEdges[" << j << "].leftFace = &faces[" << he_List[i]->halfEdges[j]->leftFace->id << "];" << std::endl;
				outFile << "halfEdges[" << j << "].next = &halfEdges[" << he_List[i]->halfEdges[j]->next->id << "];" << std::endl;
				outFile << "halfEdges[" << j << "].opp = &halfEdges[" << he_List[i]->halfEdges[j]->opp->id << "];" << std::endl;
				outFile << "halfEdges[" << j << "].prev = &halfEdges[" << he_List[i]->halfEdges[j]->prev->id << "];" << std::endl;
				outFile << "halfEdges[" << j << "].vStart = &vertices[" << he_List[i]->halfEdges[j]->vStart->id << "];" << std::endl;
			}
			//outFile << "# " << he_List[i]->halfEdges.size() << " halfedges" << std::endl;

			/*--------------------* END *--------------------*/

			outFile.close();

			std::cout << "\n\tFile [" << outName << "] has been created successfully!" << std::endl;
		}
	}

	//WRITE .OBJ FILE
	void WriteObjFile(const std::vector<solid *> &he_List)
	{
		std::cout << "Enter file name (*.obj): " << std::endl;
		std::string outName;
		std::cin >> outName;
		outName += ".obj";

		std::ofstream outFile(outName);
		if (!outFile.is_open())
		{
			std::cerr << "\n\t!!! FILE COULD NOT BE OPENED !!!\n" << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}

		/*-------------------* HEADER *-------------------*/

		outFile << "# ICARO 2019 OBJ File" << std::endl;
		outFile << "# icaro@lia.ufc.br" << std::endl;

		/*------------------* OBJECTS *-------------------*/

		int vCount = 0;
		for (int i = 0; i < he_List.size(); i++)
		{
			//group name (o)
			outFile << "o " << he_List[i]->name << std::endl;

			//geometric vertices (v)
			for (int j = 0; j < he_List[i]->vertices.size(); j++)
				outFile << "v " << he_List[i]->vertices[j]->point.x << " " << he_List[i]->vertices[j]->point.y << " " << he_List[i]->vertices[j]->point.z << std::endl;
			outFile << "# " << he_List[i]->vertices.size() << " vertices" << std::endl;

			//face vertices (f)
			for (int j = 0; j < he_List[i]->faces.size(); j++)
			{
				HED::halfEdge* he = he_List[i]->faces[j]->hEdge;
				outFile << "f " << he->vStart->id + 1;
				for (he = he_List[i]->faces[j]->hEdge->next; he != he_List[i]->faces[j]->hEdge; he = he->next)
					outFile << " " << he->vStart->id + 1;
				outFile << std::endl;
			}
			outFile << "# " << he_List[i]->faces.size() << " faces" << std::endl;

			/*--------------------* END *--------------------*/

			outFile.close();

			std::cout << "\n\tFile [" << outName << "] has been created successfully!" << std::endl;
		}
	}
}
