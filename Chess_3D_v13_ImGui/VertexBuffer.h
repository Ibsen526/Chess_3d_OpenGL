#pragma once
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/GL.h>
#include <GL/GLU.h>

struct Vertex
{
	//Positionswerte
	float x, y, z; 

	//Texturwerte
	float u, v;

	//Normalenwerte
	float xn, yn, zn;
};

class VertexBuffer
{
public:
	/*VertexBuffer()
	{
		vao = 0;
		bufferID = 0;
	}*/

	VertexBuffer(void* data, int numVertices)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &bufferID); //Hier können Vertices direkt im GPU-Speicher abgelegt werden
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, x)); //statt offsetof(struct Vertex,x) kann auch 0 stehen

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, u));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, xn));

		//offset gibt an wo die Werte beginnen.

		glBindVertexArray(0);
	}

	~VertexBuffer()
	{
		glDeleteBuffers(1, &bufferID);
	}

	void Bind()
	{
		glBindVertexArray(vao);
	}

	void Unbind()
	{
		glBindVertexArray(0);
	}

private:
	GLuint bufferID; //Ein Index im GPU-Speicher
	GLuint vao; //Vertex Array Object

};

