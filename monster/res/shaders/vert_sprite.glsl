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


// Converts pixel coord to NDC 
vec2 pixel2GL(vec2 pos)
{
	// NOTE(ck): This is fine if there is no projection matrix
	// 960/540 --- 480/270
	vec2 res = pos / vec2(480.0, 270.0);
	res = res * 2 - 1;
	//res.y *= -1;
	return res;
}

void main()
{
	ourColor = aColor;
	TexCoord = aTexCoord;
	//gl_Position = projection * vec4(aPos, 1.0);
	
	// using model matrix -- dont need?
	//vec3 pos = vec3(model * vec4(aPos, 1.0));
	//vec2 tilePos = pixel2GL(vec2(pos.x, pos.y)); // Pixel to NDCPos
	//vec3 finalPos = vec3(tilePos, aPos.z);
	
	//gl_Position = view * vec4(pos, 1.0);
	gl_Position = projection * view * vec4(aPos, 1.0);
}