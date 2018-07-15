#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <IL/il.h>

#include <iostream>
#include <vector>

#include "shaders.h"

const int WIDTH = 768;
const int HEIGHT = 576;

const float ROT_STEP = 10.0f;
const int NUM_SHADERS = 1;

enum Shader { SHADER };

struct ImageData //Struct w ktorym zawarte sa dane o plikach (heightmap oraz colormap)
{
	int width; //szerokosc
	int height; //wysokosc
	ILubyte* bytes; //kolejne bity
};

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void renderScene();
void keyboard(unsigned char key, int x, int y);
void setupShaders();
void getUniformsLocations(Shader s);

void generateTerrain();
ImageData* getImageData(const wchar_t* filename);

GLfloat heightMultiplier = 40; //Mnoznik wysokosci, uzywany do poprawnego obliczenia i wyrysowania terenu
GLuint vao; //VAO
GLuint vbo[4]; //VBO
GLuint renderElements; //liczba elementow do renderowania

Shader shader = SHADER; // aktualny program cieniowania

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
glm::vec4 lightPosition = glm::vec4(0.0f, 20.0f, 25.0f, 1.0f); // pozycja w ukladzie swiata
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

// material obiektu
glm::vec3 materialAmbient = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 materialDiffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
glm::vec3 materialSpecular = glm::vec3(0.797357, 0.723991, 0.208006);
float shininess = 32.0f;

glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
glm::vec3 scl = glm::vec3(1.0, 1.0, 1.0); //Skalowanie

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

	initGL();

	glutMainLoop();

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
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
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	generateTerrain(); //Funkcja generujaca teren

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

	glm::vec4 lightPos = mvMatrix * lightPosition; // przeliczenie polozenia swiatla do ukladu oka
	glUniform4fv(lightPositionLoc, 1, glm::value_ptr(lightPos));

	glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));

	glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));

	glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
	glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
	glUniform1f(materialShininessLoc, shininess);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Obrot
	mvMatrix *= glm::rotate(glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));

	//Skalowanie
	mvMatrix *= glm::scale(glm::vec3(scl.x, scl.y, scl.z));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(mvMatrix));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

	//Rysowanie terenu przez paski trojkatow
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, renderElements, GL_UNSIGNED_INT, 0);
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
		rot.x -= ROT_STEP;
		if (rot.x < 0.0f)
			rot.x += 360.0f;
		break;

	case 's':
		rot.x += ROT_STEP;
		if (rot.x > 360.0f)
			rot.x -= 360.0f;
		break;

	case 'a':
		rot.y -= ROT_STEP;
		if (rot.y < 0.0f)
			rot.y += 360.0f;
		break;

	case 'd':
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

		//Skalowanie obiektu
	case 'r': //przyblizenie
		scl *= 2;
		break;

	case 'f': //oddalenie
		scl /= 2;
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram[SHADER]))
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

	lightAmbientLoc = glGetUniformLocation(shaderProgram[shader], "lightAmbient");
	materialAmbientLoc = glGetUniformLocation(shaderProgram[shader], "materialAmbient");

	lightPositionLoc = glGetUniformLocation(shaderProgram[shader], "lightPosition");
	lightDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "lightDiffuse");
	materialDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "materialDiffuse");

	lightSpecularLoc = glGetUniformLocation(shaderProgram[shader], "lightSpecular");
	materialSpecularLoc = glGetUniformLocation(shaderProgram[shader], "materialSpecular");
	materialShininessLoc = glGetUniformLocation(shaderProgram[shader], "materialShininess");
}

