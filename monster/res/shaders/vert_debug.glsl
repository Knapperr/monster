#version 450 core 
layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexColour;

out VS_OUT {
	out vec4 FragColour;
} vs_out;

// change to UBO
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invViewProj;

// uniform bool isBox = false; 

// layout (std140) uniform AABBBlock
// {
	// // use for vertexPosition instead of 
	// vec3 worldPos; // 0 
// };

void main() 
{
	// vec3 pos = vec3(invViewProj * vec4(vertexPosition, 1.0));
	//vec3 pos = vec3(mat4(1.0) * vec4(vertexPosition, 1.0));
	// gl_Position = projection * view * vec4(pos, 1.0);
	vs_out.FragColour = vertexColour;
	gl_Position = projection * view * vertexPosition;
}