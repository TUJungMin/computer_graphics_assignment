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
	DIALOG = 0,
	ZIGZAG,
	RECT_SPIRAL,
	CIR_SPIRAL,
	NOTHING
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
draw_mode draw_now = NOTHING;
float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
bool make_triangle = FALSE;
GLvoid Timer(int value);
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
vec3 Triangle_vertex[3] = { {0.0f,0.15f,0.0f },
							{-0.1f,-0.15f,0.0f},
							{0.1f,-0.15f,0.0f} };

int make_triangle_num = 0;
struct Triangle {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	GLfloat DX_DY[2] = { 1.0f,1.0f };
	GLfloat move_x = 0;
	GLfloat move_y = 0;
	GLfloat interval[2] = { 1.0f,1.0f };
	vec3 move_coord_first;
	GLfloat angle = 0;
	GLfloat radius = 0;
	float theta = 0;

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
		glUniform3f(posUniformLoc, move_coord.x, move_coord.y, 0);
		int move_posUniformLoc = glGetUniformLocation(shaderProgramID, "theta");
		glUniform1f(move_posUniformLoc, theta);
		glLineWidth(5.0f);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	void Move_Dialog()
	{
		if (move_coord.x > 1.0f) {
			DX_DY[0] *= -1;
			theta = 90.0f;
		}
		else if (move_coord.x < -1.0f) {
			DX_DY[0] *= -1;
			theta = -90.0f;
		}
		if (move_coord.y > 1.0f) {
			DX_DY[1] *= -1;
			theta = 180.0f;
		}
		else if (move_coord.y < -1.0f) {
			DX_DY[1] *= -1;
			theta = 0.0f;
		}

		move_coord.x = move_coord.x + DX_DY[0] * 0.01f;
		move_coord.y = move_coord.y + DX_DY[1] * 0.01f;

	}

	void Move_Zigzag()
	{
		
		if (move_coord.x > 1.0f) {
			DX_DY[0] *= -1;
			move_coord.y = move_coord.y + DX_DY[1] * 0.1f;
			theta = 90.0f;
		}
		else if (move_coord.x < -1.0f) {
			DX_DY[0] *= -1;
			move_coord.y = move_coord.y + DX_DY[1] * 0.1f;
			theta = -90.0f;
		}
		if (move_coord.y > 1.0f) {
			DX_DY[1] *= -1;
			move_coord.y = move_coord.y + DX_DY[1] * 0.1f;
			theta = 180.0f;
		}
		else if (move_coord.y < -1.0f) {
			DX_DY[1] *= -1;
			move_coord.y = move_coord.y + DX_DY[1] * 0.1f;
			theta = 0.0f;
		}
		move_coord.x = move_coord.x + DX_DY[0] * 0.01f;
	}

	void Move_RectSpiral()
	{
		move_coord.x = move_coord.x + DX_DY[0] * 0.01f;
		move_x += DX_DY[0] * 0.01f;
		move_coord.y = move_coord.y + DX_DY[1] * 0.01f;
		move_y += DX_DY[1] * 0.01f;

		if (abs(move_x) > interval[0])
		{
			if (move_x == abs(move_x))
				theta = 0;
			else
				theta = -180;
			interval[0] -= 0.1f;
			DX_DY[1] = DX_DY[0];
			move_x = 0;
			DX_DY[0] = 0;
		}
		else if (abs(move_y) > interval[1])
		{
			if (move_y == abs(move_y))
				theta = 90;
			else
				theta = -90;
			interval[1] -= 0.1f;
			DX_DY[0] = -1 * DX_DY[1];
			move_y = 0;
			DX_DY[1] = 0;
		}
		if (interval[0] < 0 || interval[1] < 0)
		{
			interval[0] = 1.0f;
			interval[1] = 1.0f;
			DX_DY[0] = -1.0f;
			DX_DY[1] = 0.0f;
			move_x = 0;
			move_y = 0;
			move_coord.x += 0.5;
			move_coord.y += 0.6;
			
		}
	}

	void Move_circle()
	{
		move_coord.x += radius * sin(angle + 3.14f + (3.14f * 1 / 4));
		move_coord.y += radius * cos(angle + 3.14f + (3.14f * 1 / 4));
		angle += 3.14f / 10.0f;
		theta = theta-angle*radius;
		radius +=0.0002f;
		if (radius > 0.15f)
		{
			theta = 0;
			radius = 0;
			angle = 0;
			move_coord.x = move_coord_first.x;
			move_coord.y = move_coord_first.y;
		}
	}

};
vector <Triangle> g_triangles;

void initialize_triangle(){	for (auto& a : g_triangles) {		a.angle = 0;		a.radius = 0;		a.theta = 0;		a.move_x = 0;		a.move_y = 0;	}}
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
	initialize_triangle();
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(ClickFunc);
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(17, Timer, 1);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID);

	for (auto& t : g_triangles)
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
	random_device rd;
	default_random_engine dre(rd());
	uniform_int_distribution<int> urd(-1, 1);
	switch (key)
	{

	case '1':
		initialize_triangle();
		for (auto& a : g_triangles) {
			for (int i = 0; i < 2; i++)
			{
				a.DX_DY[i] = urd(dre);
				while (a.DX_DY[i] == 0)
					a.DX_DY[i] = urd(dre);
			}
		}
		draw_now = DIALOG;


		break;

	case '2':
		initialize_triangle();
		for (auto& a : g_triangles) {
			for (int i = 0; i < 2; i++)
			{
				a.DX_DY[i] = urd(dre);
				while (a.DX_DY[i] == 0)
					a.DX_DY[i] = urd(dre);
			}
		}
		draw_now = ZIGZAG;
		break;
	case '3':
		initialize_triangle();
		for (int i = 0; i < g_triangles.size(); ++i)
		{
			g_triangles[i].DX_DY[0] = -1.0f;
			g_triangles[i].DX_DY[1] = 0.0f;
		}
		g_triangles[0].move_coord = { 0,1,0 };
		g_triangles[1].move_coord = { 1,1,0 };
		g_triangles[2].move_coord = { 0,0,0 };
		g_triangles[3].move_coord = { 1,0,0 };
		draw_now = RECT_SPIRAL;
		break;
	case '4':
		initialize_triangle();
		draw_now = CIR_SPIRAL;

		break;
	default:
		break;
	}

	glutPostRedisplay();
}
GLvoid Timer(int value)
{
	switch (draw_now)
	{
	case DIALOG:
		for (auto& a : g_triangles)
			a.Move_Dialog();
		break;
	case ZIGZAG:
		for (auto& a : g_triangles)
			a.Move_Zigzag();
		break;
	case RECT_SPIRAL:
		for (auto& a : g_triangles)
			a.Move_RectSpiral();
		break;
	case CIR_SPIRAL:
		for (auto& a : g_triangles)
			a.Move_circle();
		break;
	case NOTHING:
		break;
	default:
		break;
	}
	glutPostRedisplay();
	glutTimerFunc(17, Timer, 1);
}

GLvoid ClickFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		auto glPos = ConvertWinToGL(x, y);
		if (make_triangle_num < 4) {
			Triangle temp;
			temp.move_coord.x = glPos.first;
			temp.move_coord.y = glPos.second;
			temp.color = vec3(urd(dre), urd(dre), urd(dre));
			temp.move_coord_first = temp.move_coord;
			temp.InitVbo();
			g_triangles.push_back(temp);
			make_triangle_num++;
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