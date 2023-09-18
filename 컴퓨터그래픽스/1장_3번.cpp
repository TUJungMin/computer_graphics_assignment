#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <utility>
#include <vector>

using namespace std;
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
	float size = 100;
	int prior;
};
int windowXSize = 1600;
int windowYSize = 800;
vector<Rect> rect;

pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}
void makeRect(vector <Rect>* rect, int& value)
{
	Rect temp;
	temp.r = (rand() % 256) / 256.0f;
	temp.g = (rand() % 256) / 256.0f;
	temp.b = (rand() % 256) / 256.0f;
	temp.LD.x = rand() & (windowXSize - 100); temp.LD.y = rand() & (windowYSize - 100);
	temp.RT.x = temp.LD.x + 100; temp.RT.y = temp.LD.y - 100;
	pair<float, float> glCoordsLD = ConvertWindowToGL_X(temp.LD.x, temp.LD.y);
	pair<float, float> glCoordsRT = ConvertWindowToGL_X(temp.RT.x, temp.RT.y);
	temp.LDGL.x = glCoordsLD.first; temp.LDGL.y = glCoordsLD.second; temp.RTGL.x = glCoordsRT.first; temp.RTGL.y = glCoordsRT.second;
	temp.prior = value;
	value++;


	rect->push_back(temp);
}
bool Rectin(Point LD, Point RT, int x, int y)
{
	if ((LD.x < x && x < RT.x) && (RT.y < y && y < LD.y)) {
		return false;
	}
	else return true;
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
float red, green, blue;
bool left_button;
int value;
int SelectNum;
void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(windowXSize, windowYSize); // 윈도우의 크기 지정
	glutCreateWindow("Example2"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;

		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	srand(unsigned(time(NULL)));
	red = (rand() % 256) / 256.0f;
	green = (rand() % 256) / 256.0f;
	blue = (rand() % 256) / 256.0f;
	{
		Rect temp;
		temp.r = (rand() % 256) / 256.0f;
		temp.g = (rand() % 256) / 256.0f;
		temp.b = (rand() % 256) / 256.0f;
		temp.LD.x = windowXSize / 2 - temp.size / 2; temp.LD.y = windowYSize / 2 - temp.size / 2;
		temp.RT.x = temp.LD.x + 100; temp.RT.y = temp.LD.y - 100;
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(temp.LD.x, temp.LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(temp.RT.x, temp.RT.y);
		temp.LDGL.x = glCoordsLD.first; temp.LDGL.y = glCoordsLD.second; temp.RTGL.x = glCoordsRT.first; temp.RTGL.y = glCoordsRT.second;
		temp.prior = value;
		value++;
		rect.push_back(temp);
	}

	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);// 다시 그리기 함수 지정
	
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() {//--- 콜백 함수: 그리기 콜백 함수 {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	//makeRect(&rect,value);

	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
	for (int i = 0; i < rect.size(); i++) {
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < rect.size(); ++i) {
			if (Rectin(rect[i].LD, rect[i].RT, x, y) == false) {
				left_button = true;
			}

		}
	
		
	}
	else
		left_button = false;
}
GLvoid Keyboard(unsigned char key, int x, int y)
{

	switch (key) {

	case 'a':
		if (rect.size() < 5) {
			makeRect(&rect, value);
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;

	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}


GLvoid Motion(int x, int y)
{
	vector<int> RectinNum;


	if (left_button) {
		for (int i = 0; i < rect.size(); ++i) {
			if (Rectin(rect[i].LD, rect[i].RT, x, y) == false) {
				RectinNum.push_back(i);
				for (int j = i + 1; j < rect.size(); ++j) {
					if (Rectin(rect[j].LD, rect[j].RT, x, y) == false) {
						RectinNum.push_back(j);
					}

				}
				break;
			}
		}
		SelectNum = RectinNum.back();

		pair<float, float> glCoordsLD = ConvertWindowToGL_X(x - 50, y - 50);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(x + 50, y + 50);
		rect[SelectNum].LDGL.x = glCoordsLD.first;
		rect[SelectNum].LDGL.y = glCoordsLD.second;
		rect[SelectNum].RTGL.x = glCoordsRT.first;
		rect[SelectNum].RTGL.y = glCoordsRT.second;

		rect[SelectNum].LD.x = x - 50;
		rect[SelectNum].LD.y = y + 50;
		rect[SelectNum].RT.x = x + 50;
		rect[SelectNum].RT.y = y - 50;

	}
	glutPostRedisplay(); // 화면을 다시 그리도록 요청
	
}


