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
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
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
	glGenBuffers(1, &spiVbo);//vertex buffer object 생성
	glBindBuffer(GL_ARRAY_BUFFER, spiVbo);//바인딩할 객체

	vector<vec3> vertexes;
	vertexes.reserve(1080);

	float radius = 1.0f;

	pair<float, float> center = make_pair(0, 0);
	for (int i = 0; i < 4; i++) {//어떤 반원을 그릴거임
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxVertexSize, &vertexes[0], GL_STATIC_DRAW);//m_vertex를 size만큼 위에 바인딩 한다는 vbo에 매칭?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glPointSize(2.0f);
}

pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
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
	InitBuffer();
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(ClickFunc);
	glutTimerFunc(17, Timer, 1);
	glutKeyboardFunc(KeyBoardFunc);
	glutMainLoop(); // 이벤트 처리 시작
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
	//--- 변경된 배경색 설정
	glClearColor(color.r, color.g, color.b,1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
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
	}	//wasd로 움직이는 거리+초기좌표


	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
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
	//make_vertexShaders();
	//make_fragmentShaders();
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