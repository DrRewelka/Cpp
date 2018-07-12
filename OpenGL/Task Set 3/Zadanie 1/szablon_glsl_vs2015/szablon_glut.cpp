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

const float ROT_STEP = 10.0f;
const int NUM_MODELS = 3;
const int NUM_SHADERS = 6;

enum Model { SPHERE, CYLINDER, CONE };
enum Shader { AMBIENT, DIFFUSE, SPECULAR, ADS_FLAT, ADS_VERTEX, ADS_FRAGMENT };

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void renderScene();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void setupShaders();
void getUniformsLocations(Shader s);
void makeObjects();

//******************************************************************************************
const std::string models_str[] =
{
	"teapot (1024 trojkatow)",
	"teapot (6320 trojkatow)",
	"teapot (40000 trojkatow)"
};

const std::string shader_str[] =
{
	"ambient",
	"diffuse",
	"specular",
	"ambient-diffuse-specular (flat)",
	"ambient-diffuse-specular (per vertex)",
	"ambient-diffuse-specular (per fragment)"
};

Model model = CYLINDER; // aktualny model do wyrysowania
unsigned int renderElements[NUM_MODELS];

GLuint vao[NUM_MODELS]; // identyfikatory VAO
GLuint buffers[3 * NUM_MODELS]; // identyfikatory VBO

Shader shader = ADS_VERTEX; // aktualny program cieniowania

GLuint shaderProgram[NUM_SHADERS]; // identyfikatory programow cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka
GLuint normalLoc; // lokalizacja atrybutu wierzcholka - normal dla wierzcholka

GLuint colorLoc; // lokalizacja zmiennej jednorodnej - kolor
GLuint projMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok
GLuint normalMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz sluzaca do transformacji normali

GLuint lightAmbientLoc; // lokalizacja zmiennej jednorodnej - skladowa ambient swiatla
GLuint materialAmbientLoc; // lokalizacja zmiennej jednorodnej - skladowa ambient materialu

GLuint lightPositionLoc; // lokalizacja zmiennej jednorodnej - pozycja swiatla
GLuint lightDiffuseLoc; // lokalizacja zmiennej jednorodnej - skladowa diffuse swiatla
GLuint materialDiffuseLoc; // lokalizacja zmiennej jednorodnej - skladowa diffuse materialu

GLuint lightSpecularLoc; // lokalizacja zmiennej jednorodnej - skladowa specular swiatla
GLuint materialSpecularLoc; // lokalizacja zmiennej jednorodnej - skladowa specular materialu
GLuint materialShininessLoc; // lokalizacja zmiennej jednorodnej - polyskliwosc materialu

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

					// parametry swiatla
glm::vec4 lightPosition = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f); // pozycja we ukladzie swiata
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0, 1.0, 1.0);

// material obiektu
glm::vec3 materialAmbient = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 materialDiffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
glm::vec3 materialSpecular = glm::vec3(0.797357, 0.723991, 0.208006);
float shininess = 83.2f;

bool wireframe = false; // czy rysowac siatke (true) czy wypelnienie (false)
glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
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
	glutCreateWindow("OpenGL (Core Profile) - Lighting");

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
	glDeleteBuffers(3 * NUM_MODELS, buffers);
	glDeleteVertexArrays(NUM_MODELS, vao);

	for (int i = 0; i < NUM_SHADERS; ++i)
		glDeleteProgram(shaderProgram[i]);
}

