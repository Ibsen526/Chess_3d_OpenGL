#pragma once
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/GL.h>
#include <GL/GLU.h>

class IndexBuffer
{
public:
	IndexBuffer()
	{
		bufferID = 0;
	}

	IndexBuffer(void* data, int numIndices, Uint8 elementSize)
	{
		glGenBuffers(1, &bufferID); //Hier können Vertices direkt im GPU-Speicher abgelegt werden
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, numIndices * elementSize, data, GL_STATIC_DRAW);
	}

	~IndexBuffer()
	{
		glDeleteBuffers(1, &bufferID);
	}

	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	}

	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

private:
	GLuint bufferID; //Ein Index im GPU-Speicher
};


