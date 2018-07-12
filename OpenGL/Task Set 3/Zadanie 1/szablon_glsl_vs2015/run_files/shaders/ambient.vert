#version 140

#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat3 normalMatrix; // macierz do transformacji normala
uniform mat4 projectionMatrix; // macierz projekcji

// informacje o zrodle swiatla i materiale
uniform vec3 lightAmbient;
uniform vec3 materialAmbient;
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
layout (location = 1) in vec3 vNormal; // normal

out vec3 color; // obliczony kolor wierzcholka
 
void main()
{
	color = lightAmbient * materialAmbient;
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}