#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <utility>

using namespace std;
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
	float size = 400;
};
int windowXSize = 800;
int windowYSize = 800;
Rectangle rect[4];
void makeRect(Rectangle* rect)
{
	rect[0].LD.x = rect[0].size - windowXSize / 2;
	rect[0].LD.y = windowXSize - rect[0].size;
	rect[0].RT.x = windowXSize - rect[0].size;
	rect[0].RT.y = rect[0].size - windowXSize / 2;

	rect[1].LD.x = rect[1].size;
	rect[1].LD.y = windowYSize - rect[1].size;
	rect[1].RT.x = 1200 - rect[1].size;
	rect[1].RT.y = rect[1].size - windowXSize / 2;

	rect[2].LD.x = rect[2].size - windowXSize / 2;
	rect[2].LD.y = 1200 - rect[2].size;
	rect[2].RT.x = windowXSize - rect[2].size;
	rect[2].RT.y = rect[2].size;

	rect[3].LD.x = rect[3].size;
	rect[3].LD.y = 1200 - rect[3].size;
	rect[3].RT.x = 1200 - rect[3].size;
	rect[3].RT.y = rect[3].size;
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
pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}


GLvoid Keyboard(unsigned char key, int x, int y);

GLvoid Mouse(int button, int state, int x, int y);

GLvoid Timer(int val);

float red, green, blue;
bool left_button;
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
	for (Rectangle& f_angle : rect) {
		f_angle.r = (rand() % 256) / 256.0f;
		f_angle.g = (rand() % 256) / 256.0f;
		f_angle.b = (rand() % 256) / 256.0f;
		while (red == f_angle.r && green == f_angle.g && blue == f_angle.b)
		{
			f_angle.r = (rand() % 256) / 256.0f;
			f_angle.g = (rand() % 256) / 256.0f;
			f_angle.b = (rand() % 256) / 256.0f;
		}
	}
	
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape);
	glutTimerFunc(16, Timer, 1);
	glutMouseFunc(Mouse);

	glutKeyboardFunc(Keyboard);// 다시 그리기 함수 지정
	glutMainLoop(); // 이벤트 처리 시작 }
}

GLvoid drawScene() {//--- 콜백 함수: 그리기 콜백 함수 {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	makeRect(rect);

	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다
	for (int i = 0; i < 4; i++) {

		pair<float, float> glCoordsLD = ConvertWindowToGL_X(rect[i].LD.x, rect[i].LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(rect[i].RT.x, rect[i].RT.y);
		rect[i].LDGL.x = glCoordsLD.first;
		rect[i].LDGL.y = glCoordsLD.second;
		rect[i].RTGL.x = glCoordsRT.first;
		rect[i].RTGL.y = glCoordsRT.second;
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if (Rectin(rect[0].LD, rect[0].RT, x, y) == FALSE) {
			rect[0].size+= 2;
		}
		else if (Rectin(rect[1].LD, rect[1].RT, x, y) == FALSE) {
			rect[1].size+= 2;
		}
		else if (Rectin(rect[2].LD, rect[2].RT, x, y) == FALSE) {
			rect[2].size+= 2;
		}
		else if (Rectin(rect[3].LD, rect[3].RT, x, y) == FALSE) {
			rect[3].size+= 2;
		}
		else if ((0 < x && x < 400) && (0 < y && y < 400) && Rectin(rect[0].LD, rect[0].RT, x, y))
		{
			rect[0].size-= 2;
		}
		else if ((400 < x && x < 800) && (0 < y && y < 400) && Rectin(rect[1].LD, rect[1].RT, x, y))
		{
			rect[1].size-= 2;
		}
		else if ((0 < x && x < 400) && (400 < y && y < 800) && Rectin(rect[2].LD, rect[2].RT, x, y))
		{
			rect[2].size-= 2;
		}
		else if ((400 < x && x < 800) && (400 < y && y < 800) && Rectin(rect[3].LD, rect[3].RT, x, y))
		{
			rect[3].size-= 2;
		}
			
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if ((rect[0].LD.x < x && x < rect[0].RT.x) && (rect[0].RT.y < y && y < rect[0].LD.y)) {
			rect[0].r = (rand() % 256) / 256.0f;
			rect[0].g = (rand() % 256) / 256.0f;
			rect[0].b = (rand() % 256) / 256.0f;
		}
		else if ((rect[1].LD.x < x && x < rect[1].RT.x) && (rect[1].RT.y < y && y < rect[1].LD.y)) {
			rect[1].r = (rand() % 256) / 256.0f;
			rect[1].g = (rand() % 256) / 256.0f;
			rect[1].b = (rand() % 256) / 256.0f;
		}
		else if ((rect[2].LD.x < x && x < rect[2].RT.x) && (rect[2].RT.y < y && y < rect[2].LD.y)) {
			rect[2].r = (rand() % 256) / 256.0f;
			rect[2].g = (rand() % 256) / 256.0f;
			rect[2].b = (rand() % 256) / 256.0f;
		}
		else if ((rect[3].LD.x < x && x < rect[3].RT.x) && (rect[3].RT.y < y && y < rect[3].LD.y)) {
			rect[3].r = (rand() % 256) / 256.0f;
			rect[3].g = (rand() % 256) / 256.0f;
			rect[3].b = (rand() % 256) / 256.0f;
		}
		else
		{
			red = (rand() % 256) / 256.0f;
			green = (rand() % 256) / 256.0f;
			blue = (rand() % 256) / 256.0f;
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y)
{


	switch (key) {

	case 'q':
		glutLeaveMainLoop();
		break;

	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(17, Timer, 1);
}



