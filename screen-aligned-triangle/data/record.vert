#version 420 core

in vec2 in_vertex;

void main()
{
    gl_Position = vec4(in_vertex, 0.0, 1.0);
}
