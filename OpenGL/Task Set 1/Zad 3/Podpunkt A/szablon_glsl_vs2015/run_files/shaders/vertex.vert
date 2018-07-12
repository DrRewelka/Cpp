#version 140
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec3 vColor; // kolor wierzcholka
 
out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
    //Obszary o roznych kolorach wedlug instrukcji z zestawu zadan
	//vPosition zawiera pozycje wierzcholka w ukladzie wspolrzednych [-1, 1]

	if(vPosition.x < -0.33 && vPosition.y >= 0.33)								//lewy gorny rog, kolor niebieski
		color =	vec3(0.0, 0.0, 1.0);
	
	if(vPosition.x > -0.33 && vPosition.x < 0.33 && vPosition.y >= 0.33)		//gorny srodek, kolor czarny
		color = vec3(0.0, 0.0, 0.0);

	if(vPosition.x > 0.33 && vPosition.y >= 0.33)								//prawy gorny rog, kolor rozowy
		color = vec3(1.0, 0.0, 1.0);

	if(vPosition.x < -0.33 && vPosition.y < 0.33 && vPosition.y > -0.33)		//srodkowy rzad, lewy fragment, kolor zielony
		color = vec3(0.0, 1.0, 0.0);

	if(vPosition.x > -0.33 && vPosition.x < 0.33 && vPosition.y < 0.33 && vPosition.y > -0.33)	//srodek, kolor cyjan/turkus
		color = vec3(0.0, 1.0, 1.0);

	if(vPosition.x > 0.33 && vPosition.y < 0.33 && vPosition.y > -0.33)			//srodkowy rzad, prawy fragment, kolor szary
		color = vec3(0.5, 0.5, 0.5);

	if(vPosition.x < -0.33 && vPosition.y <= -0.33)								//lewy dolny rog, kolor czerowny
		color = vec3(1.0, 0.0, 0.0);

	if(vPosition.x > -0.33 && vPosition.x < 0.33 && vPosition.y <= -0.33)		//dolny srodek, kolor zolty
		color = vec3(1.0, 1.0, 0.0);

	if(vPosition.x > 0.33 && vPosition.y <= -0.33)								//prawy dolny rog, kolor bialy
		color = vec3(1.0, 1.0, 1.0);

    gl_Position = vPosition;
}