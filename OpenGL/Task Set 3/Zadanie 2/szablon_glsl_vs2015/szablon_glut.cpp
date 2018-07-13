#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "SOIL.h"

#include "shaders.h"

const int WIDTH = 768;
const int HEIGHT = 576;

const float ROT_STEP = 10.0f;
const float SCALE_STEP = 0.25f;
const int NUM_SHADERS = 6;

enum Shader { BLINNPHONG };

GLint TextureFromFile(const char* path, std::string directory);

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	aiString path;
};

void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void renderScene();
void keyboard(unsigned char key, int x, int y);
void setupShaders();
void getUniformsLocations(Shader s);

const std::string shader_str[] =
{
	"Blinn-Phong"
};

Shader shader = BLINNPHONG; // aktualny program cieniowania

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
glm::vec4 lightPosition = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f); // pozycja w ukladzie swiata
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0, 1.0, 1.0);

// material obiektu
glm::vec3 materialAmbient = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 materialDiffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
glm::vec3 materialSpecular = glm::vec3(0.797357, 0.723991, 0.208006);
float shininess = 83.2f;

glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
glm::vec3 scl = glm::vec3(1.0, 1.0, 1.0); //Skalowanie

class Mesh
{
public:
	//Dane dla poszczegolnych meshow
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	//Funkcje
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh();
	}
	void Draw(Shader shader)
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string name = this->textures[i].type;
			std::string number = (name == "texture_diffuse") ? std::to_string(diffuseNr++) : std::to_string(specularNr++);

			glUniform1i(glGetUniformLocation(shaderProgram[shader], ("material" + name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}
		//Ustawienie wlasciwosci lsnienia materialu na wartosc podstawowa
		glUniform1f(glGetUniformLocation(shaderProgram[shader], "materialShininess"), 16.0f);

		//Rysowanie mesha
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//Powrot do ustawien poczatkowych
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

	}
private:
	//Dane do renderu
	GLuint VAO, VBO, EBO;
	//Funkcje
	void setupMesh()
	{
		glGenVertexArrays(1, &this->VAO); //Zawiera caly model
		glGenBuffers(1, &this->VBO); //Zawiera wierzcholki (czyli wspolrzedne, normale i tekstury)
		glGenBuffers(1, &this->EBO); //Zawiera indeksy dla prymitywow

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		//Pozycja wierzcholka
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		//Normale wierzcholka
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		//Wspolrzedne tekstury wierzcholka
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};

class Model
{
public:
	//Funkcje
	Model(GLchar* path) //Kontruktor, podajemy sciezke do modelu
	{
		this->loadModel(path);
	}
	void Draw(Shader shader) //Rysuje model - czyli wszystkie jego meshe
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader);
	}
private:
	//Dane modelu
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesLoaded; //Przechowuje zaladowane wczesniej tekstury, w celu optymalizacji
										 //Funkcje
	void loadModel(std::string path) //Wczytuje model obslugiwany przez ASSIMP, a wynik wrzuca do wektora meshes
	{
		//Czytanie pliku przez ASSIMP
		Assimp::Importer import;
		//Opcje czytania z pliku, Triangulate - utworzy trojkaty jezeli ich nie bedzie, FlipUVs - odwroci
		//wspolrzedne UV jezeli bedzie to konieczne, GenNormals - utworzy normale jezeli ich nie bedzie
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		//Sprawdzenie czy nie wystapily bledy
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		//Wyciagniecie lokalizacji folderu ze sciezki do pliku
		this->directory = path.substr(0, path.find_last_of('/'));
		//Przetworzenie wezlow ASSIMP'a rekurencyjnie
		this->processNode(scene->mRootNode, scene);
	}
	//Przetwarza wezel rekurencyjnie - przetwarza kazdy mesh w danym wezle i powtarza ten proces dla kazdego
	//dziecka danego wezla (jezeli istnieja)
	void processNode(aiNode* node, const aiScene* scene)
	{
		//Przetwarza wszystkie meshe w danym wezle (jezeli istnieja)
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		//Przetwarza kazde dziecko danego wezla (jezeli istnieje)
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}
	}
	//Przetworzenie mesha
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		//Wektory, do ktorych beda ladowane dane mesha
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		//Przechodzi przez kazdy wierzcholek mesha
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//Przetwarza wspolrzedne wierzcholkow, normali i tekstur
			glm::vec3 vector; //Wektor pomocniczy, ASSIMP korzysta z wlasnego wektora, ktorego nie mozna
							  //bezposrednio przekonwertowac do glm::vec3
							  //Pozycje wierzcholkow
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			//Normale
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			//Wspolrzedne tekstur
			if (mesh->mTextureCoords[0]) //Nie wiadomo, czy mesh ma tekstury
			{
				glm::vec2 vec;
				//Wierzcholek moze miec do 8 wspolrzednych tekstury, ale przyjmujemy, ze uzywamy tylko x i y
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex); //Gotowy wierzcholek wrzucamy do wczesniej zadeklarowanego kontenera
		}
		//Przetwarza indeksy dla kazdego face'a (trojkata) i zwraca indeksy wierzcholkow
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			//Pobiera wszystkie indeksy danego face'a i wrzuca do wektora indeksow
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		//Przetwarza material jezeli istnieje
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			//Przyjmujemy konwencje nazewnictwa samplerow w shaderach (N to numer od 1 do maksymalnej liczby
			//samplerow):
			//Diffuse: texture_diffuseN
			//Specular: texture_specularN
			//Normal: texture_normalN

			//Diffuse
			std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//Specular
			std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures); //Zwracamy kompletny mesh
	}
	//Sprawdza tekstury wszystkich materialow danego typu i laduje te tekstury, jezeli nie zostaly jeszcze
	//zaladowane
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			//Sprawdza czy tekstura zostala wczesniej zaladowana, jezeli tak - pomija ladowanie jej
			GLboolean skip = false;
			for (GLuint j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j].path.C_Str(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					skip = true; //Tekstura o podanej sciezce zostala zaladowana, mozna ja pominac
					break;
				}
			}
			if (!skip)
			{
				//Jezeli tekstura nie byla zaladowana wczesniej, zaladuje ja teraz
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->texturesLoaded.push_back(texture);  //Doda teksture do kontenera zaladowanych tekstur
			}
		}
		return textures; //Zwraca teksture
	}
};

GLint TextureFromFile(const char* path, std::string directory)
{
	//Generuje ID tekstury i laduje dane tej tekstury 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	//Przypisuje teksture do ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Parametry
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

Model *myModel; //Deklaracja modelu

//******************************************************************************************

/*------------------------------------------------------------------------------------------
** funkcja glowna
**------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	//atexit(onShutdown);

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

	std::cout << "Program cieniowania: " << shader_str[shader] << std::endl;
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	//Wczytanie modelu ze sciezki
	myModel = new Model("modele/dragon.obj");

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

	switch (shader)
	{
	case BLINNPHONG:
		glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
		glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));

		glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
		glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));

		glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
		glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
		glUniform1f(materialShininessLoc, shininess);
		break;
	}

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

	//Rysowanie modelu
	myModel->Draw(shader);

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
		scl += SCALE_STEP;
		break;

	case 'f': //oddalenie
		scl -= SCALE_STEP;
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/blinnPhong.vert", "shaders/blinnPhong.frag", shaderProgram[BLINNPHONG]))
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