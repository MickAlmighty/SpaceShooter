#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 FragColor;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};  
struct PointLight {    
	vec3 position;

	float constant;
	float linear;
	float quadratic;  

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};  

struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic; 

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLight;
uniform DirLight dirLight;
uniform SpotLight spotLight[2];
uniform sampler2D texture_diffuse1;
uniform bool isFromFile;
uniform vec3 viewPos;

float ambientStrength = 0.1;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); 
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);  
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
vec2 TexCoords;
vec3 Normal;
vec3 FragPos;

void main()
{
	gl_PointSize = 3;
    TexCoords = aTexCoords;    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * aNormal);
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);   
	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec4 result = CalcDirLight(dirLight, norm, viewDir);
    // faza 2: światła punktowe
    result += CalcPointLight(pointLight, norm, viewDir);
	for(int i = 0; i < 2; i++)
		result += CalcSpotLight(spotLight[i], norm, viewDir);
	FragColor = result * texture(texture_diffuse1, TexCoords);
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
	//ambient
	vec4 ambient = vec4(light.ambient, 1) * ambientStrength;
	//diffuse
	vec3 lightDir = normalize(light.position - FragPos);
	float diffuseBrightness = dot(normal, lightDir) / (length(lightDir) * length(Normal));
	diffuseBrightness = clamp(diffuseBrightness, 0 , 1);
	// specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	
	// spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    

	vec4 diffuse = vec4(light.diffuse, 1) * diffuseBrightness;
	vec4 specular = vec4(light.specular, 1) * spec;
	
	diffuse  *= intensity;
    specular *= intensity;

	// attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;

	return (ambient + diffuse + specular);
}
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	//ambient
	vec4 ambient = vec4(light.ambient, 1) * ambientStrength;

	//diffuse light
	vec3 lightDir = normalize(-light.direction);
	float diffuseBrightness = dot(normal, lightDir) / (length(lightDir) * length(normal));
	diffuseBrightness = clamp(diffuseBrightness, 0 , 1);

	//specularLight
	vec3 halfwayDir = normalize(lightDir + viewDir); //blinn-phong, wektor połowiczny 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 256.0);

	vec4 diffuse = vec4(light.diffuse, 1) * diffuseBrightness;
	vec4 specular = vec4(light.specular, 1) * spec;
	return (ambient + diffuse + specular);
} 

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	//diffuse light
	
	vec3 lightDir = normalize(light.position - FragPos);
	float diffuseBrightness = dot(normal, lightDir) / (length(lightDir) * length(normal));
	diffuseBrightness = clamp(diffuseBrightness, 0 , 1);
	
	//specularLight
	vec3 halfwayDir = normalize(lightDir + viewDir); //blinn-phong, wektor połowiczny 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 256.0);
	
	vec4 ambient = vec4(light.ambient, 1) * ambientStrength;
	vec4 diffuse = vec4(light.diffuse, 1) * diffuseBrightness;
	vec4 specular = vec4(light.specular, 1) * spec;
	
	//tlumienie
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + 
					light.quadratic * (distance * distance));  
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}