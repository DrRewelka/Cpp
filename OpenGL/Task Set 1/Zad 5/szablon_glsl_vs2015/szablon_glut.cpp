#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include "shaders.h"

const int WIDTH = 512; // szerokosc okna
const int HEIGHT = 512; // wysokosc okna
const int VAOS = 2; // liczba VAO
const int VBOS = 3; // liczba VBO

void onShutdown();
void initGL();
void renderScene();
void setupShaders();
void setupBuffers();

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania
 
GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne

GLuint vao[VAOS]; // identyfikatory VAO
GLuint buffers[VBOS]; // identyfikatory VBO
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
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
    glClear( GL_COLOR_BUFFER_BIT ); // czyszczenie bufora koloru

	glUseProgram( shaderProgram ); // wlaczenie programu cieniowania

	setupBuffers();

	// wyrysowanie pierwszego VAO (trojkat)
	glBindVertexArray( vao[0] );
    glDrawArrays( GL_TRIANGLES, 0, 3 );

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
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca VAO oraz zawarte w nim VBO z danymi o trojkacie 
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays( 2, vao ); // generowanie identyfikatora VAO
	glGenBuffers( 3, buffers ); // generowanie identyfikatorow VBO

	// wspolrzedne wierzcholkow trojkata
	float vertices[] = 
	{
		-0.9f, 0.2f, 0.0f, 1.0f,
		0.9f, -0.4f, 0.0f, 1.0f,
		0.5f, 0.8f, 0.0f, 1.0f
	};
 
	//Zadanie 5 - kolorowanie przez odleg³osc punktu od srodka masy

	GLfloat centerOfMassX;															//zmienna przechowujaca
																					//wspolrzedna X srodka masy

	GLfloat centerOfMassY;															//zmienna przechowujaca
																					//wspolrzedna Y srodka masy

	GLfloat farthestVertX;															//zmienna przechowujaca
																					//wspolrzedna X najdalszego punktu

	GLfloat farthestVertY;															//zmienna przechowujaca
																					//wspolrzedna Y najdalszego punktu

	GLfloat distance = 0;															//zmienna pomocnicza do obliczenia
																					//najdalszego punktu

	GLfloat sumOfVertsX = 0;														//zmienna pomocnicza przechowujaca
																					//sumy wspolrzednych X wierzcholkow

	GLfloat sumOfVertsY = 0;														//zmienna pomocnicza przechowujaca
																					//sumy wspolrzednych Y wierzcholkow

	for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 4)					//petla liczaca sumy wspolrzednych
	{
		sumOfVertsX += vertices[i];													//przypisanie wspolrzednych do
		sumOfVertsY += vertices[i + 1];												//zmiennych, odpowiednio X i Y
	}

	centerOfMassX = sumOfVertsX / ((sizeof(vertices) / sizeof(float)) / 4);			//obliczone ze wzoru wspolrzedne
	centerOfMassY = sumOfVertsY / ((sizeof(vertices) / sizeof(float)) / 4);			//srodka masy

	for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 4)					//petla liczaca i sprawdzajaca
	{																				//najdalszy punkt od srodka masy
		if ((vertices[i] - centerOfMassX) * (vertices[i] - centerOfMassX) + (vertices[i + 1] - centerOfMassY) * (vertices[i + 1] - centerOfMassY) > distance)
		{
			distance = (vertices[i] - centerOfMassX) * (vertices[i] - centerOfMassX) + (vertices[i + 1] - centerOfMassY) * (vertices[i + 1] - centerOfMassY);
			farthestVertX = vertices[i];											//przypisanie wspolrzednych do
			farthestVertY = vertices[i + 1];										//zmiennych, odpowiednio X i Y
		}
	}

	GLint centerOfMassLocation = glGetUniformLocation(shaderProgram, "centerOfMass");//lokacja zmiennej uniform srodka
	glUniform2f(centerOfMassLocation, centerOfMassX, centerOfMassY);				//masy i przypisanie jej X i Y

	GLint farthestVertLocation = glGetUniformLocation(shaderProgram, "farthestVert");//lokacja zmiennej uniform punktu
	glUniform2f(farthestVertLocation, farthestVertX, farthestVertY);				//najdalszego i przypisanie jej
																					//X i Y

    glBindVertexArray( vao[0] ); // dowiazanie pierwszego VAO    	
    
    // VBO dla wspolrzednych wierzcholkow
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( vertexLoc ); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
    glVertexAttribPointer( vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0 ); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

	glBindVertexArray( 0 );
}