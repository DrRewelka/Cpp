#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>

#include "shaders.h"

const int WIDTH = 768;
const int HEIGHT = 576;
const float ROT_STEP = 10.0f; //stala definiujaca krok obrotu

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h); //funkcja do zmiany wielkosci okna
void updateProjectionMatrix(); //funkcja aktualizujaca macierz projekcji
void renderScene();
void keyboard(unsigned char key, int x, int y); //funkcja do kontroli nad obrazem
void specialKeys(int key, int x, int y); //funkcja do klawiszy specjalnych - F1 i F4
void setupShaders();
void setupBuffers();

//******************************************************************************************
GLuint vao; // identyfikator VAO
GLuint buffers[2]; // identyfikatory VBO

GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka

GLuint projMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

bool wireframe = true; // czy rysowac siatke (true) czy wypelnienie (false)
glm::vec3 rotationAngles = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
float fovy = 45.0f; // kat patrzenia (uzywany do skalowania sceny)
float aspectRatio = (float)WIDTH / HEIGHT; //stosunek szerokosci do wysokosci okna

unsigned int renderElements = 0; // liczba elementow do wyrysowania
//******************************************************************************************

/*------------------------------------------------------------------------------------------
** funkcja glowna
**------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	atexit(onShutdown);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL (Core Profile) - Transformations");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "Blad: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	if (!GLEW_VERSION_3_1)
	{
		std::cerr << "Brak obslugi OpenGL 3.1\n";
		exit(2);
	}

	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);

	initGL();

	glutMainLoop();

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
}

/*------------------------------------------------------------------------------------------
** funkcja wypisujaca podstawowe informacje o ustawieniach programu
**------------------------------------------------------------------------------------------*/
void printStatus()
{
	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	setupBuffers();

	printStatus();
}

/*------------------------------------------------------------------------------------------
** funkcja wywolywana przy zmianie rozmiaru okna
** w - aktualna szerokosc okna
** h - aktualna wysokosc okna
**------------------------------------------------------------------------------------------*/
void changeSize(GLsizei w, GLsizei h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);

	aspectRatio = (float)w / h;

	updateProjectionMatrix();
}

/*------------------------------------------------------------------------------------------
** funkcja aktualizuje macierz projekcji
**------------------------------------------------------------------------------------------*/
void updateProjectionMatrix()
{
	projMatrix = glm::perspective(glm::radians(fovy), aspectRatio, 0.1f, 100.0f);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//macierz model-widok
	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram);

	//przekazanie macierzy projekcji do zmiennej jednorodnej
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	//tryb wyswietlania - siatka i wypelnienie
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));

	//przekazanie macierzy model-widok do zmiennej jednorodnej
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	//rysowanie sfery reprezentowanej przez vao
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, renderElements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja obslugujaca klawiature (klawiesze ASCII)
** key - nacisniety klawisz
** x, y - wspolrzedne kursora myszki w momencie nacisniecia klawisza key na klawiaturze
**------------------------------------------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC
		exit(0);
		break;

	case 'w':
		rotationAngles.x -= ROT_STEP;
		if (rotationAngles.x < 0.0f)
			rotationAngles.x += 360.0f;
		break;

	case 's':
		rotationAngles.x += ROT_STEP;
		if (rotationAngles.x > 360.0f)
			rotationAngles.x -= 360.0f;
		break;

	case 'a':
		rotationAngles.y -= ROT_STEP;
		if (rotationAngles.y < 0.0f)
			rotationAngles.y += 360.0f;
		break;

	case 'd':
		rotationAngles.y += ROT_STEP;
		if (rotationAngles.y > 360.0f)
			rotationAngles.y -= 360.0f;
		break;

	case 'z':
		rotationAngles.z -= ROT_STEP;
		if (rotationAngles.z < 0.0f)
			rotationAngles.z += 360.f;
		break;

	case 'q':
		rotationAngles.z += ROT_STEP;
		if (rotationAngles.z > 360.0f)
			rotationAngles.z -= 360.0f;
		break;

	case '+':
	case '=':
		fovy /= 1.1;
		updateProjectionMatrix();
		break;

	case '-':
		if (1.1f * fovy < 180.0f)
		{
			fovy *= 1.1;
			updateProjectionMatrix();
		}
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja obslugujaca klawiature (klawisze specjalne)
** key - nacisniety klawisz
** x, y - wspolrzedne kursora myszki w momencie nacisniecia klawisza key na klawiaturze
**------------------------------------------------------------------------------------------*/
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		wireframe = !wireframe;
		break;

	case GLUT_KEY_F4:
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			exit(0);
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram))
		exit(3);

	projMatrixLoc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	mvMatrixLoc = glGetUniformLocation(shaderProgram, "modelViewMatrix");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO ze sfera
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, buffers);

	glBindVertexArray(vao); //powiazanie vao, reprezentuje sfere

	std::vector<GLfloat> vertices; //wektor przechowujacy wierzcholki, dynamiczny przydzial
	std::vector<GLuint> triangles; //wektor przechowujacy trojkaty, dynamiczny przydzial

	int horizontalVertices = 16; //ilosc wierzcholkow w poziomie
	int verticalVertices = 8; //ilosc wierzcholkow w pionie
	float radius = 1.0f; //promien
	float phiMin = -90.0f; //minimalny kat
	float phiMax = 90.0f; //maksymalny kat
	float x, y, z, u, v; //wspolrzedne i zmienne u, v - kolejne wierzcholki
	float theta, phi; //kat wypelnienia i kat miedzy wierzcholkami

	//obliczenia wszystkich wartosci do wyrysowania sfery
	for (int i = 0; i < verticalVertices; i++) //petla do kolejnych poziomow wierzcholkow
	{
		for (int j = 0; j < horizontalVertices; j++) //petla do kolejnych wierzcholkow
		{											 //na jednym poziomie
			u = (float)j / (horizontalVertices - 1);
			v = (float)i / (verticalVertices - 1);
			phi = glm::radians(phiMin + v * (phiMax - phiMin));
			theta = glm::radians(u * 360.0f);
			x = radius * cos(theta) * cos(phi); //wspolrzedna x wierzcholka
			y = radius * sin(theta) * cos(phi); //wspolrzedna y wierzcholka
			z = radius * sin(phi);				//wspolrzedna z wierzcholka
			vertices.push_back(x);				//wrzucenie wspolrzednych x, y i z do
			vertices.push_back(y);				//wektora przechowujacego wierzcholki
			vertices.push_back(z);
			if (i != (verticalVertices - 1) && j != (horizontalVertices - 1)) //rysowanie
			{	//bez uwzglednienia ostatniego wierzcholka, bez sensu rysowac go dwa razy
				triangles.push_back(i * horizontalVertices + horizontalVertices + j);
				triangles.push_back(i * horizontalVertices + j + 1);
				triangles.push_back(i * horizontalVertices + j);
				//wrzucenie kolejnych wierzcholkow do wektora z trojkatami w odpowiedniej
				//kolejnosci zeby je dobrze polaczyc
				triangles.push_back(i * horizontalVertices + j + 1);
				triangles.push_back(i * horizontalVertices + horizontalVertices + j);
				triangles.push_back(i * horizontalVertices + horizontalVertices + j + 1);
			}
		}
	}
	
	renderElements = 3 * triangles.size();

	//wrzucenie wierzcholkow do pierwszego bufora przy vao
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//wrzucenie trojkatow do drugiego bufora przy vao
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), &triangles[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}