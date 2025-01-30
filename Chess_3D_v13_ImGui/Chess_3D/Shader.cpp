#pragma once
#include "Shader.h"

Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	shaderID = CreateShader(vertexShaderFilename, fragmentShaderFilename);
}

Shader::~Shader()
{
	glDeleteProgram(shaderID);
}

void Shader::Bind()
{
	glUseProgram(shaderID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

GLuint Shader::Compile(std::string shaderSource, GLenum type)
{
	//Erstellt den Shader für den übergebenen Typ (Vertex/Fragment)

	GLuint id = glCreateShader(type);
	const char* src = shaderSource.c_str();

	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	//Compile Errors abfangen
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE)
	{
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Shader compilation error: " << message << std::endl;
		delete[] message;
		return 0;
	}

	return id;
}

std::string Shader::Parse(const char* filename)
{
	//Hier werden die Shader-Dateien ausgelesen und die Inhalte zurückgegeben

	FILE* file;
	//rb: read binary
	if (fopen_s(&file, filename, "rb") != 0) {
		std::cout << "File " << filename << " not found" << std::endl;
		return "";
	}

	std::string contents;
	fseek(file, 0, SEEK_END); //Geht ans Ende der Datei
	size_t filesize = ftell(file); //Größe der Datei
	rewind(file); //Geht an den Anfang 
	contents.resize(filesize);

	fread(&contents[0], 1, filesize, file);
	fclose(file);

	//std::cout << contents << std::endl;

	return contents;
}

GLuint Shader::CreateShader(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	std::string vertexShaderSource = Parse(vertexShaderFilename);
	std::string fragmentShaderSource = Parse(fragmentShaderFilename);

	GLuint program = glCreateProgram();
	GLuint vertexShader = Compile(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = Compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	return program;
}

GLuint Shader::getShaderID()
{
	return shaderID;
}
