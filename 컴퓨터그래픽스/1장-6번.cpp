#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <utility>
#include <vector>
#include <math.h>
#include <random>
#include <time.h>
#define rect_size 100
using namespace std;

enum TIMER_CASE {    // 열거형 정의
	LRUD = 0,
	DIALOG_MOVE,
	MIX_MOVE
};

enum MOVE_CASE {    // 열거형 정의
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	LUDIALOG,
	RUDIALOG,
	LBDIALOG,
	RBDIALOG,
	STOP
};
int windowXSize = 1600;
int windowYSize = 800;
random_device rd;  // 무작위 시드 생성
mt19937 gen(rd());  // 메르센 트위스터(Mersenne Twister) 엔진을 사용한 난수 생성기
uniform_real_distribution<float> color(0, 1);
uniform_real_distribution<float> size_x(0, 1400);
uniform_real_distribution<float> size_y(200, 800);
uniform_int_distribution<int> timercase(0, 2);
uniform_int_distribution<int> Movecase(0, 7);
struct Point {
	float x;
	float y;
};
struct Rect {
	Point LD;		//좌하단 좌표
	Point RT;		//우하단 좌표
	Point LDGL;		//좌하단 GL변환 좌표
	Point RTGL;
	float r, g, b;
	float size = 200;
	int move_type;
	bool isActive = false;
	void DecreaseSize() {

	}
};
GLfloat speed[9] = { 3.0f,3.0f,3.0f,3.0f,3.0f,3.0f,3.0f,3.0f ,3.0f };
vector<Rect> rect;
vector<Rect> seperate_rect;
pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}

GLvoid convertGL(Rect& rectangle) {
	pair<float, float> glCoordsLD = ConvertWindowToGL_X(rectangle.LD.x, rectangle.LD.y);
	pair<float, float> glCoordsRT = ConvertWindowToGL_X(rectangle.RT.x, rectangle.RT.y);
	rectangle.LDGL.x = glCoordsLD.first; rectangle.LDGL.y = glCoordsLD.second;
	rectangle.RTGL.x = glCoordsRT.first; rectangle.RTGL.y = glCoordsRT.second;
}

GLvoid makeRect()
{

	Rect temp;
	temp.r = color(gen);
	temp.g = color(gen);
	temp.b = color(gen);

	temp.LD.x = size_x(gen); temp.LD.y = size_y(gen);
	temp.RT.x = temp.LD.x + temp.size; temp.RT.y = temp.LD.y - temp.size;
	convertGL(temp);
	temp.isActive = true;
	rect.push_back(temp);
}

bool Rectin(Point LD, Point RT, int x, int y)
{
	if ((LD.x < x && x < RT.x) && (RT.y < y && y < LD.y)) {
		return false;
	}
	else return true;
}

GLvoid initialRect() {
	for (int i = 0; i < 5; ++i) {
		makeRect();
	}
}
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수 {
	glViewport(0, 0, w, h);
	windowXSize = w;
	windowYSize = h;
}

void  Make4SeperateRect(const Rect& rect);
GLvoid drawScene();//--- 콜백 함수: 그리기 콜백 함수 {  // 바탕색을 ‘blue’ 로 지정
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int val);
GLvoid move_LRUD();
GLvoid CheckCollision();
void Make9SeperateRect(const Rect& rect);


bool Press_LRUD, Press_Dialog, Press_Mix;
float red, green, blue;

void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(windowXSize, windowYSize); // 윈도우의 크기 지정
	glutCreateWindow("Example5"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;

		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	red = 1.f;  green = 1.f; blue = 1.f;

	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	initialRect();
	glutKeyboardFunc(Keyboard);// 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() {//--- 콜백 함수: 그리기 콜백 함수 {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기




	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
	for (int i = 0; i < rect.size(); i++) {
		if (!rect[i].isActive)continue;
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	for (int i = 0; i < seperate_rect.size(); i++) {
		if (!seperate_rect[i].isActive)continue;
		glColor3f(seperate_rect[i].r, seperate_rect[i].g, seperate_rect[i].b);
		glRectf(seperate_rect[i].LDGL.x, seperate_rect[i].LDGL.y, seperate_rect[i].RTGL.x, seperate_rect[i].RTGL.y);
	}

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Mouse(int button, int state, int x, int y)
{


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = rect.size() - 1; i >= 0; --i) {
			if (Rectin(rect[i].LD, rect[i].RT, x, y) == false && rect[i].isActive) {

				int Timer_case = timercase(gen);
				switch (Timer_case)
				{
				case LRUD:
				{

					Make4SeperateRect(rect[i]);
					rect[i].isActive = false;
					seperate_rect[0].move_type = LEFT;
					seperate_rect[1].move_type = UP;
					seperate_rect[2].move_type = DOWN;
					seperate_rect[3].move_type = RIGHT;
					glutTimerFunc(10, Timer, LRUD);


					break;
				}
				case DIALOG_MOVE:
					Make4SeperateRect(rect[i]);
					rect[i].isActive = false;
					seperate_rect[0].move_type = LUDIALOG;
					seperate_rect[1].move_type = RUDIALOG;
					seperate_rect[2].move_type = LBDIALOG;
					seperate_rect[3].move_type = RBDIALOG;
					glutTimerFunc(10, Timer, LRUD);

					break;
				case MIX_MOVE:
					Make9SeperateRect(rect[i]);
					rect[i].isActive = false;
					seperate_rect[0].move_type = LUDIALOG;
					seperate_rect[1].move_type = UP;
					seperate_rect[2].move_type = RUDIALOG;
					seperate_rect[3].move_type = LEFT;
					seperate_rect[4].move_type = STOP;
					seperate_rect[5].move_type = RIGHT;
					seperate_rect[6].move_type = LBDIALOG;
					seperate_rect[7].move_type = DOWN;
					seperate_rect[8].move_type = RBDIALOG;
					glutTimerFunc(10, Timer, LRUD);
					break;
				}
				break;
			}
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'r':
		initialRect();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}


GLvoid Timer(int val) {

	move_LRUD();
	if (seperate_rect[3].isActive) {
		glutTimerFunc(10, Timer, LRUD);
	}
	else
	{
		seperate_rect.clear();
		for (auto& a : speed)
		{
			a = 1.0f;
		}
	}
	glutPostRedisplay();  // 화면을 다시 그리도록 요청
}

void Make4SeperateRect(const Rect& rect) {
	Rect temp;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			temp.r = rect.r;
			temp.g = rect.g;
			temp.b = rect.b;
			temp.LD.x = rect.LD.x + j * rect.size / 2; temp.LD.y = rect.LD.y - (1 - i) * rect.size / 2;
			temp.RT.x = rect.RT.x - (1 - j) * rect.size / 2; temp.RT.y = rect.RT.y + i * rect.size / 2;
			convertGL(temp);
			temp.move_type = Movecase(gen);
			temp.isActive = true;
			seperate_rect.push_back(temp);
		}
	}
}

