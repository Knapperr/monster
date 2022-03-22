#version 450 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D image;

// https://gist.github.com/Beefster09/7264303ee4b4b2086f372f1e70e8eddd
// This method is working for making the pixel art look nice
float sharpen(float pix_coord) {
	float sharpness = 4.0;

    float norm = (fract(pix_coord) - 0.5) * 2.0;
    float norm2 = norm * norm;
    return floor(pix_coord) + norm * pow(norm2, sharpness) / 2.0 + 0.5;
}

void main()
{
	// TODO(ck): new pixel shader
	vec2 textureSize = textureSize(image, 0);
	vec2 pixel = TexCoord * textureSize;
	vec2 duDv = fwidth(pixel);
	vec2 seam = floor(pixel + 0.5);
	pixel = seam + clamp((pixel - seam)/duDv, -0.5, 0.5);
	vec2 modifiedTextCoordinate = pixel / textureSize;
	vec4 tex = texture(image, modifiedTextCoordinate);
	FragColor = tex;


	// NOTE(ck): OLD method
	// vec2 vres = textureSize(image, 0);
	// color = texture(image, vec2(
	// 	sharpen(TexCoords.x * vres.x) / vres.x,
	// 	sharpen(TexCoords.y * vres.y) / vres.y
	// ));
}