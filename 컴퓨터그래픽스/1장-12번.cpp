#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;
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
enum type_shape {
	pentagon = 0,
	point,
	line,
	triangle,
	rect,
};
float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

GLvoid TimerFunc(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLint result;
GLchar errorLog[512];
vec3 Line_vertex[2] = { {-0.1f,0.0f,0.0f },{0.1f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = {
	{-0.1f, -0.1f, 0.0f},  // 왼쪽 아래 꼭짓점
	{0.1f, -0.1f, 0.0f },   // 오른쪽 아래 꼭짓점
	{0.0f, 0.1f, 0.0f}     // 위 꼭짓점
};
vec3 Rect_vertex[4] = {
	{-0.1f,-0.1f,0.0f},
	{0.1f,-0.1f,0.0f},
	{-0.1f,0.1f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{0.1f,0.1f,0.0f}
};
vec3 Pentagon_vertex[5] = {
	{-0.1f,0.05f,0.0f},
	{-0.05f,-0.05f,0.0f},
	{0.0f,0.15f,0.0f},				//Rect_vertex[2],[3].x움직여야함
	{0.05f,-0.05f,0.0f},
	{0.1f,0.05f,0.0f}
};
vec3 point_vertex = { 0.0f,0.0f,0.0f };
GLvoid Timer(int value);
GLvoid Motion(int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
struct Point {
	vec3 move_pos;
	vec3 color;
	GLuint m_vbo;
	bool catch_object = false;
	bool draw_switch = TRUE;
	bool move = FALSE;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertex), &point_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(point_vertex), &point_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 1);

	}
	void Move_Dialog()
	{
		if (move_pos.x > 1.0f) {
			DX_DY[0] *= -1;
		}
		else if (move_pos.x < -1.0f) {
			DX_DY[0] *= -1;
		}
		if (move_pos.y > 1.0f) {
			DX_DY[1] *= -1;
		}
		else if (move_pos.y < -1.0f) {
			DX_DY[1] *= -1;
		}

		move_pos.x = move_pos.x + DX_DY[0] * 0.01f;
		move_pos.y = move_pos.y + DX_DY[1] * 0.01f;

	}
};
struct Line {

	vec3 move_pos;
	vec3 color;
	GLuint m_vbo;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	bool catch_object = false;
	bool draw_switch = TRUE;
	bool move = FALSE;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);		//wasd로 움직이는 거리+초기좌표
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}
	void Move_Dialog()
	{
		if (move_pos.x > 1.0f) {
			DX_DY[0] *= -1;
		}
		else if (move_pos.x < -1.0f) {
			DX_DY[0] *= -1;
		}
		if (move_pos.y > 1.0f) {
			DX_DY[1] *= -1;
		}
		else if (move_pos.y < -1.0f) {
			DX_DY[1] *= -1;
		}

		move_pos.x = move_pos.x + DX_DY[0] * 0.01f;
		move_pos.y = move_pos.y + DX_DY[1] * 0.01f;

	}
};
struct Triangle {
	vec3 move_pos;
	vec3 color;
	GLuint m_vbo;
	bool catch_object = false;
	bool draw_switch = TRUE;
	bool move = FALSE;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
	}

	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);

		glLineWidth(5.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);

	}
	void Move_Dialog()
	{
		if (move_pos.x > 1.0f) {
			DX_DY[0] *= -1;
		}
		else if (move_pos.x < -1.0f) {
			DX_DY[0] *= -1;
		}
		if (move_pos.y > 1.0f) {
			DX_DY[1] *= -1;
		}
		else if (move_pos.y < -1.0f) {
			DX_DY[1] *= -1;
		}

		move_pos.x = move_pos.x + DX_DY[0] * 0.01f;
		move_pos.y = move_pos.y + DX_DY[1] * 0.01f;

	}


};
struct Rectangle {
	vec3 move_pos;
	vec3 color;
	GLuint m_vbo;
	bool catch_object = false;
	bool draw_switch = TRUE;
	bool move = FALSE;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);		//wasd로 움직이는 거리+초기좌표
		glLineWidth(10.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
	}
	void Move_Dialog()
	{
		if (move_pos.x > 1.0f) {
			DX_DY[0] *= -1;
		}
		else if (move_pos.x < -1.0f) {
			DX_DY[0] *= -1;
		}
		if (move_pos.y > 1.0f) {
			DX_DY[1] *= -1;
		}
		else if (move_pos.y < -1.0f) {
			DX_DY[1] *= -1;
		}

		move_pos.x = move_pos.x + DX_DY[0] * 0.01f;
		move_pos.y = move_pos.y + DX_DY[1] * 0.01f;

	}
};
struct Pentagon {
	vec3 move_pos;
	vec3 color;
	GLuint m_vbo;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	bool catch_object = false;
	bool draw_switch = TRUE;
	bool move = FALSE;
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Pentagon_vertex), Pentagon_vertex, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
		glDrawArrays(GL_TRIANGLES, 2, 3);
	}
	void Move_Dialog()
	{
		if (move_pos.x > 1.0f) {
			DX_DY[0] *= -1;
		}
		else if (move_pos.x < -1.0f) {
			DX_DY[0] *= -1;
		}
		if (move_pos.y > 1.0f) {
			DX_DY[1] *= -1;
		}
		else if (move_pos.y < -1.0f) {
			DX_DY[1] *= -1;
		}

		move_pos.x = move_pos.x + DX_DY[0] * 0.01f;
		move_pos.y = move_pos.y + DX_DY[1] * 0.01f;

	}

};


