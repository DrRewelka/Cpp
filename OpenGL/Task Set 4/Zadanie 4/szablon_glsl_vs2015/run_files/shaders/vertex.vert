#version 140

#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat3 normalMatrix; // macierz do transformacji normala
uniform mat4 projectionMatrix; // macierz projekcji

// informacje o zrodle swiatla i materiale
uniform vec4 lightPosition; // we wspolrzednych oka
uniform vec3 lightDiffuse;
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec3 vColor; // kolor

out vec3 color; // obliczony kolor wierzcholka
 
void main()
{
	vec3 position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );
	vec3 normal = normalize( normalMatrix * vNormal );
	vec3 lightDir = normalize( vec3( lightPosition ) );
	float NdotL = max(dot(normal, lightDir), 0.0);

	color = lightDiffuse * vColor * min(1,(NdotL + 0.4)); //Zmieniono materialDiffuse na vColor i rozjasniono cienie
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}