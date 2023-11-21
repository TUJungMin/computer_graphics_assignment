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
bool change = FALSE;
float WIDTH = 1000;
float HEIGHT = 1000;
bool button_r = FALSE;
bool button2 = FALSE;
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
std::vector <glm::vec3> S_vertex;
std::vector <glm::vec3> S_normal;
std::vector <glm::vec2> S_uv;

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
		Line_R_matrix = glm::rotate(Line_R_matrix, glm::radians(20.0f), glm::vec3(1.0, 0.0, 0.0));
		Line_R_matrix = glm::rotate(Line_R_matrix, glm::radians(-20.0f), glm::vec3(0.0, 1.0, 0.0));
		//Line_T_matrix = glm::translate(Line_R_matrix, glm::vec3(0.0, 0.0, 0.0));

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
	vec3 dice_pos;
	vec3 color[6];
	GLuint m_vbo;
	float rotate_x = 2.0f;
	float rotate_y = 0.0f;
	GLuint pathVBO; // 경로를 그릴 VBO
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화

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

		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1.0, 0.0, 0.0));
		R_matrix = glm::rotate(R_matrix, glm::radians(-20.0f), glm::vec3(0.0, 1.0, 0.0));

		S_matrix = glm::scale(S_matrix, glm::vec3(0.1, 0.1, 0.1));
		T_matrix = glm::translate(T_matrix, glm::vec3(-0.5, 0.0, 0.0));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
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
struct Dot {
	GLuint m_vbo;
	vector <vec3> f_angle;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1.0, 0.0, 0.0));
		R_matrix = glm::rotate(R_matrix, glm::radians(-20.0f), glm::vec3(0.0, 1.0, 0.0));

		while (T_matrix[3].x < 1) {

			T_matrix = glm::translate(T_matrix, glm::vec3(0.002f, 0, 0));
			//dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(2.0f), glm::vec3(1, 0, 0));
			rot_matrix = glm::rotate(rot_matrix, glm::radians(2.0f), glm::vec3(0, 0, 1));
			mat4 model_matrix = rot_matrix * T_matrix * R_matrix * S_matrix;
			vec3 temp1 = { model_matrix[3].x,model_matrix[3].y ,model_matrix[3].z };
			f_angle.push_back(temp1);

		}


	}
	void draw_line() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * f_angle.size(), &f_angle[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(vertexAttribLoc);
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		unsigned int rotateLocation = glGetUniformLocation(shaderProgramID, "rotate");
		glUniformMatrix4fv(rotateLocation, 1, GL_FALSE, glm::value_ptr(rot_matrix));
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		glUniform3f(colorUniformLoc, 1, 0, 0);
		glPointSize(10.0f);
		for (int i = 0; i < f_angle.size(); ++i) {
			glDrawArrays(GL_POINT, i, 1);
		}
	}
};

struct Sphere {
	vec3 sphere_pos;
	GLuint m_vbo;
	float rotate_x = 2.0f;
	float rotate_y = 2.0f;
	glm::mat4 R_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 S_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 T_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Sphere(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		R_matrix = glm::rotate(R_matrix, glm::radians(20.0f), glm::vec3(1.0, 0.0, 0.0));
		R_matrix = glm::rotate(R_matrix, glm::radians(-20.0f), glm::vec3(0.0, 1.0, 0.0));

		S_matrix = glm::scale(S_matrix, glm::vec3(0.02, 0.02, 0.02));
		T_matrix = glm::translate(T_matrix, glm::vec3(0.5, 0.0, 0.0));
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


		//glEnable(GL_CULL_FACE);
		glUniform3f(colorUniformLoc, 1, 0, 0);
		glDrawArrays(GL_LINE_LOOP, 0, 360);

	}
};

