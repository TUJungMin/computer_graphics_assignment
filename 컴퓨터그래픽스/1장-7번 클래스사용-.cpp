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
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��

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
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
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
		glGenBuffers(1, &m_vbo);//vertex buffer object ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);//���ε��� ��ü
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 3, m_vertex, GL_STATIC_DRAW);//m_vertex�� size��ŭ ���� ���ε� �Ѵٴ� vbo�� ��Ī?
	}
	void Draw(vec3 pos, vec3 color) {
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");//���̴����� ��ġ ã��
		glEnableVertexAttribArray(vertexAttribLoc);//���̴����� �� Ư�� ����Ѵٰ� �˸�
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);//�׸� ��ü ���ε�
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);// �׸���, 3���� �׸��Ű� �� �ϳ��� ����� ũ�� (�� 3���ϱ� vec3), ���� ��ġ

		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");//������ ��ġ ã��
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);//������ �ѱ��
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");//���� ����
		glUniform3f(posUniformLoc, pos.x, pos.y, pos.z);//�굵

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

		// ���� ��ǥ ����
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(WIDTH, HEIGHT); // �������� ũ�� ����
	glutCreateWindow("OPENGL"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;

	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	g_TriangleShape.InitVbo();
	g_lineShape.InitVbo();
	g_pointShape.InitVbo();
	g_rectShape.InitVbo();
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(16, TimerFunc, 0);
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutMouseFunc(ClickFunc);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	glUseProgram(shaderProgramID);
	for (auto& t : g_triangles)
		g_TriangleShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_lines)
		g_lineShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_points)
		g_pointShape.Draw(t.m_pos, t.m_color);
	for (auto& t : g_rects)
		g_rectShape.Draw(t.m_pos, t.m_color);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}

void make_vertexShaders() {
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex.glsl");

	//vertex shader ���� ���ְ� ������ �����͸� vertexShader��� ������ ����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//�����ϸ� 0�� �ƴѰ�

	//���̴� ��ü, 1, ���̴� �ҽ� �ڵ�, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, ���ڿ����� ����, ���ڿ����� �迭, ���ڿ����� ���̸� ������ int�迭

	//������ �ؾ�����.
	glCompileShader(vertexShader);

	//���̴� ��� ��� �ƴ���? ���� Ȯ���ϴ� �ڵ�
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0�̸� ������ �ִ�.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
		return;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;
		return;
	}
}

GLuint make_shaderProgram() {
	//vertex, frament shader�� �̹� �������� �� ��Ȳ
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�

	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
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