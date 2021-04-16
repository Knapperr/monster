#version 420
in vec2 TexCoords;
in vec2 Pixel;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

// https://gist.github.com/Beefster09/7264303ee4b4b2086f372f1e70e8eddd
// This method is working for making the pixel art look nice
float sharpen(float pix_coord) {
	float sharpness = 2.0;

    float norm = (fract(pix_coord) - 0.5) * 2.0;
    float norm2 = norm * norm;
    return floor(pix_coord) + norm * pow(norm2, sharpness) / 2.0 + 0.5;
}

// https://www.shadertoy.com/view/MlB3D3 - d7Samurai
void main()
{
	// emulate point sampling
	// vec2 uv = floor(TexCoords) + 0.5;
	// subpixel algorithm
	// uv += 1.0 - clamp((1.0 - fract(TexCoords)) * 1.0, 0.0, 1.0);
	//color = vec4(spriteColor, 1.0) * texture(image, uv);
	// color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
	//color = texture(image, TexCoords);


	// Get the other method working.. don't like this
	// this is meant to be paired with an FBO I think
	vec2 vres = textureSize(image, 0);
	color = texture(image, vec2(
		sharpen(TexCoords.x * vres.x) / vres.x,
		sharpen(TexCoords.y * vres.y) / vres.y
	));
}