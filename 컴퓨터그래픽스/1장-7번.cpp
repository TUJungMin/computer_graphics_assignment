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
enum draw_mode {
	DRAW_POINT = 0,
	DRAW_LINE,
	DRAW_TRIANGLE,
	DRAW_RECT,
	DRAW_NOTHING
};
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
int draw_num = 0;
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);
GLvoid TimerFunc(int value);
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

GLuint vao, vbo2;

vec3 Line_vertex[2] = { {-0.2f,0.0f,0.0f },{0.2f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = { {0.0f,0.2f,0.0f },{-0.17f,-0.1f,0.0f},{0.17f,-0.1f,0.0f} };
vec3 Rect_vertex[4] = { {-0.1f,-0.15f,0.0f },{0.1f,-0.15f,0.0f},{-0.1f,0.15f,0.0f},{0.1f,0.15f,0.0f} };
vec3 Point_vertex = { 0.0f,0.0f,0.0f };



vec3 move_Pos;

struct Point {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;

	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point_vertex), &Point_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 1);

	}
};

struct Line {

	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;

	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(Line_vertex), Line_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}
};

struct Triangle {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;

	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_vertex), Triangle_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);	//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
		glLineWidth(10.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);

	}

};

struct Rect {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;

	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_vertex), Rect_vertex, GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, color.x, color.y, color.z);
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, move_Pos.x + move_coord.x, move_Pos.y + move_coord.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
		glLineWidth(10.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 1, 3);
	}

};
Triangle g_TriangleShape;

Line g_lineShape;

Point g_pointShape;

Rect g_rectShape;
draw_mode g_currentDrawMode = DRAW_NOTHING;
vector <Triangle> g_triangles;
vector <Point> g_points;
vector <Line> g_lines;
vector <Rect> g_rects;
//void InitBuffer(vec3& vertex, GLuint vbo)
//{
//	glGenBuffers(1, &vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
//	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//	//--- triShape �迭�� ������: 9 * float
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex, GL_STATIC_DRAW);
//	GLint vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
//	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
//	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
//	//--- attribute �ε��� 0���� ��밡���ϰ� ��
//	glEnableVertexAttribArray(vertexAttribLoc);
//
//}

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
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
	glutMouseFunc(ClickFunc);
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoardFunc);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����
	
	

	for (auto& t : g_lines)
		t.Draw();
	for (auto& t : g_points)
		t.Draw();
	for (auto& t : g_triangles)
		t.Draw();
	for (auto& t : g_rects)
		t.Draw();

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}



GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		g_currentDrawMode = DRAW_POINT;
		break;
	case 't':
		g_currentDrawMode = DRAW_TRIANGLE;
		break;
	case 'r':
		g_currentDrawMode = DRAW_RECT;
		break;
	case 'l':
		g_currentDrawMode = DRAW_LINE;
		break;
	case 'w':
		move_Pos.y += 0.1f;
		break;
	case 's':
		move_Pos.y -= 0.1f;
		break;
	case 'a':
		move_Pos.x -= 0.1f;
		break;
	case 'd':
		move_Pos.x += 0.1f;
		break;
	case 'c':
		g_lines.clear();
		g_rects.clear();
		g_triangles.clear();
		g_points.clear();
		draw_num = 0;
		g_currentDrawMode = DRAW_NOTHING;
	default:
		break;
	}
	glutPostRedisplay();
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
			case DRAW_LINE:
				Line NEW_LINE;
				NEW_LINE.move_coord = vec3(glPos.first-move_Pos.x, glPos.second-move_Pos.y, 0.0f);		// ���콺 ��ǥ �Է�
				NEW_LINE.color = vec3(urd(dre), urd(dre), urd(dre));				//rgb�� �ֱ�
				NEW_LINE.InitVbo();
				g_lines.push_back(NEW_LINE);
				draw_num++;
				break;

			case DRAW_POINT:
				Point NEW_POINT;
				NEW_POINT.move_coord = vec3(glPos.first - move_Pos.x, glPos.second - move_Pos.y, 0.0f);		// ���콺 ��ǥ �Է�
				NEW_POINT.color = vec3(urd(dre), urd(dre), urd(dre));				//rgb�� �ֱ�
				NEW_POINT.InitVbo();
				g_points.push_back(NEW_POINT);
				draw_num++;
				break;
			case DRAW_TRIANGLE:
				Triangle NEW_TRIANGLE;
				NEW_TRIANGLE.move_coord = vec3(glPos.first - move_Pos.x, glPos.second - move_Pos.y, 0.0f);		// ���콺 ��ǥ �Է�
				NEW_TRIANGLE.color = vec3(urd(dre), urd(dre), urd(dre));				//rgb�� �ֱ�
				NEW_TRIANGLE.InitVbo();
				g_triangles.push_back(NEW_TRIANGLE);
				draw_num++;
				break;
			case DRAW_RECT:
				Rect NEW_RECT;
				NEW_RECT.move_coord = vec3(glPos.first - move_Pos.x, glPos.second - move_Pos.y, 0.0f);		// ���콺 ��ǥ �Է�
				NEW_RECT.color = vec3(urd(dre), urd(dre), urd(dre));				//rgb�� �ֱ�
				NEW_RECT.InitVbo();
				g_rects.push_back(NEW_RECT);
				draw_num++;
				break;
			}
		}
	}

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
	//make_vertexShaders(); //--- ���ؽ� ���̴� �����
	//make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
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