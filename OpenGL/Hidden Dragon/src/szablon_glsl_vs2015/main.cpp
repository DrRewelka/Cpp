#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp> //Do modeli
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "SOIL.h" //Do tekstur

#include "irrKlang\irrKlang.h" //Do muzyki

#include "shaders.h"

const int WIDTH = 1024;
const int HEIGHT = 576;

const float SCALE_STEP = 0.25f;
const float MOVE_STEP = 0.1f; //Krok przesuniecia obiektow, do przyspieszenia
const float DRAGON_MOVE_STEP = 0.25f; //Krok przesuniecia smoka podczas zmiany linii - plynny ruch
const int NUM_SHADERS = 1;

//Collidery dla jablka
const glm::vec3 BOX_TOP_LEFT_COLLIDER = glm::vec3(-0.125f, 0.125f, 0.0f);
const glm::vec3 BOX_TOP_RIGHT_COLLIDER = glm::vec3(0.125f, 0.125f, 0.0f);
const glm::vec3 BOX_BOT_LEFT_COLLIDER = glm::vec3(-0.125f, -0.125f, 0.0f);
const glm::vec3 BOX_BOT_RIGHT_COLLIDER = glm::vec3(0.125f, -0.125f, 0.0f);

//Stale okreslajace granice losowan liczb
const float LOWER_BORDER_X = -10.0f;
const float UPPER_BORDER_X = 0.0f;
const float LOWER_BORDER_Y = 3.0f;
const float UPPER_BORDER_Y = 0.0f;


enum Shader { TEXTURES };

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
void getUniforms(Shader s);
GLint TextureFromFile(const char* path);

//Funkcje do gry
void mapMovement(int value); //Timer - dziala caly czas - przemieszczanie sie jablek
void dragonMoveUp(int value); //Timer - przejscie na wyzsza linie
void dragonMoveDown(int value); //Timer - przejscie na nizsza linie
void appleCollision(glm::vec3 tempPosition); //Kolizje z jablkami
void rotation(); //Obrot jablka

Shader shader = TEXTURES; // aktualny program cieniowania

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

GLuint offsetXLoc; //Lokalizacja uniforma dla przesuniecia tekstury

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

// parametry swiatla
glm::vec4 lightPosition = glm::vec4(5.0f, 10.0f, -5.0f, 1.0f); // pozycja w ukladzie swiata
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

// material obiektu
glm::vec3 materialAmbient = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 materialDiffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
glm::vec3 materialSpecular = glm::vec3(0.797357, 0.723991, 0.208006);
float shininess = 32.0f;

glm::vec3 trs = glm::vec3(0.0f, 0.0f, 0.0f); //Przesuniecie

//ZMIENNE

//Wspolrzedne wierzcholkow tworzacych boxa - collidera dla smoka
glm::vec3 dragonTopLeftCollider = glm::vec3(-5.1f, 0.5f, 0.0f);
glm::vec3 dragonTopRightCollider = glm::vec3(-2.5f, 0.5f, 0.0f);
glm::vec3 dragonBotLeftCollider = glm::vec3(-5.1f, -1.0f, 0.0f);
glm::vec3 dragonBotRightCollider = glm::vec3(-2.5f, -1.0f, 0.0f);

bool isDead = false; //Czy smok jest martwy
bool isMovingUp = false; //Zmienne pomocnicze do plynnego ruchu
bool isMovingDown = false;
bool appleActive = true; //Czy jablko jest aktywne (mozliwa do zebrania)
bool isFirstApple = true; //Zmienna pomocnicza do generowania pierwszego jablka

float move_step = 0.4f; //Predkosc przemieszczania sie
float rotationAngleY = 0.0f; //Kat obrotu wokol osi Y dla obiektu
float moveHeight; //Maksymalna wysokosc przesuniecia smoka
float appleMoveX, appleMoveY; //Zmienne, do ktorych ladowane sa wartosci losowe
int playerScore = 0; //Wynik gracza
int playerHunger = 3; //Wskaznik glodu
int moveCounter = 0; //Licznik pomocniczy do przyspieszenia
int currentLane = 2; //Obecna linia, po ktorej porusza sie smok

