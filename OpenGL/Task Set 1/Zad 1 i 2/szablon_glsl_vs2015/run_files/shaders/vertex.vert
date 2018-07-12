#version 140
 
in vec4 vPosition;	// pozycja wierzcholka w lokalnym ukladzie wspolrzednych
					// nie potrzeba przesylac koloru wierzcholka, poniewaz ma on byc zdefiniowany dla calego
					// rysowanego obiektu

void main()
{
    gl_Position = vPosition;
}