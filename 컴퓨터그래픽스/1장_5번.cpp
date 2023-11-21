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
int windowXSize = 1600;
int windowYSize = 800;
random_device rd;  // 무작위 시드 생성
mt19937 gen(rd());  // 메르센 트위스터(Mersenne Twister) 엔진을 사용한 난수 생성기
uniform_real_distribution<float> color(0, 1);
uniform_real_distribution<float> size_x(0, 1500);
uniform_real_distribution<float> size_y(100, 800);
uniform_int_distribution<int> rect_num(20, 40);
struct Point {
	float x;
	float y;
};
struct Rectangle {
	Point LD;		//좌하단 좌표
	Point RT;		//우하단 좌표
	Point LDGL;		//좌하단 GL변환 좌표
	Point RTGL;
	float r, g, b;
	float size = 50;
};

pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}
void makeRect(vector <Rectangle>& rect)
{
	//srand(unsigned(time(NULL)));
	Rectangle temp;
	temp.r = (rand() % 256) / 256.0f;
	temp.g = (rand() % 256) / 256.0f;
	temp.b = (rand() % 256) / 256.0f;

	temp.LD.x = rand() % 1550; temp.LD.y = rand() % 750 + temp.size;
	temp.RT.x = temp.LD.x + temp.size; temp.RT.y = temp.LD.y - temp.size;
	pair<float, float> glCoordsLD = ConvertWindowToGL_X(temp.LD.x, temp.LD.y);
	pair<float, float> glCoordsRT = ConvertWindowToGL_X(temp.RT.x, temp.RT.y);
	temp.LDGL.x = glCoordsLD.first; temp.LDGL.y = glCoordsLD.second; temp.RTGL.x = glCoordsRT.first; temp.RTGL.y = glCoordsRT.second;

	rect.push_back(temp);
}
bool Rectin(Point LD, Point RT, int x, int y)
{
	if ((LD.x < x && x < RT.x) && (RT.y < y && y < LD.y)) {
		return false;
	}
	else return true;
}

void initialRect(vector <Rectangle>& rect) {
	srand(unsigned(time(NULL)));
	int num = rand() % 21 + 20;
	for (int i = 0; i < num; ++i) {
		makeRect(rect);
	}
}
GLvoid drawScene();//--- 콜백 함수: 그리기 콜백 함수 {  // 바탕색을 ‘blue’ 로 지정
GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수 {
	glViewport(0, 0, w, h);
	windowXSize = w;
	windowYSize = h;
}
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int val);
GLvoid Motion(int x, int y);
int interval = 0;
vector<Rectangle> rect;
bool make_rect, alive_Eraser;
float red, green, blue;
bool left_button;
int value;
int SelectNum;
Rectangle eraser;
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

	srand(unsigned(time(NULL)));
	red = 1.f;  green = 1.f; blue = 1.f;
	
	
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	initialRect(rect);

	glutKeyboardFunc(Keyboard);// 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() {//--- 콜백 함수: 그리기 콜백 함수 {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기


	//makeRect(rect);

	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
	for (int i = 0; i < rect.size(); i++) {
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	if (alive_Eraser) {
		glColor3f(eraser.r, eraser.g, eraser.b);

		glRectf(eraser.LDGL.x, eraser.LDGL.y, eraser.RTGL.x, eraser.RTGL.y);
	}

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (make_rect == false) {
			eraser.size = 100;
			eraser.r = 0.f; eraser.g = 0.f; eraser.b = 0.f;
			eraser.LD.x = x - eraser.size/2; eraser.LD.y = y + eraser.size/2;
			eraser.RT.x = eraser.LD.x + eraser.size; eraser.RT.y = eraser.LD.y - eraser.size;
			pair<float, float> glCoordsLD = ConvertWindowToGL_X(x - eraser.size / 2, y - eraser.size / 2);
			pair<float, float> glCoordsRT = ConvertWindowToGL_X(x + eraser.size / 2, y + eraser.size / 2);
			eraser.LDGL.x = glCoordsLD.first;
			eraser.LDGL.y = glCoordsLD.second;
			eraser.RTGL.x = glCoordsRT.first;
			eraser.RTGL.y = glCoordsRT.second;
			make_rect = true;
			alive_Eraser = true;
		}
		if (Rectin(eraser.LD, eraser.RT, x, y) == false) {
			left_button = true;
		}
	}
	else
		left_button = false;

	if (state == GLUT_UP)
	{
		make_rect = false;
		alive_Eraser = false;
	}
	

}
GLvoid Keyboard(unsigned char key, int x, int y)
{

	switch (key) {

	case 'r':
		rect.clear();
		initialRect(rect);
		make_rect = false;
		alive_Eraser = false;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;

	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
GLvoid Motion(int x, int y)
{
	if (left_button) {
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(x - eraser.size/2, y - eraser.size / 2);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(x + eraser.size / 2, y + eraser.size / 2);
		eraser.LDGL.x = glCoordsLD.first;
		eraser.LDGL.y = glCoordsLD.second;
		eraser.RTGL.x = glCoordsRT.first;
		eraser.RTGL.y = glCoordsRT.second;
		
		eraser.LD.x = x - eraser.size/2;
		eraser.LD.y = y + eraser.size/2;
		eraser.RT.x = x + eraser.size/2;
		eraser.RT.y = y - eraser.size/2;
	}
	for (int i = 0; i < rect.size(); ++i) {
		if (eraser.LD.x < rect[i].RT.x && eraser.RT.x > rect[i].LD.x && eraser.RT.y < rect[i].LD.y && eraser.LD.y > rect[i].RT.y) {
			eraser.size += 10;
			eraser.r = rect[i].r; eraser.g = rect[i].g; eraser.b = rect[i].b;
			rect.erase(rect.begin() + i);
		}
	}
	glutPostRedisplay(); // 화면을 다시 그리도록 요청

}

