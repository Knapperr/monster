#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 worldPos;

out VS_OUT {
	out vec3 FragPos;
	out vec3 Normal;
	out vec2 TexCoords;
} vs_out;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraRight_worldspace;
uniform vec3 cameraUp_worldspace;

uniform bool rotateBatch;

uniform float texCoordScale = 1.0;

void main()
{
	// NOTE(ck): Go back to model matrix
	// vec3 billboardPos = aPos;
	// vec2 billboardSize = vec2(1.0, 1.0);
	// vec3 vertexPositionWorldSpace = billboardPos 
	// 	+ cameraRight_worldspace * aPos.y * billboardSize.x
	//  	+ cameraUp_worldspace * aPos.z * billboardSize.y;
	/*
	vec3 particleCenter_wordspace = BillboardPos;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * BillboardSize.x
		+ CameraUp_worldspace * squareVertices.y * BillboardSize.y;


	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
	*/

	//vs_out.FragPos = vec3(vec4(vertexPositionWorldSpace, 1.0));
	
	/*
	Calculate model matrix in shader
	vec3 p = vec3(aPos.x, aPos.y, aPos.z);
	float newY = p.y*cos(45.0) - p.z*sin(45.0);
	float newZ = (p.z*cos(45.0) - p.y*sin(45.0));
	
	// NOTE(ck): hacky way to get worldPos into model matrix
	mat4 modelMat = mat4(1.0);
	mat4 translationMatrix = mat4(1.0);
	translationMatrix[3] = vec4(aPos, 1.0);

    // Build the rotation matrix
	float angle = 45.0;
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0 - c;

	vec3 rotationAxis = vec3(1.0, 0.0, 0.0);
    mat3 rotMatrix = mat3(
        c + rotationAxis.x * rotationAxis.x * t, rotationAxis.x * rotationAxis.y * t - rotationAxis.z * s, rotationAxis.x * rotationAxis.z * t + rotationAxis.y * s,
        rotationAxis.y * rotationAxis.x * t + rotationAxis.z * s, c + rotationAxis.y * rotationAxis.y * t, rotationAxis.y * rotationAxis.z * t - rotationAxis.x * s,
        rotationAxis.z * rotationAxis.x * t - rotationAxis.y * s, rotationAxis.z * rotationAxis.y * t + rotationAxis.x * s, c + rotationAxis.z * rotationAxis.z * t
    );
    // Apply the rotation matrix to the model matrix
    mat4 combinedMat = translationMatrix * mat4(rotMatrix);

	// now move the position back after rotation
	mat4 nextTranslationMatrix = mat4(1.0);
	nextTranslationMatrix[3] = vec4(-aPos, 1.0);
	combinedMat = combinedMat * nextTranslationMatrix;

	*/

	//
	// face camera
	//
/*
	mat4 modelMat = mat4(1.0);
	vec3 vertPos = worldPos + cameraRight_worldspace * aPos.x * 1.0
		+ cameraUp_worldspace * aPos.y * 1.0;
*/
	//
	//	rotation matrix
	//

	// Add our world position the model matrix
	mat4 modelMat = mat4(1.0);
	if (rotateBatch)
	{
		mat4 translationMatrix = mat4(1.0);
		translationMatrix[3] = vec4(worldPos, 1.0);
		modelMat = modelMat * translationMatrix;

		// Build the rotation matrix
		float angle = 45.0;
		float c = cos(angle);
		float s = sin(angle);
		float t = 1.0 - c;

		// add rotation in
		vec3 rotationAxis = vec3(1.0, 0.0, 0.0);
		mat3 rotMatrix = mat3(
			c + rotationAxis.x * rotationAxis.x * t, rotationAxis.x * rotationAxis.y * t - rotationAxis.z * s, rotationAxis.x * rotationAxis.z * t + rotationAxis.y * s,
			rotationAxis.y * rotationAxis.x * t + rotationAxis.z * s, c + rotationAxis.y * rotationAxis.y * t, rotationAxis.y * rotationAxis.z * t - rotationAxis.x * s,
			rotationAxis.z * rotationAxis.x * t - rotationAxis.y * s, rotationAxis.z * rotationAxis.y * t + rotationAxis.x * s, c + rotationAxis.z * rotationAxis.z * t
		);
		// Apply the rotation matrix to the model matrix
		modelMat = modelMat * mat4(rotMatrix);

		// now remove worldPos and move back after rotating
		translationMatrix = mat4(1.0);
		translationMatrix[3] = vec4(-worldPos, 1.0);
		modelMat = modelMat * translationMatrix;
	}

	//vs_out.FragPos = vec3(modelMat * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0));
	vs_out.FragPos = vec3(modelMat * vec4(aPos.x, aPos.y, aPos.z, 1.0));
	vs_out.Normal = mat3(transpose(inverse(modelMat))) * aNormal;
	vs_out.TexCoords = aTexCoords * texCoordScale;
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
	//gl_Position = projection * view * vec4(vertexPositionWorldSpace, 1.0);
}