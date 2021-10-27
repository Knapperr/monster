#version 420
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D image;

// https://gist.github.com/Beefster09/7264303ee4b4b2086f372f1e70e8eddd
// This method is working for making the pixel art look nice
float sharpen(float pix_coord) {
	float sharpness = 5.0;

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
	vec4 tex = texture2D(image, modifiedTextCoordinate);
	FragColor = tex;

	// NOTE(ck): old pixel shader
	// vec2 vres = textureSize(image, 0);
	// FragColor = texture(image, vec2(
	// 	sharpen(TexCoord.x * vres.x) / vres.x,
	// 	sharpen(TexCoord.y * vres.y) / vres.y
	// ));

	// NOTE(ck): only line in shader
	//FragColor = texture(image, TexCoord);
}