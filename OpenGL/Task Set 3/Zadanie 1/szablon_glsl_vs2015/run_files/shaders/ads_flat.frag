#version 140

flat in vec3 color; // kolor obliczony w shaderze wierzcholkow (bez interpolacji)
out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
    fColor = vec4( color, 1.0 );
}