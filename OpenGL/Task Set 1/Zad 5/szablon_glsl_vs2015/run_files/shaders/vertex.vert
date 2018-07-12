#version 140
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych

out vec2 fPosition;															//przekazanie wektora na pozycje obecnego
																			//wierzcholka do shadera fragmentow

void main()
{
	fPosition.x = vPosition.x;												//przypisanie wspolrzednych odpowiednio
	fPosition.y = vPosition.y;												//X i Y do wektora przekazywanego shaderowi
																			//fragmentow
	gl_Position = vPosition;
}