void generateTerrain() //Generowanie terenu
{
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ImageData* terrainData = getImageData((const wchar_t*)(L"terrain.png")); //Pobiera dane z pliku .png
	ImageData* colorData = getImageData((const wchar_t*)(L"colormap.png"));

	std::vector<GLfloat> vertices; //Kontenery na potrzebne dane
	std::vector<GLfloat> normals;
	std::vector<GLuint> indices;
	std::vector<GLfloat> colors;

	int primitiveEnd = terrainData->width * terrainData->height; //Koniec prymitywu
	int currentVert; //Obecny wierzcholek

	std::cout << "size: " << (sizeof(&(terrainData->bytes)) / sizeof(&(terrainData->bytes[0]))) << std::endl; //Rozmiar

	for (int i = 0; i < terrainData->height; i++) //Petla po kolejnych poziomach (wysokosc)
	{
		for (int j = 0; j < terrainData->width; j++) //Petla po wierzcholkach na danym poziomie (szerokosc)
		{
			currentVert = j + i * terrainData->width;

			//Obliczenie i wrzucenie wierzcholkow do kontenera
			vertices.push_back(j - terrainData->width / 2);
			vertices.push_back(i - terrainData->height / 2);
			vertices.push_back((GLfloat)terrainData->bytes[currentVert * 3] / 255 * heightMultiplier);

			//Indeks mapy kolorow
			int colormapIndex = ((GLfloat)terrainData->bytes[currentVert * 3] / 255) * colorData->width;

			//Obliczenie i wrzucenie kolorow do kontenera
			colors.push_back(colorData->bytes[colormapIndex * 3] / 255.0f);
			colors.push_back(colorData->bytes[colormapIndex * 3 + 1] / 255.0f);
			colors.push_back(colorData->bytes[colormapIndex * 3 + 2] / 255.0f);

			if (i != terrainData->height - 1) //Warunek do sprawdzenia, czy to ostatni wierzcholek wysokosci
			{
				indices.push_back(j + i * terrainData->width);
				indices.push_back(j + (i + 1) * terrainData->height);
			}
		}
		indices.push_back(primitiveEnd); //Zakonczenie jednego paska
	}

	for (int i = 0; i < terrainData->height; i++) //Petla po kolejnych poziomach (wysokosc)
	{
		for (int j = 0; j < terrainData->width; j++) //Petla po wierzcholkach na danym poziomie (szerokosc)
		{
			int numberOfVertices = 5; //Zmienna pomocnicza do obliczania normali metoda usredniania
			if (j == 0 || j == terrainData->height - 1) numberOfVertices--; //Warunki do wierzcholkow na krawedzi
			if (i == 0 || i == terrainData->width - 1) numberOfVertices--;

			//Obliczenie normali metoda usredniania normali
			GLfloat vertexX = vertices[(j + i * terrainData->width) * 3] +
				(j == 0 ? 0 : vertices[((j - 1) + i * terrainData->width) * 3]) +
				(j == terrainData->height - 1 ? 0 : vertices[((j + 1) + i * terrainData->width) * 3]) +
				(i == 0 ? 0 : vertices[(j + (i - 1) * terrainData->width) * 3]) +
				(i == terrainData->width - 1 ? 0 : vertices[(j + (i + 1) * terrainData->width) * 3]);

			GLfloat vertexY = vertices[(j + i * terrainData->width) * 3 + 1] +
				(j == 0 ? 0 : vertices[((j - 1) + i * terrainData->width) * 3 + 1]) +
				(j == terrainData->height - 1 ? 0 : vertices[((j + 1) + i * terrainData->width) * 3 + 1]) +
				(i == 0 ? 0 : vertices[(j + (i - 1) * terrainData->width) * 3 + 1]) +
				(i == terrainData->width - 1 ? 0 : vertices[(j + (i + 1) * terrainData->width) * 3 + 1]);

			GLfloat vertexZ = vertices[(j + i * terrainData->width) * 3 + 2] +
				(j == 0 ? 0 : vertices[((j - 1) + i * terrainData->width) * 3 + 2]) +
				(j == terrainData->height - 1 ? 0 : vertices[((j + 1) + i * terrainData->width) * 3 + 2]) +
				(i == 0 ? 0 : vertices[(j + (i - 1) * terrainData->width) * 3 + 2]) +
				(i == terrainData->width - 1 ? 0 : vertices[(j + (i + 1) * terrainData->width) * 3 + 2]);

			//Wrzucenie normali do kontenera
			normals.push_back(vertexX / numberOfVertices);
			normals.push_back(vertexY / numberOfVertices);
			normals.push_back(vertexZ / numberOfVertices);
		}
	}
	renderElements = indices.size() * 3;

	//Przypisanie VAO, VBO i zaladowanie do nich odpowiednich danych
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, vbo);
	glBindVertexArray(vao);

	//Wierzcholki
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Normale
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Kolory
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Indeksy
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(primitiveEnd); //Ustawienie restartu paska

	glBindVertexArray(0);
}

ImageData* getImageData(const wchar_t* filename) //Pobieranie wartosci o obrazku, w parametrze nazwa obrazka
{
	ImageData* data = new ImageData(); //Nowy struct (z poczatku programu), w ktorym beda przechowywane dane
	ILuint imageName; //Zmienna potrzebna do generowania obrazu
	ilGenImages(1, &imageName); //Funkcja z biblioteki DevIL, generowanie obrazka do komorki pamieci imageName
	ilBindImage(imageName); //Powiazanie obrazka

	//Sprawdzenie czy obrazek o takiej nazwie istnieje, jezeli nie czysci pamiec i wylacza program
	if (!ilLoadImage(filename))
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageName);
		exit(1);
	}

	data->width = ilGetInteger(IL_IMAGE_WIDTH); //Pobranie szerokosci obrazka
	data->height = ilGetInteger(IL_IMAGE_HEIGHT); //Pobranie wysokosci obrazka
	data->bytes = new ILubyte[data->width * data->height * 3]; //Obliczenie ilosci bitow

	ilCopyPixels(0, 0, 0, data->width, data->height, 1, IL_RGB, IL_UNSIGNED_BYTE, data->bytes); //Kopiowanie pikseli

	//Czyszczenie pamieci
	ilBindImage(0);
	ilDeleteImages(1, &imageName);

	return data;
}