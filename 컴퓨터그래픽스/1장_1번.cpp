#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);



float red, green, blue;
bool roopStart = FALSE;
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;

		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape);

	glutTimerFunc(100, TimerFunction, 1);

	glutKeyboardFunc(Keyboard);// �ٽ� �׸��� �Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ���� }
}

GLvoid drawScene() {//--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� {  // �������� ��blue�� �� ����
	glClearColor(red, green, blue, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� {
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{


	switch (key) {
	case 'c':
		blue = 1;
		green = 1;
		red = 0;
		break; //--- ������ û�ϻ����� ����
	case 'm':
		blue = 1;
		green = 0;
		red = 1;
		break; //--- ������ ��ȫ������ ����
	case 'y':
		blue = 0;
		green = 1;
		red = 1;
		break; //--- ������ ��������� ����
	case 'a':
		srand(unsigned(time(NULL)));
		blue = (rand() % 256) / 256.0f;
		green = (rand() % 256) / 256.0f;
		red = (rand() % 256) / 256.0f;
		break;

	case 't':
		roopStart = TRUE;
		break;
	case 's':
		roopStart = FALSE;
		break;
	case 'w':
		blue = 1;
		green = 1;
		red = 1;
		break;
	case 'k':
		blue = 0;
		green = 0;
		red = 0;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;

	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}
void TimerFunction(int value)
{
	if (roopStart) {
		blue = (rand() % 256) / 256.0f;
		green = (rand() % 256) / 256.0f;
		red = (rand() % 256) / 256.0f;
	}
	glutPostRedisplay(); // ȭ�� �� ���

	glutTimerFunc(100, TimerFunction, 1);

}
