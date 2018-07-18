#version 140

#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat3 normalMatrix; // macierz do transformacji normala
uniform mat4 projectionMatrix; // macierz projekcji
uniform float offsetX; //Przesuniecie tekstury
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec2 vTexCoords; //Wspolrzedne tekstur

out vec3 position; // pozycja wierzcholka we wspolrzednych oka
out vec3 normal; // przetranformowany normal
out vec2 fTexCoords; //Przeslanie wspolrzednych tekstur do shadera fragmentow

void main()
{
	position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );
	normal = normalize( normalMatrix * vNormal );
	
	fTexCoords = vec2(vTexCoords.x + offsetX, vTexCoords.y); //Offset do przesuniecia tekstury - wrazenie latania po niebie
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}