#version 140

uniform vec2 centerOfMass;														//zmienne typu uniform przekazywane do
uniform vec2 farthestVert;														//shadera przez program, odpowiednio dla
																				//srodka masy i najdalszego punktu, obie
																				//w postaci wektorow o dwoch wartosciach
 
in vec2 fPosition;																//przyjecie pozycji obecnego wierzcholka
																				//z shadera wierzcholkow

out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
	float t = distance(fPosition, centerOfMass) / distance(farthestVert, centerOfMass); //obliczenie zmiennej do
																				//kolorowania obiektu w sposob pokazany
																				//w tresci zadania

    fColor = vec4(1 - t, 0, t, 1);												//ustawienie koloru na podstawie zmiennej
																				//t, vec4 bo (r, g, b, a)
}