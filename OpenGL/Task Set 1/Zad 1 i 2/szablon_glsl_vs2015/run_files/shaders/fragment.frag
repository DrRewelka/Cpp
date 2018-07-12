#version 140

uniform vec4 color;								//zmienna uniform, do ktorej przekazywany jest kolor z programu dla
												//calego rysowanego obiektu, bez koniecznosci przekazywania informacji
												//o kolorze przy kazdym wierzcholku 

out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
    fColor = color;
}