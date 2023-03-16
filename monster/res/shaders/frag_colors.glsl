#version 450 core
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

in VS_OUT {
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoords;
} fs_in;



uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform bool collider;
uniform bool pixelTexture;
uniform vec3 colliderColor = vec3(0.4, 0.5, 0.5);

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
        vec4 texColor = texture(texture_diffuse1, fs_in.TexCoords);
        if (texColor.a < 0.1)
        {
            discard;
        }

        // IMPORTANT(ck): sets texture without pixel filter on it
        //FragColor = texColor;
        if (pixelTexture)
        {
            // Smooth the texture
            // https://handmade.network/forums/t/7883-pixel_art_fragment_shader
            // Make sure texture has bilinear sampling set, and does not have mipmaps
            vec2 textureSize = textureSize(texture_diffuse1, 0);
            vec2 pixel = fs_in.TexCoords * textureSize;
            vec2 duDv = fwidth(pixel);
            vec2 seam = floor(pixel + 0.5);
            pixel = seam + clamp((pixel - seam)/duDv, -0.5, 0.5);
            vec2 modifiedTextCoordinate = pixel / textureSize;
            // NOTE(ck): this is to just use the texture no lighting
            //vec4 tex = texture(texture_diffuse1, modifiedTextCoordinate);
            //FragColor = tex;

            // ambient
            // material.diffuse = texture_diffuse1 
            // NOTE(ck): diffuse map is part of the material on learnopengl.
            vec3 ambient = light.ambient * texture(texture_diffuse1, modifiedTextCoordinate).rgb;
            
            // diffuse 
            vec3 norm = normalize(fs_in.Normal);
            vec3 lightDir = normalize(light.pos - fs_in.FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, modifiedTextCoordinate).rgb;  
            
            // specular
            vec3 viewDir = normalize(viewPos - fs_in.FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            // fix specular to use material.specular 
            // https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/4.1.lighting_maps_diffuse_map/4.1.lighting_maps.fs
            // the material uses sampler2D for diffuse and specular earlier version uses vec3 instead
            vec3 specular = light.specular * spec * texture(texture_diffuse1, modifiedTextCoordinate).rgb;  
                
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, texture(texture_diffuse1, modifiedTextCoordinate).a);
            
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
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.pos - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.specular);

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}