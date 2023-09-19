#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <time.h>
#include <utility>
#include <vector>
#include <math.h>
#include <random>
#define rect_size 100
using namespace std;
int windowXSize = 1600;
int windowYSize = 800;
random_device rd;  // ������ �õ� ����
mt19937 gen(rd());  // �޸��� Ʈ������(Mersenne Twister) ������ ����� ���� ������
uniform_int_distribution<int> dist(-10, 10);
enum TIMER_CASE {    // ������ ����
	DIALOG_MOVE = 0,
	ZIGZAG_MOVE,
	RANDOM_SCALE,
	RANDOM_COLOR
};
struct Point {
	float x;
	float y;
};
struct Rect {
	Point LD;		//���ϴ� ��ǥ
	Point RT;		//���ϴ� ��ǥ
	Point LDGL;		//���ϴ� GL��ȯ ��ǥ
	Point RTGL;
	Point first_LDGL;
	Point first_RTGL;
	Point first_LD;
	Point first_RT;
	float r, g, b;
	float size = 100;
	float dx, dy;
	float angle;
};
GLvoid  COLOR_MOVE(vector<Rect>& rect);
pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}
void makeRect(vector <Rect>* rect, int& value, int x, int y)
{
	Rect temp;
	temp.r = (rand() % 256) / 256.0f;
	temp.g = (rand() % 256) / 256.0f;
	temp.b = (rand() % 256) / 256.0f;
	temp.dx = dist(gen); temp.dy = dist(gen);
	temp.LD.x = x - rect_size / 2; temp.LD.y = y + rect_size / 2;
	temp.RT.x = temp.LD.x + 100; temp.RT.y = temp.LD.y - 100;
	pair<float, float> glCoordsLD = ConvertWindowToGL_X(temp.LD.x, temp.LD.y);
	pair<float, float> glCoordsRT = ConvertWindowToGL_X(temp.RT.x, temp.RT.y);
	temp.LDGL.x = glCoordsLD.first; temp.LDGL.y = glCoordsLD.second; temp.RTGL.x = glCoordsRT.first; temp.RTGL.y = glCoordsRT.second;
	temp.first_LDGL = temp.LDGL; temp.first_RTGL = temp.RTGL; temp.first_LD = temp.LD; temp.first_RT = temp.RT;
	temp.angle = rand() % 360;
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
void CalculateDiagonalMove(vector <Rect>& rect);
void CalculatZigzag(vector <Rect>& rect);
GLvoid  Scale_Move(vector<Rect>& rect);
GLvoid drawScene();//--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� {  // �������� ��blue�� �� ����
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� {
	glViewport(0, 0, w, h);
	windowXSize = w;
	windowYSize = h;
}
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int val);
int interval = 0;
vector<Rect> rect;
bool toggle_a, toggle_i, toggle_c, toggle_o;
float red, green, blue;
bool left_button;
int value;
int SelectNum;

