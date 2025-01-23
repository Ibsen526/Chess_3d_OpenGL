#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GUI.h"

#include "libs/glm/glm.hpp" 
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

static class glo 
{
public:
	static bool LoadObj(std::string path, std::vector<Vertex>* vertices, std::vector<Uint32>* indices)
	{
		std::fstream file;
		std::string line;
		std::vector<glm::vec3> v; //vertices
		std::vector<glm::vec2> vt; //vertex textures
		std::vector<glm::vec3> vn; //vertex normals
		std::vector<glm::vec3> f; //indices
		Uint32 indexCounter = 0;

		file.open(path, std::ios::in);
		if (!file) return false;


		while (std::getline(file, line))
		{
			std::stringstream puffer(line);
			std::string type = "";

			puffer >> type;

			if (type == "v")
			{
				float x = 0.0f, y = 0.0f, z = 0.0f;

				puffer >> x >> y >> z;

				v.push_back({ x, y, z });
			}
			else if (type == "vt")
			{
				float u = 0.0f, v = 0.0f;

				puffer >> u >> v;

				vt.push_back({ u, v });
			}
			else if (type == "vn")
			{
				float x = 0.0f, y = 0.0f, z = 0.0f;

				puffer >> x >> y >> z;

				vn.push_back({ x, y, z });
			}
			else if (type == "f")
			{
				// v/t/n ... indices of the vertex, texture and normals
				std::string f1 = "", f2 = "", f3 = "";
				
				puffer >> f1 >> f2 >> f3;

				storeIndices(f1, f);
				storeIndices(f2, f);
				storeIndices(f3, f);

				/*StoreVertexByIndex(vertices, indices, v, vt, vn, f1, indexCounter);
				StoreVertexByIndex(vertices, indices, v, vt, vn, f2, indexCounter);
				StoreVertexByIndex(vertices, indices, v, vt, vn, f3, indexCounter);*/
			}
		}

		file.close();

		//Slow but less memory
		//Store the gained vertex/texture coords according to the indices
		/*Uint32 ti = 0; //counts only the original faces (the size of vertices)
		std::vector<glm::vec3> tF; //stores all the faces only once
		for (Uint32 i = 0; i < f.size(); i++)
		{			
			bool isIn = false;
			for (Uint32 j = 0; j < tF.size(); j++)
			{
				if (f[i].x == tF[j].x && f[i].y == tF[j].y && f[i].z == tF[j].z) //checks if the current face is already stored
				{
					isIn = true;
					indices->push_back(j);
					break;
				}
			}

			if (!isIn)
			{
				vertices->push_back({ v[f[i].x].x, v[f[i].x].y, v[f[i].x].z,
					vt[f[i].y].x, vt[f[i].y].y,
					vn[f[i].z].x, vn[f[i].z].y, vn[f[i].z].z });
				tF.push_back(f[i]);
				indices->push_back(ti);
				ti++;
			}
		}*/ 

		//Faster but more memory usage (indexing gets useless!)
		for (Uint32 i = 0; i < f.size(); i++)
		{
			glm::vec3* face = &f[i];
			vertices->push_back({ v[face->x].x, v[face->x].y, v[face->x].z,
					vt[face->y].x, vt[face->y].y,
					vn[face->z].x, vn[face->z].y, vn[face->z].z });
			indices->push_back(i);
		}

		//Slow but maybe a better implementation
		/*Uint32 nOriginalFaces = 0;
		std::vector<glm::vec3> facesAlreadyStored;
		for (Uint32 i = 0; i < f.size(); i++)
		{
			auto storedFaceIter = std::find(facesAlreadyStored.begin(), facesAlreadyStored.end(), f[i]);
			if (storedFaceIter == facesAlreadyStored.end())
			{
				//Face not in vector
				vertices->push_back({ v[f[i].x].x, v[f[i].x].y, v[f[i].x].z,
					vt[f[i].y].x, vt[f[i].y].y,
					vn[f[i].z].x, vn[f[i].z].y, vn[f[i].z].z });
				indices->push_back(nOriginalFaces);
				facesAlreadyStored.push_back(f[i]);
				nOriginalFaces++;
			}
			else
			{
				//Face already in vector
				indices->push_back(storedFaceIter - facesAlreadyStored.begin());
			}
		}*/

		return true;
	}

	static bool ReadMObjFile(std::string path, std::vector<Vertex>& vertices, std::vector<Uint32>& indices)
	{
		std::fstream file;
		file.open(path, std::ios::in | std::ios::binary);
		if (!file)
			return false;

		//Read vertices
		std::string line;
		std::getline(file, line);
		std::stringstream lineStream(line);
		Uint32 vertCount = 0, indCount = 0;
		lineStream >> vertCount >> indCount;
		for (Uint32 i = 0; i < vertCount; i++)
		{
			Vertex v;
			file.read((char*)&v, sizeof(Vertex));
			vertices.push_back(v);
		}

		//Read indices
		for (Uint32 i = 0; i < indCount; i++)
		{
			Uint32 readInd = 0;
			file.read((char*)&readInd, sizeof(Uint32));
			indices.push_back(readInd);
		}
	}

	static bool Load2dObj(std::string path, std::vector<VertexGUI>& vertices)
	{
		std::fstream file;
		std::string line;
		std::vector<glm::vec2> v; //vertices
		std::vector<glm::vec2> vt; //vertex textures


		file.open(path, std::ios::in);
		if (!file) return false;


		while (std::getline(file, line))
		{
			std::stringstream puffer(line);
			std::string type = "";

			puffer >> type;

			if (type == "v")
			{
				float x = 0.0f, y = 0.0f;

				puffer >> x >> y;

				v.push_back({ x, y });
			}
			else if (type == "vt")
			{
				float u = 0.0f, v = 0.0f;

				puffer >> u >> v;

				vt.push_back({ u, v });
			}
		}

		//Combine the vertex and texture coords into the gui vertex
		for (Uint8 i = 0; i < v.size(); i++)
		{
			vertices.push_back({ v[i].x, v[i].y, vt[i].x, vt[i].y });
		}

		file.close();

		return true;
	}

private:
	static inline void storeIndices(std::string x, std::vector<glm::vec3>& f)
	{
		Uint32 iv = 0, ivt = 0, ivn = 0;

		for (char& c : x)
			if (c == '/')
				c = ' ';

		std::stringstream puffer(x);
		puffer >> iv >> ivt >> ivn;

		f.push_back({ iv - 1, ivt - 1, ivn - 1 });
	}

	static inline void StoreVertexByIndex(std::vector<Vertex>* vertices, std::vector<Uint32>* indices,
		std::vector<glm::vec3>& v, std::vector<glm::vec2>& vt, std::vector<glm::vec3>& vn,
		std::string vertexDataIndices, Uint32& indexCounter)
	{
		Uint32 iv = 0, ivt = 0, ivn = 0;

		for (char& c : vertexDataIndices)
			if (c == '/')
				c = ' ';

		std::stringstream puffer(vertexDataIndices);
		puffer >> iv >> ivt >> ivn;
		iv--; ivt--; ivn--;

		vertices->push_back({ v[iv].x, v[iv].y, v[iv].z,
				vt[ivt].x, vt[ivt].y,
				vn[ivn].x, vn[ivn].y, vn[ivn].z });
		indices->push_back(indexCounter);
		indexCounter++;
	}
};
