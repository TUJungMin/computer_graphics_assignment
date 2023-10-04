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

float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
float r = 0, g = 0, b = 1.0;

enum DRAW_MODE {
	DRAW_TRIANGLE,
	DRAW_LINE,
	DRAW_POINT,
	DRAW_RECTANGLE,
	NOTHING
};


void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
//void InitBuffer();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid TimerFunc(int value);
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}

class Triangle {
	vec3 m_vertex[3];
	GLuint m_vbo;
public:
	Triangle() {
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0;
		m_vertex[0].x = 0.0f;
		m_vertex[0].y = 2.0f;

		m_vertex[1].x = -1.7f;
		m_vertex[1].y = -1.0f;

		m_vertex[2].x = 1.7f;
		m_vertex[2].y = -1.0f;
		Scale(0.1f);
	}
private:
	void Scale(float sValue) {
		for (int i = 0; i < 3; i++) {
			m_vertex[i].x *= sValue;
			m_vertex[i].y *= sValue;
			m_vertex[i].z *= sValue;
		}
	}
public:
	void InitVbo() {
		glGenBuffers(1, &m_vbo);//vertex buffer object 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);//바인딩할 객체
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 3, m_vertex, GL_STATIC_DRAW);//m_vertex를 size만큼 위에 바인딩 한다는 vbo에 매칭?
	}
	void Draw(vec3 pos, vec3 color) {
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");//세이더에서 위치 찾기
		glEnableVertexAttribArray(vertexAttribLoc);//세이더에서 이 특성 사용한다고 알림
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);//그릴 객체 바인딩
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);// 그리기, 3개씩 그릴거고 점 하나당 사용할 크기 (점 3개니까 vec3), 시작 위치

		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");//유니폼 위치 찾기
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);//데이터 넘기기
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");//위와 같음
		glUniform3f(posUniformLoc, pos.x, pos.y, pos.z);//얘도

		glDrawArrays(GL_TRIANGLES, 0, 3);

	}
};
class Line {
	vec3 m_vertex[2];
	GLuint m_vbo;
public:
	Line() {
		m_vertex[0].z = m_vertex[1].z = 0;
		m_vertex[0].x = -2.0f;
		m_vertex[0].y = 0.0f;

		m_vertex[1].x = 2.0f;
		m_vertex[1].y = 0.0f;

		Scale(0.1f);
	}
public:
	void Scale(float sValue) {
		for (int i = 0; i < 2; i++) {
			m_vertex[i].x *= sValue;
			m_vertex[i].y *= sValue;
		}
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 2, m_vertex, GL_STATIC_DRAW);
	}
	void Draw(vec3 pos, vec3 color) {
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, pos.x, pos.y, pos.z);

		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);
	}
};
class Point {
	vec3 m_vertex;

	GLuint m_vbo;
public:
	Point() {
		m_vertex.z = 0;
		m_vertex.x = 0.0f;
		m_vertex.y = 0.0f;
	}
public:
	void InitVbo() {
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), &m_vertex, GL_STATIC_DRAW);
	}
	void Draw(vec3 pos, vec3 color) {
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, pos.x, pos.y, pos.z);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 1);
	}
};
class Rect {
	vec3 m_vertex[4];
	GLuint m_vbo;

public:
	Rect() {
		m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = m_vertex[3].z = 0;

		// 정점 좌표 설정
		m_vertex[0].x = -1.0f;
		m_vertex[0].y = -1.5f;

		m_vertex[1].x = 1.0f;
		m_vertex[1].y = -1.5f;

		m_vertex[2].x = -1.0f;
		m_vertex[2].y = 1.5f;

		m_vertex[3].x = 1.0f;
		m_vertex[3].y = 1.5f;

		Scale(0.1f);
	}
private:
	void Scale(float sValue) {
		for (int i = 0; i < 4; i++) {
			m_vertex[i].x *= sValue;
			m_vertex[i].y *= sValue;
			m_vertex[i].z *= sValue;
		}
	}

public:
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 4, m_vertex, GL_STATIC_DRAW);
	}

	void Draw(vec3 pos, vec3 color) {
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, pos.x, pos.y, pos.z);
		glPointSize(10.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);

	}
};

struct ShapeData {
	vec3 m_pos;
	vec3 m_color;
	ShapeData() {
		m_pos.x = m_pos.y = m_pos.z = 0;
		m_color.x = m_color.y = m_color.z = 0;
	}
	ShapeData(vec3 pos, vec3 color) {
		m_pos = pos;
		m_color = color;
	}
	void Move(float dx, float dy)
	{
		m_pos.x += dx;
		m_pos.y += dy;
	}
};

