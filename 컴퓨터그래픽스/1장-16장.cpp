#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;
enum type {
	DICE = 0,
	Pyramid,
	both
};
void ReadObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& normalVertex, std::vector<glm::vec2>& vtVertex)
{
	int lineCount = 0;
	std::string line;
	std::string check[6];
	int vertexNum = 0;
	int normalNum = 0;
	int cordiNum = 0;
	std::ifstream inFile(objfilename);
	std::vector<glm::vec4>face;
	std::vector<glm::vec4>Noramlface;
	std::vector<glm::vec4>vtface;
	while (std::getline(inFile, line)) {
		if (line[0] == 'v' && line[1] == ' ') {
			vertexNum++;
		}
		if (line[0] == 'v' && line[1] == 'n') {
			normalNum++;
		}
		if (line[0] == 'v' && line[1] == 't') {
			cordiNum++;
		}
		std::cout << line << std::endl;
	}
	glm::vec4* vertexData = new glm::vec4[vertexNum];
	glm::vec4* normalData = new glm::vec4[normalNum];
	glm::vec2* cordinaterData = new glm::vec2[cordiNum];

	inFile.clear();
	inFile.seekg(0, std::ios::beg);
	vertexNum = 0;
	normalNum = 0;
	cordiNum = 0;
	char head[2];
	int faceNum[3];
	int vnNum[3];
	int vtNum[3];
	std::string nt;
	char n;
	char s;
	while (inFile >> std::noskipws >> head[0]) { //빈칸을 안 씹겠다
		if (head[0] == 'v') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> vertexData[vertexNum].x >> vertexData[vertexNum].y >> vertexData[vertexNum].z;
				vertexNum++;
			}
			else if (head[1] == 'n') {
				inFile >> std::skipws >> normalData[normalNum].x >> normalData[normalNum].y >> normalData[normalNum].z;
				normalNum++;
			}
			else if (head[1] == 't') {
				float trash;
				inFile >> std::skipws >> cordinaterData[cordiNum].x >> cordinaterData[cordiNum].y >> trash;
				cordiNum++;
			}
			head[1] = '\0';
		}
		if (head[0] == 'f') {
			inFile >> std::noskipws >> head[1];
			if (head[1] == ' ') {
				inFile >> std::skipws >> faceNum[0];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[0];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[0];


				inFile >> std::skipws >> faceNum[1];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[1];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[1];


				inFile >> std::skipws >> faceNum[2];
				inFile >> std::skipws >> n;
				inFile >> std::skipws >> vtNum[2];
				inFile >> std::skipws >> s;
				inFile >> std::skipws >> vnNum[2];

				glm::vec4 temp = { faceNum[0], faceNum[1], faceNum[2], 1 };//faceNum[3]
				glm::vec4 vttemp = { vtNum[0], vtNum[1], vtNum[2], 1 }; //vtNum[3]
				glm::vec4 vntemp = { vnNum[0], vnNum[1], vnNum[2], 1 };//vnNum[3]
				face.push_back(temp);
				vtface.push_back(vttemp);
				Noramlface.push_back(vntemp);
			}
			head[1] = '\0';
		}
	}
	for (auto iter = face.begin(); iter < face.end(); iter++) {
		vertex.push_back(vertexData[(int)(iter->x) - 1]);
		vertex.push_back(vertexData[(int)(iter->y) - 1]);
		vertex.push_back(vertexData[(int)(iter->z) - 1]);                     //버텍스 좌표
	}
	for (auto iter = vtface.begin(); iter < vtface.end(); iter++) {
		vtVertex.push_back(cordinaterData[(int)(iter->x) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->y) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->z) - 1]);                //텍스쳐 좌표
	}

	for (auto iter = Noramlface.begin(); iter < Noramlface.end(); iter++) {
		normalVertex.push_back(normalData[(int)(iter->x) - 1]);
		normalVertex.push_back(normalData[(int)(iter->y) - 1]);
		normalVertex.push_back(normalData[(int)(iter->z) - 1]);               //노멀 좌표
	}
	delete[] vertexData;
	delete[] cordinaterData;
	delete[] normalData;
	inFile.close();
}
char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}
bool change = FALSE;
float WIDTH = 800;
float HEIGHT = 600;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Timer(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };

