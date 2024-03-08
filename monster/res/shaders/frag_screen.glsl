#version 450 core 

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float brightness = 1.0;

void main()
{
	
    vec3 col = texture(screenTexture, TexCoords).rgb * brightness;
    col *= texture(screenTexture, TexCoords).a; 
    FragColor = vec4(col, 1.0);
    // vec4 col = texture(screenTexture, TexCoords).rgba;
    // FragColor = col;
} 