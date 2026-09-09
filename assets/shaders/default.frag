#version 460 core

struct Material 
{
	vec3 emission;
    sampler2D emissionMap;
	vec3 diffuse;
    sampler2D diffuseMap;
	vec3 specular;
    sampler2D specularMap;
    float shininess;
}; 

struct PointLight 
{
    vec3 position;

	float constant; // Base value of light
	float linear; // Linear falloff over distance
	float quadratic; // Quadratic falloff over distance^2

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float intensity;
};

struct DirLight 
{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SceneLight
{
	vec3 ambient;
};

#define MAX_POINT_LIGHTS 32

out vec4 FragColor; 

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 texCoord;

layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 FragPos;

layout(location = 4) in float depth;

// Camera's position
uniform vec3 viewPos;
uniform bool useTexture;
uniform vec4 objColor;

// Sun
uniform DirLight sun;

uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform Material material;

uniform SceneLight sceneLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;

	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
	}

	//result += CalcDirLight(sun, Normal, viewDir);

	vec4 texColor = texture(material.diffuseMap, texCoord);
	if(texColor.a < 0.1)
        discard;
	FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseMap, texCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseMap, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, texCoord));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	light.constant = 1.0f;
	vec3 diffuse_ =  material.diffuse.rgb * (texture(material.diffuseMap, texCoord).rgb != vec3(0) ? texture(material.diffuseMap, texCoord).rgb : vec3(1.0f));
	vec3 emission_ = material.emission * texture(material.emissionMap, texCoord).rgb;
	vec3 specular_ = material.specular * texture(material.specularMap, texCoord).rgb;

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(Normal, lightDir), 0);	

	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

	float distanceFromLight = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distanceFromLight + light.quadratic * (distanceFromLight * distanceFromLight));

	vec3 ambient = (light.ambient * emission_);
	vec3 diffuse = light.diffuse * (diff * diffuse_)*light.intensity;
	vec3 specular = (spec * specular_);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + sceneLight.ambient/25.0f * (texture(material.diffuseMap, texCoord).rgb != vec3(0) ? texture(material.diffuseMap, texCoord).rgb : material.diffuse.rgb) + diffuse + specular);
}