//Zmienna przechowujaca obecny poziom (start, game, end)
std::string currentLevel = "start";

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

			glUniform1i(glGetUniformLocation(shaderProgram[shader], (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}
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
	//Dane modelu
	std::vector<Mesh> meshes;
	GLchar* texturePath;
	float offsetX = 0.0f;
	//Funkcje
	Model(GLchar* path, GLchar* texturePath) //Kontruktor, podajemy sciezke do modelu i nazwe tekstury
	{
		this->texturePath = texturePath;
		this->loadModel(path);
	}
	void Draw(Shader shader) //Rysuje model - czyli wszystkie jego meshe
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader);
	}
	bool hasTextures; //Sprawdzenie, czy model ma tekstury - do zadania

private:
	//Dane modelu
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
		hasTextures = hasTextureCoords(mesh);
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
			std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, texturePath);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//Specular
			std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, texturePath);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures); //Zwracamy kompletny mesh
	}
	//Sprawdza tekstury wszystkich materialow danego typu i laduje te tekstury, jezeli nie zostaly jeszcze
	//zaladowane
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, GLchar* texturePath)
	{
		std::vector<Texture> textures;
		for (GLuint i = 0; i <= mat->GetTextureCount(type); i++)
		{
			//Jezeli tekstura nie byla zaladowana wczesniej, zaladuje ja teraz
			Texture texture;
			texture.id = TextureFromFile(texturePath); //Wczytanie tekstury, 
			//tekstura musi byc w folderze z modelem, w tym wypadku jest to folder /modele
			if (type == aiTextureType_DIFFUSE)
				texture.type = "texture_diffuse";
			else
				texture.type = "texture_specular";
			texture.path = texturePath;
			textures.push_back(texture);
			this->texturesLoaded.push_back(texture);  //Doda teksture do kontenera zaladowanych tekstur
		}
		return textures; //Zwraca teksture
	}
	bool hasTextureCoords(aiMesh* mesh) //Sprawdzenie, czy model ma tekstury - do zadania
	{
		if (mesh->mTextureCoords[0] == NULL)
			return false;
		else
			return true;
	}
};

GLint TextureFromFile(const char* path)
{
	//Generuje ID tekstury i laduje dane tej tekstury 
	std::string filename = std::string(path);
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

//Deklaracja modeli
Model *dragon;
Model *apple;
Model *background;
Model *startScreen;
Model *endScreen;

irrklang::ISoundEngine *bgSoundEngine = irrklang::createIrrKlangDevice(); //Utworzenie urzadzenia do muzyki w tle
irrklang::ISoundEngine *soundFXEngine = irrklang::createIrrKlangDevice(); //Utworzenie urzadzenia do efektow dzwiekowych

//******************************************************************************************

//Funkcja glowna
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Hidden Dragon");

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

	srand(time(NULL)); //Ustawienie wartosci losowych na podstawie czasu komputera

	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);

	glutKeyboardFunc(keyboard);

	glutTimerFunc(1, mapMovement, 1);

	initGL();

	glutMainLoop();

	return 0;
}

//Funkcja wypisujaca informacje o programie
void printStatus()
{
	system("cls");

	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
	
	//Informacje dodatkowe
	std::cout << "Autorzy: " << std::endl;
	std::cout << "- Paulina Jarosz" << std::endl;
	std::cout << "- Dawid Maslowski" << std::endl << std::endl;
}

//Funkcja inicjujaca OpenGL
void initGL()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	//MODELE
	//Smok
	dragon = new Model("modele/dragon.obj", "modele/dragon.png");

	//Jablko
	apple = new Model("modele/apple.obj", "modele/apple.png");

	//Tlo
	background = new Model("modele/background.obj", "modele/background.png");

	//Ekran poczatkowy i koncowy
	startScreen = new Model("modele/background.obj", "modele/startScreen.png");
	endScreen = new Model("modele/background.obj", "modele/endScreen.png");

	printStatus();
}

//Funkcja wywolywana przy zmianie rozmiaru okna
void changeSize(GLsizei w, GLsizei h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);

	float aspectRatio = (float)w / h;

	projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

