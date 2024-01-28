#version 450 core
out vec4 FragColor;

in VS_OUT {
	in vec4 FragColour;
} fs_in;

void main() {
	
	// frustum 
	FragColor = fs_in.FragColour;//vec4(1.0, 1.0, 0.45, 0.15); // White color
}