#version 140

uniform float turnRadius;
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec3 vColor; // kolor wierzcholka
 
out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
    color = vColor;

	//radians sluzy do konwersji kata ze stopni na radiany, co pozwala na obliczanie polozenia wierzcholka po obrocie

	float x = vPosition.x * cos(radians(turnRadius)) - vPosition.y * sin(radians(turnRadius));	//zmienna, ktora oblicza ze
																								//wzoru wspolrzedna X danego
																								//wierzcholka po obrocie
																								//o dany kat

	float y = vPosition.x * sin(radians(turnRadius)) + vPosition.y * cos(radians(turnRadius));	//zmienna, ktora oblicza ze
																								//wzoru wsporzedna Y danego
																								//wierzcholka po obrocie
																								//o dany kat

	vec4 newPosition = vec4(x, y, vPosition.z, vPosition.w);									//stworzenie nowej zmiennej
																								//zawierajacej wspolrzedne
																								//wierzcholka po obrocie

    gl_Position = newPosition;
}