//Rysowanie sceny
void renderScene()
{
	glm::vec3 tempApplePosition; //Pomocniczy wektor do obliczen kolizji z monetami

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram[shader]);
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	glm::vec4 lightPos = mvMatrix * lightPosition; // przeliczenie polozenia swiatla do ukladu oka
	glUniform4fv(lightPositionLoc, 1, glm::value_ptr(lightPos));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//Ekran poczatkowy
	if (currentLevel == "start")
	{
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		startScreen->Draw(shader);
	}

	//Ekran z gra
	else if (currentLevel == "game")
	{
		//Tlo
		mvMatrix *= glm::translate(glm::vec3(0.0f, 0.0f, -2.5f));

		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		if (isDead == false)
		{
			glUniform1f(offsetXLoc, background->offsetX);
			background->offsetX += 0.001f; //Do przesuwania tekstury
		}
		else
			glUniform1f(offsetXLoc, background->offsetX);

		background->Draw(shader);

		//Powrot
		mvMatrix *= glm::translate(glm::vec3(0.0f, 0.0f, 2.5f));

		//SMOK
		//Przesuniecie
		mvMatrix *= glm::translate(glm::vec3(-4.0f, trs.y, 0.0f));

		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		glUniform1f(offsetXLoc, 0.0f);

		dragon->Draw(shader);

		//Powrot
		mvMatrix *= glm::translate(glm::vec3(4.0f, -trs.y, 0.0f));

		//JABLKO
		mvMatrix *= glm::translate(glm::vec3(20.0f + appleMoveX, appleMoveY, 0.0f));
		mvMatrix *= glm::translate(glm::vec3(trs.x, 0.0f, 0.0f));

		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		//Zmienne pomocnicze do kolizji jablka
		tempApplePosition.x = 20.0f + appleMoveX + trs.x;
		tempApplePosition.y = appleMoveY;
		tempApplePosition.z = trs.z;

		appleCollision(tempApplePosition); //Kolizje jablek

		if (isDead == false)
			rotation();

		if (appleActive) //Aktywne lub nie
		{
			apple->Draw(shader);
		}

		//Powrot
		if (isDead == false)
			mvMatrix *= glm::rotate(glm::radians(-rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(mvMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

		rotationAngleY += 10.0f;
	}

	//Ekran koncowy
	else if (currentLevel == "end")
	{
		mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

		endScreen->Draw(shader);
		bgSoundEngine->stopAllSounds();
	}

	glutSwapBuffers();
}

//Obsluga klawiatury
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: //ESC - wylaczenie gry
		exit(0);
		break;

	case 32: //Spacja - uruchomienie gry
		if (currentLevel == "start")
		{
			system("cls"); //Czyszczenie konsoli
			currentLevel = "game";
			bgSoundEngine->play2D("audio/bgMusic.mp3", GL_TRUE); //Wybranie utworu, pierwszy parametr - sciezka, drugi - loop
		}
		if (currentLevel == "end")
		{
			system("cls");
			soundFXEngine->stopAllSounds(); //Wylaczenie dzwiekow danego urzadzenia
			isDead = false; //Ustawienie wartosci poczatkowych i pozycji smoka na srodek
			playerScore = 0;
			playerHunger = 3;
			move_step = 0.4f;
			if (currentLane == 1)
				dragonMoveDown(1);
			else if (currentLane == 3)
				dragonMoveUp(1);
			currentLevel = "game";
			bgSoundEngine->play2D("audio/bgMusic.mp3", GL_TRUE);
		}
		break;

	case 'w': //Zmiana linii na wyzsza
		if (isDead == false && currentLane != 1 && isMovingUp == false && isMovingDown == false && currentLevel == "game")
		{
			isMovingUp = true;
			dragonMoveUp(1);
		}
		break;

	case 's': //Zmiana linii na nizsza
		if (isDead == false && currentLane != 3 && isMovingUp == false && isMovingDown == false && currentLevel == "game")
		{
			isMovingDown = true;
			dragonMoveDown(1);
		}
		break;
	}

	glutPostRedisplay();
}

//Funkcja tworzaca shader
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram[TEXTURES]))
		exit(3);
	getUniforms(shader);
}

//Funkcja pobiera i przesyla uniformy dla shadera
void getUniforms(Shader shader)
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

	glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(materialAmbient));

	glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(materialDiffuse));

	glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
	glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(materialSpecular));
	glUniform1f(materialShininessLoc, shininess);

	offsetXLoc = glGetUniformLocation(shaderProgram[shader], "offsetX");
}

