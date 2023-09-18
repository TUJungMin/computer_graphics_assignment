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
	Point LD;		//���ϴ� ��ǥ
	Point RT;		//���ϴ� ��ǥ
	Point LDGL;		//���ϴ� GL��ȯ ��ǥ
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

	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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
	glutPostRedisplay(); // ȭ���� �ٽ� �׸����� ��û
	
}


