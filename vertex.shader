#version 410 core

//in vec2 position;
//in vec3 color;

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 color;


out vec3 Color;



void main()
{
    Color = color;
    gl_Position = vec4(position, 1.0f);

}