//Przemieszczanie sie obiektow na mapie
void mapMovement(int value)
{
	float randomY = 0.0f;

	if (isDead == false && currentLevel == "game") //Dziala tylko gdy smok zyje
	{
		trs.x -= move_step; //Przesuniecie obiektow
		moveCounter++; //Licznik do przyspieszenia
		if (isFirstApple == true)
		{
			appleMoveX = glm::linearRand(LOWER_BORDER_X, UPPER_BORDER_X); //Zmienne do losowania spawnu jablek
			randomY = glm::linearRand(LOWER_BORDER_Y, UPPER_BORDER_Y);
			//Trzy linie do spawnu jablek
			if (randomY >= 0 && randomY < 1)
				appleMoveY = 2.0f;
			else if (randomY >= 1 && randomY < 2)
				appleMoveY = 0.0f;
			else if (randomY >= 2 && randomY <= 3)
				appleMoveY = -2.0f;

			appleActive = true; //Zmienna ustawiajaca jablko na aktywne - mozna je zebrac
			isFirstApple = false;
		}
		if (trs.x < -25.0f && isFirstApple == false) //Warunek powrotu obiektow - nie trzeba tworzyc nowych
		{
			trs.x = 20.0f; //Powrot do poczatku miejsca spawnu obiektow
			appleMoveX = glm::linearRand(LOWER_BORDER_X, UPPER_BORDER_X); //Zmienne do losowania spawnu jablek
			randomY = glm::linearRand(LOWER_BORDER_Y, UPPER_BORDER_Y);
			//Trzy linie do spawnu jablek
			if (randomY >= 0 && randomY < 1)
				appleMoveY = 2.0f;
			else if (randomY >= 1 && randomY < 2)
				appleMoveY = 0.0f;
			else if (randomY >= 2 && randomY <= 3)
				appleMoveY = -2.0f;

			appleActive = true; //Zmienna ustawiajaca jablko na aktywne - mozna je zebrac
		}
		if (moveCounter % 500 == 0) //Warunek do przyspieszenia
		{
			move_step += MOVE_STEP; //Przyspieszenie
			if (move_step >= 1.0f) //Maksymalne przyspieszenie
				move_step = 1.0f;
		}
	}

	glutPostRedisplay();
	
	glutTimerFunc(50, mapMovement, 1);
}

//Zmiana linii na wyzsza
void dragonMoveUp(int value)
{
	if (currentLane == 2) //Sprawdzenie linii, smok nie moze isc w gore gdy jest na samej gorze
		moveHeight = 2.0f;
	else if (currentLane == 3)
		moveHeight = 0.0f;

	if (trs.y < moveHeight && isMovingDown == false) //Plynne przejscie smoka miedzy liniami
	{
		trs.y += DRAGON_MOVE_STEP;
		dragonBotLeftCollider.y += DRAGON_MOVE_STEP;
		dragonBotRightCollider.y += DRAGON_MOVE_STEP;
		dragonTopLeftCollider.y += DRAGON_MOVE_STEP;
		dragonTopRightCollider.y += DRAGON_MOVE_STEP;
	}
	if (trs.y >= moveHeight && isMovingDown == false) //Ustawienie odpowiedniej wspolrzednej dla smoka po zmianie linii
	{
		trs.y = moveHeight;
		dragonBotLeftCollider.y = trs.y;
		dragonBotRightCollider.y = trs.y;
		dragonTopLeftCollider.y = trs.y;
		dragonTopRightCollider.y = trs.y;
		currentLane--;
		if (currentLane < 1)
			currentLane = 1;
		isMovingUp = false;
		return;
	}

	glutTimerFunc(50, dragonMoveUp, 1);
}

//Zmiana linii na nizsza
void dragonMoveDown(int value)
{
	if (currentLane == 2) //Sprawdzenie linii, smok nie moze isc w dol gdy jest na samym dole
		moveHeight = -2.0f;
	else if (currentLane == 1)
		moveHeight = 0.0f;

	if (trs.y > moveHeight && isMovingUp == false) //Plynne przejscie smoka miedzy liniami
	{
		trs.y -= DRAGON_MOVE_STEP;
		dragonBotLeftCollider.y -= DRAGON_MOVE_STEP;
		dragonBotRightCollider.y -= DRAGON_MOVE_STEP;
		dragonTopLeftCollider.y -= DRAGON_MOVE_STEP;
		dragonTopRightCollider.y -= DRAGON_MOVE_STEP;
	}
	if (trs.y <= moveHeight && isMovingUp == false) //Ustawienie odpowiedniej wspolrzednej dla smoka po zmianie linii
	{
		trs.y = moveHeight;
		dragonBotLeftCollider.y = trs.y;
		dragonBotRightCollider.y = trs.y;
		dragonTopLeftCollider.y = trs.y;
		dragonTopRightCollider.y = trs.y;
		currentLane++;
		if (currentLane > 3)
			currentLane = 3;
		isMovingDown = false;
		return;
	}

	glutTimerFunc(50, dragonMoveDown, 1);
}

