#version 450 core 

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float brightness = 1.0;

void main()
{
	
    vec4 col = texture(screenTexture, TexCoords).rgba * brightness;

    // col.rgb /= col.a;
    // if (col.a < 0.1)
		//   discard;
    if (col.a > 0.1) 
    {
        col.rgb /= col.a;
    }
	  else 
	  {
		  discard;
	  }

    FragColor = col;
    // vec4 col = texture(screenTexture, TexCoords).rgba;
    // FragColor = col;
} 