std::vector <glm::vec3> D_vertex;
std::vector <glm::vec3> D_normal;
std::vector <glm::vec2> D_uv;
std::vector <glm::vec3> P_vertex;
std::vector <glm::vec3> P_normal;
std::vector <glm::vec2> P_uv;
std::vector <glm::vec3> S_vertex;
std::vector <glm::vec3> S_normal;
std::vector <glm::vec2> S_uv;
std::vector <glm::vec3> C_vertex;
std::vector <glm::vec3> C_normal;
std::vector <glm::vec2> C_uv;


pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}


GLvoid Timer(int value);
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 Line_R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 Line_S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1; color[0].g = 0; color[0].b = 0;
		color[1].r = 0; color[1].g = 1; color[1].b = 0;
		color[2].r = 0; color[2].g = 0; color[2].b = 1;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(POS), POS, GL_STATIC_DRAW);
		Line_R_matrix = glm::rotate(Line_R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));//y축 20도
		Line_R_matrix = glm::rotate(Line_R_matrix, glm::radians(20.0f), glm::vec3(1.0f, 0, 0));//x축 20도
		
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(Line_R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Line_S_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));
		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
		}
	}

};
struct Dice {
	bool button_x = FALSE; bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	bool rotate[2] = { FALSE,FALSE };
	vec3 color[6];
	GLuint m_vbo;
	float rotate_x = 2.0f;
	float rotate_y = 2.0f;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Dice(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);

		for (int i = 0; i < 6; ++i)
		{
			color[i].r = urd(dre);
			color[i].g = urd(dre);
			color[i].b = urd(dre);
		}

		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));
		
		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
		T_matrix = glm::translate(T_matrix, glm::vec3(-0.5, 0.0, 0.0));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));

		for (int i = 0; i < 6; ++i) {
			//glEnable(GL_CULL_FACE);
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_TRIANGLES, 6 * i, 6);
		}
	}
};
struct PYRAMID {
	bool button_x = FALSE;  bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	vec3 color[6];
	float rotate_x = 2.0f;
	float rotate_y = 2.0f;
	bool rotate[2] = { FALSE,FALSE };

	GLuint m_vbo;
	bool Dodraw = FALSE;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	PYRAMID(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color[0].r = 0; color[0].g = 1; color[0].b = 0;
		color[1].r = 0; color[1].g = 0; color[1].b = 1;
		color[2].r = 1; color[2].g = 1; color[2].b = 0;
		color[3].r = 0; color[3].g = 1; color[3].b = 1;
		color[4].r = 1; color[4].g = 0; color[4].b = 0;
		color[5].r = 1; color[5].g = 0; color[5].b = 0;

		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));
		
		T_matrix = glm::translate(T_matrix, glm::vec3(0.5, 0.0, 0.0));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * P_vertex.size(), &P_vertex[0], GL_STATIC_DRAW);
	}
	void Draw() {

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		//--- modelTransform 변수에 변환 값 적용하기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));

		for (int i = 0; i < 6; ++i) {
			//glEnable(GL_CULL_FACE);
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			
			glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			

		}
	}

};
struct Sphere {
	bool button_x = FALSE; bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	bool rotate[2] = { FALSE,FALSE };
	GLuint m_vbo;
	float rotate_x = 2.0f;
	float rotate_y = 2.0f;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Sphere(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));

		S_matrix = glm::scale(S_matrix, glm::vec3(0.05, 0.05, 0.05));
		T_matrix = glm::translate(T_matrix, glm::vec3(-0.5, 0.0, 0.0));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * S_vertex.size(), &S_vertex[0], GL_STATIC_DRAW);

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));

		for (int i = 0; i < 120; ++i) {
			//glEnable(GL_CULL_FACE);
			glUniform3f(colorUniformLoc, 1, 0, 0);
			glDrawArrays(GL_TRIANGLES, 3*i, 3);
		}
	}
};
struct Circone {
	bool button_x = FALSE; bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	bool rotate[2] = { FALSE,FALSE };
	GLuint m_vbo;
	float rotate_x = 2.0f;
	float rotate_y = 2.0f;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Circone(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));

		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.5, 0.0, 0.0));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * C_vertex.size(), &C_vertex[0], GL_STATIC_DRAW);

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));

		for (int i = 0; i < C_vertex.size() / 3; ++i) {
			//glEnable(GL_CULL_FACE);
			glUniform3f(colorUniformLoc, 0, 1, 0);
			glDrawArrays(GL_TRIANGLES, 3 * i, 3);
		}
	}
};
vector <Dice> dice;
vector <Sector> Line;
vector <PYRAMID> pyramid;
vector <Sphere> sphere;
vector <Circone> circone;