Triangle g_TriangleShape;

Line g_lineShape;

Point g_pointShape;

Rect g_rectShape;

vector<ShapeData> g_triangles;
vector<ShapeData> g_points;
vector<ShapeData> g_lines;
vector<ShapeData> g_rects;


DRAW_MODE g_currentDrawMode = NOTHING;

int draw_num = 0;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
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
	g_TriangleShape.InitVbo();
	g_lineShape.InitVbo();
	g_pointShape.InitVbo();
	g_rectShape.InitVbo();
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(16, TimerFunc, 0);
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMouseFunc(ClickFunc);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() { //--- 콜백 함수: 그리기 콜백 함수
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	glUseProgram(shaderProgramID);
	for (auto& t : g_triangles)
		g_TriangleShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_lines)
		g_lineShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_points)
		g_pointShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_rects)
		g_rectShape.Draw(t.m_pos, t.m_color);

	glutSwapBuffers(); // 화면에 출력하기
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
	//vertex, frament shader가 이미 컴파일은 된 상황
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}

GLvoid ClickFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		auto glPos = ConvertWinToGL(x, y);
		if (draw_num < 10) {
			switch (g_currentDrawMode)
			{

			case DRAW_TRIANGLE:
				g_triangles.emplace_back(vec3(glPos.first, glPos.second, 0.0f), vec3(urd(dre), urd(dre), urd(dre)));
				draw_num++;
				break;
			case DRAW_LINE:
				g_lines.emplace_back(vec3(glPos.first, glPos.second, 0.0f), vec3(urd(dre), urd(dre), urd(dre)));
				draw_num++;
				break;
			case DRAW_POINT:
				g_points.emplace_back(vec3(glPos.first, glPos.second, 0.0f), vec3(urd(dre), urd(dre), urd(dre)));
				draw_num++;
				break;
			case DRAW_RECTANGLE:
				g_rects.emplace_back(vec3(glPos.first, glPos.second, 0.0f), vec3(urd(dre), urd(dre), urd(dre)));
				draw_num++;
			default:
				break;

			}
		}
	}
	//glutPostRedisplay();
}

GLvoid TimerFunc(int value)
{
	switch (value)
	{
	case 0:
	{
		glutPostRedisplay();
		glutTimerFunc(16, TimerFunc, 0);
	}
	break;
	default:
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'W':
	case 'w':
	{
		for (auto& t : g_triangles)
			t.Move(0, 0.08f);
		for (auto& t : g_lines)
			t.Move(0, 0.08f);
		for (auto& t : g_points)
			t.Move(0, 0.08f);
		for (auto& t : g_rects)
			t.Move(0, 0.08f);
	}
	break;
	case 's':
	case 'S':
	{
		for (auto& t : g_triangles)
			t.Move(0, -0.08f);
		for (auto& t : g_lines)
			t.Move(0, -0.08f);
		for (auto& t : g_points)
			t.Move(0, -0.08f);
		for (auto& t : g_rects)
			t.Move(0, -0.08f);
	}
	break;
	case 'a':
	case 'A':
	{
		for (auto& t : g_triangles)
			t.Move(-0.08f, 0);
		for (auto& t : g_lines)
			t.Move(-0.08f, 0);
		for (auto& t : g_points)
			t.Move(-0.08f, 0);
		for (auto& t : g_rects)
			t.Move(-0.08f, 0);
	}
	break;
	case 'd':
	case 'D':
	{
		for (auto& t : g_triangles)
			t.Move(0.08f, 0);
		for (auto& t : g_lines)
			t.Move(0.08f, 0);
		for (auto& t : g_points)
			t.Move(0.08f, 0);
		for (auto& t : g_rects)
			t.Move(0.08f, 0);
	}
	break;
	case 'r':
	case 'R':
	{
		g_currentDrawMode = DRAW_RECTANGLE;
	}
	break;
	case 'p':
	case 'P':
	{
		g_currentDrawMode = DRAW_POINT;
	}
	break;
	case 't':
	case 'T':
	{
		g_currentDrawMode = DRAW_TRIANGLE;
	}
	break;
	case 'l':
	case 'L':
	{
		g_currentDrawMode = DRAW_LINE;
	}
	break;

	case 'c':
	case 'C':
		g_triangles.clear();
		g_points.clear();
		g_lines.clear();
		g_rects.clear();
		draw_num = 0;
		g_currentDrawMode = NOTHING;

		break;
	default:
		break;
	}
}