vector <Dice> dice;
vector <Sector> Line;
vector <Sphere> sphere;
Dot point;
bool button_1[2] = { FALSE,FALSE };
bool button_2 = FALSE;
bool button_3 = FALSE;
bool button_f = FALSE;
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
	ReadObj("sphere.obj", S_vertex, S_normal, S_uv);
	dice.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	sphere.emplace_back(400, 300);
	glutKeyboardFunc(KeyBoardFunc);
	point.InitVbo();
	glutTimerFunc(20, Timer, 1);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
void drawScene()
{
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	{
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		Line[0].Draw();
		dice[0].Draw();
		sphere[0].Draw();
		point.draw_line();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}
	glutSwapBuffers(); //화면에 출력
}
GLvoid Timer(int value)
{
	if (button_r) {
		if (dice[0].T_matrix[3].x < 1) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0.002f, 0, 0));
			//dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(2.0f), glm::vec3(1, 0, 0));
			dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(2.0f), glm::vec3(0, 1, 1));



		}
		else
		{
			dice[0].T_matrix = glm::mat4(1.0f);
			dice[0].rot_matrix = glm::mat4(1.0f);


		}
		if (sphere[0].T_matrix[3].x < 1) {
			sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0.002f, 0, 0));
			
			sphere[0].rot_matrix = glm::rotate(sphere[0].rot_matrix, glm::radians(-2.0f), glm::vec3(0, 1, 1));
		}
		else {
			sphere[0].T_matrix = glm::mat4(1.0f);
			sphere[0].rot_matrix = glm::mat4(1.0f);
		}
	}
	if (button_1[0]) {
		if (!button_1[1]) {
			if (abs(dice[0].T_matrix[3].x) > 0.001) {
				dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-dice[0].dice_pos.x / 100, -dice[0].dice_pos.y / 100, -dice[0].dice_pos.z / 100));
				sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(-sphere[0].sphere_pos.x / 100, -sphere[0].sphere_pos.y / 100, -sphere[0].sphere_pos.z / 100));

			}
			else
			{
				button_1[1] = TRUE;
			}
		}
		else
		{
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(dice[0].dice_pos.x / 100, dice[0].dice_pos.y / 100, dice[0].dice_pos.z / 100));
			sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(sphere[0].sphere_pos.x / 100, sphere[0].sphere_pos.y / 100, sphere[0].sphere_pos.z / 100));
			if (abs(dice[0].T_matrix[3].x - dice[0].dice_pos.x) < 0.001) {
				button_1[0] = FALSE;
			}
		}
	}
	if (button_2)
	{

		//dice[0].rot_matrix = glm::rotate(dice[0].rot_matrix, glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 0));
		dice[0].rotate_y += 1.0f;
		sphere[0].rotate_y += 1.0f;
		//dice[0].R_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 0));
		dice[0].rot_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 1));
		sphere[0].rot_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(sphere[0].rotate_y), glm::vec3(0, 1, 1));
		if (dice[0].rotate_y>= 180.0f)
		{
			dice[0].rotate_y = 0;
			sphere[0].rotate_y = 0;
			//dice[0].rot_matrix = glm::mat4(1.0f);
			//sphere[0].rot_matrix = glm::mat4(1.0f);
			button_2 = FALSE;
			button2 = !button2;
			//dice[0].rotate_y = 1.0f;
			//sphere[0].rotate_y = 1.0f;
		}


		//sphere[0].rot_matrix = glm::rotate(sphere[0].rot_matrix, glm::radians(sphere[0].rotate_y), glm::vec3(0, 1, 0));
	}
	if (button_f) {
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(((sphere[0].sphere_pos.x - dice[0].dice_pos.x) / 100.0f), ((sphere[0].sphere_pos.y - dice[0].dice_pos.y) / 100.0f), ((sphere[0].sphere_pos.z - dice[0].dice_pos.z) / 100.0f)));
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(((dice[0].dice_pos.x - sphere[0].sphere_pos.x) / 100.0f), ((dice[0].dice_pos.y - sphere[0].sphere_pos.y) / 100.0f), ((dice[0].dice_pos.z - sphere[0].sphere_pos.z) / 100.0f)));
		if (dice[0].dice_pos.x <= sphere[0].sphere_pos.x) {
			if (dice[0].T_matrix[3].x >= sphere[0].sphere_pos.x) {
				button_f = FALSE;
			}
		}
		if (dice[0].dice_pos.x >= sphere[0].sphere_pos.x) {
			if (dice[0].T_matrix[3].x <= sphere[0].sphere_pos.x) {
				button_f = FALSE;
			}
		}

	}
	if (button_3) {
		// 두 도형이 y 축으로 이동 (예: 2단계로 나누어 이동)
		if (abs(dice[0].T_matrix[3].x) > 0.001f) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-dice[0].dice_pos.x / 100, -dice[0].dice_pos.x / 100, 0.0f));
			//sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0.0f, 0.02f, 0.0f));
		}
		else {
			// 다시 초기 위치로 이동
			if (abs(dice[0].T_matrix[3].y - dice[0].dice_pos.y) < 0.001) {
				button_3 = FALSE;
				// 이동 애니메이션이 끝났을 때 button_3을 FALSE로 설정
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 's':
	{
		button_1[0] = FALSE;
		button_1[1] = FALSE;
		button_2 = FALSE;
		button_3 = FALSE;
		button_r = FALSE;
		button_f = FALSE;
		dice[0].T_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5, 0.0, 0.0));
		dice[0].rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
		dice[0].model_matrix = glm::mat4(1.0f); //단위행렬로 초기화

		sphere[0].T_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.0, 0.0));//단위행렬로 초기화
		sphere[0].rot_matrix = glm::mat4(1.0f); //단위행렬로 초기화
		sphere[0].model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
		break;
	}
	case 'z':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-0.05f, 0, 0));
		break;
	}
	case 'Z':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(-0.05f, 0, 0));
		break;
	}
	case 'x':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0.05f, 0, 0));
		break;
	}
	case 'X':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0.05f, 0, 0));
		break;
	}
	case 'c':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, -0.05f, 0));
		break;
	}
	case 'C':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, -0.05f, 0));
		break;
	}
	case 'v':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, 0.05f, 0));
		break;
	}
	case 'V':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, 0.05f, 0));
		break;
	}
	case 'b':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, 0, -0.05f));
		break;
	}
	case 'B':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, 0, -0.05f));
		break;
	}
	case 'n':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, 0, 0.05f));
		break;
	}
	case 'N':
	{
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, 0, 0.05f));
		break;
	}
	case '=':
	{
		dice[0].S_matrix = glm::scale(dice[0].S_matrix, glm::vec3(1.1, 1.1, 1.1));
		break;
	}
	case '+':
	{
		sphere[0].S_matrix = glm::scale(sphere[0].S_matrix, glm::vec3(1.1, 1.1, 1.1));
		break;
	}
	case '-':
	{
		dice[0].S_matrix = glm::scale(dice[0].S_matrix, glm::vec3(0.9, 0.9, 0.9));
		break;
	}
	case '_':
	{
		sphere[0].S_matrix = glm::scale(sphere[0].S_matrix, glm::vec3(0.9, 0.9, 0.9));
		break;
	}
	case ';':		//원점 기준 신축
	{
		dice[0].S_matrix = glm::scale(dice[0].S_matrix, glm::vec3(0.9, 0.9, 0.9));
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-dice[0].T_matrix[3].x / 10.0f, -dice[0].T_matrix[3].y / 10.0f, -dice[0].T_matrix[3].z / 10.0f));
		break;
	}
	case ':':		//원점 기준 신축
	{
		sphere[0].S_matrix = glm::scale(sphere[0].S_matrix, glm::vec3(0.9, 0.9, 0.9));
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(-sphere[0].T_matrix[3].x / 10.0f, -sphere[0].T_matrix[3].y / 10.0f, -sphere[0].T_matrix[3].z / 10.0f));
		break;
	}
	case 'l':		//원점 기준 확대
	{
		dice[0].S_matrix = glm::scale(dice[0].S_matrix, glm::vec3(1.1, 1.1, 1.1));
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(dice[0].T_matrix[3].x / 10.0f, dice[0].T_matrix[3].y / 10.0f, dice[0].T_matrix[3].z / 10.0f));
		break;
	}
	case 'L':		//원점 기준 확대
	{
		sphere[0].S_matrix = glm::scale(sphere[0].S_matrix, glm::vec3(1.1, 1.1, 1.1));
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(sphere[0].T_matrix[3].x / 10.0f, sphere[0].T_matrix[3].y / 10.0f, sphere[0].T_matrix[3].z / 10.0f));
		break;
	}
	case 'q':
	{
		if (!button2) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, 0.05f, 0.0f));
			sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, 0.05f, 0.0f));
		}
		else
		{
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, 0.0f, 0.05f));
			sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, 0.0f, 0.05f));
		}
		Line[0].model_matrix = glm::translate(Line[0].model_matrix, glm::vec3(0, 0.05f, 0.0f));
		break;
	}
	case 'w':
	{
		dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0, -0.05f, 0.0f));
		sphere[0].T_matrix = glm::translate(sphere[0].T_matrix, glm::vec3(0, -0.05f, 0.0f));
		Line[0].model_matrix = glm::translate(Line[0].model_matrix, glm::vec3(0, -0.05f, 0.0f));
		break;
	}
	case 'r':
	{
		dice[0].model_matrix = dice[0].rot_matrix * dice[0].T_matrix * dice[0].R_matrix * dice[0].S_matrix;
		dice[0].T_matrix = glm::mat4(1.0f);
		sphere[0].model_matrix = sphere[0].rot_matrix * sphere[0].T_matrix * sphere[0].R_matrix * sphere[0].S_matrix;
		sphere[0].T_matrix = glm::mat4(1.0f);
		button_r = !button_r;
		break;
	}
	case 't':
	{
		button_1[0] = TRUE;
		button_1[1] = FALSE;
		dice[0].dice_pos.x = dice[0].T_matrix[3].x;
		dice[0].dice_pos.y = dice[0].T_matrix[3].y;
		dice[0].dice_pos.z = dice[0].T_matrix[3].z;
		sphere[0].sphere_pos.x = sphere[0].T_matrix[3].x;
		sphere[0].sphere_pos.y = sphere[0].T_matrix[3].y;
		sphere[0].sphere_pos.z = sphere[0].T_matrix[3].z;
		break;
	}
	case '2':
	{
		button_2 = TRUE;
		dice[0].dice_pos.x = dice[0].T_matrix[3].x;
		dice[0].dice_pos.y = dice[0].T_matrix[3].y;
		dice[0].dice_pos.z = dice[0].T_matrix[3].z;
		sphere[0].sphere_pos.x = sphere[0].T_matrix[3].x;
		sphere[0].sphere_pos.y = sphere[0].T_matrix[3].y;
		sphere[0].sphere_pos.z = sphere[0].T_matrix[3].z;
		break;
	}
	case '1':
		button_f = TRUE;
		dice[0].dice_pos.x = dice[0].T_matrix[3].x;
		dice[0].dice_pos.y = dice[0].T_matrix[3].y;
		dice[0].dice_pos.z = dice[0].T_matrix[3].z;
		sphere[0].sphere_pos.x = sphere[0].T_matrix[3].x;
		sphere[0].sphere_pos.y = sphere[0].T_matrix[3].y;
		sphere[0].sphere_pos.z = sphere[0].T_matrix[3].z;
		break;
	case '3':
		dice[0].dice_pos.x = dice[0].T_matrix[3].x;
		dice[0].dice_pos.y = dice[0].T_matrix[3].y;
		dice[0].dice_pos.z = dice[0].T_matrix[3].z;
		sphere[0].sphere_pos.x = sphere[0].T_matrix[3].x;
		sphere[0].sphere_pos.y = sphere[0].T_matrix[3].y;
		sphere[0].sphere_pos.z = sphere[0].T_matrix[3].z;
		button_3 = TRUE;
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