/*------------------------------------------------------------------------------------------
** funkcja wypisujaca podstawowe informacje o ustawieniach programu
**------------------------------------------------------------------------------------------*/
void printStatus()
{
	system("cls");

	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;

	std::cout << "Model: " << models_str[model] << std::endl;
	std::cout << "Program cieniowania: " << shader_str[shader] << std::endl;
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	makeObjects();

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

	float aspectRatio = (float)w / h;

	projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram[shader]);
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	if (shader != AMBIENT)
	{
		glm::vec4 lightPos = mvMatrix * lightPosition; // przeliczenie polozenia swiatla do ukladu oka
		glUniform4fv(lightPositionLoc, 1, glm::value_ptr(lightPos));
	}

	switch (shader)
	{
	case AMBIENT:
		glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
		glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));
		break;

	case DIFFUSE:
		glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
		glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));
		break;

	case SPECULAR:
		glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
		glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
		glUniform1f(materialShininessLoc, shininess);
		break;

	case ADS_FLAT:
	case ADS_VERTEX:
	case ADS_FRAGMENT:
		glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
		glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));

		glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
		glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));

		glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
		glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
		glUniform1f(materialShininessLoc, shininess);
		break;
	}

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mvMatrix *= glm::rotate(glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(mvMatrix));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

	glBindVertexArray(vao[model]);
	glDrawElements(GL_TRIANGLES, renderElements[model], GL_UNSIGNED_INT, 0);
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

	case 'a':
		rot.x -= ROT_STEP;
		if (rot.x < 0.0f)
			rot.x += 360.0f;
		break;

	case 'd':
		rot.x += ROT_STEP;
		if (rot.x > 360.0f)
			rot.x -= 360.0f;
		break;

	case 's':
		rot.y -= ROT_STEP;
		if (rot.y < 0.0f)
			rot.y += 360.0f;
		break;

	case 'w':
		rot.y += ROT_STEP;
		if (rot.y > 360.0f)
			rot.y -= 360.0f;
		break;

	case 'z':
		rot.z -= ROT_STEP;
		if (rot.z < 0.0f)
			rot.z += 360.f;
		break;

	case 'q':
		rot.z += ROT_STEP;
		if (rot.z > 360.0f)
			rot.z -= 360.0f;
		break;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
		shader = (Shader)(key - 49);
		getUniformsLocations(shader);
		printStatus();
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
		printStatus();
		break;

	case GLUT_KEY_F2:
		model = SPHERE;
		printStatus();
		break;

	case GLUT_KEY_F3:
		model = CYLINDER;
		printStatus();
		break;

	case GLUT_KEY_F4:
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			exit(0);
		else
		{
			model = CONE;
			printStatus();
		}
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/ambient.vert", "shaders/ambient.frag", shaderProgram[AMBIENT]))
		exit(3);

	if (!setupShaders("shaders/diffuse.vert", "shaders/diffuse.frag", shaderProgram[DIFFUSE]))
		exit(3);

	if (!setupShaders("shaders/specular.vert", "shaders/specular.frag", shaderProgram[SPECULAR]))
		exit(3);

	if (!setupShaders("shaders/ads_flat.vert", "shaders/ads_flat.frag", shaderProgram[ADS_FLAT]))
		exit(3);

	if (!setupShaders("shaders/ads_per_vertex.vert", "shaders/ads_per_vertex.frag", shaderProgram[ADS_VERTEX]))
		exit(3);

	if (!setupShaders("shaders/ads_per_fragment.vert", "shaders/ads_per_fragment.frag", shaderProgram[ADS_FRAGMENT]))
		exit(3);

	getUniformsLocations(shader);
}

