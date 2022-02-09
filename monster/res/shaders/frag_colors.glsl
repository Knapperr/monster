#version 330 core
out vec4 FragColor;
  
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform bool collider;
uniform bool pixelTexture;
uniform vec3 colliderColor;

// this is a better way to do pixel art
// https://www.shadertoy.com/view/MlB3D3 - d7Samurai

// This method is working for making the pixel art look nice
// https://gist.github.com/Beefster09/7264303ee4b4b2086f372f1e70e8eddd
float sharpen(float pix_coord) {
	float sharpness = 1.0;

    float norm = (fract(pix_coord) - 0.5) * 2.0;
    float norm2 = norm * norm;
    return floor(pix_coord) + norm * pow(norm2, sharpness) / 2.0 + 0.5;
}

void main()
{
    if (useTexture)
    {
        // Remove white pixels on texture
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
        {
            discard;
        }

        // IMPORTANT(ck): sets texture without pixel filter on it
        //FragColor = texColor;
        if (pixelTexture)
        {
            vec2 textureSize = textureSize(texture_diffuse1, 0);
            vec2 pixel = TexCoords * textureSize;
            vec2 duDv = fwidth(pixel);
            vec2 seam = floor(pixel + 0.5);
            pixel = seam + clamp((pixel - seam)/duDv, -0.5, 0.5);
            vec2 modifiedTextCoordinate = pixel / textureSize;
            vec4 tex = texture(texture_diffuse1, modifiedTextCoordinate);
            FragColor = tex;
            return;
            // vec2 vres = textureSize(texture_diffuse1, 0);
            // FragColor = texture(texture_diffuse1, vec2(
		    //     sharpen(TexCoords.x * vres.x) / vres.x,
		    //     sharpen(TexCoords.y * vres.y) / vres.y
            // )); 
        }
        
        return;
    }

    if (collider)
    {
        FragColor = vec4(colliderColor, 1.0);
        return;
    }

    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.specular);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}