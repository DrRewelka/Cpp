#version 140

uniform vec4 uColor;
 
out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
    fColor = uColor;
}