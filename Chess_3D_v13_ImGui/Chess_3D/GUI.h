#pragma once

struct VertexGUI
{
	float x, y;
	float u, v;
};

class GUI
{
public:
	GUI()
	{
		vaoGUI = 0;
		bufferIdGUI = 0;
	}

	GUI(void* data, int numVertices)
	{
		glGenVertexArrays(1, &vaoGUI);
		glBindVertexArray(vaoGUI);

		glGenBuffers(1, &bufferIdGUI);
		glBindBuffer(GL_ARRAY_BUFFER, bufferIdGUI);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexGUI), data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGUI), (void*)offsetof(struct VertexGUI, x)); //statt offsetof(struct Vertex,x) kann auch 0 stehen

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGUI), (void*)offsetof(struct VertexGUI, u));

		glBindVertexArray(0);
	}

	~GUI()
	{
		glDeleteBuffers(1, &bufferIdGUI);
	}

	void BindGUI()
	{
		glBindVertexArray(vaoGUI);
	}

	void UnbindGUI()
	{
		glBindVertexArray(0);
	}

private:
	GLuint vaoGUI;
	GLuint bufferIdGUI;


};