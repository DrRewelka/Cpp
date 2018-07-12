#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "shaders.h"

//Zadanie jest wykonane w funkcji renderScene(), utworzono rowniez dwie dodatkowe funkcje
//makeSquare() oraz makeTriangle() do tworzenia kwadratu i trojkata

const int WIDTH = 512; // szerokosc okna
const int HEIGHT = 512; // wysokosc okna
const int VAOS = 2; // liczba VAO
const int VBOS = 4; // liczba VBO

void onShutdown();
void initGL();
void renderScene();
void setupShaders();
void setupBuffers();

void makeSquare();
void makeTriangle();

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania
 
GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne
GLuint colorLoc; // lokalizacja atrybutu wierzcholka - kolor

GLuint vao[VAOS]; // identyfikatory VAO
GLuint buffers[VBOS]; // identyfikatory VBO

GLuint projMatrixLoc; //lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; //lokalizacja zmiennej jednorodnej - macierz model-widok

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

glm::vec3 rotationAngles = glm::vec3(0.0, 0.0, 0.0); // katy obrotu wokol danych osi
float aspectRatio = (float)WIDTH / HEIGHT;
float fovy = 45.0f; // kat patrzenia (uzywany do skalowania sceny)

//******************************************************************************************

/*------------------------------------------------------------------------------------------
** funkcja glowna
**------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	// rejestracja funkcji wykonywanej przy wyjsciu
	atexit( onShutdown );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

	glutInitContextVersion( 3, 1 ); // inicjacja wersji kontekstu
	glutInitContextFlags( GLUT_DEBUG );
	glutInitContextProfile( GLUT_CORE_PROFILE ); // incicjacja profilu rdzennego

	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( WIDTH, HEIGHT );
    glutCreateWindow( "OpenGL - Core Profile" );

	// inicjacja GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( err != GLEW_OK )	
	{
		std::cerr << "Blad: " << glewGetErrorString( err ) << std::endl;
		exit( 1 );
	}

	if( !GLEW_VERSION_3_1 )
	{
		std::cerr << "Brak obslugi OpenGL 3.1\n";
		exit( 2 );
	}
    
    glutDisplayFunc( renderScene );

	initGL();

    glutMainLoop();
    
    return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers( VBOS, buffers );  // usuniecie VBO
	glDeleteVertexArrays( VAOS, vao ); // usuiecie VAO
	glDeleteProgram( shaderProgram ); // usuniecie programu cieniowania
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	std::cout << "GLEW = "		  << glewGetString( GLEW_VERSION ) << std::endl;
	std::cout << "GL_VENDOR = "	  << glGetString( GL_VENDOR ) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString( GL_RENDERER ) << std::endl;
	std::cout << "GL_VERSION = "  << glGetString( GL_VERSION ) << std::endl;
	std::cout << "GLSL = "		  << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // kolor uzywany do czyszczenia bufora koloru

	setupShaders();

	setupBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	int squareCounter = 0; //licznik pomocniczy dla kwadratow
	int triangleCounter = 0; //licznik pomocniczy dla trojkatow

    glClear( GL_COLOR_BUFFER_BIT ); //czyszczenie bufora koloru
	
	//ustawienie macierzy projekcji
	projMatrix = glm::perspective(glm::radians(fovy), aspectRatio, 0.1f, 100.0f);

	//ustawienie macierzy model-widok
	mvMatrix = glm::lookAt(glm::vec3(0.0, 0.0, 8.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	glUseProgram( shaderProgram ); //wlaczenie programu cieniowania

	//ustawienie zmiennej jednorodnej dla macierzy projekcji
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	//obrot kwadratu
	rotationAngles.z = 45.0f;

	//wyrysowanie pierwszego kwadratu - w lewym gornym rogu (po transformacjach)
	glBindVertexArray( vao[0] );
	mvMatrix *= glm::translate(glm::vec3(-2.75f, 2.5f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::scale(glm::vec3(0.7f));
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 ); //rysowanie
	squareCounter++; //licznik pomocniczy do zatrzymania rysowania

	//rysowanie reszty kwadratow
	for (int j = 0; j <= 4; j++) //petla rysujaca kolejne linie kwadratow
	{
		for (int i = 0; i <= 3; i++) //petla rysujaca kolejne kwadraty w linii
		{
			mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
			mvMatrix *= glm::translate(glm::vec3(1.9f, 0.0f, 0.0f));
			mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			squareCounter++;
		}
		mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		mvMatrix *= glm::translate(glm::vec3(-7.6f, -1.75f, 0.0f));
		mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
		if(squareCounter < 25) //zapobiega rysowaniu dodatkowego kwadratu
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		squareCounter++;
	}

	//powrot do srodka ekranu
	mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::translate(glm::vec3(3.5f, 5.25f, 0.0f));
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	
	rotationAngles.z = 135.0f; //zmiana kata obrotu
	
	//wyrysowanie pierwszego trojkata - lewy gorny rog (po transformacjach)
	glBindVertexArray( vao[1] );
	mvMatrix *= glm::translate(glm::vec3(-4.0f, 4.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 3); //rysowanie
	triangleCounter++; //licznik pomocniczy do zatrzymania rysowania
	
	//rysowanie pierwszej linii trojkatow, znajdujacej sie przy gornej krawedzi kwadratow
	for (int j = 0; j <= 4; j++) //petla rysujaca kolejne linie trojkatow
	{
		for (int i = 0; i <= 8; i++) //petla rysujaca trojkaty w linii
		{
			if (i % 2 == 0) //rysowanie naprzemian prawego i lewego trojkata dla kwadratu
			{
				mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mvMatrix *= glm::translate(glm::vec3(1.0f, 0.0f, 0.0f));
				mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);
				triangleCounter++;
			}
			else
			{
				mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mvMatrix *= glm::translate(glm::vec3(0.9f, 0.0f, 0.0f));
				mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);
				triangleCounter++;
			}
		}
		mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		mvMatrix *= glm::translate(glm::vec3(-8.6f, -1.75f, 0.0f));
		mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
		if(triangleCounter < 49) //zapobiega rysowaniu dodatkowego trojkata
			glDrawArrays(GL_TRIANGLES, 0, 3);
		triangleCounter++;
	}

	//przejscie do pierwszego trojkata przy lewym gornym kwadracie, na jego lewej dolnej
	//krawedzi
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	rotationAngles.z = 45.0f; //zmiana kata obrotu
	mvMatrix *= glm::translate(glm::vec3(0.0f, 7.75f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 3); //rysowanie
	triangleCounter++;

	//rysowanie drugiej linii trojkatow, znajdujacej sie przy dolnej stronie kwadratow
	for (int j = 0; j <= 4; j++) //petla, ktora rysuje kolejne linie
	{
		for (int i = 0; i <= 8; i++) //petla rysujaca kolejne trojkaty w jednej linii
		{
			if (i % 2 == 0) //rysowanie naprzemian prawego i lewego trojkata dla kwadratu
			{
				mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mvMatrix *= glm::translate(glm::vec3(1.0f, 0.0f, 0.0f));
				mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);
				triangleCounter++;
			}
			else
			{
				mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mvMatrix *= glm::translate(glm::vec3(0.9f, 0.0f, 0.0f));
				mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);
				triangleCounter++;
			}
		}
		//przejscie do nowej linii trojkatow
		mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		mvMatrix *= glm::translate(glm::vec3(-8.6f, -1.75f, 0.0f));
		mvMatrix *= glm::rotate(glm::radians(-rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
		if (triangleCounter < 99) //zapobiega rysowaniu dodatkowego trojkata
			glDrawArrays(GL_TRIANGLES, 0, 3);
		triangleCounter++;
	}
    
	glBindVertexArray( 0 );

    glutSwapBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if( !setupShaders( "shaders/vertex.vert", "shaders/fragment.frag", shaderProgram ) )
		exit( 3 );
 
    vertexLoc = glGetAttribLocation( shaderProgram, "vPosition" );
    colorLoc = glGetAttribLocation( shaderProgram, "vColor" );

	projMatrixLoc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	mvMatrixLoc = glGetUniformLocation(shaderProgram, "modelViewMatrix");
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca VAO oraz zawarte w nim VBO z danymi o trojkacie 
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays( 2, vao ); // generowanie identyfikatora VAO
	glGenBuffers( 4, buffers ); // generowanie identyfikatorow VBO

	makeSquare();
	makeTriangle();

	glBindVertexArray(0);
}

void makeSquare() //tworzenie kwadratu w vao[0]
{
	float squareVertices[] = //wspolrzedne wierzcholkow kwadratu
	{
		-0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f
	};

	std::vector<float> squareColor; //kolory wierzcholkow kwadratu
	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);
	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);

	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);
	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);

	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);
	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);

	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);
	squareColor.push_back(0.0f);
	squareColor.push_back(1.0f);

	glBindVertexArray(vao[0]);	// dowiazanie pierwszego VAO    	

								// VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

								// VBO dla kolorow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, squareColor.size() * sizeof(float), reinterpret_cast<GLfloat *>(&squareColor[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc); // wlaczenie tablicy atrybutu wierzcholka - kolory
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzcholka - kolory
}

void makeTriangle() //tworzenie trojkata w vao[1]
{
	float triangleVertices[] = //wspolrzedne wierzcholkow trojkata
	{
		0.0f, 0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, 0.0f, 1.0f
	};

	std::vector<float> triangleColor; //kolory wierzcholkow trojkata
	triangleColor.push_back(1.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(1.0f);

	triangleColor.push_back(1.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(1.0f);

	triangleColor.push_back(1.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(0.0f);
	triangleColor.push_back(1.0f);

	glBindVertexArray(vao[1]);	  // dowiazanie drugiego VAO    	

								  // VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

								  // VBO dla kolorow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, triangleColor.size() * sizeof(float), reinterpret_cast<GLfloat *>(&triangleColor[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc); // wlaczenie tablicy atrybutu wierzcholka - kolory
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzcholka - kolory
}