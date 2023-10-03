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

float WIDTH = 1200;
float HEIGHT = 1000;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
GLvoid ClickFunc(int button, int state, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Timer(int value);
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
GLfloat maxVertexSize;
GLuint spiVbo;
vec3 mouse_pos;
struct cir_spiral {
	GLfloat coord_x;
	GLfloat coord_y;
	GLfloat theta = 0;
	int current_size = 0;;
};
vector <cir_spiral> circle_spiral;
bool button_p, button_l;
void InitBuffer()
{
	glGenBuffers(1, &spiVbo);//vertex buffer object ����
	glBindBuffer(GL_ARRAY_BUFFER, spiVbo);//���ε��� ��ü

	vector<vec3> vertexes;
	vertexes.reserve(1080);

	float radius = 1.0f;

	pair<float, float> center = make_pair(0, 0);
	for (int i = 0; i < 4; i++) {//� �ݿ��� �׸�����
		float currentTheta = 0.0f;
		if (i % 2 != 0)
			currentTheta = 180.0f;
		for (int theta = 0; theta < 61; theta++) {
			float x = radius * cos(currentTheta * 3.14f / 180.0f);
			float y = radius * sin(currentTheta * 3.14f / 180.0f);
			x += center.first;
			vertexes.emplace_back(x, y, 0);
			currentTheta += 3.0f;

		}
		if (i % 2 != 0) center.first -= radius;
		else center.first += radius;
		radius *= 2.0f;
	}
	auto backIdx = vertexes.back();
	for (auto& p : vertexes) {
		p.x -= backIdx.x;
	}

	vector<vec3> reverseVertexes = vertexes;
	reverse(reverseVertexes.begin(), reverseVertexes.end());

	for (auto& p : reverseVertexes) {
		p.x *= -1.0f;
		p.y *= -1.0f;
		vertexes.push_back(p);
	}
	for (auto& p : vertexes) {
		p.x *= 0.03f;
		p.y *= 0.03f;
	}
	maxVertexSize = vertexes.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxVertexSize, &vertexes[0], GL_STATIC_DRAW);//m_vertex�� size��ŭ ���� ���ε� �Ѵٴ� vbo�� ��Ī?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glPointSize(2.0f);
}

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}

void make_sprial(int num)
{
	circle_spiral.clear();

	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> x_pos(200, 800);
	uniform_real_distribution<float> y_pos(200, 800);
	for (int i = 0; i < num; ++i) {
		auto glPos = ConvertWinToGL(x_pos(dre), y_pos(dre));
		cir_spiral temp;
		temp.coord_x = glPos.first;
		temp.coord_y = glPos.second;
		temp.current_size = 1;
		circle_spiral.push_back(temp);
	}
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
	InitBuffer();
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(ClickFunc);
	glutTimerFunc(17, Timer, 1);
	glutKeyboardFunc(KeyBoardFunc);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
vec3 color;

void change_color()
{
	random_device rd;
	default_random_engine dre(rd());
	uniform_real_distribution<float> random(0, 1);

	color.r = random(dre);
	color.g = random(dre);
	color.b = random(dre);
}
GLvoid drawScene()
{
	//--- ����� ���� ����
	glClearColor(color.r, color.g, color.b,1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID);

	int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
	glEnableVertexAttribArray(vertexAttribLoc);
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

	for (int i = 0; i < circle_spiral.size(); i++) {
		int posUniformLoc = glGetUniformLocation(shaderProgramID, "uPos");
		glUniform3f(posUniformLoc, circle_spiral[i].coord_x, circle_spiral[i].coord_y, 0);
		int angleUniformLoc = glGetUniformLocation(shaderProgramID, "theta");
		glUniform1f(angleUniformLoc,circle_spiral[i].theta);
		if (button_p) {
			glPointSize(3.0f);
			glDrawArrays(GL_POINTS, 0, circle_spiral[i].current_size);
		}
		else if (button_l)
		{
			glLineWidth(5.0f);

			for (int j = 0; j < circle_spiral[i].current_size - 1; ++j)
			{
				glDrawArrays(GL_LINES, j, 2);

			}
		}
	}	//wasd�� �����̴� �Ÿ�+�ʱ���ǥ


	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	//random_device rd;
	//default_random_engine dre(rd());
	//uniform_int_distribution<int> urd(-1, 1);

	switch (key)
	{
	case 'p':
		button_p = TRUE;
		button_l = FALSE;
		break;
	case 'l':

		button_p = FALSE;
		button_l = TRUE;
		break;
	case '1':
		change_color();
		make_sprial(1);
		break;

	case '2':
		change_color();
		make_sprial(2);
		break;
	case '3':
		change_color();
		make_sprial(3);
		break;
	case '4':
		change_color();
		make_sprial(4);
		break;
	case '5':
		change_color();
		make_sprial(5);

		break;
	case '+':
		for (int i = 0; i < circle_spiral.size(); ++i) {
			circle_spiral[i].theta += 1.0f;
		}
		break;

	case '-':
		for (int i = 0; i < circle_spiral.size(); ++i) {
			circle_spiral[i].theta -= 1.0f;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
GLvoid Timer(int value)
{
	for (int i = 0; i < circle_spiral.size(); ++i) {
		if (circle_spiral[i].current_size < maxVertexSize)
			circle_spiral[i].current_size++;
	}
	
	glutPostRedisplay();
	glutTimerFunc(17, Timer, 1);
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
	//make_vertexShaders();
	//make_fragmentShaders();
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

GLvoid ClickFunc(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		circle_spiral.clear();
		change_color();
		auto glPos = ConvertWinToGL(x, y);
		cir_spiral temp;
		temp.coord_x = glPos.first;
		temp.coord_y = glPos.second;
		temp.current_size = 0;
		circle_spiral.push_back(temp);
	}

}