/*------------------------------------------------------------------------------------------
** funkcja pobiera lokalizacje zmiennych jednorodnych dla wybranego programu cieniowania
** shader - indeks programu cieniowania w tablicy z identyfikatorami programow
**------------------------------------------------------------------------------------------*/
void getUniformsLocations(Shader shader)
{
	projMatrixLoc = glGetUniformLocation(shaderProgram[shader], "projectionMatrix");
	mvMatrixLoc = glGetUniformLocation(shaderProgram[shader], "modelViewMatrix");
	normalMatrixLoc = glGetUniformLocation(shaderProgram[shader], "normalMatrix");

	switch (shader)
	{
	case AMBIENT:
		lightAmbientLoc = glGetUniformLocation(shaderProgram[shader], "lightAmbient");
		materialAmbientLoc = glGetUniformLocation(shaderProgram[shader], "materialAmbient");
		break;

	case DIFFUSE:
		lightPositionLoc = glGetUniformLocation(shaderProgram[shader], "lightPosition");
		lightDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "lightDiffuse");
		materialDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "materialDiffuse");
		break;

	case SPECULAR:
		lightPositionLoc = glGetUniformLocation(shaderProgram[shader], "lightPosition");
		lightSpecularLoc = glGetUniformLocation(shaderProgram[shader], "lightSpecular");
		materialSpecularLoc = glGetUniformLocation(shaderProgram[shader], "materialSpecular");
		materialShininessLoc = glGetUniformLocation(shaderProgram[shader], "materialShininess");
		break;

	case ADS_FLAT:
	case ADS_VERTEX:
	case ADS_FRAGMENT:
		lightAmbientLoc = glGetUniformLocation(shaderProgram[shader], "lightAmbient");
		materialAmbientLoc = glGetUniformLocation(shaderProgram[shader], "materialAmbient");

		lightPositionLoc = glGetUniformLocation(shaderProgram[shader], "lightPosition");
		lightDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "lightDiffuse");
		materialDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "materialDiffuse");

		lightSpecularLoc = glGetUniformLocation(shaderProgram[shader], "lightSpecular");
		materialSpecularLoc = glGetUniformLocation(shaderProgram[shader], "materialSpecular");
		materialShininessLoc = glGetUniformLocation(shaderProgram[shader], "materialShininess");
		break;
	}
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void makeObjects()
{
	glGenVertexArrays(NUM_MODELS, vao);
	glGenBuffers(3 * NUM_MODELS, buffers);

	// czajnik z mala liczba wierzcholkow
	glBindVertexArray(vao[SPHERE]);

	std::vector<GLfloat> sphereVertices; //wektor przechowujacy wierzcholki, dynamiczny przydzial
	std::vector<GLuint> sphereTriangles; //wektor przechowujacy trojkaty, dynamiczny przydzial
	std::vector<GLfloat> sphereNormalsHorizontalPos; //wektor przechowujacy normale, dynamiczny przydzial
	std::vector<GLfloat> sphereNormalsVerticalPos;
	std::vector<GLfloat> sphereNormalsTemp;
	std::vector<GLfloat> sphereNormals;

	int sphereHorizontalVertices = 16; //ilosc wierzcholkow w poziomie
	int sphereVerticalVertices = 8; //ilosc wierzcholkow w pionie

	float sphereRadius = 1.0f; //promien
	float spherePhiMin = -90.0f; //minimalny kat
	float spherePhiMax = 90.0f; //maksymalny kat
	float sphereX, sphereY, sphereZ, sphereU, sphereV; //wspolrzedne i zmienne u, v - kolejne wierzcholki
	float sphereNormalX, sphereNormalY, sphereNormalZ;
	float sphereNormalHorizontalX, sphereNormalHorizontalY, sphereNormalHorizontalZ; //wspolrzedne normali poszczegolnych wierzcholkow
	float sphereNormalVerticalX, sphereNormalVerticalY, sphereNormalVerticalZ;
	float sphereTheta, spherePhi; //kat wypelnienia i kat miedzy wierzcholkami

	//obliczenia wszystkich wartosci do wyrysowania sfery
	for (int i = 0; i < sphereVerticalVertices; i++) //petla do kolejnych poziomow wierzcholkow
	{
		for (int j = 0; j < sphereHorizontalVertices; j++) //petla do kolejnych wierzcholkow
		{												   //na jednym poziomie
			sphereU = (float)j / (sphereHorizontalVertices - 1);
			sphereV = (float)i / (sphereVerticalVertices - 1);
			spherePhi = glm::radians(spherePhiMin + sphereV * (spherePhiMax - spherePhiMin));
			sphereTheta = glm::radians(sphereU * 360.0f);
			sphereX = sphereRadius * cos(sphereTheta) * cos(spherePhi); //wspolrzedna x wierzcholka
			sphereY = sphereRadius * sin(sphereTheta) * cos(spherePhi); //wspolrzedna y wierzcholka
			sphereZ = sphereRadius * sin(spherePhi);					//wspolrzedna z wierzcholka
			sphereVertices.push_back(sphereX);							//wrzucenie wspolrzednych x, y i z do
			sphereVertices.push_back(sphereY);							//wektora przechowujacego wierzcholki
			sphereVertices.push_back(sphereZ);

			if (i != (sphereVerticalVertices - 1) && j != (sphereHorizontalVertices - 1)) //rysowanie
			{	//bez uwzglednienia ostatniego wierzcholka, bez sensu rysowac go dwa razy
				sphereTriangles.push_back(i * sphereHorizontalVertices + sphereHorizontalVertices + j);
				sphereTriangles.push_back(i * sphereHorizontalVertices + j + 1);
				sphereTriangles.push_back(i * sphereHorizontalVertices + j);
				//wrzucenie kolejnych wierzcholkow do wektora z trojkatami w odpowiedniej
				//kolejnosci zeby je dobrze polaczyc
				sphereTriangles.push_back(i * sphereHorizontalVertices + j + 1);
				sphereTriangles.push_back(i * sphereHorizontalVertices + sphereHorizontalVertices + j);
				sphereTriangles.push_back(i * sphereHorizontalVertices + sphereHorizontalVertices + j + 1);
			}

			if (sphereVertices.size() > 3) //j=1, wektory na danym poziomie
			{
				sphereNormalHorizontalX = sphereVertices[3 * j - 3] - sphereVertices[3 * j];
				sphereNormalHorizontalY = sphereVertices[3 * j - 2] - sphereVertices[3 * j + 1];
				sphereNormalHorizontalZ = sphereVertices[3 * j - 1] - sphereVertices[3 * j + 2];
				sphereNormalsHorizontalPos.push_back(sphereNormalHorizontalX);
				sphereNormalsHorizontalPos.push_back(sphereNormalHorizontalY);
				sphereNormalsHorizontalPos.push_back(sphereNormalHorizontalZ);
			}
		}

		if (i > 0) //i=1, wektory pionowe miedzy dwoma poziomami
		{
			sphereNormalVerticalX = sphereVertices[3 * (i - 1)] - sphereVertices[3 * (i - 1) + 3 * (sphereHorizontalVertices - 1)];
			sphereNormalVerticalY = sphereVertices[3 * (i - 1) + 1] - sphereVertices[3 * (i - 1) + 3 * (sphereHorizontalVertices - 1) + 1];
			sphereNormalVerticalZ = sphereVertices[3 * (i - 1) + 2] - sphereVertices[3 * (i - 1) + 3 * (sphereHorizontalVertices - 1) + 2];
			sphereNormalsVerticalPos.push_back(sphereNormalVerticalX);
			sphereNormalsVerticalPos.push_back(sphereNormalVerticalY);
			sphereNormalsVerticalPos.push_back(sphereNormalVerticalZ);
		}
	}

	for (int i = 2; i < sphereHorizontalVertices * sphereVerticalVertices + 2; i = i + 3)
	{
		sphereNormalsTemp[i - 2] = sphereNormalsHorizontalPos[i - 1] * sphereNormalsVerticalPos[i] - sphereNormalsVerticalPos[i - 1] * sphereNormalsHorizontalPos[i];
		sphereNormalsTemp[i - 1] = sphereNormalsVerticalPos[i - 2] * sphereNormalsHorizontalPos[i] - sphereNormalsHorizontalPos[i - 2] * sphereNormalsVerticalPos[i];
		sphereNormalsTemp[i] = sphereNormalsHorizontalPos[i - 2] * sphereNormalsVerticalPos[i - 1] - sphereNormalsVerticalPos[i - 2] * sphereNormalsHorizontalPos[i - 1];
	}

	//obliczenie normali metoda usredniania
	for (int i = 2; i < sphereNormalsTemp.size() + 2; i = i + 3)
	{
		if (i < sphereHorizontalVertices * 3)
		{
			sphereNormals[i - 2] = sphereNormalsTemp[i - 2] + sphereNormalsTemp[(i - 2) + 3] + sphereNormalsTemp[(i - 2) + 8] + sphereNormalsTemp[(i - 2) + 12];
			sphereNormals[i - 1] = sphereNormalsTemp[i - 1] + sphereNormalsTemp[(i - 1) + 3] + sphereNormalsTemp[(i - 1) + 8] + sphereNormalsTemp[(i - 1) + 12];
			sphereNormals[i] = sphereNormalsTemp[i] + sphereNormalsTemp[(i) + 3] + sphereNormalsTemp[(i) + 8] + sphereNormalsTemp[(i) + 12];
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT_LOW]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotLowPosition), teapotLowPosition, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT_LOW + 1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotLowNormal), teapotLowNormal, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3 * TEAPOT_LOW + 2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotLowIndices), teapotLowIndices, GL_STATIC_DRAW);

	renderElements[SPHERE] = sphereTriangles.size() * 3;

	// czajnik ze srednia liczba wierzcholkow
	glBindVertexArray(vao[CYLINDER]);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotPosition), teapotPosition, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT + 1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotNormal), teapotNormal, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3 * TEAPOT + 2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotIndices), teapotIndices, GL_STATIC_DRAW);

	renderElements[TEAPOT] = TEAPOT_INDICES_COUNT * 3;

	// czajnik z duza liczba wierzcholkow
	glBindVertexArray(vao[CONE]);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT_HIGH]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotHighPosition), teapotHighPosition, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3 * TEAPOT_HIGH + 1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapotHighNormal), teapotHighNormal, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3 * TEAPOT_HIGH + 2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotHighIndices), teapotHighIndices, GL_STATIC_DRAW);

	renderElements[TEAPOT_HIGH] = TEAPOT_HIGH_INDICES_COUNT * 3;

	glBindVertexArray(0);
}