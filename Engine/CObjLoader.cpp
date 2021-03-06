#include "CObjLoader.h"
#include <fstream>
#include <vector>

CObj CObjLoader::Load(LPCWSTR _fileName)
{
	loadFile(_fileName);
	//SafeFile();

	return m_obj;
}

CObj CObjLoader::Load(EPrimitives _primitiveType)
{
	m_prim = _primitiveType;

	loadPrimVertices();
	loadPrimIndices();

	return m_obj;
}

CObj CObjLoader::LoadTerrain(int _width, int _height)
{
	CTerrain terrain;
	
	return m_obj = terrain.Init(_width, _height);
}

void CObjLoader::loadPrimVertices()
{
	switch (m_prim)
	{
	case EPrimitives::Cube:
	{
		float hs = 0.5f;
		// quad - with uv & normals
		m_obj.vertices = {
			// Front Face
			CVertex(-hs, -hs, -hs, 0, 1, 0, 0, 1),	//Bottom	Left
			CVertex(-hs, hs, -hs, 0, 0, 0, 0, 1),	//Top		Left
			CVertex(hs, hs, -hs, 1, 0, 0, 0, 1),	//Top		Right
			CVertex(hs, -hs, -hs, 1, 1, 0, 0, 1),	//Bottom	Right

			// Left Face
			CVertex(-hs, -hs, hs, 0, 1, 1, 0, 0),	//Bottom	Left
			CVertex(-hs, hs, hs, 0, 0, 1, 0, 0),	//Top		Left
			CVertex(-hs, hs, -hs, 1, 0, 1, 0, 0),	//Top		Right
			CVertex(-hs, -hs, -hs, 1, 1, 1, 0, 0),	//Bottom	Right

			// Back Face
			CVertex(hs, -hs, hs, 0, 1, 0, 0, -1),	//Bottom	Left
			CVertex(hs, hs, hs, 0, 0, 0, 0, -1),	//Top		Left
			CVertex(-hs, hs, hs, 1, 0, 0, 0, -1),	//Top		Right
			CVertex(-hs, -hs, hs, 1, 1, 0, 0, -1),	//Bottom	Right

			// Right Face
			CVertex(hs, -hs, -hs, 0, 1, -1, 0, 0),	//Bottom	Left
			CVertex(hs, hs, -hs, 0, 0, -1, 0, 0),	//Top		Left
			CVertex(hs, hs, hs, 1, 0, -1, 0, 0),	//Top		Right
			CVertex(hs, -hs, hs, 1, 1, -1, 0, 0),	//Bottom	Right

			// Top Face
			CVertex(-hs, hs, -hs, 0, 0, 0, -1, 0),	//Top		Right
			CVertex(-hs, hs, hs, 1, 0, 0, -1, 0),	//Bottom	Right
			CVertex(hs, hs, hs, 1, 1, 0, -1, 0),	//Bottom	Left
			CVertex(hs, hs, -hs, 0, 1, 0, -1, 0),	//Top		Left

			// Base Face					 	
			CVertex(-hs, -hs, hs, 1, 1, 0, 1, 0),	//Bottom	Left
			CVertex(-hs, -hs, -hs, 0, 1, 0, 1, 0),	//Top		Left
			CVertex(hs, -hs, -hs, 0, 0, 0, 1, 0),	//Top		Right
			CVertex(hs, -hs, hs, 1, 0, 0, 1, 0),	//Bottom	Right
		};
		break;
	}
	case EPrimitives::Plane:
	{
		float hs = 5;
		// quad - with uv & normals
		m_obj.vertices = {
			// Face
			CVertex(-hs, 0, -hs, 0, 1, 0, -1, 0),	//Bottom	Left
			CVertex(-hs, 0, hs, 0, 0, 0, -1, 0),	//Top		Left
			CVertex(hs, 0, hs, 1, 0, 0, -1, 0),		//Top		Right
			CVertex(hs, 0, -hs, 1, 1, 0, -1, 0),	//Bottom	Right
		};
		break;
	}
	case EPrimitives::Cylinder:
	{

		break;
	}
	case EPrimitives::Pyramid:
	{

		break;
	}
	case EPrimitives::Sphere:
	{

		break;
	}
	default:
		break;
	}
}

void CObjLoader::loadPrimIndices()
{
	switch (m_prim)
	{
	case EPrimitives::Cube:
	{
		// quad
		m_obj.indices = {
			// Front Face
			0, 1, 2,
			0, 2, 3,
			// Left Face
			4, 5, 6,
			4, 6, 7,
			// Back Face
			8, 9, 10,
			8, 10, 11,
			// Right Face
			12, 13, 14,
			12, 14, 15,
			// Top Face
			16, 17, 18,
			16, 18, 19,
			// Bottom Face
			20, 21, 22,
			20, 22, 23,
		};
		break;
	}
	case EPrimitives::Plane:
	{
		// quad
		m_obj.indices = {
			// Face
			0, 1, 2,
			0, 2, 3,
		};
		break;
	}
	case EPrimitives::Cylinder:
	{

		break;
	}
	case EPrimitives::Pyramid:
	{

		break;
	}
	case EPrimitives::Sphere:
	{

		break;
	}
	default:
		break;
	}
}

