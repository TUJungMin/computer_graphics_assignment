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

	DRAW_LINE = 0,
	DRAW_TRIANGLE,
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
vec3 Line_vertex[2] = { {-0.2f,0.0f,0.0f },{0.2f,0.0f,0.0f} };
vec3 Triangle_vertex[3] = { {0.0f,0.2f,0.0f },{-0.17f,-0.1f,0.0f},{0.17f,-0.1f,0.0f} };
vec3 raw[2] = { { -1,0,0 }, { 1,0,0 } };
vec3 col[2] = { { 0,-1,0 }, { 0,1,0 } };


bool button_l = TRUE;
struct Sector {
	vec3 move_coord;
	GLuint m_vbo;

	void InitVbo(vec3* a) {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), a, GL_STATIC_DRAW);
	}
	void Draw() {

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glEnableVertexAttribArray(vertexAttribLoc);
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "uColor");
		glUniform3f(colorUniformLoc, 1.0f, 0, 0);
		int UvertexUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(UvertexUniformLoc, 0, 0, 0);
		int scaleUniformLoc = glGetUniformLocation(shaderProgramID, "scale");
		glUniform3f(scaleUniformLoc, 1, 1, 0);

		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);
		
	}

};
struct Triangle {
	vec3 move_coord;
	vec3 color;
	GLuint m_vbo;
	vec3 SCALE = { 1,1,1 };
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
		glUniform3f(posUniformLoc,  move_coord.x, move_coord.y,0);
		int SCALEUniformLoc = glGetUniformLocation(shaderProgramID, "scale");
		glUniform3f(SCALEUniformLoc, SCALE.x, SCALE.y, 0);		//wasd�� �����̴� �Ÿ�+�ʱ���ǥ
		glLineWidth(5.0f);
		if (button_l) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else
		glDrawArrays(GL_LINE_LOOP, 0, 3);

	}
	void Setscale()
	{
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0.1, 1);

		SCALE.x = urd(dre);
		SCALE.y = urd(dre);
	}

};

Sector RAW;
Sector COL;
Triangle g_TriangleShape;
Triangle g_triangles[4];

void initialize_triangle(){	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> urd(0, 1);	for (int i = 0; i<2; i++)	{		for (int j = 0; j < 2; j++) {			g_triangles[2 * i + j].move_coord = vec3(-0.5 + 1 * j, -0.5 + 1 * i, 0);			g_triangles[2 * i + j].color = vec3(urd(dre), urd(dre), urd(dre));			g_triangles[2 * i + j].InitVbo();		}	}}

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
	RAW.InitVbo(raw);
	COL.InitVbo(col);
	initialize_triangle();
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
	glUseProgram(shaderProgramID);
	RAW.Draw();	COL.Draw();

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
	switch (key)
	{
	case 'a':
		button_l = !button_l;
		break;

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

		if (-1.0f < glPos.first && glPos.first < 0 && -1.0f < glPos.second && glPos.second < 0)  //1�� �и�
		{
			g_triangles[0].move_coord.x = glPos.first;
			g_triangles[0].move_coord.y = glPos.second;
			
			g_triangles[0].Setscale();
		}

		else if (0 < glPos.first && glPos.first < 1.0f && -1.0f < glPos.second && glPos.second < 0)  //2�� �и�
		{
			g_triangles[1].move_coord.x = glPos.first;
			g_triangles[1].move_coord.y = glPos.second;
			g_triangles[1].Setscale();
		}

		if (-1.0f < glPos.first && glPos.first < 0 && 0 < glPos.second && glPos.second < 1.0f)  //3�� �и�
		{
			g_triangles[2].move_coord.x = glPos.first;
			g_triangles[2].move_coord.y = glPos.second;
			g_triangles[2].Setscale();
		}

		if (0 < glPos.first && glPos.first < 1.0f && 0 < glPos.second && glPos.second < 1.0f)  //4�� �и�
		{
			g_triangles[3].move_coord.x = glPos.first;
			g_triangles[3].move_coord.y = glPos.second;
			g_triangles[3].Setscale();
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