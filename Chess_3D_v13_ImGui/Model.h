#pragma once
#include "LoadObj.h"

#include <cmath>


class Model
{
public:

	Model()
	{
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
	}

	Model(std::string meshPath, bool isObj, glm::vec2 transform = { 0.0f,0.0f })
	{
		//Loads the mesh
		if (isObj)
			glo::LoadObj(meshPath, &vertices, &indices);
		else
			glo::ReadMObjFile(meshPath, vertices, indices);

		for (Vertex& v : vertices)
		{
			v.x += transform.x;
			v.y += transform.y;
		}

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
	}

	void InitBuffers()
	{
		vertexBuffer = new VertexBuffer(vertices.data(), vertices.size());
		indexBuffer = new IndexBuffer(indices.data(), indices.size(), sizeof(indices[0]));
	}

	static void InitTextures(std::string texPath, GLuint& textureId, int texMode, bool pixely)
	{
		//Loads the texture
		SDL_Surface* surface = IMG_Load(texPath.c_str());

		//Flips the surface
		SDL_LockSurface(surface);

		int pitch = surface->pitch; // row size
		char* temp = new char[pitch]; // intermediate buffer
		char* pixels = (char*)surface->pixels;

		for (int i = 0; i < surface->h / 2; ++i) 
		{
			// get pointers to the two rows to swap
			char* row1 = pixels + i * pitch;
			char* row2 = pixels + (surface->h - i - 1) * pitch;

			// swap rows
			memcpy(temp, row1, pitch);
			memcpy(row1, row2, pitch);
			memcpy(row2, temp, pitch);
		}

		delete[] temp;
		SDL_UnlockSurface(surface);


		textureId = 0;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pixely ? GL_NEAREST : GL_LINEAR); //Wenn die Kamera nahe bzw. weit weg von der Texture ist, muss sie kleiner gezeichnet werden
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pixely ? GL_NEAREST : GL_LINEAR); //linear: verschwommen wenn nahe dran, nearest: pixelig
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //Große Texturen werden nicht gestreckt/abgeschnitten, sondern gespiegelt und nebeneinander gelegt
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_EDGE

		//GL_RGBA abhängig von der Bittiefe des Bilds, also ob es einen Alpha Channel hat oder nicht
		glTexImage2D(GL_TEXTURE_2D, 0, texMode, surface->w, surface->h, 0, texMode, GL_UNSIGNED_BYTE, surface->pixels); //GL_RGBA8 bei erstem

		glBindTexture(GL_TEXTURE_2D, 0);
		SDL_FreeSurface(surface);

	}

	void DrawModel(GLuint textureId, int mId, glm::mat4 modelMat, int viewId, glm::mat4 viewMat, int projId, glm::mat4 projMat)
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();

		//Set the model and view matrix
		glUniformMatrix4fv(mId, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(viewId, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(projId, 1, GL_FALSE, &projMat[0][0]);

		//Draw Texture
		glActiveTexture(GL_TEXTURE0); //Nutzt den 0ten Texture-Slot
		glBindTexture(GL_TEXTURE_2D, textureId);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 

		indexBuffer->Unbind();
		vertexBuffer->Unbind();
	}


private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	std::vector<Vertex> vertices;
	std::vector<Uint32> indices;
	//store the indices.size and not the vertices and indices in the vectors
	//GLuint textureId;

	

};
