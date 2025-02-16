#version 330 core

layout(location = 0) in vec3 a_position; //a steht für Attribut
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normals;

out vec2 v_texCoords;
out vec3 v_normal;
out vec3 v_fragPos;

uniform mat4 u_model; //Uniform der Model-Matrix
uniform mat4 u_view;
uniform mat4 u_projection;

void main() //Vertex Shader
{
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0f);
	v_texCoords = a_texCoords;

	v_fragPos = vec3(u_model * vec4(a_position, 1.0f));
	v_normal = mat3(transpose(inverse(u_model))) * a_normals;
	//v_normal = a_normals;
}