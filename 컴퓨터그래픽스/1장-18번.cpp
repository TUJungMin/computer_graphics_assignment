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
float WIDTH = 1000;
float HEIGHT = 1000;
bool button_h = FALSE; bool button_y = FALSE; bool button_p = FALSE;
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
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}


GLvoid Timer(int value);
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
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
		//Line_T_matrix = glm::translate(Line_R_matrix, glm::vec3(0.0, 0.0, 0.0));

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));


		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
		}
	}

};
struct animation {
	bool button = FALSE;
	float angle = 0.0f;
	bool activate = FALSE;
};
struct Dice {
	bool appear = FALSE;
	bool rotate[3] = { TRUE,TRUE,TRUE };
	bool revolution[3] = { TRUE,TRUE,TRUE };
	float angle[2] = { 0,0 };
	animation button_f;
	animation button_b;
	animation button_s;
	animation button_t;
	vec3 color[6];
	GLuint m_vbo;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	GLuint pathVBO; // 경로를 그릴 VBO
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

		model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		model_matrix = glm::rotate(model_matrix, glm::radians(angle[0]), glm::vec3(rotate[0], rotate[1], rotate[2]));
		model_matrix = glm::translate(model_matrix, move_pos);
		model_matrix = glm::rotate(model_matrix, glm::radians(angle[1]), glm::vec3(revolution[0], revolution[1], revolution[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));


		glUniform3f(colorUniformLoc, color[0].r, color[0].g, color[0].b);		//아랫면
		glDrawArrays(GL_TRIANGLES, 6 * 0, 6);


		glUniform3f(colorUniformLoc, color[1].r, color[1].g, color[1].b);		//윗면
		glDrawArrays(GL_TRIANGLES, 6 * 1, 6);

		glUniform3f(colorUniformLoc, color[2].r, color[2].g, color[2].b);		//앞면
		glDrawArrays(GL_TRIANGLES, 6 * 2, 6);

		glUniform3f(colorUniformLoc, color[3].r, color[3].g, color[3].b);		//오른면
		glDrawArrays(GL_TRIANGLES, 6 * 3, 6);

		glUniform3f(colorUniformLoc, color[4].r, color[4].g, color[4].b);		//뒷면
		glDrawArrays(GL_TRIANGLES, 6 * 4, 6);

		glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);		//왼쪽 면
		glDrawArrays(GL_TRIANGLES, 6 * 5, 6);

	}


};
struct PYRAMID {
	bool appear = FALSE;
	bool rotate[3] = { TRUE,TRUE,TRUE };
	bool revolution[3] = { TRUE,TRUE,TRUE };
	float angle[2] = { 0,0 };
	animation button_r;
	animation button_o;
	vec3 color[6];
	GLuint m_vbo;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	GLuint pathVBO; // 경로를 그릴 VBO
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화

	PYRAMID(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);

		for (int i = 0; i < 5; ++i)
		{
			color[i].r = urd(dre);
			color[i].g = urd(dre);
			color[i].b = urd(dre);
		}
		color[5].r = color[4].r;
		color[5].g = color[4].g;
		color[5].b = color[4].b;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * P_vertex.size(), &P_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		model_matrix = glm::rotate(model_matrix, glm::radians(angle[0]), glm::vec3(rotate[0], rotate[1], rotate[2]));
		model_matrix = glm::translate(model_matrix, move_pos);
		model_matrix = glm::rotate(model_matrix, glm::radians(angle[1]), glm::vec3(revolution[0], revolution[1], revolution[2]));
		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));


		glUniform3f(colorUniformLoc, color[0].r, color[0].g, color[0].b);		//아랫면
		glDrawArrays(GL_TRIANGLES, 3 * 0, 3);


		glUniform3f(colorUniformLoc, color[1].r, color[1].g, color[1].b);		//윗면
		glDrawArrays(GL_TRIANGLES, 3 * 1, 3);

		glUniform3f(colorUniformLoc, color[2].r, color[2].g, color[2].b);		//앞면
		glDrawArrays(GL_TRIANGLES, 3 * 2, 3);

		glUniform3f(colorUniformLoc, color[3].r, color[3].g, color[3].b);		//오른면
		glDrawArrays(GL_TRIANGLES, 3 * 3, 3);

		glUniform3f(colorUniformLoc, color[4].r, color[4].g, color[4].b);		//뒷면
		glDrawArrays(GL_TRIANGLES, 3 * 4, 3);

		glUniform3f(colorUniformLoc, color[5].r, color[5].g, color[5].b);		//왼쪽 면
		glDrawArrays(GL_TRIANGLES, 3 * 5, 3);

	}


};