int selectNum = -1;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정

	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	ReadObj("pyramid.obj", P_vertex, P_normal, P_uv);
	ReadObj("sphere.obj", S_vertex, S_normal, S_uv);
	ReadObj("cone.obj", C_vertex, C_normal, C_uv);
	dice.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	pyramid.emplace_back(400, 300);
	sphere.emplace_back(400, 300);

	circone.emplace_back(400, 300);
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(20, Timer, 1);

	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
void drawScene()
{
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sphere[0].T_matrix = dice[0].T_matrix;
	sphere[0].R_matrix = dice[0].R_matrix;
	sphere[0].rot_matrix = dice[0].rot_matrix;
	circone[0].T_matrix = pyramid[0].T_matrix;
	circone[0].R_matrix = pyramid[0].R_matrix;
	circone[0].rot_matrix = pyramid[0].rot_matrix;
	glUseProgram(shaderProgramID);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	Line[0].Draw();
	if (!change) {
		dice[0].Draw();
		pyramid[0].Draw();
	}
	else {
		sphere[0].Draw();
		circone[0].Draw();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //화면에 출력
}
GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	
	switch (key)
	{
	case '1':
		if (dice[0].T_matrix[3].x < pyramid[0].T_matrix[3].x)
		{
			selectNum = DICE;
			pyramid[0].button_x = FALSE;
			pyramid[0].button_X = FALSE;
			pyramid[0].button_y = FALSE;
			pyramid[0].button_Y = FALSE;
		}
		else
		{
			selectNum = Pyramid;
			dice[0].button_x = FALSE;
			dice[0].button_X = FALSE;
			dice[0].button_y = FALSE;
			dice[0].button_Y = FALSE;
		}
		break;
	case '2':
		if (dice[0].T_matrix[3].x < pyramid[0].T_matrix[3].x)
		{
			selectNum = Pyramid;
			dice[0].button_x = FALSE;
			dice[0].button_X = FALSE;
			dice[0].button_y = FALSE;
			dice[0].button_Y = FALSE;
		}
		else
		{
			selectNum = DICE;
			pyramid[0].button_x = FALSE;
			pyramid[0].button_X = FALSE;
			pyramid[0].button_y = FALSE;
			pyramid[0].button_Y = FALSE;
		}
		break;
	case '3':
		selectNum = both;
		dice[0].button_x = FALSE; pyramid[0].button_x = FALSE;
		dice[0].button_X = FALSE; pyramid[0].button_X = FALSE;
		dice[0].button_y = FALSE; pyramid[0].button_y = FALSE;
		dice[0].button_Y = FALSE; pyramid[0].button_Y = FALSE;
		break;
	case 'x':
		
			dice[0].button_x = !dice[0].button_x;
			dice[0].button_X = FALSE;
			pyramid[0].button_x = !pyramid[0].button_x;
			pyramid[0].button_X = FALSE;
		
		break;

	case 'X':
			dice[0].button_X = !dice[0].button_X;
			dice[0].button_x = FALSE;
			pyramid[0].button_X = !pyramid[0].button_X;
			pyramid[0].button_x = FALSE;
		
		break;
	case 'y':
		dice[0].button_y = !dice[0].button_y;
		dice[0].button_Y= FALSE;
		pyramid[0].button_y = !pyramid[0].button_y;
		pyramid[0].button_Y = FALSE;
		break;
	case 'Y':
		dice[0].button_Y = !dice[0].button_Y;
		dice[0].button_y = FALSE;
		pyramid[0].button_Y = !pyramid[0].button_Y;
		pyramid[0].button_y = FALSE;
		break;
	
	
	case 'r':
		dice[0].rotate[0] = !dice[0].rotate[0];
		pyramid[0].rotate[0] = !pyramid[0].rotate[0];
		dice[0].rotate[1] = FALSE;
		pyramid[0].rotate[1] = FALSE;
		
		break;
	case 'R':
		dice[0].rotate[0] = FALSE;
		pyramid[0].rotate[0] = FALSE;
		dice[0].rotate[1] = !dice[0].rotate[1];
		pyramid[0].rotate[1] = !pyramid[0].rotate[1];
		break;


	case 's':
		for (int i = 0; i < 2; ++i) {
			dice[0].rotate[i] = FALSE;
			pyramid[0].rotate[i] = FALSE;
			}
			dice[0].button_y = FALSE; dice[0].button_Y = FALSE;
			dice[0].R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
			dice[0].T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
			dice[0].rot_matrix = glm::mat4(1.0f);
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-0.5, 0.0, 0.0));

			pyramid[0].button_y = FALSE; pyramid[0].button_Y = FALSE;
			pyramid[0].R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
			pyramid[0].T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
			pyramid[0].rot_matrix = glm::mat4(1.0f);
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(20.0f), glm::vec3(0, 1, 0));
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(20.0f), glm::vec3(1, 0, 0));
			pyramid[0].T_matrix = glm::translate(pyramid[0].T_matrix, glm::vec3(0.5, 0.0, 0.0));
			change = FALSE;
			break;
	case 'c':
		change = !change;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Timer(int value)
{
	if (selectNum == DICE || selectNum == both) {
		if (dice[0].button_x) {
			dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(dice[0].rotate_x), glm::vec3(1, 0, 0));
		}
		else if (dice[0].button_X)
		{
			dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(-dice[0].rotate_x), glm::vec3(1, 0, 0));
		}
		if (dice[0].button_y) {
			dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 1));
		}
		else if (dice[0].button_Y)
		{
			dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(-dice[0].rotate_y), glm::vec3(0, 1, 1));
		}
	}
	if (selectNum == Pyramid || selectNum == both) {
		if (pyramid[0].button_x) {
			pyramid[0].rot_matrix = glm::rotate(pyramid[0].rot_matrix, glm::radians(pyramid[0].rotate_x), glm::vec3(1, 0, 0));
		}
		else if (pyramid[0].button_X)
		{
			pyramid[0].rot_matrix = glm::rotate(pyramid[0].rot_matrix, glm::radians(-pyramid[0].rotate_x), glm::vec3(1, 0, 0));
		}
		if (pyramid[0].button_y) {

			pyramid[0].rot_matrix = glm::rotate(pyramid[0].rot_matrix, glm::radians(pyramid[0].rotate_y), glm::vec3(0, 1, 1));
		}
		else if (pyramid[0].button_Y)
		{
			pyramid[0].rot_matrix = glm::rotate(pyramid[0].rot_matrix, glm::radians(-pyramid[0].rotate_y), glm::vec3(0, 1, 1));
		}
	}

	if (dice[0].rotate[0])
	{
		dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 0));//y축 -30도
	}
	else if (dice[0].rotate[1])
	{
		dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(-dice[0].rotate_y), glm::vec3(0, 1, 0));//y축 -30도

	}
	if (pyramid[0].rotate[0])
	{
		pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(pyramid[0].rotate_y), glm::vec3(0, 1, 0));//y축 -30도
	}
	else if (pyramid[0].rotate[1])
	{
		pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(-pyramid[0].rotate_y), glm::vec3(0, 1, 0));//y축 -30도

	}

	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex2.glsl");

	//vertex shader 생성 해주고 생성한 데이터를 vertexShader라는 변수에 저장
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//성공하면 0이 아닌값

	//세이더 객체, 1, 세이더 소스 코드, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, 문자열들의 갯수, 문자열들의 배열, 문자열들의 길이를 저장할 int배열

	//컴파일 해야지요.
	glCompileShader(vertexShader);

	//세이더 결과 어떻게 됐는지? 상태 확인하는 코드
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment2.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}

GLuint make_shaderProgram() {
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

