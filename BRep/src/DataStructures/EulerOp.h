#pragma once
#include "HalfEdge.h"
#include "Linear_Algebra.h"

namespace EulerOp
{
	//MAKE Vertex Face Solid
	void mvfs(std::vector<HED::solid*> &model, const Vector4Df& point)
	{
		//Initialize
		HED::solid* newSolid = new HED::solid;
		HED::vertex* newVertex = new HED::vertex;
		HED::face* newFace = new HED::face;
		HED::halfEdge* newHE = new HED::halfEdge;

		//Solid
		newSolid->id = model.size();

		//Vertex
		newVertex->id = 0;
		newVertex->point = point;
		newVertex->enable = 0;
		newSolid->vertices.push_back(newVertex);

		//Face
		newFace->id = 0;
		newFace->hEdge = newHE;
		newFace->HedSolid = newSolid;
		newSolid->faces.push_back(newFace);

		//Half-Edge
		newHE->id = 0;
		newHE->leftFace = newFace;
		newHE->next = newHE;
		newHE->opp = newHE;
		newHE->prev = newHE;
		newHE->vStart = newVertex;

		//RETURN
		model.push_back(newSolid);
	}

	//MAKE Edge Vertex
	void mev(HED::halfEdge* he1, HED::halfEdge* he2, const int &v, const Vector4Df& point)
	{
		//Get Solid
		HED::solid* currentSolid = he1->leftFace->HedSolid;

		//Initialize
		HED::vertex* newVertex = new HED::vertex;
		HED::halfEdge* newHE1 = new HED::halfEdge;
		HED::halfEdge* newHE2 = new HED::halfEdge;

		//Vertex
		newVertex->id = currentSolid->vertices.size();
		newVertex->point = point;
		newVertex->enable = 0;
		currentSolid->vertices.push_back(newVertex);

		//Half-Edge
		newHE1->id = currentSolid->halfEdges.size();
		newHE2->id = currentSolid->halfEdges.size() + 1;
		newHE1->leftFace = he1->leftFace;
		newHE1->vStart = currentSolid->vertices[v];
		newHE2->vStart = currentSolid->vertices.back();

		if (he2 == NULL)
		{
			newHE2->leftFace = he1->leftFace;
			newHE1->next = newHE2;
			newHE1->opp = newHE2;
			newHE2->prev = newHE1;
			newHE2->opp = newHE1;

			if (currentSolid->halfEdges.empty())
			{
				newHE1->prev = newHE2;
				newHE2->next = newHE1;
				he1 = newHE1;
			}
			else
			{
				newHE1->prev = he1;
				newHE2->next = he1->next;
				he1->next->prev = newHE2;
				he1->next = newHE1;
			}
		}
		else
		{
			newHE2->leftFace = he2->leftFace;
			
			std::cout << "entrou" << std::endl;
			
			for (HED::halfEdge* he = he1; he != he2; he = he->opp->next)
				he->vStart = currentSolid->vertices.back();
			
			std::cout << "meio" << std::endl;
			
			//adjacencies
			newHE1->next = he1;
			newHE1->prev = he1->prev;
			he1->prev->next = newHE1;
			he1->prev = newHE1;
			he1->opp->next = newHE2;
			newHE2->next = he2;
			newHE2->prev = he2->prev;
			he2->prev->next = newHE2;
			he2->prev = newHE2;
			
			std::cout << "saiu" << std::endl;
		}
		
		//RETURN
		currentSolid->halfEdges.push_back(newHE1);
		currentSolid->halfEdges.push_back(newHE2);
	}

