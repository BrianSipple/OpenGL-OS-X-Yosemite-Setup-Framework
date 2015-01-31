#version 410 core

in vec3 Color;

out vec4 outColor;


void main(void)
{

    outColor = vec4(Color, 1.0f);

}
