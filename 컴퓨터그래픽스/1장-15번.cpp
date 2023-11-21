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
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
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
	while (inFile >> std::noskipws >> head[0]) { //��ĭ�� �� �ðڴ�
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
		vertex.push_back(vertexData[(int)(iter->z) - 1]);                     //���ؽ� ��ǥ
	}
	for (auto iter = vtface.begin(); iter < vtface.end(); iter++) {
		vtVertex.push_back(cordinaterData[(int)(iter->x) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->y) - 1]);
		vtVertex.push_back(cordinaterData[(int)(iter->z) - 1]);                //�ؽ��� ��ǥ
	}

	for (auto iter = Noramlface.begin(); iter < Noramlface.end(); iter++) {
		normalVertex.push_back(normalData[(int)(iter->x) - 1]);
		normalVertex.push_back(normalData[(int)(iter->y) - 1]);
		normalVertex.push_back(normalData[(int)(iter->z) - 1]);               //��� ��ǥ
	}
	delete[] vertexData;
	delete[] cordinaterData;
	delete[] normalData;
	inFile.close();
}


enum DrawType {
	CUBE = 0,
	Pyramid
};



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
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
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
vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };

std::vector <glm::vec3> D_vertex;
std::vector <glm::vec3> D_normal;
std::vector <glm::vec2> D_uv;
std::vector <glm::vec3> P_vertex;
std::vector <glm::vec3> P_normal;
std::vector <glm::vec2> P_uv;



pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}


GLvoid Timer(int value);
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 Line_R_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 Line_S_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1; color[0].g = 0; color[0].b = 0;
		color[1].r = 0; color[1].g = 1; color[1].b = 0;
		color[2].r = 0; color[2].g = 0; color[2].b = 1;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(POS), POS, GL_STATIC_DRAW);
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
		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
		}
	}

};
struct Dice {
	bool button_h = FALSE; bool button_w = FALSE; bool button_x = FALSE; bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	vec3 color[6];
	GLuint m_vbo;
	float rotate_x = 1.0f;
	float rotate_y = 1.0f;
	bool Dodraw = FALSE;
	glm::mat4 R_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 S_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 T_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
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
		R_matrix = glm::rotate(R_matrix, glm::radians(-30.0f), glm::vec3(0, 1, 0));//y�� -30��
		R_matrix = glm::rotate(R_matrix, glm::radians(30.0f), glm::vec3(1, 0, 0));//x�� -30��
		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		if (Dodraw) {
			for (int i = 0; i < 6; ++i) {
				//glEnable(GL_CULL_FACE);
				glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
				if (button_h) {
					glEnable(GL_DEPTH_TEST);
					glFrontFace(GL_CCW);
					glEnable(GL_CULL_FACE);
				}
				else {
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
				}

				if (!button_w) {

					glDrawArrays(GL_TRIANGLES, 6 * i, 6);
				}
				else {
					glDrawArrays(GL_LINE_LOOP, 6 * i, 3);
				}

			}
		}


	}





};

