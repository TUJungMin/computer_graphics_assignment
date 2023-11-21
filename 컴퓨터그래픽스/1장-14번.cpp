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

float WIDTH = 800;
float HEIGHT = 600;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid TimerFunc(int value);
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

char slide = 'm';

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

GLvoid TimerFunc(int value);
GLvoid Timer(int value);
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 Line_R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 Line_S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
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
		//--- modelTransform 변수에 변환 값 적용하기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Line_S_matrix));
		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
		}
	}

};
struct Dice {
	vec3 color[6];
	GLuint m_vbo;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
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
		R_matrix = glm::rotate(R_matrix, glm::radians(-30.0f), glm::vec3(0, 1, 0));//y축 -30도
		R_matrix = glm::rotate(R_matrix, glm::radians(30.0f), glm::vec3(1, 0, 0));//x축 -30도
		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
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
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		//--- modelTransform 변수에 변환 값 적용하기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		switch (slide) {
		case '1':
			glUniform3f(colorUniformLoc, color[0].r, color[0].g, color[0].b);
			glDrawArrays(GL_TRIANGLES, 0, 6);		//아랫면
			break;
		case '2':
			glUniform3f(colorUniformLoc, color[1].r, color[1].g, color[1].b);
			glDrawArrays(GL_TRIANGLES, 6, 6);		//윗면
			break;
		case '3':
			glUniform3f(colorUniformLoc, color[2].r, color[2].g, color[2].b);
			glDrawArrays(GL_TRIANGLES, 12, 6);		//앞면
			break;
		case '4':
			glUniform3f(colorUniformLoc, color[3].r, color[3].g, color[3].b);
			glDrawArrays(GL_TRIANGLES, 18, 6);		//오른쪽 옆면
			break;
		case '5':
			glUniform3f(colorUniformLoc, color[4].r, color[4].g, color[4].b);
			glDrawArrays(GL_TRIANGLES, 24, 6);			//뒷면
			break;
		case '6':

			glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);
			glDrawArrays(GL_TRIANGLES, 30, 6);			//뒷면
			break;
		case 'c':
		{
			random_device rd;
			default_random_engine dre(rd());
			uniform_int_distribution<int> num(0, 5);
			int slide1 = num(dre);
			int slide2 = num(dre);
			while (slide1 == slide2)
			{
				slide2 = num(dre);
			}
			glUniform3f(colorUniformLoc, color[slide1].r, color[slide1].g, color[slide1].b);
			glDrawArrays(GL_TRIANGLES, 6 * slide1, 6);

			glUniform3f(colorUniformLoc, color[slide2].r, color[slide2].g, color[slide2].b);
			glDrawArrays(GL_TRIANGLES, 6 * slide2, 6);
		}
		break;
		default:
			break;
		}
	}

};

struct Tetrahedron {
	vec3 vertex[4];
	vec3 color[4];
	GLuint m_vbo, EBO;
	unsigned int index[12] = {
		0, 1, 2, // 첫 번째 삼각형
		0, 3, 1, // 두 번째 삼각형
		1, 3, 2, // 세 번째 삼각형
		0, 2, 3  // 네 번째 삼각형
	};
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Tetrahedron(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		for (int i = 0; i < 4; ++i)
		{
			color[i].r = urd(dre);
			color[i].g = urd(dre);
			color[i].b = urd(dre);
		}
		R_matrix = glm::rotate(R_matrix, glm::radians(-10.0f), glm::vec3(0, 1, 0));//y축 -30도
		R_matrix = glm::rotate(R_matrix, glm::radians(10.0f), glm::vec3(1, 0, 0));//x축 -30도
		S_matrix = glm::scale(S_matrix, glm::vec3(2, 2, 2));

		vertex[0] = { 0,0,0.1 }; vertex[1] = { -0.1,0,-0.1 }; vertex[2] = { 0.1,0,-0.1 }; vertex[3] = { 0,0.15,-0.03 };


		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //--- GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		//--- modelTransform 변수에 변환 값 적용하기
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		switch (slide) {
		case '7':
			glUniform3f(colorUniformLoc, color[1].r, color[1].g, color[1].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * 1 * sizeof(GLuint)));
			break;
		case '8':
			glUniform3f(colorUniformLoc, color[2].r, color[2].g, color[2].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * 2 * sizeof(GLuint)));
			break;
		case '9':
			glUniform3f(colorUniformLoc, color[3].r, color[3].g, color[3].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * 3 * sizeof(GLuint)));
			break;
		case '0':
			glUniform3f(colorUniformLoc, color[0].r, color[0].g, color[0].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * 0 * sizeof(GLuint)));
			break;
		case 't':
		{

			//glEnable(GL_CULL_FACE);
			//glCullFace(GL_BACK);
			//glCullFace(GL_CW);

			random_device rd;
			default_random_engine dre(rd());
			uniform_int_distribution<int> num(0, 3);
			int slide1 = num(dre);
			int slide2 = num(dre);
			while (slide1 == slide2)
			{
				slide2 = num(dre);
			}
			glUniform3f(colorUniformLoc, color[slide1].r, color[slide1].g, color[slide1].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * slide1 * sizeof(GLuint)));

			glUniform3f(colorUniformLoc, color[slide2].r, color[slide2].g, color[slide2].b);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(3 * slide2 * sizeof(GLuint)));
		}
		//glDisable(GL_CULL_FACE);
		break;
		default:
			break;
		}
	}

};
vector <Dice> dice;
vector <Sector> Line;
vector <Tetrahedron> triangle;
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
	dice.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	triangle.emplace_back(400, 300);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgramID);
	Line[0].Draw();
	dice[0].Draw();
	triangle[0].Draw();
	glutSwapBuffers(); //화면에 출력
}
GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		slide = '1';
		break;
	case '2':
		slide = '2';
		break;
	case '3':
		slide = '3';
		break;
	case '4':
		slide = '4';
		break;
	case '5':
		slide = '5';
		break;
	case '6':
		slide = '6';
		break;
	case '7':
		slide = '7';
		break;
	case '8':
		slide = '8';
		break;
	case '9':
		slide = '9';
		break;
	case '0':
		slide = '0';
		break;
	case 'c':
		slide = 'c';
		break;
	case 't':
		slide = 't';
		break;
	default:
		break;
	}
	glutPostRedisplay();
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

