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
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
GLint result;
GLchar errorLog[512];
vec3 Line_vertex[2] = { {-0.1f,0.0f,0.0f },{0.1f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = {
	{-0.1f, -0.1f, 0.0f},  // ���� �Ʒ� ������
	{0.1f, -0.1f, 0.0f },   // ������ �Ʒ� ������
	{0.0f, 0.1f, 0.0f}     // �� ������
};
vec3 Rect_vertex[4] = {
	{-0.1f,-0.1f,0.0f},
	{0.1f,-0.1f,0.0f},
	{-0.1f,0.1f,0.0f},				//Rect_vertex[2],[3].x����������
	{0.1f,0.1f,0.0f}
};
vec3 Pentagon_vertex[5] = {
	{-0.1f,0.05f,0.0f},
	{-0.05f,-0.05f,0.0f},
	{0.0f,0.15f,0.0f},				//Rect_vertex[2],[3].x����������
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
		glUniform3f(posUniformLoc, move_pos.x, move_pos.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
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
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
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
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(200, 200); // �������� ��ġ ����
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
	initialize_triangle();
	initialize_rect();
	initialize_pentagon();
	initialize_line();
	initialize_point();
	glutMouseFunc(ClickFunc);
	glutMotionFunc(Motion);
	glutTimerFunc(20, Timer, 1);
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	//glutTimerFunc(20, Timer, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����
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

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
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
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�

	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

