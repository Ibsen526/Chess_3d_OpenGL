#version 330 core

layout(location = 0) out vec4 f_color; //Gibt die Farb und Positionswerte zurück zum Programm

in vec2 v_texCoords;
in vec3 v_normal;
in vec3 v_fragPos;

uniform sampler2D u_texture;
uniform vec3 u_lightPos;
uniform vec4 u_lightColor;
uniform float u_lightPower;
uniform vec3 u_viewPos;

void main() //Fragment Shader
{
    vec4 texColor = texture(u_texture, v_texCoords);

    //Ambient lighting
    vec4 ambient = u_lightColor * 0.1f;

    //Diffuse lighting
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    vec3 normal = normalize(v_normal);
    vec4 diffuse = u_lightColor * max(dot(normal, lightDir), 0.0f);

    //Specular lighting
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal); //Reflect needs to point from lightPos to the Fragment: negate lightDir

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec4 specular = 0.5f * u_lightColor * spec; 

    f_color = (ambient + diffuse + specular) * texColor; 
}