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
struct Rectangle {
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
GLvoid  COLOR_MOVE(vector<Rectangle>& rect);
pair<float, float> ConvertWindowToGL_X(int windowX, int windowY)
{
	float glX = ((float)windowX - (float)windowXSize / 2.0f) / ((float)windowXSize / 2.0f);
	float glY = (((float)windowYSize / 2.0f) - (float)windowY) / ((float)windowYSize / 2.0f);


	return make_pair(glX, glY);
}
void makeRect(vector <Rectangle>* rect, int& value, int x, int y)
{
	Rectangle temp;
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
void CalculateDiagonalMove(vector <Rectangle>& rect);
void CalculatZigzag(vector <Rectangle>& rect);
GLvoid  Scale_Move(vector<Rectangle>& rect);
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
vector<Rectangle> rect;
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
		for (Rectangle& f_angle : rect)
		{
			f_angle.LD = f_angle.first_LD;
			f_angle.RT = f_angle.first_RT;
			f_angle.LDGL = f_angle.first_LDGL;
			f_angle.RTGL = f_angle.first_RTGL;
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

GLvoid CalculateDiagonalMove(vector<Rectangle>& rect) {

	for (Rectangle& f_angle : rect) {
		// ��ǥ ������Ʈ
		if (f_angle.LD.x <= 0 || f_angle.RT.x >= windowXSize)
		{
			f_angle.dx *= -1;

		}
		if (f_angle.LD.y >= windowYSize || f_angle.RT.y <= 0)
		{
			f_angle.dy *= -1;
		}
		f_angle.LD.x += f_angle.dx * cos(f_angle.angle);
		f_angle.LD.y += f_angle.dy * sin(f_angle.angle);
		f_angle.RT.x += f_angle.dx * cos(f_angle.angle);
		f_angle.RT.y += f_angle.dy * sin(f_angle.angle);

		// GL ��ȯ ��ǥ ������Ʈ
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(f_angle.LD.x, f_angle.LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(f_angle.RT.x, f_angle.RT.y);
		f_angle.LDGL.x = glCoordsLD.first;
		f_angle.LDGL.y = glCoordsLD.second;
		f_angle.RTGL.x = glCoordsRT.first;
		f_angle.RTGL.y = glCoordsRT.second;

	}
}
GLvoid  CalculatZigzag(vector<Rectangle>& rect)
{
	if (interval <= 100) {
		for (Rectangle& f_angle : rect) {
			// ��ǥ ������Ʈ
			if (toggle_a == false) {
				if (f_angle.LD.x <= 0 || f_angle.RT.x >= windowXSize)
				{
					f_angle.dx *= -1;

				}
				if (f_angle.LD.y >= windowYSize || f_angle.RT.y <= 0)
				{
					f_angle.dy *= -1;
				}
			}
			f_angle.LD.x += f_angle.dx * cos(45);
			f_angle.LD.y += f_angle.dy * sin(45);
			f_angle.RT.x += f_angle.dx * cos(45);
			f_angle.RT.y += f_angle.dy * sin(45);

			// GL ��ȯ ��ǥ ������Ʈ
			pair<float, float> glCoordsLD = ConvertWindowToGL_X(f_angle.LD.x, f_angle.LD.y);
			pair<float, float> glCoordsRT = ConvertWindowToGL_X(f_angle.RT.x, f_angle.RT.y);
			f_angle.LDGL.x = glCoordsLD.first;
			f_angle.LDGL.y = glCoordsLD.second;
			f_angle.RTGL.x = glCoordsRT.first;
			f_angle.RTGL.y = glCoordsRT.second;
		}
		interval++;
	}
	else
	{
		interval = 0;
		for (Rectangle& f_angle : rect) {
			f_angle.dx *= -1;
		}
	}
}
GLvoid  Scale_Move(vector<Rectangle>& rect)
{
	for (Rectangle& f_angle : rect) {
		// ��ǥ ������Ʈ
		if (f_angle.LD.x <= 0 || f_angle.RT.x >= windowXSize || f_angle.RT.x <= 0 || f_angle.LD.x >= windowXSize)
		{
			f_angle.dx *= -1;

		}
		if (f_angle.LD.y >= windowYSize || f_angle.RT.y <= 0 || f_angle.RT.y >= windowYSize || f_angle.LD.y <= 0)
		{
			f_angle.dy *= -1;
		}
		f_angle.LD.x -= f_angle.dx;
		f_angle.LD.y += f_angle.dy;
		f_angle.RT.x += f_angle.dx;
		f_angle.RT.y -= f_angle.dy;

		// GL ��ȯ ��ǥ ������Ʈ
		pair<float, float> glCoordsLD = ConvertWindowToGL_X(f_angle.LD.x, f_angle.LD.y);
		pair<float, float> glCoordsRT = ConvertWindowToGL_X(f_angle.RT.x, f_angle.RT.y);
		f_angle.LDGL.x = glCoordsLD.first;
		f_angle.LDGL.y = glCoordsLD.second;
		f_angle.RTGL.x = glCoordsRT.first;
		f_angle.RTGL.y = glCoordsRT.second;
	}



}


GLvoid  COLOR_MOVE(vector<Rectangle>& rect) {

	for (Rectangle& f_angle : rect)
	{
		f_angle.r = (rand() % 256) / 256.0f;
		f_angle.g = (rand() % 256) / 256.0f;
		f_angle.b = (rand() % 256) / 256.0f;
	}
}