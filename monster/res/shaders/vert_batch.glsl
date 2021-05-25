#version 420
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texture;

out vec2 tex;
out vec4 col;

uniform mat4 transformMatrix;
 

void main()
{
    gl_Position = transformMatrix * vec4(position.xy, 0, 1);
}