void CObjLoader::loadFile(LPCWSTR _fileName)
{
	std::ifstream fileStream(_fileName);
	if (!fileStream.is_open())
		return;

	readFile(&fileStream);

	fileStream.close();
}

void CObjLoader::readFile(std::ifstream* _fileStream)
{
	m_obj = CObj();
	std::vector<XMFLOAT3> positions = {};
	std::vector<XMFLOAT2> uv = {};
	std::vector<XMFLOAT3> normals = {};
	std::vector<std::string> split_f = {};
	std::vector<std::string> split_l = {};
	std::string line = {};
	int faceCount = 0;
	float vertexCount = 0;

	while (std::getline(*_fileStream, line))
	{
		split_l = splitString(line, " ");
#pragma region //Vertex
		if (split_l[0] == "v")
		{
			int i = (split_l[1] == "") ? 1 : 0;
			positions.push_back(
				XMFLOAT3(std::stof(split_l[1 + i]), std::stof(split_l[2 + i]), std::stof(split_l[3 + i])));
			continue;
		}
#pragma endregion
#pragma region //UV
		if (split_l[0] == "vt")
		{
			uv.push_back(
				XMFLOAT2(std::stof(split_l[1]), std::stof(split_l[2])));
			continue;
		}
#pragma endregion
#pragma region //Normal
		if (split_l[0] == "vn")
		{
			normals.push_back(
				XMFLOAT3(std::stof(split_l[1]), std::stof(split_l[2]), std::stof(split_l[3])));
			continue;
		}
#pragma endregion
#pragma region //Face
		if (split_l[0] == "f")
		{
			vertexCount = split_l.size() - 1;
			for (int i = 1; i <= vertexCount; i++)
			{
				if (split_l[i].empty())
				{
					vertexCount--;
					continue;
				}
#pragma region //Set Vertices
				split_f = splitString(split_l[i], "/");

				XMFLOAT3 tangent = {};
				m_obj.vertices.push_back(CVertex(
					-positions[std::stof(split_f[0]) - 1].x,
					-positions[std::stof(split_f[0]) - 1].y,
					-positions[std::stof(split_f[0]) - 1].z,
					uv[std::stof(split_f[1]) - 1].x,
					-uv[std::stof(split_f[1]) - 1].y,
					normals[std::stof(split_f[2]) - 1].x,
					normals[std::stof(split_f[2]) - 1].y,
					normals[std::stof(split_f[2]) - 1].z));
#pragma endregion
			}
#pragma region //Set Indices
			float offSet = (faceCount * vertexCount);
			m_obj.indices.push_back(0 + offSet);
			m_obj.indices.push_back(2 + offSet);
			m_obj.indices.push_back(1 + offSet);
			if (vertexCount == 4)
			{
				m_obj.indices.push_back(0 + offSet);
				m_obj.indices.push_back(3 + offSet);
				m_obj.indices.push_back(2 + offSet);
			}
#pragma endregion
			faceCount++;
		}

		if (vertexCount == 3)
			m_obj.triangles = true;
#pragma endregion
	}
}

std::vector<std::string> CObjLoader::splitString(std::string _line, std::string _delim)
{
	//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	//https://stackoverflow.com/users/90464/fret
	std::vector<std::string> list;
	size_t pos = 0;
	std::string token;

	while ((pos = _line.find(_delim)) != std::string::npos) {
		token = _line.substr(0, pos);
		list.push_back(token);
		_line.erase(0, pos + _delim.length());
	}
	list.push_back(_line);
	return list;
}

void CObjLoader::SafeFile(std::string _fileName, CObj _obj)
{
	std::ofstream fileStream(_fileName + ".txt");
	if (!fileStream.is_open())
		return;

	std::string s = "";

	for (int i = 0; i < _obj.vertices.size(); i++)
	{
		s.append("CVertex(");

		s.append(std::to_string(_obj.vertices[i].pos.x) + ", ");
		s.append(std::to_string(_obj.vertices[i].pos.y) + ", ");
		s.append(std::to_string(_obj.vertices[i].pos.z) + ", ");
		s.append(std::to_string(_obj.vertices[i].texCoord.x) + ", ");
		s.append(std::to_string(_obj.vertices[i].texCoord.y) + ", ");
		s.append(std::to_string(_obj.vertices[i].normal.x) + ", ");
		s.append(std::to_string(_obj.vertices[i].normal.y) + ", ");
		s.append(std::to_string(_obj.vertices[i].normal.z) + "");

		s.append((i == _obj.vertices.size() - 1) ? ") \n" : "), \n");
	}

	s.append("\n");

	for (int i = 0; i < _obj.indices.size(); i++)
	{
		s.append(std::to_string(_obj.indices[i]) + ", ");
	}

	fileStream << s;

	fileStream.close();
}
