#version 140
 
uniform int windowWidth;														//zawiera szerokosc okna
uniform int windowHeight;														//zawiera wysokosc okna

out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
	//gl_FragCoord zawiera wspolrzedne obecnie przetwarzanego fragmentu, jest dostepny tylko w shaderze fragmentow

	int miniWindowWidth = windowWidth / 3;										//podzielenie szerokosci na 3 rowne obszary
	int miniWindowHeight = windowHeight / 3;									//podzielenie wysokosci na 3 rowne obszary

	if(gl_FragCoord.x < miniWindowWidth && gl_FragCoord.y >= miniWindowHeight * 2)
		fColor = vec4(0.0, 0.0, 1.0, 1.0);										//lewy gorny rog, kolor niebieski

	if(gl_FragCoord.x > miniWindowWidth && gl_FragCoord.x < miniWindowWidth * 2 && gl_FragCoord.y >= miniWindowHeight * 2)
		fColor = vec4(0.0, 0.0, 0.0, 1.0);										//gorny srodek, kolor czarny

	if(gl_FragCoord.x > miniWindowWidth * 2 && gl_FragCoord.y >= miniWindowHeight * 2)
		fColor = vec4(1.0, 0.0, 1.0, 1.0);										//prawy gorny rog, kolor rozowy

	if(gl_FragCoord.x < miniWindowWidth && gl_FragCoord.y > miniWindowHeight && gl_FragCoord.y < miniWindowHeight * 2)
		fColor = vec4(0.0, 1.0, 0.0, 1.0);										//srodkowy rzad, lewa strona, kolor zielony

	if(gl_FragCoord.x > miniWindowWidth && gl_FragCoord.x < miniWindowWidth * 2 && gl_FragCoord.y > miniWindowHeight && gl_FragCoord.y < miniWindowHeight * 2)
		fColor = vec4(0.0, 1.0, 1.0, 1.0);										//srodek, kolor cyjan/turkus

	if(gl_FragCoord.x > miniWindowWidth * 2 && gl_FragCoord.y > miniWindowHeight && gl_FragCoord.y < miniWindowHeight * 2)
		fColor = vec4(0.5, 0.5, 0.5, 1.0);										//srodkowy rzad, prawa strona, kolor szary

	if(gl_FragCoord.x < miniWindowWidth && gl_FragCoord.y <= miniWindowHeight)
		fColor = vec4(1.0, 0.0, 0.0, 1.0);										//lewy dolny rog, kolor czerwony

	if(gl_FragCoord.x > miniWindowWidth && gl_FragCoord.x < miniWindowWidth * 2 && gl_FragCoord.y <= miniWindowHeight)
		fColor = vec4(1.0, 1.0, 0.0, 1.0);										//dolny srodek, kolor zolty

	if(gl_FragCoord.x > miniWindowWidth * 2 && gl_FragCoord.y <= miniWindowHeight)
		fColor = vec4(1.0, 1.0, 1.0, 1.0);										//prawy dolny rog, kolor bialy
}