struct PYRAMID {
	bool button_h = FALSE; bool button_w = FALSE; bool button_x = FALSE;  bool button_X = FALSE;
	bool button_y = FALSE; bool button_Y = FALSE;
	vec3 color[6];
	float rotate_x = 1.0f;
	float rotate_y = 1.0f;
	GLuint m_vbo;
	bool Dodraw = FALSE;
	glm::mat4 R_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 S_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 T_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	PYRAMID(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color[0].r = 0; color[0].g = 1; color[0].b = 0;
		color[1].r = 0; color[1].g = 0; color[1].b = 1;
		color[2].r = 1; color[2].g = 1; color[2].b = 0;
		color[3].r = 0; color[3].g = 1; color[3].b = 1;
		color[4].r = 1; color[4].g = 0; color[4].b = 0;
		color[5].r = 1; color[5].g = 0; color[5].b = 0;

		S_matrix = glm::scale(S_matrix, glm::vec3(0.5, 0.5, 0.5));
		R_matrix = glm::rotate(R_matrix, glm::radians(-30.0f), glm::vec3(0, 1, 0));//y�� -30��
		R_matrix = glm::rotate(R_matrix, glm::radians(30.0f), glm::vec3(1, 0, 0));//x�� -30��
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * P_vertex.size(), &P_vertex[0], GL_STATIC_DRAW);
	}
	void Draw() {

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");
		unsigned int R_matrixUniformLoc = glGetUniformLocation(shaderProgramID, "uRmatrix");
		glUniformMatrix4fv(R_matrixUniformLoc, 1, GL_FALSE, glm::value_ptr(R_matrix));
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "scale");
		//--- modelTransform ������ ��ȯ �� �����ϱ�
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(S_matrix));

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(T_matrix));

		if (Dodraw) {
			for (int i = 0; i < 6; ++i) {
				//glEnable(GL_CULL_FACE);
				glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
				if (button_h) {
					glEnable(GL_DEPTH_TEST);
					glFrontFace(GL_CCW);
					glEnable(GL_CULL_FACE);
				}
				else {
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
				}
				if (!button_w) {
					glDrawArrays(GL_TRIANGLES, 3 * i, 3);
				}
				else
					glDrawArrays(GL_LINE_LOOP, 3 * i, 3);

			}
		}
	}

};
vector <Dice> dice;
vector <Sector> Line;
vector <PYRAMID> pyramid;
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����

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
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	ReadObj("pyramid.obj", P_vertex, P_normal, P_uv);
	dice.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	pyramid.emplace_back(400, 300);
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(20, Timer, 1);
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	Line[0].Draw();
	dice[0].Draw();
	pyramid[0].Draw();

	glutSwapBuffers(); //ȭ�鿡 ���
}
GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':
		pyramid[0].Dodraw = FALSE;
		dice[0].Dodraw = TRUE;
		break;
	case 'p':
		pyramid[0].Dodraw = TRUE;
		dice[0].Dodraw = FALSE;
		break;
	case 'h':
		if (dice[0].Dodraw) {
			dice[0].button_h = !dice[0].button_h;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_h = !pyramid[0].button_h;
		}
		break;
	case 'w':
		if (dice[0].Dodraw) {
			dice[0].button_w = !dice[0].button_w;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_w = !pyramid[0].button_w;
		}
		break;
	case 'x':
		if (dice[0].Dodraw) {
			dice[0].button_x = !dice[0].button_x;
			dice[0].button_X = FALSE;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_x = !pyramid[0].button_x;
			pyramid[0].button_X = FALSE;
		}
		break;
	case 'X':
		if (dice[0].Dodraw) {
			dice[0].button_X = !dice[0].button_X;
			dice[0].button_x = FALSE;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_X = !pyramid[0].button_X;
			pyramid[0].button_x = FALSE;
		}
		break;
	case 'y':
		if (dice[0].Dodraw) {
			dice[0].button_y = !dice[0].button_y;
			dice[0].button_Y = FALSE;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_y = !pyramid[0].button_y;
			pyramid[0].button_Y = FALSE;
		}
		break;
	case 'Y':
		if (dice[0].Dodraw) {
			dice[0].button_Y = !dice[0].button_Y;
			dice[0].button_y = FALSE;
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_Y = !pyramid[0].button_Y;
			pyramid[0].button_y = FALSE;
		}
		break;
	case 'b':
		if (dice[0].Dodraw) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].T_matrix = glm::translate(pyramid[0].T_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
		}
		break;
	case 'm':
		if (dice[0].Dodraw) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].T_matrix = glm::translate(pyramid[0].T_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
		}
		break;
	case 'n':
		if (dice[0].Dodraw) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0.f, -0.1f, 0.0f));
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].T_matrix = glm::translate(pyramid[0].T_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
		}
		break;
	case 'j':
		if (dice[0].Dodraw) {
			dice[0].T_matrix = glm::translate(dice[0].T_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].T_matrix = glm::translate(pyramid[0].T_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
		}
		break;

	case 's':
		if (dice[0].Dodraw) {
			dice[0].button_x = FALSE; dice[0].button_X = FALSE; dice[0].button_y = FALSE; dice[0].button_Y = FALSE;
			dice[0].R_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
			dice[0].T_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(-30.0f), glm::vec3(0, 1, 0));//y�� -30��
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(30.0f), glm::vec3(1, 0, 0));//x�� -30��
			
		}
		else if (pyramid[0].Dodraw) {
			pyramid[0].button_x = FALSE; pyramid[0].button_X = FALSE; pyramid[0].button_y = FALSE; pyramid[0].button_Y = FALSE;
			pyramid[0].R_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
			pyramid[0].T_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(-30.0f), glm::vec3(0, 1, 0));//y�� -30��
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(30.0f), glm::vec3(1, 0, 0));//x�� -30��

		}
		
		break;


	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Timer(int value)
{
	if (dice[0].Dodraw) {
		if (dice[0].button_x)
		{
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(dice[0].rotate_x), glm::vec3(1, 0, 0));//x�� -30��
		}
		else if (dice[0].button_X)
		{
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(-dice[0].rotate_x), glm::vec3(1, 0, 0));//x�� -30��

		}

		if (dice[0].button_y)
		{
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(dice[0].rotate_y), glm::vec3(0, 1, 0));//y�� -30��
		}
		else if (dice[0].button_Y)
		{
			dice[0].R_matrix = glm::rotate(dice[0].R_matrix, glm::radians(-dice[0].rotate_y), glm::vec3(0, 1, 0));//y�� -30��

		}
	}
	else if (pyramid[0].Dodraw) {
		if (pyramid[0].button_x)
		{
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(pyramid[0].rotate_x), glm::vec3(1, 0, 0));//x�� -30��
		}
		else if (pyramid[0].button_X)
		{
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(-pyramid[0].rotate_x), glm::vec3(1, 0, 0));//x�� -30��

		}

		if (pyramid[0].button_y)
		{
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(pyramid[0].rotate_y), glm::vec3(0, 1, 0));//y�� -30��
		}
		else if (pyramid[0].button_Y)
		{
			pyramid[0].R_matrix = glm::rotate(pyramid[0].R_matrix, glm::radians(-pyramid[0].rotate_y), glm::vec3(0, 1, 0));//y�� -30��

		}
	}
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 1);
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
	vertexSource = filetobuf("vertex2.glsl");

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
	fragmentSource = filetobuf("fragment2.glsl"); // �����׼��̴� �о����
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

