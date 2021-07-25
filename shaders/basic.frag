#version 410 core
in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 FragPos;
in vec4 position;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform vec3 lightColor1;
uniform vec3 lightColor2;
uniform vec3 viewPos;
uniform int fogSwitch;
uniform vec3 lightPos;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;


//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

float constant = 1.0f;
float linear = 0.07f;
float quadratic = 0.14;

float ambientPoint = 0.5f;
float specularStrengthPoint = 0.5f;
float shininessPoint = 32.0f;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;


float shadow;

void computeDirLight()
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}


vec3 calcPointLight (vec3 pozitiaLuminii, vec3 culoare)
{

	vec3 cameraPosEye = vec3(0.0f);

	vec3 normalEye = normalize(normalMatrix * fNormal);

	vec3 lightDirection = normalize(pozitiaLuminii.xyz - position.xyz);
	float distance = length(pozitiaLuminii.xyz - position.xyz);

	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 halfVector = normalize(lightDirection + viewDirN);
	float att = 1.0f / (constant + linear * distance + quadratic * (distance * distance));

	vec3 ambient1 = ambientPoint * culoare;

	//calculate the diffuse light 
	vec3 diffuse1 = max(dot(normalEye, lightDirection), 0.0f) * culoare;

	//compute specular light 
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32.0f);
	vec3 specular1 = specularStrengthPoint * specCoeff * culoare;

	return (ambient1 + diffuse1 + specular1) * att;
}



float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;

	// Check whether current frag pos is in shadow
	//float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	
	//return shadow;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	return shadow;
}

float computeFog()
{
 float fogDensity = 0.01f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    computeDirLight();
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;
    
    shadow = computeShadow();

	//compute final vertex color
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec3 color1 = calcPointLight(lightPos, lightColor1);
	color = color + color1;

	if (fogSwitch == 1)
	{
	 float fogFactor = computeFog();
	 vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
	 vec3 c = vec3(1.0f, 1.0f, 0.0f);
	 vec3 c1 = calcPointLight(lightPos1, c);
	 vec3 c2 = calcPointLight(lightPos2, c);
	 vec3 c3 = calcPointLight(lightPos3, c);
	 vec3 c4 = calcPointLight(lightPos4, c);
	 color = color + c1;
	 color = color + c2;
	 color = color + c3;
	 color = color + c4;
	 fColor = vec4(mix(fogColor, color, fogFactor), 1.0f);
	}
	else 
	{
	  fColor = vec4(color, 1.0f);
	}  
}
