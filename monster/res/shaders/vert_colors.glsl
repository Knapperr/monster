#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	out vec3 FragPos;
	out vec3 Normal;
	out vec2 TexCoords;
} vs_out;


uniform float texCoordScale = 1.0;


layout (std140) uniform CameraBlock
{
	mat4 projection; // 0   
	mat4 view; // 64
	vec3 lightDirection; // 0
	vec3 lightColour; // 16
};

layout (std140) uniform ModelBlock
{
	mat4 model;
	vec4 colour; // offset 64 (64 bytes from mat4) -- size16 (alligned vec3)
};

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.TexCoords = aTexCoords * texCoordScale;
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}