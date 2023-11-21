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
random_device rd;  // ������ �õ� ����
mt19937 gen(rd());  // �޸��� Ʈ������(Mersenne Twister) ������ ����� ���� ������
uniform_real_distribution<float> color(0, 1);
uniform_real_distribution<float> size_x(0, 1500);
uniform_real_distribution<float> size_y(100, 800);
uniform_int_distribution<int> rect_num(20, 40);
struct Point {
	float x;
	float y;
};
struct Rectangle {
	Point LD;		//���ϴ� ��ǥ
	Point RT;		//���ϴ� ��ǥ
	Point LDGL;		//���ϴ� GL��ȯ ��ǥ
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
GLvoid drawScene();//--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� {  // �������� ��blue�� �� ����
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� {
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
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(windowXSize, windowYSize); // �������� ũ�� ����
	glutCreateWindow("Example5"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;

		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	srand(unsigned(time(NULL)));
	red = 1.f;  green = 1.f; blue = 1.f;
	
	
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	initialRect(rect);

	glutKeyboardFunc(Keyboard);// �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() {//--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�


	//makeRect(rect);

	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�
	for (int i = 0; i < rect.size(); i++) {
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	if (alive_Eraser) {
		glColor3f(eraser.r, eraser.g, eraser.b);

		glRectf(eraser.LDGL.x, eraser.LDGL.y, eraser.RTGL.x, eraser.RTGL.y);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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
	glutPostRedisplay(); // ȭ���� �ٽ� �׸����� ��û

}

