#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stack>

#include <vector>
#include <iostream>

#include "shaders.h"

const int NUMBER_OF_MODELS = 3; //ilosc modeli do wyrysowania

const int WIDTH = 768;
const int HEIGHT = 576;
const int TIME = 40; // czas po jakim zostanie uruchomiona funkcja timera

//struct z wlasciwosciami kazdego modelu (Slonca, Ziemi i Ksiezyca)
struct Model
{
	GLuint vao;
	GLuint buffers[2];
	unsigned int renderElements;
	glm::vec4 color;
	glm::vec3 offsetPos;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h); //funkcja do zmiany wielkosci okna
void renderScene();
void renderModel(Model model); //rysowanie modelu
void keyboard(unsigned char key, int x, int y); //funkcja do obslugi programu klawiatura
void specialKeys(int key, int x, int y); //funkcja do klawiszy specjalnych
void setupShaders();
void setupBuffers();
void timer(int value); //funkcja timera do animacji

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka

GLuint projMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok
GLuint uColorLoc;

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok
std::stack<glm::mat4> mvStack; // stos macierzy model-widok

float aspectRatio = (float)WIDTH / HEIGHT; // proporcje okna

Model models[NUMBER_OF_MODELS];
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
	glutCreateWindow("OpenGL (Core Profile) - Animation");

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

	glutTimerFunc(TIME, timer, 1); // rejestracja funkcji timera

	initGL();

	glutMainLoop();

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	for (int i = 0; i < NUMBER_OF_MODELS; i++)
	{
		glDeleteBuffers(2, models[i].buffers);
		glDeleteVertexArrays(1, &models[i].vao);
		glDeleteProgram(shaderProgram);
	}
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

	projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//macierz model-widok
	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram);

	//przekazanie macierzy projekcji do zmiennej jednorodnej
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //tryb rysowania - wypelnienie

	//transformacje i rysowanie Slonca
	mvStack.push(mvMatrix); //wrzucenie macierzy model-widok na stos
	//transformacje Slonca
	mvMatrix *= glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(models[0].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(models[0].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(models[0].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMatrix *= glm::scale(models[0].scale);

	//przekazanie macierzy model-widok do zmiennej jednorodnej
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	renderModel(models[0]); //rysowanie modelu

	mvMatrix = mvStack.top(); //przypisanie szczytu stosu do macierzy model-widok
	mvStack.pop(); //zrzucenie macierzy model-widok ze stosu

	//transformacje i rysowanie Ziemi
	mvStack.push(mvMatrix); //wrzucenie macierzy model-widok na stos
	//transformacje Ziemi
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMatrix *= glm::translate(models[1].offsetPos);
	mvMatrix *= glm::scale(models[1].scale);

	//przekazanie macierzy model-widok do zmiennej jednorodnej
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	renderModel(models[1]); //rysowanie Ziemi

	mvMatrix = mvStack.top(); //przypisanie szczytu stosu do macierzy model-widok
	mvStack.pop(); //zrzucenie macierzy model-widok ze stosu

	//transformacje i rysowanie Ksiezyca
	mvStack.push(mvMatrix); //wrzucenie macierzy model-widok na stos
	//tranformacje Ksiezyca
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(models[1].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMatrix *= glm::translate(models[1].offsetPos);
	mvMatrix *= glm::rotate(glm::radians(models[2].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(models[2].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(models[2].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMatrix *= glm::translate(models[2].offsetPos);
	mvMatrix *= glm::scale(models[2].scale);

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	renderModel(models[2]); //rysowanie Ksiezyca

	mvMatrix = mvStack.top(); //przypisanie szczytu stosu do macierzy model-widok
	mvStack.pop(); //zrzucenie macierzy model-widok ze stosu

	glutSwapBuffers();
}

void renderModel(Model model)
{
	glUniform4fv(uColorLoc, 1, glm::value_ptr(model.color));

	glBindVertexArray(model.vao);
	glDrawElements(GL_TRIANGLES, model.renderElements, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
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
	uColorLoc = glGetUniformLocation(shaderProgram, "uColor");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z cialami niebieskimi
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	//sfera (ogolnie, z zadania 1 podpunkt A)
	std::vector<GLfloat> vertices;
	std::vector<GLuint> triangles;

	int horizontalVertices = 32; //ilosc wierzcholkow w poziomie
	int verticalVertices = 8; //ilosc wierzcholkow w pionie
	float radius = 0.5f; //promien
	float phiMin = -90.0f; //minimalny kat
	float phiMax = 90.0f; //maksymalny kat
	float x, y, z, u, v; //wspolrzedne i zmienne u, v - kolejne wierzcholki
	float theta, phi; //kat wypelnienia i kat miedzy wierzcholkami

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

	//Slonce
	glGenVertexArrays(1, &models[0].vao); //generowanie vao
	glGenBuffers(2, models[0].buffers); //generowanie vbo

	glBindVertexArray(models[0].vao); //dowiazanie vao

	glBindBuffer(GL_ARRAY_BUFFER, models[0].buffers[0]); //dowiazanie vbo dla wierzcholkow
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[0].buffers[1]); //vbo dla trojkatow
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), &triangles[0], GL_STATIC_DRAW);

	models[0].renderElements = 3 * triangles.size();

	//ustawienia Slonca
	models[0].offsetPos = glm::vec3(0, 0, 0);
	models[0].scale = glm::vec3(2.3f, 2.3f, 2.3f);
	models[0].color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);

	//Ziemia
	glGenVertexArrays(1, &models[1].vao); //generowanie vao
	glGenBuffers(2, models[1].buffers); //generowanie vbo

	glBindVertexArray(models[1].vao); //dowiazanie vao

	glBindBuffer(GL_ARRAY_BUFFER, models[1].buffers[0]); //dowiazanie vbo dla wierzcholkow
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[1].buffers[1]); //vbo dla trojkatow
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), &triangles[0], GL_STATIC_DRAW);

	models[1].renderElements = 3 * triangles.size();

	//ustawienia Ziemi
	models[1].offsetPos = glm::vec3(2.5f, 0, 0);
	models[1].scale = glm::vec3(0.7f, 0.7f, 0.7f);
	models[1].color = glm::vec4(0.0f, 0.2f, 1.0f, 1.0f);

	//Ksiezyc
	glGenVertexArrays(1, &models[2].vao); //generowanie vao
	glGenBuffers(2, models[2].buffers); //generowanie vbo

	glBindVertexArray(models[2].vao); //dowiazanie vao

	glBindBuffer(GL_ARRAY_BUFFER, models[2].buffers[0]); //dowiazanie vbo dla wierzcholkow
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[2].buffers[1]); //vbo dla trojkatow
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), &triangles[0], GL_STATIC_DRAW);

	models[2].renderElements = 3 * triangles.size();

	//ustawienia Ksiezyca
	models[2].offsetPos = glm::vec3(1, 0, 0);
	models[2].scale = glm::vec3(0.15f, 0.15f, 0.15f);
	models[2].color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	glBindVertexArray(0);
}

/*------------------------------------------------------------------------------------------
** funkcja timera
**------------------------------------------------------------------------------------------*/
void timer(int value)
{
	models[1].rotation.y += 1.5f;
	if (models[1].rotation.y > 360.0f)
		models[1].rotation.y -= 360.0f;

	models[2].rotation.y += 2.5f;
	if (models[2].rotation.y > 360.0f)
		models[2].rotation.y -= 360.0f;

	glutPostRedisplay();

	glutTimerFunc(TIME, timer, 1);
}