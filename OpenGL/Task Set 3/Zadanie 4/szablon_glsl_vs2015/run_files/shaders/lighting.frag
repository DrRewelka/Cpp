#version 140

//Material - informacje
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

//Swiatlo kierunkowe - informacje
uniform vec3 dirLightDirection;	
uniform vec3 dirLightAmbient;
uniform vec3 dirLightDiffuse;
uniform vec3 dirLightSpecular;

//Swiatlo punktowe - informacje
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;
uniform vec3 pointLightAmbient;
uniform vec3 pointLightDiffuse;
uniform vec3 pointLightSpecular;

//Swiatlo typu reflektor - informacje
uniform vec3 spotLightDirection;
uniform float spotLightCutOff;
uniform float spotLightOuterCutOff;
uniform float spotLightConstant;
uniform float spotLightLinear;
uniform float spotLightQuadratic;
uniform vec3 spotLightAmbient;
uniform vec3 spotLightDiffuse;
uniform vec3 spotLightSpecular;       

uniform vec3 lightPosition;

uniform float lightType;

in vec3 position;
in vec3 normal;

out vec4 color;

// Function prototypes
vec3 CalcDirLight(vec3 dirLightDirection, vec3 dirLightAmbient, vec3 dirLightDiffuse, vec3 dirLightSpecular, vec3 norm, vec3 viewDir);
vec3 CalcPointLight(vec3 lightPosition, float pointLightConstant, float pointLightLinear, float pointLightQuadratic, vec3 pointLightAmbient, vec3 pointLightDiffuse, vec3 pointLightSpecular, vec3 norm, vec3 position, vec3 viewDir);
vec3 CalcSpotLight(vec3 lightPosition, vec3 spotLightDirection, float spotLightCutOff, float spotLightOuterCutOff, float spotLightConstant, float spotLightLinear, float spotLightQuadratic, vec3 spotLightAmbient, vec3 spotLightDiffuse, vec3 spotLightSpecular, vec3 norm, vec3 position, vec3 viewDir);

void main()
{    
    // Properties
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - position);
    
	if(lightType == 2.0f) //Swiatlo kierunkowe
	{
		vec3 result = CalcDirLight(dirLightDirection, dirLightAmbient, dirLightDiffuse, dirLightSpecular, norm, viewDir);
		color = vec4(result, 1.0);
    }
	else if(lightType == 1.0f) //Swiatlo punktowe
    {
		vec3 result = CalcPointLight(lightPosition, pointLightConstant, pointLightLinear, pointLightQuadratic, pointLightAmbient, pointLightDiffuse, pointLightSpecular, norm, position, viewDir);    
		color = vec4(result, 1.0);
	}
	else if(lightType == 3.0f) //Swiatlo typu reflektor
    {
		vec3 result = CalcSpotLight(lightPosition, spotLightDirection, spotLightCutOff, spotLightOuterCutOff, spotLightConstant, spotLightLinear, spotLightQuadratic, spotLightAmbient, spotLightDiffuse, spotLightSpecular, norm, position, viewDir);    
		color = vec4(result, 1.0);
	}
}

//Obliczenia dla swiatla kierunkowego
vec3 CalcDirLight(vec3 dirLightDirection, vec3 dirLightAmbient, vec3 dirLightDiffuse, vec3 dirLightSpecular, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLightDirection);
    vec3 refl = reflect(-lightDir, norm);

    vec3 ambient = dirLightAmbient * materialAmbient;
	vec3 diffuse = dirLightDiffuse * materialDiffuse * max( dot( lightDir, norm ), 0.0 );
	vec3 specular = pow( max( 0.0, dot( viewDir, refl ) ), materialShininess ) * materialSpecular * dirLightSpecular;
    
	return (ambient + diffuse + specular);
}

//Obliczenia dla swiatla punktowego
vec3 CalcPointLight(vec3 lightPosition, float pointLightConstant, float pointLightLinear, float pointLightQuadratic, vec3 pointLightAmbient, vec3 pointLightDiffuse, vec3 pointLightSpecular, vec3 norm, vec3 position, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPosition - position);

    //Oslabienie swiatla
    float distance = length(lightPosition - position);
    float attenuation = 1.0f / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));    
    
    vec3 ambient = pointLightAmbient * materialAmbient;
	vec3 diffuse = pointLightDiffuse * materialDiffuse * max( dot( lightDir, norm ), 0.0 );
	vec3 refl = reflect(-lightDir, norm);
	vec3 specular = pow( max( 0.0, dot( viewDir, refl ) ), materialShininess ) * materialSpecular * pointLightSpecular;
    
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

//Obliczenia dla swiatla typu reflektor
vec3 CalcSpotLight(vec3 lightPosition, vec3 spotLightDirection, float spotLightCutOff, float spotLightOuterCutOff, float spotLightConstant, float spotLightLinear, float spotLightQuadratic, vec3 spotLightAmbient, vec3 spotLightDiffuse, vec3 spotLightSpecular, vec3 norm, vec3 position, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPosition - position);

    //Oslabienie swiatla
    float distance = length(lightPosition - position);
    float attenuation = 1.0f / (spotLightConstant + spotLightLinear * distance + spotLightQuadratic * (distance * distance));    
    
	//Intensywnosc swiatla
    float theta = dot(lightDir, normalize(-spotLightDirection)); 
    float epsilon = spotLightCutOff - spotLightOuterCutOff;
    float intensity = clamp((theta - spotLightOuterCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = spotLightAmbient * materialAmbient;
	vec3 diffuse = spotLightDiffuse * materialDiffuse * max( dot( lightDir, norm ), 0.0 );
	vec3 refl = reflect(-lightDir, norm);
	vec3 specular = pow( max( 0.0, dot( viewDir, refl ) ), materialShininess ) * materialSpecular * spotLightSpecular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}