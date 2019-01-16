#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform samplerCube depthCubemap;

uniform float metallic;
uniform float roughness;
uniform float ao;
uniform vec3 camPos;
uniform float reflectionStrength;
uniform float refraction;
uniform float far_plane;

// światła
struct PointLight {
    vec3 position;
    vec3 color;
    bool enabled;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float lightStrength;
    bool enabled;
};

struct SpotLight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 color; 
    bool enabled;
};
uniform PointLight pointLight;
uniform DirLight dirLight;
uniform SpotLight spotLight[2];

const float PI = 3.14159265359;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0);
vec3 calculatePointLight(vec3 albedo, vec3 N, vec3 V);
vec3 calculateDirLight(vec3 albedo, vec3 N, vec3 V);
vec3 calculateSpotLight(vec3 albedo, vec3 N, vec3 V, SpotLight light);
vec3 calculateReflection(vec3 N, vec3 I);
vec3 calculateRefraction(vec3 N, vec3 I);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal, vec3 lightDir);
float PointLightShadowCalculation(vec3 fragPos);



void main()
{
    vec3 albedo  = vec3(texture(texture_diffuse1, TexCoords));
    vec3 N = normalize(Normal);
    //vec3 N = Normal;
    vec3 V = normalize(camPos - FragPos);
    vec3 I = normalize(FragPos - camPos);

    vec3 color = vec3(0);
    color += calculatePointLight(albedo, N, V);
    color += calculateDirLight(albedo, N, V);
    for(int i = 0; i < 2; i++)
        color += calculateSpotLight(albedo, N, V, spotLight[i]);
    color += calculateReflection(N, I);
    color += calculateRefraction(N, I);
    
    //korekcja gamma
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    //wynikowy kolor fragmentu
    FragColor = vec4(color, 1.0);
}


vec3 calculateSpotLight(vec3 albedo, vec3 N, vec3 V, SpotLight light)
{
    
    if(light.enabled)
    {
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // równanie odbicia
        vec3 Lo = vec3(0.0);
        // obliczy radiancję per-światło
        vec3 L = normalize(light.position - FragPos);
        vec3 H = normalize(V + L);

        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (0.002 * (distance * distance));
        vec3 radiance     = light.color * attenuation;        

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float NdotV = max(dot(N,V),0);
        float G   = GeometrySchlickGGX(NdotV, roughness);             
        //float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0);       

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  

        // dodaj do wynikowej radiancji Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  

        vec3 ambient = vec3(0.03) * albedo * ao;
        vec3 color = ambient + Lo;

        //spotlight (miękkie krawędzie)
        float theta = dot(L, normalize(-light.direction)); 
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        color *= intensity;
        return color;
    }
    return vec3(0);

}

vec3 calculatePointLight(vec3 albedo, vec3 N, vec3 V)
{
    if(pointLight.enabled)
    {
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // równanie odbicia
        vec3 Lo = vec3(0.0);
        // obliczy radiancję per-światło
        vec3 L = normalize(pointLight.position - FragPos);
        vec3 H = normalize(V + L);
        float distance    = length(pointLight.position - FragPos);
        float attenuation = 1.0 / (0.001 * (distance * distance));
        vec3 radiance     = pointLight.color * attenuation;        

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float NdotV = max(dot(N,V),0);
        float G   = GeometrySchlickGGX(NdotV, roughness);             
        //float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0);       

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  

        // dodaj do wynikowej radiancji Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  

        vec3 ambient = vec3(0.03) * albedo * ao;
        vec3 color = ambient + Lo;// * (1 - PointLightShadowCalculation(FragPos));
        return color;
    }
    return vec3(0);
}

vec3 calculateDirLight(vec3 albedo, vec3 N, vec3 V)
{
    if(dirLight.enabled)
    {
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // równanie odbicia
        vec3 Lo = vec3(0.0);
        // obliczy radiancję per-światło
        vec3 L = normalize(dirLight.direction); //kierunek, z którego pada światło
        vec3 H = normalize(V + L);
        vec3 radiance = dirLight.color * dirLight.lightStrength;        

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float NdotV = max(dot(N,V),0);
        float G   = GeometrySchlickGGX(NdotV, roughness);             
        //float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0);       

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  

        // dodaj do wynikowej radiancji Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  

        vec3 ambient = vec3(0.03) * albedo * ao;
        
        vec3 color = ambient + Lo * (1 - ShadowCalculation(FragPosLightSpace, N, L));
        return color; 
    }
    return vec3(0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0)
{
    //F0 - odbicie powierzchniowe przy zerowym kącie padania
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

//rzucanie cieni przez mikrościanki (zależy od chropowatości)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 calculateReflection(vec3 N, vec3 I)
{
    vec3 R = reflect(I, N);
    vec3 skyboxReflect = texture(skybox, R).rgb * reflectionStrength;
    return skyboxReflect;
}

vec3 calculateRefraction(vec3 N, vec3 I)
{
    float ratio = 1.00 / refraction;
    vec3 R = refract(I, N, ratio);
    vec3 skyboxRefract;
    if(refraction != 0.0f)
        skyboxRefract = texture(skybox, R).rgb;
    else
        skyboxRefract = vec3(0);
    return skyboxRefract;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // float shadow = 0.0;
    // vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    // for(int x = -1; x <= 1; ++x)
    // {
    //     for(int y = -1; y <= 1; ++y)
    //     {
    //         float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
    //         shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
    //     }    
    // }
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

float PointLightShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLight.position; 
    // float closestDepth = texture(depthCubemap, fragToLight).r;
    // closestDepth *= far_plane;  
     float currentDepth = length(fragToLight);
    // // now test for shadows
    // float shadow  = 0.0;
    // float bias    = 0.05; 
    // float samples = 4.0;
    // float offset  = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
    //     for(float y = -offset; y < offset; y += offset / (samples * 0.5))
    //     {
    //         for(float z = -offset; z < offset; z += offset / (samples * 0.5))
    //         {
    //             float closestDepth = texture(depthCubemap, fragToLight + vec3(x, y, z)).r; 
    //             closestDepth *= far_plane;   // Undo mapping [0;1]
    //             if(currentDepth - bias > closestDepth)
    //                 shadow += 1.0;
    //         }
    //     }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(camPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthCubemap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
    return shadow;
}