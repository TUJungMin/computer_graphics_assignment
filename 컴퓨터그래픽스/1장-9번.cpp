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
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
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
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
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
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
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
	initialize_triangle();
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(ClickFunc);
	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(17, Timer, 1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene()
{
	//--- 변경된 배경색 설정
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	for (auto& t : g_triangles)
		t.Draw();


	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
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