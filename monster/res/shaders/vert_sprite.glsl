#version 450 core
layout (location = 0) in vec3 aPos; // vec2 position, vec2 texCoords
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

// TODO(ck): remove model matrix
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	ourColor = aColor;
	TexCoord = aTexCoord;
	//gl_Position = projection * vec4(aPos, 1.0);
	
	// using model matrix -- dont need?
	vec3 pos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(pos, 1.0);
}