vector <Line> g_line;
vector <Triangle> g_Triangle;
vector<Rectangle> g_rect;
vector<Pentagon> g_pentagon;
vector<Point> g_point;
void initialize_point() {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	uniform_real_distribution<float> coord(-0.9f, 0.9f);
	for (int i = 0; i < 3; i++) {
		Point temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = coord(dre);
		temp.move_pos.y = coord(dre);
		temp.move_pos.z = 0;
		temp.InitVbo();
		g_point.push_back(temp); // Add the Triangle object to the vector
	}
}
void initialize_line() {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	uniform_real_distribution<float> coord(-0.9f, 0.9f);
	for (int i = 0; i < 3; i++) {
		Line temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = coord(dre);
		temp.move_pos.y = coord(dre);
		temp.move_pos.z = 0;
		temp.InitVbo();
		g_line.push_back(temp); // Add the Triangle object to the vector
	}
}
void initialize_triangle() {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	uniform_real_distribution<float> coord(-0.9f, 0.9f);
	for (int i = 0; i < 3; i++) {
		Triangle temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = coord(dre);
		temp.move_pos.y = coord(dre);
		temp.move_pos.z = 0;
		temp.InitVbo();
		g_Triangle.push_back(temp); // Add the Triangle object to the vector
	}
}
void initialize_rect() {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	uniform_real_distribution<float> coord(-0.9f, 0.9f);
	for (int i = 0; i < 3; i++) {
		Rectangle temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = coord(dre);
		temp.move_pos.y = coord(dre);
		temp.move_pos.z = 0;
		temp.InitVbo();
		g_rect.push_back(temp); // Add the Triangle object to the vector
	}
}
void initialize_pentagon() {
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	uniform_real_distribution<float> coord(-0.9f, 0.9f);
	for (int i = 0; i < 3; i++) {
		Pentagon temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = coord(dre);
		temp.move_pos.y = coord(dre);
		temp.move_pos.z = 0;
		temp.InitVbo();
		g_pentagon.push_back(temp); // Add the Triangle object to the vector
	}
}
void make_Polygon(int D_vertex,GLfloat x_pos, GLfloat y_pos)
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);
	switch (D_vertex)
	{
	case point:
	{
		Point temp;
		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = x_pos;
		temp.move_pos.y = y_pos;
		temp.move_pos.z = 0;
		temp.catch_object = FALSE;
		temp.InitVbo();
		temp.move = TRUE;
		g_point.push_back(temp); // Add the Triangle object to the vector
	}
		break;
	case line:
	{
		Line temp;

		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = x_pos;
		temp.move_pos.y = y_pos;
		temp.move_pos.z = 0;
		temp.catch_object = FALSE;
		temp.InitVbo();
		temp.move = TRUE;
		g_line.push_back(temp);
	}
		break;
	case triangle:
	{
		Triangle temp;

		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = x_pos;
		temp.move_pos.y = y_pos;
		temp.move_pos.z = 0;
		temp.catch_object = FALSE;
		temp.InitVbo();
		temp.move = TRUE;
		g_Triangle.push_back(temp);
	}
		break;
	case rect:
	{
		Rectangle temp;

		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = x_pos;
		temp.move_pos.y = y_pos;
		temp.move_pos.z = 0;
		temp.catch_object = FALSE;
		temp.InitVbo();
		temp.move = TRUE;
		g_rect.push_back(temp);
	}
		break;
	case pentagon:
	{
		Pentagon temp;

		temp.color.r = urd(dre);
		temp.color.g = urd(dre);
		temp.color.b = urd(dre);
		temp.move_pos.x = x_pos;
		temp.move_pos.y = y_pos;
		temp.move_pos.z = 0;
		temp.catch_object = FALSE;
		temp.InitVbo();
		temp.move = TRUE;
		g_pentagon.push_back(temp);
	}
		break;
	default:
		break;
	}
}

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(200, 200); // 윈도우의 위치 지정
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
	initialize_triangle();
	initialize_rect();
	initialize_pentagon();
	initialize_line();
	initialize_point();
	glutMouseFunc(ClickFunc);
	glutMotionFunc(Motion);
	glutTimerFunc(20, Timer, 1);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	//glutTimerFunc(20, Timer, 1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	for (int i = 0; i < g_point.size(); ++i) {
		if (g_point[i].draw_switch) {
			g_point[i].Draw();
		}
	}
	for (int i = 0; i < g_line.size(); ++i) {
		if (g_line[i].draw_switch) {
			g_line[i].Draw();
		}
	}
	for (int i = 0; i < g_Triangle.size(); ++i) {
		if (g_Triangle[i].draw_switch) {
			g_Triangle[i].Draw();
		}
	}
	for (int i = 0; i < g_rect.size(); ++i) {
		if (g_rect[i].draw_switch) {
			g_rect[i].Draw();
		}
	}
	for (int i = 0; i < g_pentagon.size(); ++i) {
		if (g_pentagon[i].draw_switch) {
			g_pentagon[i].Draw();
		}
	}

	glutSwapBuffers(); //--- 화면에 출력하기
}
pair<float, float> pos;
GLvoid ClickFunc(int button, int state, int x, int y)
{
	pos = ConvertWinToGL(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < g_point.size(); ++i) {
			if (abs(pos.first - (g_point[i].move_pos.x)) <= 0.05 && abs(pos.second - (g_point[i].move_pos.y)) <= 0.05) {
				g_point[i].catch_object = TRUE;
			}
		}
		for (int i = 0; i < g_line.size(); ++i) {
			if (abs(pos.first - (g_line[i].move_pos.x)) <= 0.1 && abs(pos.second - (g_line[i].move_pos.y)) <= 0.01) {
				g_line[i].catch_object = TRUE;
			}
		}
		for (int i = 0; i < g_Triangle.size(); ++i) {
			if (abs(pos.first - (g_Triangle[i].move_pos.x)) <= 0.05 && abs(pos.second - (g_Triangle[i].move_pos.y)) <= 0.05) {
				g_Triangle[i].catch_object = TRUE;
			}
		}
		for (int i = 0; i < g_rect.size(); ++i) {
			if (abs(pos.first - (g_rect[i].move_pos.x)) <= 0.05 && abs(pos.second - (g_rect[i].move_pos.y)) <= 0.05) {
				g_rect[i].catch_object = TRUE;
			}
		}
		for (int j = 0; j < g_pentagon.size(); ++j) {
			if (abs(pos.first - (g_pentagon[j].move_pos.x)) <= 0.05 && abs(pos.second - (g_pentagon[j].move_pos.y)) <= 0.05) {
				g_pentagon[j].catch_object = TRUE;
			}
		}



	}
	if (state == GLUT_UP)
	{
		auto m_pos = ConvertWinToGL(x, y);
		int num = 0;
		bool check_point = FALSE; int index_point = 0;
		bool check_line = FALSE; int index_line = 0;
		bool check_triangle = FALSE; int index_triangle = 0;
		bool check_rect = FALSE; int index_rect = 0;
		bool check_pentagon = FALSE; int index_pentagon = 0;

		for (int i = 0; i < g_point.size(); ++i) {
			if (abs(m_pos.first - (g_point[i].move_pos.x)) <= 0.05 && abs(m_pos.second - (g_point[i].move_pos.y)) <= 0.05)
			{
				num += 1;
				if (!check_point) {
					index_point = i;
					check_point = TRUE;
				}
				else
				{
					g_point.erase(g_point.begin() + index_point);
					g_point.erase(g_point.begin() + i - 1);
					make_Polygon(line, m_pos.first, m_pos.second);
					break;
				}

			}
		}
		for (int i = 0; i < g_line.size(); ++i) {
			if (abs(m_pos.first - (g_line[i].move_pos.x)) <= 0.05 && abs(m_pos.second - (g_line[i].move_pos.y)) <= 0.05) {
				num += 2;
				if (!check_line) {
					index_line = i;
					check_line = TRUE;
				}
				else {
					g_line.erase(g_line.begin() + index_line);
					g_line.erase(g_line.begin() + i - 1);
					make_Polygon(rect, m_pos.first, m_pos.second);
					return;
					
				}
			}
		}
		for (int i = 0; i < g_Triangle.size(); ++i) {
			if (abs(m_pos.first - (g_Triangle[i].move_pos.x)) <= 0.05 && abs(m_pos.second - (g_Triangle[i].move_pos.y)) <= 0.05) 
			{
				num += 3;
				if (!check_triangle) {
					index_triangle = i;
					check_triangle = TRUE;
				}
				else {
					g_Triangle.erase(g_Triangle.begin() + index_triangle);
					g_Triangle.erase(g_Triangle.begin() + i - 1);
					make_Polygon(point, m_pos.first, m_pos.second);
					return;
				}
			}
		}
		for (int i = 0; i < g_rect.size(); ++i) {
			if (abs(m_pos.first - (g_rect[i].move_pos.x)) <= 0.05 && abs(m_pos.second - (g_rect[i].move_pos.y)) <= 0.05) 
			{
				num += 4;
				if (!check_rect) {
					index_rect = i;
					check_rect = TRUE;
				}
				else 
				{
					g_rect.erase(g_rect.begin() + index_rect);
					g_rect.erase(g_rect.begin() + i - 1);
					make_Polygon(triangle, m_pos.first, m_pos.second);
					return;
				}
			}
		}
		for (int i = 0; i < g_pentagon.size(); ++i) {
			if (abs(m_pos.first - (g_pentagon[i].move_pos.x)) <= 0.05 && abs(m_pos.second - (g_pentagon[i].move_pos.y)) <= 0.05) 
			{
				num += 5;
				if (!check_pentagon) {
					index_pentagon = i;
					check_pentagon = TRUE;
				}
				else {
					g_pentagon.erase(g_pentagon.begin() + index_pentagon);
					g_pentagon.erase(g_pentagon.begin() + i - 1);
					make_Polygon(pentagon, m_pos.first, m_pos.second);
					return;
				}
			}
		}
		{
			if (check_point && check_line)
			{
				g_point.erase(g_point.begin() + index_point);
				g_line.erase(g_line.begin() + index_line);
				make_Polygon(triangle, m_pos.first, m_pos.second);
			}
			else if (check_point && check_triangle)
			{
				g_point.erase(g_point.begin() + index_point);
				g_Triangle.erase(g_Triangle.begin() + index_triangle);
				make_Polygon(rect, m_pos.first, m_pos.second);
			}
			else if (check_point && check_rect)
			{
				g_point.erase(g_point.begin() + index_point);
				g_rect.erase(g_rect.begin() + index_rect);
				make_Polygon(pentagon, m_pos.first, m_pos.second);
			}
			else if (check_point && check_pentagon)
			{
				g_point.erase(g_point.begin() + index_point);
				g_pentagon.erase(g_pentagon.begin() + index_pentagon);
				make_Polygon(point, m_pos.first, m_pos.second);
			}
			else if (check_line && check_triangle)
			{
				g_line.erase(g_line.begin() + index_line);
				g_Triangle.erase(g_Triangle.begin() + index_triangle);
				make_Polygon(pentagon, m_pos.first, m_pos.second);
			}
			else if (check_line && check_rect)
			{
				g_line.erase(g_line.begin() + index_line);
				g_rect.erase(g_rect.begin() + index_rect);
				make_Polygon(point, m_pos.first, m_pos.second);
			}
			else if (check_line && check_pentagon)
			{
				g_line.erase(g_line.begin() + index_line);
				g_pentagon.erase(g_pentagon.begin() + index_pentagon);
				make_Polygon(line, m_pos.first, m_pos.second);
			}
			else if (check_triangle && check_rect)
			{
				g_Triangle.erase(g_Triangle.begin() + index_triangle);
				g_rect.erase(g_rect.begin() + index_rect);
				make_Polygon(line, m_pos.first, m_pos.second);
			}
			else if (check_triangle && check_pentagon)
			{
				g_Triangle.erase(g_Triangle.begin() + index_triangle);
				g_pentagon.erase(g_pentagon.begin() + index_pentagon);
				make_Polygon(triangle, m_pos.first, m_pos.second);
			}
			else if (check_rect && check_pentagon)
			{
				g_rect.erase(g_rect.begin() + index_rect);
				g_pentagon.erase(g_pentagon.begin() + index_pentagon);
				make_Polygon(rect, m_pos.first, m_pos.second);
			}
		}


		for (int i = 0; i < g_point.size(); ++i) {
			g_point[i].catch_object = FALSE;
		}
		for (int i = 0; i < g_line.size(); ++i) {
			g_line[i].catch_object = FALSE;
		}
		for (int i = 0; i < g_Triangle.size(); ++i) {
			g_Triangle[i].catch_object = FALSE;
		}
		for (int i = 0; i < g_rect.size(); ++i) {
			g_rect[i].catch_object = FALSE;
		}
		for (int i = 0; i < g_pentagon.size(); ++i) {
			g_pentagon[i].catch_object = FALSE;
		}
	}
}
GLvoid Motion(int x, int y)
{
	auto move_pos = ConvertWinToGL(x, y);
	for (int i = 0; i < g_point.size(); ++i) {
		if (g_point[i].catch_object)
		{
			float dx = move_pos.first - pos.first;
			float dy = move_pos.second - pos.second;
			g_point[i].move_pos.x += dx;
			g_point[i].move_pos.y += dy;
			pos = move_pos;
		}
	}
	for (int i = 0; i < g_line.size(); ++i) {
		if (g_line[i].catch_object)
		{
			float dx = move_pos.first - pos.first;
			float dy = move_pos.second - pos.second;
			g_line[i].move_pos.x += dx;
			g_line[i].move_pos.y += dy;
			pos = move_pos;
		}
	}
	for (int i = 0; i < g_Triangle.size(); ++i) {
		if (g_Triangle[i].catch_object)
		{
			float dx = move_pos.first - pos.first;
			float dy = move_pos.second - pos.second;
			g_Triangle[i].move_pos.x += dx;
			g_Triangle[i].move_pos.y += dy;
			pos = move_pos;
		}
	}
	for (int i = 0; i < g_rect.size(); ++i) {
		if (g_rect[i].catch_object)
		{
			float dx = move_pos.first - pos.first;
			float dy = move_pos.second - pos.second;
			g_rect[i].move_pos.x += dx;
			g_rect[i].move_pos.y += dy;
			pos = move_pos;
		}
	}
	for (int i = 0; i < g_pentagon.size(); ++i) {
		if (g_pentagon[i].catch_object)
		{
			float dx = move_pos.first - pos.first;
			float dy = move_pos.second - pos.second;
			g_pentagon[i].move_pos.x += dx;
			g_pentagon[i].move_pos.y += dy;
			pos = move_pos;
		}
	}


	glutPostRedisplay();
}

GLvoid Timer(int value)
{
	for (auto& f_angle : g_point)
	{
		if (f_angle.move)
			f_angle.Move_Dialog();
	}
	for (auto& f_angle : g_line)
	{
		if (f_angle.move)
			f_angle.Move_Dialog();
	}
	for (auto& f_angle : g_Triangle)
	{
		if (f_angle.move)
			f_angle.Move_Dialog();
	}
	for (auto& f_angle : g_rect)
	{
		if (f_angle.move)
			f_angle.Move_Dialog();
	}
	for (auto& f_angle : g_pentagon)
	{
		if (f_angle.move)
			f_angle.Move_Dialog();
	}
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
}

void make_vertexShaders() {
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

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
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
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