//Rotacja jablek
void rotation()
{
	mvMatrix *= glm::rotate(glm::radians(rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
}

//Kolizje z jablkami
void appleCollision(glm::vec3 tempPosition)
{
	//Obliczenie wierzcholkow elementu kolizyjnego (box'a)
	glm::vec3 boxTopLeftCollider = glm::vec3(BOX_TOP_LEFT_COLLIDER.x + tempPosition.x, BOX_TOP_LEFT_COLLIDER.y + tempPosition.y, BOX_TOP_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxTopRightCollider = glm::vec3(BOX_TOP_RIGHT_COLLIDER.x + tempPosition.x, BOX_TOP_RIGHT_COLLIDER.y + tempPosition.y, BOX_TOP_RIGHT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotLeftCollider = glm::vec3(BOX_BOT_LEFT_COLLIDER.x + tempPosition.x, BOX_BOT_LEFT_COLLIDER.y + tempPosition.y, BOX_BOT_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotRightCollider = glm::vec3(BOX_BOT_RIGHT_COLLIDER.x + tempPosition.x, BOX_BOT_RIGHT_COLLIDER.y + tempPosition.y, BOX_BOT_RIGHT_COLLIDER.z + tempPosition.z);

	//Sprawdzanie kolizji
	if (boxTopLeftCollider.x >= -5.1f && boxTopRightCollider.x <= -2.5f && appleActive) //Wspolrzedne x
	{
		if (boxTopLeftCollider.y <= 2.5f && boxBotLeftCollider.y >= 1.5f && boxTopRightCollider.y <= 2.5f && boxBotRightCollider.y >= 1.5f) //Wspolrzedne y
		{
			if (currentLane == 1) //Sprawdzenie linii - jezeli smok oraz jablko sa na tej samej linii, smok zje jablko
			{
				system("cls");
				soundFXEngine->play2D("audio/appleCrunch.mp3");
				playerScore += 100;
				playerHunger++;
				if (playerHunger > 3)
					playerHunger = 3;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
			else //Jezeli natomiast sa na roznych liniach, smok zrobi sie glodny
			{
				system("cls");
				playerHunger--;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
		}
		else if (boxTopLeftCollider.y < 1.5f && boxBotLeftCollider.y > -1.5f && boxTopRightCollider.y < 1.5f && boxBotRightCollider.y > -1.5f)
		{
			if (currentLane == 2)
			{
				system("cls");
				soundFXEngine->play2D("audio/appleCrunch.mp3");
				playerScore += 100;
				playerHunger++;
				if (playerHunger > 3)
					playerHunger = 3;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
			else
			{
				system("cls");
				playerHunger--;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
		}
		else if (boxTopLeftCollider.y <= -1.5f && boxBotLeftCollider.y >= -2.5f && boxTopRightCollider.y <= -1.5f && boxBotRightCollider.y >= -2.5f)
		{
			if (currentLane == 3)
			{
				system("cls");
				soundFXEngine->play2D("audio/appleCrunch.mp3");
				playerScore += 100;
				playerHunger++;
				if (playerHunger > 3)
					playerHunger = 3;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
			else
			{
				system("cls");
				playerHunger--;
				std::cout << "Glod: " << playerHunger << std::endl << std::endl;
				std::cout << "Wynik: " << playerScore << std::endl << std::endl;
				appleActive = false;
			}
		}
	}

	//Gdy smok nie zje trzech jablek z rzedu umrze z glodu
	if (playerHunger == 0)
	{
		isDead = true;
		soundFXEngine->play2D("audio/end.mp3", GL_TRUE);
		currentLevel = "end";
		system("cls");
		std::cout << std::endl << "Wynik: " << playerScore << std::endl;
	}
}