vector <Dice> dice;
vector <Sector> Line;
vector <PYRAMID> pyramid;
bool button_1[2] = { FALSE,FALSE };
bool button_2 = FALSE;
bool button_3 = FALSE;
int pyramid_sequence = 0;
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
	//ReadObj("sphere.obj", S_vertex, S_normal, S_uv);
	dice.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	pyramid.emplace_back(400, 300);
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(20, Timer, 1);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
vec3 camerapos{ -0.3f,0.3f,0.5f };
void drawScene()
{
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	{
		glm::vec3 cameraPos = camerapos; //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		if (button_p) {
			projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
		}
		else {
			projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f); //--- 투영 공간 설정: fovy, aspect, near, far
			projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0)); //--- 공간을 z축 이동
		}
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glEnable(GL_DEPTH_TEST);
		if (button_h) {
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);
		}
		Line[0].Draw();
		if (dice[0].appear) {
			dice[0].Draw();
		}
		if (pyramid[0].appear) {
			pyramid[0].Draw();
		}
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}
	glutSwapBuffers(); //화면에 출력
}
GLvoid Timer(int value)
{
	if (dice[0].appear) {
		if (button_y)
		{
			dice[0].angle[0] += 1.0f;
			if (dice[0].angle[0] >= 360.0f)
			{
				dice[0].angle[0] = 0.0f;
			}
		}

		{

			if (dice[0].button_f.button)
			{
				if (dice[0].button_f.angle <= 90 && !dice[0].button_f.activate) {
					dice[0].button_f.angle++;
					D_vertex[15].z = D_vertex[13].z + sin(glm::radians(dice[0].button_f.angle));
					D_vertex[15].y = D_vertex[13].y + cos(glm::radians(dice[0].button_f.angle));
					D_vertex[14].z = D_vertex[13].z + sin(glm::radians(dice[0].button_f.angle));
					D_vertex[14].y = D_vertex[13].y + cos(glm::radians(dice[0].button_f.angle));
					D_vertex[16].z = D_vertex[17].z + sin(glm::radians(dice[0].button_f.angle));
					D_vertex[16].y = D_vertex[17].y + cos(glm::radians(dice[0].button_f.angle));
				}
				else
					dice[0].button_f.activate = TRUE;

			}

			else if (dice[0].button_f.activate && dice[0].button_f.angle >= 0)
			{
				dice[0].button_f.angle--;
				D_vertex[15].z = D_vertex[13].z + sin(glm::radians(dice[0].button_f.angle));
				D_vertex[15].y = D_vertex[13].y + cos(glm::radians(dice[0].button_f.angle));
				D_vertex[14].z = D_vertex[13].z + sin(glm::radians(dice[0].button_f.angle));
				D_vertex[14].y = D_vertex[13].y + cos(glm::radians(dice[0].button_f.angle));
				D_vertex[16].z = D_vertex[17].z + sin(glm::radians(dice[0].button_f.angle));
				D_vertex[16].y = D_vertex[17].y + cos(glm::radians(dice[0].button_f.angle));
			}
			else
			{
				dice[0].button_f.activate = FALSE;
			}
		}
		{
			if (dice[0].button_b.button)
			{
				if (D_vertex[24].x > D_vertex[25].x && !dice[0].button_b.activate) {
					D_vertex[24].x -= 0.01; 	D_vertex[24].y += 0.01;
					D_vertex[25].x += 0.01; 	D_vertex[25].y += 0.01;
					D_vertex[26].x += 0.01; 	D_vertex[26].y -= 0.01;
					D_vertex[27].x += 0.01; 	D_vertex[27].y -= 0.01;
					D_vertex[28].x -= 0.01; 	D_vertex[28].y -= 0.01;
					D_vertex[29].x -= 0.01; 	D_vertex[29].y += 0.01;
				}
				else
					dice[0].button_b.activate = TRUE;

			}
			else if (dice[0].button_b.activate && D_vertex[24].x < 0.5f)
			{
				D_vertex[24].x += 0.01; 	D_vertex[24].y -= 0.01;
				D_vertex[25].x -= 0.01; 	D_vertex[25].y -= 0.01;
				D_vertex[26].x -= 0.01; 	D_vertex[26].y += 0.01;
				D_vertex[27].x -= 0.01; 	D_vertex[27].y += 0.01;
				D_vertex[28].x += 0.01; 	D_vertex[28].y += 0.01;
				D_vertex[29].x += 0.01; 	D_vertex[29].y -= 0.01;
			}
			else
			{
				dice[0].button_b.activate = FALSE;
			}
		}

		if (dice[0].button_s.button)
		{
			if (D_vertex[31].y < 0.5 && !dice[0].button_s.activate) {
				for (int i = 18; i < 24; ++i) {
					D_vertex[i].y += 0.1f;
				}
				for (int i = 30; i < 36; ++i) {
					D_vertex[i].y += 0.1f;
				}
			}
			else
				dice[0].button_s.activate = TRUE;
		}
		else if (dice[0].button_s.activate && D_vertex[31].y > -0.5f)
		{
			for (int i = 18; i < 24; ++i) {
				D_vertex[i].y -= 0.1f;
			}
			for (int i = 30; i < 36; ++i) {
				D_vertex[i].y -= 0.1f;
			}
		}
		else
		{
			dice[0].button_s.activate = FALSE;
		}

		if (dice[0].button_t.button)
		{
			dice[0].button_t.angle++;
			D_vertex[6].x = 0.5 * -cos(glm::radians(dice[0].button_t.angle));
			D_vertex[6].y = 0.5 + 0.5 * -sin(glm::radians(dice[0].button_t.angle));
			D_vertex[7].x = 0.5 * cos(glm::radians(dice[0].button_t.angle));
			D_vertex[7].y = 0.5 + 0.5 * sin(glm::radians(dice[0].button_t.angle));
			D_vertex[8].x = 0.5 * cos(glm::radians(dice[0].button_t.angle));
			D_vertex[8].y = 0.5 + 0.5 * sin(glm::radians(dice[0].button_t.angle));
			D_vertex[9].x = 0.5 * cos(glm::radians(dice[0].button_t.angle));
			D_vertex[9].y = 0.5 + 0.5 * sin(glm::radians(dice[0].button_t.angle));
			D_vertex[10].x = 0.5 * -cos(glm::radians(dice[0].button_t.angle));
			D_vertex[10].y = 0.5 + 0.5 * -sin(glm::radians(dice[0].button_t.angle));
			D_vertex[11].x = 0.5 * -cos(glm::radians(dice[0].button_t.angle));
			D_vertex[11].y = 0.5 + 0.5 * -sin(glm::radians(dice[0].button_t.angle));

			if (dice[0].button_t.angle > 360.0f)
			{
				dice[0].button_t.angle = 0.0f;
			}


		}
	}

	else if (pyramid[0].appear)
	{
		if (button_y)
		{
			pyramid[0].angle[0] += 1.0f;
			if (pyramid[0].angle[0] >= 360.0f)
			{
				pyramid[0].angle[0] = 0.0f;
			}
		}
		if (pyramid[0].button_o.button)
		{
			glm::vec4 rotatedVertex;
			glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
			if (pyramid[0].button_o.angle < 180.0f && !pyramid[0].button_o.activate) {
				pyramid[0].button_o.angle++;  // 회전 각도(라디안)를 설정

				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[0] = glm::vec3(rotatedVertex);

				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[3] = glm::vec3(rotatedVertex);

				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[6] = glm::vec3(rotatedVertex);

				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[9] = glm::vec3(rotatedVertex);
			}
			else
				pyramid[0].button_o.activate = TRUE;
		}
		else if (pyramid[0].button_o.activate && pyramid[0].button_o.angle > 0.0f)
		{
			pyramid[0].button_o.angle--;
			glm::vec4 rotatedVertex;
			glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[0] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[3] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[6] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[9] = glm::vec3(rotatedVertex);
		}
		else
			pyramid[0].button_o.activate = FALSE;
	}




	if (pyramid[0].button_o.button)
	{
		glm::vec4 rotatedVertex;
		glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
		if (pyramid[0].button_o.angle < 180.0f && !pyramid[0].button_o.activate) {
			pyramid[0].button_o.angle++;  // 회전 각도(라디안)를 설정

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[0] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[3] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[6] = glm::vec3(rotatedVertex);

			rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
			rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
			P_vertex[9] = glm::vec3(rotatedVertex);
		}
		else
			pyramid[0].button_o.activate = TRUE;
	}
	else if (pyramid[0].button_o.activate && pyramid[0].button_o.angle > 0.0f)
	{
		pyramid[0].button_o.angle--;
		glm::vec4 rotatedVertex;
		glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
		rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
		rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
		P_vertex[0] = glm::vec3(rotatedVertex);

		rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
		rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
		P_vertex[3] = glm::vec3(rotatedVertex);

		rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_o.angle), glm::vec3(1.0f, 0.0f, 0.0f));
		rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
		P_vertex[6] = glm::vec3(rotatedVertex);

		rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_o.angle), glm::vec3(0.0f, 0.0f, 1.0f));
		rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
		P_vertex[9] = glm::vec3(rotatedVertex);
	}
	else
		pyramid[0].button_o.activate = FALSE;
	if (pyramid[0].button_r.button)
	{
		glm::vec4 rotatedVertex;
		glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
		if (!pyramid[0].button_r.activate) {
			pyramid[0].button_r.angle++;  // 회전 각도(라디안)를 설정
			switch (pyramid_sequence)
			{
			case 0:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_r.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[0] = glm::vec3(rotatedVertex);
				break;
			case 1:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_r.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[3] = glm::vec3(rotatedVertex);
				break;
			case 2:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_r.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[6] = glm::vec3(rotatedVertex);
				break;
			case 3:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_r.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[9] = glm::vec3(rotatedVertex);
				break;
			}
			if (pyramid[0].button_r.angle >= 180.0f) {
				if (pyramid_sequence != 3) {
					pyramid_sequence++;
					pyramid[0].button_r.angle = 0.0f;
				}
				else
				{
					pyramid[0].button_r.activate = TRUE;
					pyramid[0].button_r.angle = 180.0f;
				}
			}
		}

	}
	else
	{
		glm::vec4 rotatedVertex;
		glm::mat4 rotationMatrix = glm::mat4(1.0f);  // 단위 행렬로 초기화
		if (pyramid[0].button_r.activate) {
			pyramid[0].button_r.angle--;  // 회전 각도(라디안)를 설정
			switch (pyramid_sequence)
			{
			case 0:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_r.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[0] = glm::vec3(rotatedVertex);
				break;
			case 1:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_r.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[3] = glm::vec3(rotatedVertex);
				break;
			case 2:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(-pyramid[0].button_r.angle), glm::vec3(1.0f, 0.0f, 0.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[6] = glm::vec3(rotatedVertex);
				break;
			case 3:
				rotationMatrix = glm::rotate(glm::mat4(1.0f), radians(pyramid[0].button_r.angle), glm::vec3(0.0f, 0.0f, 1.0f));
				rotatedVertex = rotationMatrix * glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
				P_vertex[9] = glm::vec3(rotatedVertex);
				break;
			}
			if (pyramid[0].button_r.angle <= 0.0f) {
				if (pyramid_sequence != 0) {
					pyramid_sequence--;
					pyramid[0].button_r.angle = 180.0f;
				}
				else
				{
					pyramid[0].button_r.activate = FALSE;
					pyramid[0].button_r.angle = 0.0f;
				}
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
	case '1':
		dice[0].appear = TRUE;
		pyramid[0].appear = FALSE;
		break;

	case '2':
		pyramid[0].appear = TRUE;
		dice[0].appear = FALSE;
		break;

	case '-':
		camerapos.z -= 0.1f;
		break;
	case '=':
		camerapos.z += 0.1f;
		break;
	case 'h':
		button_h = !button_h;
		break;
	case 'y':
		button_y = !button_y;
		dice[0].rotate[0] = FALSE;
		dice[0].rotate[1] = TRUE;
		dice[0].rotate[2] = FALSE;
		pyramid[0].rotate[0] = FALSE;
		pyramid[0].rotate[1] = TRUE;
		pyramid[0].rotate[2] = FALSE;
		break;
	case 'f':
		dice[0].button_f.button = !dice[0].button_f.button;
		break;
	case 'b':
		dice[0].button_b.button = !dice[0].button_b.button;
		break;
	case 's':
		dice[0].button_s.button = !dice[0].button_s.button;
		break;
	case 't':
		dice[0].button_t.button = !dice[0].button_t.button;
		break;
	case 'o':
		pyramid[0].button_o.button = !pyramid[0].button_o.button;
		break;
	case 'r':
		pyramid[0].button_r.button = !pyramid[0].button_r.button;
		break;
	case 'p':
		button_p = !button_p;
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
	vertexSource = filetobuf("vertex3.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment2.glsl");
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

