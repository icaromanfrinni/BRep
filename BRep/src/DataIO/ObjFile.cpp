#include "ObjFile.h"
#include <iostream>
#include <fstream>
#include <sstream>

//DEFAULT CONSTRUCTOR
ObjFile::ObjFile()
{
	objectList.clear();
	//viewEnable = false;
	//allVertices.clear();
}

//DESTRUCTOR
ObjFile::~ObjFile()
{
}

//CLEAR OBJ VARIABLE TYPE
void ObjFile::ClearCurrentObj(obj &_obj)
{
	_obj.Name = "";
	_obj.Vertices.clear();
	_obj.Lines.clear();
	_obj.Faces.clear();
}

//READ .OBJ FILE
bool ObjFile::ReadObjFile(const std::string& inName)
{
	std::ifstream inFile(inName);
	if (!inFile.is_open())
	{
		std::cerr << "\n\t!!! FILE COULD NOT BE OPENED !!!\n" << std::endl;
		return false;
	}

	//Initialize
	objectList.clear();
	//allVertices.clear();
	int vertexCount = 0;
	//Read a file line by line
	std::string line;
	obj currentObj;
	bool object = false;

	while (!inFile.eof())
	{
		getline(inFile, line);

		/*-------------------* OBJECT *-------------------*/

		if (line.substr(0, 1) == "o")
		{
			//Objeto anterior, se existir
			//e limpa a variável q recebe o objeto corrente
			if (object == true)
			{
				vertexCount += currentObj.Vertices.size();
				objectList.push_back(currentObj);
				ClearCurrentObj(currentObj);
			}

			object = true;							//ativa a leitura de um novo objeto
			currentObj.Name = line.substr(2);		//recebe o nome do objeto
			std::cout << "\n\tReading Object ...: " << currentObj.Name << std::endl;
		}
		else if (object == true)
		{
			/*-------------------* VERTEX *-------------------*/

			if (line.substr(0, 1) == "v")
			{
				Vector4Df vertex;
				line = line.substr(1);
				std::istringstream s(line);
				s >> vertex.x;
				s >> vertex.y;
				s >> vertex.z;
				vertex.w = 1.0f;
				currentObj.Vertices.push_back(vertex);
				//allVertices.push_back(vertex);
			}

			/*--------------------* LINE *--------------------*/

			if (line.substr(0, 1) == "l")
			{
				int vFrom, vTo;
				objLine edge;
				line = line.substr(1);
				std::istringstream s(line);
				s >> vFrom;
				s >> vTo;
				//s >> edge.vStart;
				//s >> edge.vEnd;
				edge.vStart = vFrom - vertexCount;
				edge.vEnd = vTo - vertexCount;
				//edge.vFrom = allVertices[vFrom - 1];
				//edge.vTo = allVertices[vTo - 1];
				currentObj.Lines.push_back(edge);
			}

			/*--------------------* FACE *--------------------*/

			if (line.substr(0, 1) == "f")
			{
				int index;
				objFace face;
				line = line.substr(1);
				std::istringstream s(line);

				while (true)
				{
					s >> index;
					if (!s)
						break;
					face.vertices.push_back(index - vertexCount);
				}
				currentObj.Faces.push_back(face);
			}
		}
	}
	//END OF FILE

	//Recebe último objeto válido
	if (object == true)
		objectList.push_back(currentObj);

	std::cout << "\n\tFile [" << inName << "] has been opened successfully!" << std::endl;

	return true;
}

//WRITE .OBJ FILE
void ObjFile::WriteObjFile()
{
	//string outName = fName + "Out.obj";
	//ofstream outFile(outName);
	//if (!outFile.is_open())
	//{
	//	cerr << "\n\t!!! FILE COULD NOT BE OPENED !!!\n" << endl;
	//	system("pause");
	//	exit(EXIT_FAILURE);
	//}

	///*-------------------* HEADER *-------------------*/

	//outFile << "# ICARO 2019 OBJ File" << endl;
	//outFile << "# icaro@lia.ufc.br" << endl;

	///*------------------* OBJECTS *-------------------*/

	//int vCount = 0;
	//for (int i = 0; i < vObj.size(); i++)
	//{
	//	//group name (o)
	//	outFile << "o " << vObj[i].Name << endl;

	//	//geometric vertices (v)
	//	for (int j = 0; j < vObj[i].Vertices.size(); j++)
	//		outFile << "v " << vObj[i].Vertices[j].x << " " << vObj[i].Vertices[j].y << " " << "0.00" << endl;
	//	outFile << "# " << vObj[i].Vertices.size() << " vertices" << endl;

	//	//geometric lines (l)
	//	for (int j = 0; j < vObj[i].Lines.size(); j++)
	//		outFile << "l " << vObj[i].Lines[j].vFrom << " " << vObj[i].Lines[j].vTo << endl;
	//	outFile << "# " << vObj[i].Lines.size() << " lines" << endl;

	//	//face vertices (f)
	//	outFile << "f";
	//	for (int j = 0; j < vObj[i].Face.size(); j++)
	//		outFile << " " << vObj[i].Face[j];
	//	outFile << endl;
	//}

	///*--------------------* END *--------------------*/

	//outFile.close();

	//cout << "\n\tFile [" << outName << "] has been created successfully!" << endl;
}

