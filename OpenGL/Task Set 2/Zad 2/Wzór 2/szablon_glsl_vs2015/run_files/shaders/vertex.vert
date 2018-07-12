#version 140
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat4 projectionMatrix; // macierz projekcji

in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec4 vColor; // kolor wierzcholka
 
out vec4 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
	//ustawienie koloru wierzcholka
    color = vColor;

	//ustawienie pozycji wierzcholka wzgledem macierzy projekcji i model-widok
    gl_Position = projectionMatrix * modelViewMatrix * vPosition;
}