#version 420
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	ourColor = aColor;
	TexCoord = aTexCoord;
	//gl_Position =  projection * model * vec4(aPos, 1.0);
	gl_Position = projection * vec4(aPos, 1.0);
}