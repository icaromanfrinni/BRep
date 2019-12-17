#pragma once
#ifndef OBJFILE_H
#define OBJFILE_H

#include <string>
#include <vector>
#include "Linear_Algebra.h"

//------------------------------------------------------------------------------
// TYPES
//------------------------------------------------------------------------------

struct objLine
{
	int vStart, vEnd;
	//Vector4Df vFrom;
	//Vector4Df vTo;
};

struct objFace
{
	std::vector<int> vertices;
};

//only indexes (just like the text file)
struct obj
{
	std::string Name;
	std::vector<Vector4Df> Vertices;
	//std::vector<int> Vertices;
	std::vector<objLine> Lines;
	std::vector<objFace> Faces;
};

//------------------------------------------------------------------------------
// CLASSES
//------------------------------------------------------------------------------

class ObjFile
{
public:
	//DEFAULT CONSTRUCTOR
	ObjFile();
	//DESTRUCTOR
	~ObjFile();

	//CLEAR A OBJ VARIABLE TYPE
	void ClearCurrentObj(obj &_obj);
	//READ .OBJ FILE
	bool ReadObjFile(const std::string& inName);
	//WRITE .OBJ FILE
	void WriteObjFile();

	//VARIABLES
	std::vector<obj> objectList; //vector of objets from OBJ File
	//bool viewEnable;
private: 
	//std::vector<Vector4Df> allVertices; /* Create a unique vector of OBJ points */
};

#endif // OBJFILE_H