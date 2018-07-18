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

#include "shaders.h"

const int WIDTH = 1024;
const int HEIGHT = 576;

const float ROT_STEP = 10.0f;
const float SCALE_STEP = 0.25f;
const float JUMP_STEP = 0.5f;
const float MOVE_STEP = 0.1f;
const int NUM_SHADERS = 1;

//Wspolrzedne wierzcholkow tworzacych boxa - collidera dla krowy
const glm::vec3 COW_TOP_LEFT_COLLIDER = glm::vec3(-5.1f, -1.0f, 0.0f);
const glm::vec3 COW_TOP_RIGHT_COLLIDER = glm::vec3(-2.5f, -1.0f, 0.0f);
const glm::vec3 COW_BOT_LEFT_COLLIDER = glm::vec3(-5.1f, -2.5f, 0.0f);
const glm::vec3 COW_BOT_RIGHT_COLLIDER = glm::vec3(-2.5f, -2.5f, 0.0f);

//Collidery dla przeszkody
const glm::vec3 BOX_TOP_LEFT_COLLIDER = glm::vec3(-0.125f, 0.125f, 0.0f);
const glm::vec3 BOX_TOP_RIGHT_COLLIDER = glm::vec3(0.125f, 0.125f, 0.0f);
const glm::vec3 BOX_BOT_LEFT_COLLIDER = glm::vec3(-0.125f, -0.125f, 0.0f);
const glm::vec3 BOX_BOT_RIGHT_COLLIDER = glm::vec3(0.125f, -0.125f, 0.0f);

//Stale okreslajace granice randomow
const float LOWER_BORDER_X = -20.0f;
const float UPPER_BORDER_X = 20.0f;
const float LOWER_BORDER_Y = -2.0f;
const float UPPER_BORDER_Y = 0.0f;


enum Shader { WITHTEXTURESMOVING };

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
void mapMovement(int value); //Timer - dziala caly czas
void cowJump(int value); //Timer - wywolywany podczas skoku
void cowSlide(int value); //Timer - wywolywany podczas slizgu
void cowAlive(int value); //Timer - do animacji ruchu krowy
void coinCollision(glm::vec3 tempPosition); //Kolizje z monetami
void boxCollision(glm::vec3 tempPosition); //Kolizje z przeszkodami
void rotation(int value); //Obrot obiektow

Shader shader = WITHTEXTURESMOVING; // aktualny program cieniowania

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
glm::vec4 lightPosition = glm::vec4(0.0f, 20.0f, 25.0f, 1.0f); // pozycja w ukladzie swiata
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

// material obiektu
glm::vec3 materialAmbient = glm::vec3(1.0f, 0.5f, 0.0f);
glm::vec3 materialDiffuse = glm::vec3(0.34615f, 0.3143f, 0.0903f);
glm::vec3 materialSpecular = glm::vec3(0.797357, 0.723991, 0.208006);
float shininess = 32.0f;

glm::vec3 trs = glm::vec3(0.0, 0.0, 0.0); //Przesuniecie
glm::vec3 jmp = glm::vec3(0.0, 0.0, 0.0); //Skok

//ZMIENNE
bool isJumping = false; //Czy krowa skacze
bool isAtPeak = false; //Czy krowa jest w najwyzszym punkcie skoku
bool isSliding = false; //Czy krowa robi slizg
bool isDead = false; //Czy krowa jest martwa
bool coinActive = true; //Czy moneta jest aktywna (mozliwa do zebrania)

float move_step = 0.4f; //Predkosc przemieszczania sie
float rotationAngleZ = 0.0f; //Kat obrotu wokol osi Z dla obiektu
float rotationAngleY = 0.0f; //Kat obrotu wokol osi Y dla obiektu
float jumpHeight = -1.5f; //Wysokosc skoku
float slideHeight = 0.5f; //Wysokosc slizgu
float shurikenMoveX, shurikenMoveY, coinMoveX, coinMoveY; //Zmienne, do ktorych ladowane sa wartosci losowe
int playerScore = 0; //Wynik gracza
int moveCounter = 0; //Licznik pomocniczy do przyspieszenia
int slideCounter = 0; //Licznik pomocniczy do slizgu