void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(windowXSize, windowYSize); // �������� ũ�� ����
	glutCreateWindow("Example2"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;

		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	srand(unsigned(time(NULL)));
	red = 0.411765;  green = 0.411765; blue = 0.411765;

	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);

	glutKeyboardFunc(Keyboard);// �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() {//--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� {  

	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	//makeRect(&rect,value);

	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�
	for (int i = 0; i < rect.size(); i++) {
		glColor3f(rect[i].r, rect[i].g, rect[i].b);
		glRectf(rect[i].LDGL.x, rect[i].LDGL.y, rect[i].RTGL.x, rect[i].RTGL.y);
	}
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		if (rect.size() < 5) {
			makeRect(&rect, value, x, y);
		}
	}
}
GLvoid Keyboard(unsigned char key, int x, int y)
{

	switch (key) {

	case 'a':
		toggle_a = !toggle_a;
		if (toggle_a) {
			glutTimerFunc(10, Timer, DIALOG_MOVE);
		}
		break;
	case 'i':
		toggle_i = !toggle_i;
		if (toggle_i) {
			glutTimerFunc(10, Timer, ZIGZAG_MOVE);
		}

		break;
	case 'c':
		toggle_c = !toggle_c;
		if (toggle_c) {
			glutTimerFunc(50, Timer, RANDOM_SCALE);
		}
		break;
	case 'o':
		toggle_o = !toggle_o;
		if (toggle_o) {
			glutTimerFunc(100, Timer, RANDOM_COLOR);
		}

		break;
	case 's':
		toggle_a = false;
		toggle_c = false;
		toggle_i = false;
		toggle_o = false;
		break;
	case 'm':
		for (Rect& a : rect)
		{
			a.LD = a.first_LD;
			a.RT = a.first_RT;
			a.LDGL = a.first_LDGL;
			a.RTGL = a.first_RTGL;
		}
		break;
	case 'r':
		if (rect.size() >= 1)
		{
			rect.clear();
			value = 0;
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;

	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

GLvoid Timer(int val) {
	switch (val)
	{
	case DIALOG_MOVE:
		CalculateDiagonalMove(rect);
		if (toggle_a) {// �밢�� �̵� ��� �Լ� ȣ��
			glutTimerFunc(10, Timer, DIALOG_MOVE);  // Ÿ�̸� �缳��
		}
		break;
	case ZIGZAG_MOVE:
		CalculatZigzag(rect);
		if (toggle_i) {// �밢�� �̵� ��� �Լ� ȣ��
			glutTimerFunc(10, Timer, ZIGZAG_MOVE);  // Ÿ�̸� �缳��
		}
		break;
	case RANDOM_SCALE:
		Scale_Move(rect);
		if (toggle_c) {// �밢�� �̵� ��� �Լ� ȣ��
			glutTimerFunc(50, Timer, RANDOM_SCALE);  // Ÿ�̸� �缳��
		}
		break;
	case RANDOM_COLOR:
		COLOR_MOVE(rect);
		if (toggle_o) {// �밢�� �̵� ��� �Լ� ȣ��
			glutTimerFunc(100, Timer, RANDOM_COLOR);  // Ÿ�̸� �缳��
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();  // ȭ���� �ٽ� �׸����� ��û
}

GLvoid CalculateDiagonalMove(vector<Rect>& rect) {

	for (Rect& a : rect) {
		// ��ǥ ������Ʈ
		if (a.LD.x <= 0 || a.RT.x >= windowXSize)
		{
			a.dx *= -1;

		}
		if (a.LD.y >= windowYSize || a.RT.y <= 0)
		{
			a.dy *= -1;
		}
		a.LD.x += a.dx * cos(a.angle);
		a.LD.y += a.dy * sin(a.angle);
		a.RT.x += a.dx * cos(a.angle);
		a.RT.y += a.dy * sin(a.angle);

		// GL ��ȯ ��ǥ ������Ʈ
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(a.LD.x, a.LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(a.RT.x, a.RT.y);
		a.LDGL.x = glCoordsLD.first;
		a.LDGL.y = glCoordsLD.second;
		a.RTGL.x = glCoordsRT.first;
		a.RTGL.y = glCoordsRT.second;

	}
}
GLvoid  CalculatZigzag(vector<Rect>& rect)
{
	if (interval <= 100) {
		for (Rect& a : rect) {
			// ��ǥ ������Ʈ
			if (toggle_a == false) {
				if (a.LD.x <= 0 || a.RT.x >= windowXSize)
				{
					a.dx *= -1;

				}
				if (a.LD.y >= windowYSize || a.RT.y <= 0)
				{
					a.dy *= -1;
				}
			}
			a.LD.x += a.dx * cos(45);
			a.LD.y += a.dy * sin(45);
			a.RT.x += a.dx * cos(45);
			a.RT.y += a.dy * sin(45);

			// GL ��ȯ ��ǥ ������Ʈ
			pair<float, float> glCoordsLD = ConvertWindowToGL_X(a.LD.x, a.LD.y);
			pair<float, float> glCoordsRT = ConvertWindowToGL_X(a.RT.x, a.RT.y);
			a.LDGL.x = glCoordsLD.first;
			a.LDGL.y = glCoordsLD.second;
			a.RTGL.x = glCoordsRT.first;
			a.RTGL.y = glCoordsRT.second;
		}
		interval++;
	}
	else
	{
		interval = 0;
		for (Rect& a : rect) {
			a.dx *= -1;
		}
	}
}
GLvoid  Scale_Move(vector<Rect>& rect)
{
	for (Rect& a : rect) {
		// ��ǥ ������Ʈ
		if (a.LD.x <= 0 || a.RT.x >= windowXSize || a.RT.x <= 0 || a.LD.x >= windowXSize)
		{
			a.dx *= -1;

		}
		if (a.LD.y >= windowYSize || a.RT.y <= 0 || a.RT.y >= windowYSize || a.LD.y <= 0)
		{
			a.dy *= -1;
		}
		a.LD.x -= a.dx;
		a.LD.y += a.dy;
		a.RT.x += a.dx;
		a.RT.y -= a.dy;

		// GL ��ȯ ��ǥ ������Ʈ
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(a.LD.x, a.LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(a.RT.x, a.RT.y);
		a.LDGL.x = glCoordsLD.first;
		a.LDGL.y = glCoordsLD.second;
		a.RTGL.x = glCoordsRT.first;
		a.RTGL.y = glCoordsRT.second;
	}



}


GLvoid  COLOR_MOVE(vector<Rect>& rect) {

	for (Rect& a : rect)
	{
		a.r = (rand() % 256) / 256.0f;
		a.g = (rand() % 256) / 256.0f;
		a.b = (rand() % 256) / 256.0f;
	}
}