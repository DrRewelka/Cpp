#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include <cmath>
#include "shaders.h"

using namespace std;

const int WIDTH = 512; // szerokosc okna
const int HEIGHT = 512; // wysokosc okna
const int VAOS = 2; // liczba VAO
const int VBOS = 3; // liczba VBO

void onShutdown();
void initGL();
void renderScene();
void setupShaders();
void setupBuffers();
void polygonGen(GLint vertNumb, GLfloat radius);

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

									//Zadanie 1 - jednolity kolor obiektu

	GLfloat setColors[4] = { 1.0f, 0.0f, 0.0f, 1.0f };						//zmienna, ktora przechowuje kolor
																			//i zostaje przekazana do shadera fragmentow
	
	GLint setColorsLocation = glGetUniformLocation(shaderProgram, "color");	//do zmiennej setColors, ktora jest powyzej,
																			//zostaje przypisana lokacja zmiennej uniform
																			//color z shadera fragmentow
	
	glUniform4fv(setColorsLocation, 1, setColors);							//przekazanie setColors do setColorsLocation

									//Zadanie 2 - generowanie wielokata foremnego

	GLint polyNumb = 5;														//zmienna przechowujaca liczbe wierzcholkow
	GLfloat objRadius = 0.5f;												//zmienna przechowujaca promien wielokata
	polygonGen(polyNumb, objRadius);										//funkcja generujaca wielokat

	glBindVertexArray( vao[0] );											//wrysowanie wielokata do okna, zmiana trybu
    glDrawArrays( GL_POLYGON, 0, polyNumb );								//rysowania na GL_POLYGON, aby laczyc wszystkie
																			//wierzcholki podczas rysowania

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
		-0.9f, -0.5f, 0.0f, 1.0f,
		0.9f, -0.5f, 0.0f, 1.0f,
		0.9f, 0.5f, 0.0f, 1.0f
	};
}

void polygonGen(GLint vertices, GLfloat radius)
{
	glGenVertexArrays(2, vao);												//generowanie VAO
	glGenBuffers(3, buffers);												//generowanie VBO

	GLfloat vertX;															//wspolrzedna X generowanego punktu
	GLfloat vertY;															//wspolrzedna Y generowanego punktu

	GLfloat angle = 2 * M_PI / vertices;									//obliczanie kata miedzy wierzcholkami

	vector<float> verts;													//zmienna typu vector zawierajaca liczby typu
																			//float, wybrano vector bo mozna przydzielic
																			//dynamiczna liczbe katow

	for (int i = 0; i < vertices; i++)										//generowanie wspolrzednych wierzcholkow
	{																		//wielokata
		vertX = radius * sin(i * angle);
		vertY = radius * cos(i * angle);
		verts.push_back(vertX);
		verts.push_back(vertY);
		verts.push_back(0.0f);
		verts.push_back(1.0f);
	}

	glBindVertexArray(vao[0]);												//dowiazanie pierwszego VAO

	//VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), reinterpret_cast<GLfloat *>(&verts[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka dla wspolrzednych
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzcholka dla wspolrzednych

	glBindVertexArray(0);
}