//Zmienna przechowujaca nazwe obecnego modelu
std::string currentModel = "";

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
Model *cowMove1Model;
Model *cowMove2Model;
Model *cowSlideModel;
Model *cowJumpModel;
Model *cowDeadModel;
Model *shuriken;
Model *coin;
Model *coinPickedUp;
Model *mapFloor;
Model *background;

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
	glutCreateWindow("OpenGL (Core Profile) - Cow Runaway");

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
	std::cout << "- Radoslaw Slipko" << std::endl;
	std::cout << "- Radoslaw Botwina" << std::endl << std::endl;
}

//Funkcja inicjujaca OpenGL
void initGL()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	//MODELE
	//Krowa
	cowMove1Model = new Model("modele/cowMove1.obj", "modele/cow.png");
	cowMove2Model = new Model("modele/cowMove2.obj", "modele/cow.png");
	cowSlideModel = new Model("modele/cowSlide.obj", "modele/cow.png");
	cowJumpModel = new Model("modele/cowJump.obj", "modele/cow.png");
	cowDeadModel = new Model("modele/cowDead.obj", "modele/cow.png");
	currentModel = "Move1";

	//Shuriken
	shuriken = new Model("modele/shuriken.obj", "modele/shuriken.png");
	
	//Moneta
	coin = new Model("modele/coin.obj", "modele/coin.png");
	coinPickedUp = new Model("", "");

	//Droga
	mapFloor = new Model("modele/ground.obj", "modele/ground.png");

	//Tlo
	background = new Model("modele/background.obj", "modele/background.png");

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
	glm::vec3 tempBoxPosition; //Pomocniczy wektor do obliczen kolizji z shurikenem
	glm::vec3 tempCoinPosition; //Pomocniczy wektor do obliczen kolizji z monetami

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram[shader]);
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	glm::vec4 lightPos = mvMatrix * lightPosition; // przeliczenie polozenia swiatla do ukladu oka
	glUniform4fv(lightPositionLoc, 1, glm::value_ptr(lightPos));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Tlo
	mvMatrix *= glm::translate(glm::vec3(0.0f, 1.4f + jmp.y, -2.5f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	
	if (isDead == false)
	{
		glUniform1f(offsetXLoc, background->offsetX);
		background->offsetX += 0.001f;
	}
	else
		glUniform1f(offsetXLoc, background->offsetX);

	background->Draw(shader);

	//Powrot
	mvMatrix *= glm::translate(glm::vec3(0.0f, -1.4f - jmp.y, 2.5f));

	//KROWA
	//Przesuniecie
	mvMatrix *= glm::translate(glm::vec3(-4.0f, -1.5f, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	glUniform1f(offsetXLoc, 0.0f);

	if (isDead == false) //Stan krowy - zywa/martwa
		cowAlive(1);
	else
		cowDeadModel->Draw(shader);

	//Powrot
	mvMatrix *= glm::translate(glm::vec3(4.0f, 1.5f, 0.0f));

	//DROGA
	mvMatrix *= glm::translate(glm::vec3(0.0f, -2.5f + jmp.y, 0.0f)); //jmp.y potrzebny do skoku krowy

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	if (isDead == false)
	{
		glUniform1f(offsetXLoc, mapFloor->offsetX);
		mapFloor->offsetX += 0.001f * 50.0f / 16.0f;
	}
	else
		glUniform1f(offsetXLoc, mapFloor->offsetX);

	mapFloor->Draw(shader);

	//Powrot
	mvMatrix *= glm::translate(glm::vec3(0.0f, 2.5f, 0.0f));

	//SHURIKEN
	mvMatrix *= glm::translate(glm::vec3(20.0f + shurikenMoveX, shurikenMoveY, 0.0f));

	//Przesuniecie w zaleznosci od stanu krowy (skok/slizg)
	mvMatrix *= glm::translate(glm::vec3(trs.x, trs.y + jmp.y, trs.z));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	
	glUniform1f(offsetXLoc, 0.0f);

	//Zmienne pomocnicze do kolizji shurikena
	tempBoxPosition.x = 20.0f + shurikenMoveX + trs.x;
	tempBoxPosition.y = shurikenMoveY + jmp.y + trs.y;
	tempBoxPosition.z = trs.z;
	
	boxCollision(tempBoxPosition); //Kolizje shurikena
	if(isDead == false)
		rotation(1); //Obrot shurikena
	shuriken->Draw(shader); //Rysowanie

	//Powrot
	if(isDead == false)
		mvMatrix *= glm::rotate(glm::radians(-rotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::translate(glm::vec3(-shurikenMoveX, -shurikenMoveY, 0.0f));

	//COIN
	mvMatrix *= glm::translate(glm::vec3(coinMoveX, coinMoveY, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	//Zmienne pomocnicze do kolizji monety
	tempCoinPosition.x = 20.0f + coinMoveX + trs.x;
	tempCoinPosition.y = coinMoveY + trs.y + jmp.y;
	tempCoinPosition.z = trs.z;
	
	coinCollision(tempCoinPosition); //Kolizje monety
	if(isDead == false)
		rotation(2); //Obrot monety
	if (coinActive) //Aktywna lub nie
		coin->Draw(shader);
	else
		coinPickedUp->Draw(shader);

	//Powrot
	if(isDead == false)
		mvMatrix *= glm::rotate(glm::radians(-rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(mvMatrix));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

	//Katy obrotu dla obiektow
	rotationAngleZ += 20.0f;
	rotationAngleY += 10.0f;

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

	case 'w': //Skok
		if (isDead == false)
		{
			if (isJumping == false)
			{
				currentModel = "Jump";
				cowJump(1);
			}
		}
		break;

	case 's': //Slizg
		if (isDead == false)
		{
			if (isSliding == false)
			{
				currentModel = "Slide";
				slideCounter = 0;
				cowSlide(1);
			}
		}
		break;
	}

	glutPostRedisplay();
}

//Funkcja tworzaca shader
void setupShaders()
{
	if (!setupShaders("shaders/withTexturesMoving.vert", "shaders/withTexturesMoving.frag", shaderProgram[WITHTEXTURESMOVING]))
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
	if (isDead == false) //Dziala tylko gdy krowa zyje
	{
		trs.x -= move_step; //Przesuniecie obiektow
		moveCounter++; //Licznik do przyspieszenia
		if (trs.x < -40.0f) //Warunek powrotu obiektow - nie trzeba tworzyc nowych
		{
			trs.x = 0; //Powrot do poczatku miejsca spawnu obiektow
			shurikenMoveX = glm::linearRand(LOWER_BORDER_X, UPPER_BORDER_X); //Cztery zmienne do losowania
			shurikenMoveY = glm::linearRand(LOWER_BORDER_Y, UPPER_BORDER_Y); //przesuniec obiektow
			coinMoveX = glm::linearRand(LOWER_BORDER_X, UPPER_BORDER_X);
			coinMoveY = glm::linearRand(LOWER_BORDER_Y, UPPER_BORDER_Y);
			coinActive = true; //Zmienna ustawiajaca monete na aktywna - mozna ja zebrac
		}
		if (moveCounter % 100 == 0) //Warunek do przyspieszenia
		{
			move_step += MOVE_STEP; //Przyspieszenie
			if (move_step >= 1.5f) //Maksymalne przyspieszenie
				move_step = 1.5f;
		}
	}

	glutPostRedisplay();
	
	glutTimerFunc(50, mapMovement, 1);
}

//Skok krowy
void cowJump(int value)
{
	if (jmp.y <= 0.0f && jmp.y > jumpHeight && isAtPeak == false)
	{
		isJumping = true;
		jmp.y -= JUMP_STEP / 4;
		if (jmp.y == jumpHeight)
		{
			jmp.y = jumpHeight;
			isAtPeak = true;
		}
	}
	else if (isAtPeak == true)
		jmp.y += JUMP_STEP / 4;

	if (jmp.y == 0.0f)
	{
		currentModel = "Move1";
		isAtPeak = false;
		isJumping = false;
		return;
	}

	glutTimerFunc(50, cowJump, 1);
}

//Slizg krowy
void cowSlide(int value)
{
	if (trs.y >= 0.0f && slideCounter < 10)
	{
		trs.y = slideHeight;
		slideCounter++;
		isSliding = true;
	}

	if (slideCounter == 10 && isSliding == true)
	{
		currentModel = "Move1";
		trs.y = 0.0f;
		isSliding = false;
		return;
	}

	glutTimerFunc(50, cowSlide, 1);
}

//Wszystkie statusy (modele) zywej krowy
void cowAlive(int value)
{
	if (currentModel == "Move1")
	{
		cowMove1Model->Draw(shader);
		currentModel = "Move2";
	}
	else if (currentModel == "Move2")
	{
		cowMove2Model->Draw(shader);
		currentModel = "Move1";
	}
	else if (currentModel == "Slide")
	{
		cowSlideModel->Draw(shader);
	}
	else if (currentModel == "Jump")
	{
		cowJumpModel->Draw(shader);
	}
	
	glutTimerFunc(50, cowAlive, 1);
}

//Rotacja obiektow (dla shurikena i monety)
void rotation(int value)
{
	if (value == 1)
	{
		mvMatrix *= glm::rotate(glm::radians(rotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	}
	else if (value == 2)
	{
		mvMatrix *= glm::rotate(glm::radians(rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	}
}

//Kolizje z monetami
void coinCollision(glm::vec3 tempPosition)
{
	//Poszczegolne wierzcholki box'a jako collidera
	glm::vec3 boxTopLeftCollider = glm::vec3(BOX_TOP_LEFT_COLLIDER.x + tempPosition.x, BOX_TOP_LEFT_COLLIDER.y + tempPosition.y, BOX_TOP_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxTopRightCollider = glm::vec3(BOX_TOP_RIGHT_COLLIDER.x + tempPosition.x, BOX_TOP_RIGHT_COLLIDER.y + tempPosition.y, BOX_TOP_RIGHT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotLeftCollider = glm::vec3(BOX_BOT_LEFT_COLLIDER.x + tempPosition.x, BOX_BOT_LEFT_COLLIDER.y + tempPosition.y, BOX_BOT_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotRightCollider = glm::vec3(BOX_BOT_RIGHT_COLLIDER.x + tempPosition.x, BOX_BOT_RIGHT_COLLIDER.y + tempPosition.y, BOX_BOT_RIGHT_COLLIDER.z + tempPosition.z);

	//Z perspektywy krowy
	bool collisionBotRight = (boxTopLeftCollider.x <= COW_BOT_RIGHT_COLLIDER.x && boxTopLeftCollider.x >= COW_BOT_LEFT_COLLIDER.x) && (boxTopLeftCollider.y >= COW_BOT_RIGHT_COLLIDER.y && boxTopLeftCollider.y <= COW_TOP_RIGHT_COLLIDER.y);
	bool collisionTopRight = (boxBotLeftCollider.x <= COW_TOP_RIGHT_COLLIDER.x && boxBotLeftCollider.x >= COW_TOP_LEFT_COLLIDER.x) && (boxBotLeftCollider.y >= COW_BOT_RIGHT_COLLIDER.y && boxBotLeftCollider.y <= COW_TOP_RIGHT_COLLIDER.y);
	bool collisionTopLeft = (boxBotRightCollider.x >= COW_TOP_LEFT_COLLIDER.x && boxBotRightCollider.x <= COW_TOP_RIGHT_COLLIDER.x) && (boxBotRightCollider.y <= COW_TOP_LEFT_COLLIDER.y && boxBotRightCollider.y >= COW_BOT_LEFT_COLLIDER.y);
	bool collisionBotLeft = (boxTopRightCollider.x >= COW_BOT_LEFT_COLLIDER.x && boxTopRightCollider.x <= COW_BOT_RIGHT_COLLIDER.x) && (boxTopRightCollider.y >= COW_BOT_LEFT_COLLIDER.y && boxTopRightCollider.y <= COW_TOP_LEFT_COLLIDER.y);
	
	if (collisionBotRight && collisionBotLeft && collisionTopRight && collisionTopLeft && isDead == false)
	{
		if (coinActive == true)
		{
			playerScore += 100;
			coinActive = false;
		}
	}
}

//Kolizje z shurikenem
void boxCollision(glm::vec3 tempPosition)
{
	//Poszczegolne wierzcholki box'a jako collidera
	glm::vec3 boxTopLeftCollider = glm::vec3(BOX_TOP_LEFT_COLLIDER.x + tempPosition.x, BOX_TOP_LEFT_COLLIDER.y + tempPosition.y, BOX_TOP_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxTopRightCollider = glm::vec3(BOX_TOP_RIGHT_COLLIDER.x + tempPosition.x, BOX_TOP_RIGHT_COLLIDER.y + tempPosition.y, BOX_TOP_RIGHT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotLeftCollider = glm::vec3(BOX_BOT_LEFT_COLLIDER.x + tempPosition.x, BOX_BOT_LEFT_COLLIDER.y + tempPosition.y, BOX_BOT_LEFT_COLLIDER.z + tempPosition.z);
	glm::vec3 boxBotRightCollider = glm::vec3(BOX_BOT_RIGHT_COLLIDER.x + tempPosition.x, BOX_BOT_RIGHT_COLLIDER.y + tempPosition.y, BOX_BOT_RIGHT_COLLIDER.z + tempPosition.z);
	
	//Z perspektywy krowy
	bool collisionBotRight = (boxTopLeftCollider.x <= COW_BOT_RIGHT_COLLIDER.x && boxTopLeftCollider.x >= COW_BOT_LEFT_COLLIDER.x) && (boxTopLeftCollider.y >= COW_BOT_RIGHT_COLLIDER.y && boxTopLeftCollider.y <= COW_TOP_RIGHT_COLLIDER.y);
	bool collisionTopRight = (boxBotLeftCollider.x <= COW_TOP_RIGHT_COLLIDER.x && boxBotLeftCollider.x >= COW_TOP_LEFT_COLLIDER.x) && (boxBotLeftCollider.y >= COW_BOT_RIGHT_COLLIDER.y && boxBotLeftCollider.y <= COW_TOP_RIGHT_COLLIDER.y);
	bool collisionTopLeft = (boxBotRightCollider.x >= COW_TOP_LEFT_COLLIDER.x && boxBotRightCollider.x <= COW_TOP_RIGHT_COLLIDER.x) && (boxBotRightCollider.y <= COW_TOP_LEFT_COLLIDER.y && boxBotRightCollider.y >= COW_BOT_LEFT_COLLIDER.y);
	bool collisionBotLeft = (boxTopRightCollider.x >= COW_BOT_LEFT_COLLIDER.x && boxTopRightCollider.x <= COW_BOT_RIGHT_COLLIDER.x) && (boxTopRightCollider.y >= COW_BOT_LEFT_COLLIDER.y && boxTopRightCollider.y <= COW_TOP_LEFT_COLLIDER.y);

	if (collisionBotRight && collisionBotLeft && collisionTopRight && collisionTopLeft && isDead == false)
	{
		isDead = true;
		std::cout << "Wynik gracza: " << playerScore << " pkt.";
	}
}