void Make9SeperateRect(const Rect& rect) {
	Rect temp;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			temp.r = rect.r;
			temp.g = rect.g;
			temp.b = rect.b;
			temp.LD.x = rect.LD.x + j * rect.size / 3; temp.LD.y = rect.LD.y - (2 - i) * rect.size / 3;
			temp.RT.x = rect.RT.x - (2 - j) * rect.size / 3; temp.RT.y = rect.RT.y + i * rect.size / 3;
			convertGL(temp);
			temp.move_type = Movecase(gen);
			temp.isActive = true;
			seperate_rect.push_back(temp);
		}
	}
}

GLvoid move_LRUD()
{
	GLfloat num = 0.8f;

	for (int i = 0; i < seperate_rect.size(); ++i) {
		if (seperate_rect[i].LD.y > seperate_rect[i].RT.y && seperate_rect[i].LD.x < seperate_rect[i].RT.x)
		{
			//seperate_rect[i].size -= 1.f;
			switch (seperate_rect[i].move_type)
			{
			case UP:
			{
				CheckCollision();
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) - speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) - speed[i];
				seperate_rect[i].LD.x += num / 2.f;
				seperate_rect[i].RT.x -= num / 2.f;
				convertGL(seperate_rect[i]);
				break;
			}
			case DOWN:
			{
				CheckCollision();
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) + speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) + speed[i];
				seperate_rect[i].LD.x += num / 2.f;
				seperate_rect[i].RT.x -= num / 2.f;
				convertGL(seperate_rect[i]);
				break;
			}
			case LEFT:
			{
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) - speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) - speed[i];
				seperate_rect[i].LD.y -= num / 2.f;
				seperate_rect[i].RT.y += num / 2.f;
				convertGL(seperate_rect[i]);
				break;
			}
			case RIGHT:
			{
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) + speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) + speed[i];
				seperate_rect[i].LD.y -= num / 2.f;
				seperate_rect[i].RT.y += num / 2.f;
				convertGL(seperate_rect[i]);
				break;
			}

			case LUDIALOG:
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) - speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) - speed[i];
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) - speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) - speed[i];
				convertGL(seperate_rect[i]);
				break;

			case RUDIALOG:
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) + speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) + speed[i];
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) - speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) - speed[i];
				convertGL(seperate_rect[i]);
				break;

			case LBDIALOG:
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) - speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) - speed[i];
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) + speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) + speed[i];
				convertGL(seperate_rect[i]);
				break;

			case RBDIALOG:
				CheckCollision();
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f) + speed[i];
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f) + speed[i];
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f) + speed[i];
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f) + speed[i];
				convertGL(seperate_rect[i]);
				break;
			case STOP:
				seperate_rect[i].LD.x = seperate_rect[i].LD.x + (num / 2.f);
				seperate_rect[i].RT.x = seperate_rect[i].RT.x - (num / 2.f);
				seperate_rect[i].LD.y = seperate_rect[i].LD.y - (num / 2.f);
				seperate_rect[i].RT.y = seperate_rect[i].RT.y + (num / 2.f);
				convertGL(seperate_rect[i]);
			}
		}
		else
		{
			seperate_rect[i].isActive = false;
		}
	}
}


GLvoid CheckCollision()
{
	for (int i = 0; i < seperate_rect.size(); ++i) {
		if (seperate_rect[i].RT.y <= 0)
		{
			speed[i] *= -1;
			seperate_rect[i].RT.y += 2;
			seperate_rect[i].LD.y += 2;
		}
		if (seperate_rect[i].LD.y >= windowYSize)
		{
			speed[i] *= -1;
			seperate_rect[i].LD.y -= 2;
			seperate_rect[i].RT.y -= 2;
		}
		if (seperate_rect[i].LD.x <= 0)
		{
			speed[i] *= -1;
			seperate_rect[i].RT.x += 2;
			seperate_rect[i].LD.x += 2;
		}
		if (seperate_rect[i].RT.x >= windowXSize)
		{
			speed[i] *= -1;
			seperate_rect[i].LD.x -= 2;
			seperate_rect[i].RT.x -= 2;
		}
	}
}
