#version 140
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych

void main()
{
    gl_Position = vPosition;
}