	//MAKE Edge Face
	void mef(HED::halfEdge* he1, HED::halfEdge* he2, const int& f)
	{
		//Get Solid
		HED::solid* currentSolid = he1->leftFace->HedSolid;

		//Initialize
		HED::face* newFace = new HED::face;
		HED::halfEdge* newHE1 = new HED::halfEdge;
		HED::halfEdge* newHE2 = new HED::halfEdge;

		/* FUNCIONANDO PRA APENAS UMA ORIENTAÇÃO */

		//Face
		//ok
		newFace->id = currentSolid->faces.size();
		newFace->HedSolid = currentSolid;

		{
			//nok
			newFace->hEdge = he1;
			//nok
			for (HED::halfEdge* he = he1; he != he2; he = he->next)
				he->leftFace = newFace; //currentSolid->faces.back();
		}

		//ok
		currentSolid->faces.push_back(newFace);

		//Half-Edge
		//ok
		newHE1->id = currentSolid->halfEdges.size();
		newHE2->id = currentSolid->halfEdges.size() + 1;
		newHE1->leftFace = currentSolid->faces.back();
		newHE2->leftFace = currentSolid->faces[f];
		newHE1->opp = newHE2;
		newHE2->opp = newHE1;

		{
			//nok
			newHE1->vStart = he2->vStart;
			newHE2->vStart = he1->vStart;
			newHE1->next = he1;
			newHE2->next = he2;
			newHE1->prev = he2->prev;
			newHE2->prev = he1->prev;
			//novas adjacências das half-edges existentes
			//nok
			he1->prev->next = newHE2;
			he2->prev->next = newHE1;
			he1->prev = newHE1;
			he2->prev = newHE2;
			//nova half-edge de referência da face existente
			//nok
			currentSolid->faces[f]->hEdge = he2;
		}

		/* CONDIÇÃO DE ORIENTAÇÃO NÃO FUNCIONANDO */
		/*
		//Face
		//ok
		newFace->id = currentSolid->faces.size();
		newFace->HedSolid = currentSolid;
		//check the loop orientation
		if (he2->vStart == currentSolid->faces[f]->hEdge->vStart)
		{
			//nok
			newFace->hEdge = he1;
			//nok
			for (HED::halfEdge* he = he1; he != he2; he = he->next)
				he->leftFace = newFace; //currentSolid->faces.back();
		}
		else
		{
			newFace->hEdge = he2;
			for (HED::halfEdge* he = he2; he != he1; he = he->next)
				he->leftFace = newFace;
		}
		//ok
		currentSolid->faces.push_back(newFace);

		//Half-Edge
		//ok
		newHE1->id = currentSolid->halfEdges.size();
		newHE2->id = currentSolid->halfEdges.size() + 1;		
		newHE1->leftFace = currentSolid->faces.back();
		newHE2->leftFace = currentSolid->faces[f];
		newHE1->opp = newHE2;
		newHE2->opp = newHE1;
		//check the loop orientation
		if (he2->vStart == currentSolid->faces[f]->hEdge->vStart)
		{
			//nok
			newHE1->vStart = he2->vStart;
			newHE2->vStart = he1->vStart;
			newHE1->next = he1;
			newHE2->next = he2;
			newHE1->prev = he2->prev;
			newHE2->prev = he1->prev;
			//novas adjacências das half-edges existentes
			//nok
			he1->prev->next = newHE2;
			he2->prev->next = newHE1;
			he1->prev = newHE1;
			he2->prev = newHE2;
			//nova half-edge de referência da face existente
			//nok
			currentSolid->faces[f]->hEdge = he2;
		}
		else
		{
			newHE1->vStart = he1->vStart;
			newHE2->vStart = he2->vStart;
			newHE1->next = he2;
			newHE2->next = he1;
			newHE1->prev = he1->prev;
			newHE2->prev = he2->prev;
			//novas adjacências das half-edges existentes
			he1->prev->next = newHE1;
			he2->prev->next = newHE2;
			he1->prev = newHE2;
			he2->prev = newHE1;
			//nova half-edge de referência da face existente
			//nok
			currentSolid->faces[f]->hEdge = he1;
		}
		*/

		//RETURN
		currentSolid->halfEdges.push_back(newHE1);
		currentSolid->halfEdges.push_back(newHE2);
	}
}