#version 450

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texC;
layout(location = 3) uniform vec3 diffuseColor;
layout(location = 4) uniform vec3 ambientColor;
layout(location = 5) uniform vec3 specularColor;
layout(location = 6) uniform float shininessValue;

out vec3 passPosition;
out vec3 passNormal;
out vec2 passTexCoord;
out vec4 passShadowCoord;
//out vec3 passColor;

out struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
} material;

void main()
{
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

    passPosition = (modelMatrix * vec4(position, 1.f)).xyz;
    passNormal = (modelMatrix * vec4(normal, 0)).xyz;
    passTexCoord = texC;

    material.diffuseColor = diffuseColor;
    material.ambientColor = ambientColor;
    material.specularColor = specularColor;
    material.shininess = shininessValue;
}
