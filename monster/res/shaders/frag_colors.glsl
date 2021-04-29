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

void main()
{
    if (useTexture)
    {
        FragColor = texture(texture_diffuse1, TexCoords);
        return;
    }

    if (collider)
    {
        vec3 colliderColor = vec3(0.2, 0.7, 0.3);
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