#version 450 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D image;
uniform bool collider;

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
/*
from martins to clamp uv coords??
vec2 size = vec2(textureSize(your_texture_sampler, 0));
uv = uv * size;
vec2 duv = fwidth(uv);
uv = floor(uv) + vec2(0.5) + clamp((fract(uv) - vec2(0.5) + duv)/duv, 0, 1);
uv /= size;
*/

	// cant do this in a batcher
	// if (collider)
	// {
	// 	vec4 orange = vec4(1.0f, 0.5f, 0.2f, 1.0f); 
	// 	FragColor = orange;
	// 	return;
	// }


	// TODO(ck): https://www.shadertoy.com/view/ltBGWc
	// vec2 textureSize = textureSize(image, 0);

	// vec2 pixel = TexCoord * textureSize;
	// vec2 fatPixel = floor(pixel)+0.5;
	// float texelsPerPixel = 16.0;
	// // subpixel aa algorithm (COMMENT OUT TO COMPARE WITH POINT SAMPLING)
	// fatPixel += 1 - clamp((1.0 - fract(pixel)) * texelsPerPixel, 0, 1);

	// vec2 coord = fatPixel / textureSize;

	// vec4 tex = texture(image, coord);
	// FragColor = tex;

	// TODO(ck): new pixel shader
	// Smooth the texture
    // https://handmade.network/forums/t/7883-pixel_art_fragment_shader
    // Make sure texture has bilinear sampling set, and does not have mipmaps


/*
NEW FROM MARTINS
vec4 texture2DAA(sampler2D tex, vec2 uv) {
    vec2 texsize = vec2(textureSize(tex,0));
    vec2 uv_texspace = uv*texsize;
    vec2 seam = floor(uv_texspace+.5);
    uv_texspace = (uv_texspace-seam)/fwidth(uv_texspace)+seam;
    uv_texspace = clamp(uv_texspace, seam-.5, seam+.5);
    return texture(tex, uv_texspace/texsize);
}
*/
/*
 OLD ONE
	vec2 texSize = textureSize(image, 0);
	vec2 pixel = (TexCoord * texSize);
	vec2 seam = floor(pixel + 0.5);
	vec2 duDv = fwidth(pixel);
	pixel = seam + clamp((pixel - seam)/duDv, -0.5, 0.5);
	vec2 modifiedTextCoordinate = pixel / texSize;
	vec4 tex = texture(image, modifiedTextCoordinate);
	*/

	vec2 texSize = textureSize(image, 0);
	vec2 uv_texspace = (TexCoord * texSize);
	vec2 seam = floor(uv_texspace + 0.5);
	uv_texspace = (uv_texspace - seam)/fwidth(uv_texspace)+seam;
	uv_texspace = clamp(uv_texspace, seam - 0.5, seam + 0.5);
	vec4 tex = texture(image, uv_texspace/texSize);

	//vec4 orange = vec4(1.0, 0.5, 0.2, 1.0);

	FragColor = tex;
	//FragColor = texture(image, TexCoord);
	return;
	

	// NOTE(ck): OLD method
	// vec2 vres = textureSize(image, 0);
	// color = texture(image, vec2(
	// 	sharpen(TexCoords.x * vres.x) / vres.x,
	// 	sharpen(TexCoords.y * vres.y) / vres.y
	// ));
}