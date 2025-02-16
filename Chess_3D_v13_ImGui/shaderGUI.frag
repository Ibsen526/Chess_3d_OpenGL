#version 330 core

layout(location = 0) out vec4 f_color; //Gibt die Farb und Positionswerte zurück zum Programm

in vec2 v_texCoords;

uniform sampler2D u_texture_gui;

void main() //Fragment Shader
{
    vec4 texColor = texture(u_texture_gui, v_texCoords);
    if(texColor.a < 0.1f)
        discard;
    f_color = texColor; //Zuweisung der Farbe aus Uniform
}