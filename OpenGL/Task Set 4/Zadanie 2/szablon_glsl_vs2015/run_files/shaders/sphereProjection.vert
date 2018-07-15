#version 140

#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat3 normalMatrix; // macierz do transformacji normala
uniform mat4 projectionMatrix; // macierz projekcji
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
layout (location = 1) in vec3 vNormal; // normal

out vec3 position; // pozycja wierzcholka we wspolrzednych oka
out vec3 normal; // przetranformowany normal
out vec2 fTexCoords; //Przeslanie wspolrzednych tekstur do shadera fragmentow

void main()
{
	position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );
	normal = normalize( normalMatrix * vNormal );
	
	//Obliczenie wspolrzednych tekstury dla projekcji sferycznej
	float r = sqrt(pow(vPosition.x, 2) + pow(vPosition.y, 2) + pow(vPosition.z, 2));
	float theta = atan(vPosition.y / vPosition.x);
	float phi = acos(vPosition.z / r);
	
	if((theta <= (3.14 / 2) && theta >= 0) || (theta <= (2 * 3.14) && theta >= (3.14 * 1.5)))
		fTexCoords = vec2(theta, phi);
	else
		fTexCoords = vec2(-theta, phi);
	
	gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}