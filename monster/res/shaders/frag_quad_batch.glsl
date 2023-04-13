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
// this is a better way to do pixel art
// https://www.shadertoy.com/view/MlB3D3 - d7Samurai
void main()
{
        // Remove white pixels on texture
        vec4 texColor = texture(texture_diffuse1, fs_in.TexCoords);
        if (texColor.a < 0.1)
        {
            discard;
        }

        // Smooth the texture
        vec2 textureSize = textureSize(texture_diffuse1, 0);
        vec2 pixel = fs_in.TexCoords * textureSize;
        vec2 duDv = fwidth(pixel);
        vec2 seam = floor(pixel + 0.5);
        pixel = seam + clamp((pixel - seam)/duDv, -0.5, 0.5);
        vec2 modifiedTextCoordinate = pixel / textureSize;
        // NOTE(ck): this is to just use the texture no lighting
        vec4 tex = texture(texture_diffuse1, modifiedTextCoordinate);
        FragColor = tex;

        // ambient
        // material.diffuse = texture_diffuse1 
        // NOTE(ck): diffuse map is part of the material on learnopengl.
/*
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
*/
        // vec2 vres = textureSize(texture_diffuse1, 0);
        // FragColor = texture(texture_diffuse1, vec2(
        //     sharpen(TexCoords.x * vres.x) / vres.x,
        //     sharpen(TexCoords.y * vres.y) / vres.y
        // ));
}