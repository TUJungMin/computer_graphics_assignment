#include "stdafx.h"
#include "camera.h"
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
using namespace std;
using namespace glm;

vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };
float WIDTH = 800;
float HEIGHT = 600;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoardFunc(unsigned char key, int x, int y);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid Timer(int value);
//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
std::vector <glm::vec3> D_vertex;
std::vector <glm::vec3> D_normal;
std::vector <glm::vec2> D_uv;
std::vector <glm::vec3> P_vertex;
std::vector <glm::vec3> P_normal;
std::vector <glm::vec2> P_uv;
vec3 camerapos = { -0.5, 1.0, 1.0 };
bool button_m = FALSE;
bool button_r = FALSE;
struct Light {
	vec3 lightpos = { 0.0,0,0.5 };
	vec3 lightRevol = { 0,0,0.0 };
	vec3 color = { 1,1,1 };
};
Light light;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}
GLvoid Timer(int value);



struct Sector {
	GLuint g_vbo;
	GLuint g_vbo_normal;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color[3];
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	bool Dodraw = TRUE;
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 parents_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ

	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1;color[0].g = 0;color[0].b = 0;
		color[1].r = 0;color[1].g = 1;color[1].b = 0;
		color[2].r = 0;color[2].g = 0;color[2].b = 1;
		InitVbo();
		
	}

	void InitVbo()
	{
		glGenBuffers(1, &g_vbo); // VBO ����
	}

	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(POS), POS, GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);

		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		model_matrix = parents_matrix * model_matrix;

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glm::mat4 lightMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(light.lightRevol.z), AXIS_Z);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.y), AXIS_Y);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.x), AXIS_X);

		lightMatrix = glm::translate(lightMatrix, (light.lightpos));

		unsigned int lightLocation = glGetUniformLocation(shaderProgramID, "lightMatix");
		glUniformMatrix4fv(lightLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
		glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
		glUniform3f(viewPosLocation, camerapos.x, camerapos.y, camerapos.z);

		if (Dodraw) {
			for (int i = 0; i < 3; ++i) {
				glUniform3f(objColorLocation, color[i].r, color[i].g, color[i].b);
				glDrawArrays(GL_LINES, 2 * i, 2);
			}
		}

	}
};
struct Dice {
	GLuint g_vbo;
	GLuint g_vbo_normal;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	bool Dodraw = TRUE;
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 parents_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ

	Dice(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);

		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &g_vbo); // VBO ����
		glGenBuffers(1, &g_vbo_normal); // VBO ����
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_normal.size(), &D_normal[0], GL_STATIC_DRAW);
		int noramldata = glGetAttribLocation(shaderProgramID, "vNormal");
		glVertexAttribPointer(noramldata, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(noramldata);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);

		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		model_matrix = parents_matrix * model_matrix;

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glm::mat4 lightMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(light.lightRevol.z), AXIS_Z);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.y), AXIS_Y);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.x), AXIS_X);

		lightMatrix = glm::translate(lightMatrix, (light.lightpos));

		unsigned int lightLocation = glGetUniformLocation(shaderProgramID, "lightMatix");
		glUniformMatrix4fv(lightLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���

		if (!button_m) {
			glUniform3f(lightColorLocation, light.color.r, light.color.g, light.color.b);
		}
		else
			glUniform3f(lightColorLocation, 0.0, 0.0, 0.0);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		glUniform3f(objColorLocation, color.r, color.g, color.b);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
		glUniform3f(viewPosLocation, camerapos.x, camerapos.y, camerapos.z);

		if (Dodraw) {
			for (int i = 0; i < 6; ++i) {

				glDrawArrays(GL_TRIANGLES, 6 * i, 6);
			}
		}

	}


};
struct PYRAMID {
	GLuint g_vbo;
	GLuint g_vbo_normal;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale[3] = { 1.0f,1.0f,1.0f };
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	bool Dodraw = FALSE;
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 parents_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	bool open_button = FALSE;
	PYRAMID(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		random_device rd;
		default_random_engine dre(rd());
		uniform_real_distribution<float> urd(0, 1);
		color.r = urd(dre);
		color.g = urd(dre);
		color.b = urd(dre);

		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &g_vbo); // VBO ����
		glGenBuffers(1, &g_vbo_normal); // VBO ����
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * P_vertex.size(), &P_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * P_vertex.size(), &P_vertex[0], GL_STATIC_DRAW);
		int noramldata = glGetAttribLocation(shaderProgramID, "vNormal");
		glVertexAttribPointer(noramldata, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(noramldata);

		model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(gimbalAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[1]), AXIS_Y);
		//model_matrix = glm::rotate(model_matrix, glm::radians(revolutionAngle[0]), AXIS_X);

		model_matrix = glm::translate(model_matrix, (move_pos));

		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[2]), AXIS_Z);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[0]), AXIS_X);
		model_matrix = glm::rotate(model_matrix, glm::radians(rotateAngle[1]), AXIS_Y);

		model_matrix = glm::scale(model_matrix, glm::vec3(scale[0], scale[1], scale[2]));
		model_matrix = parents_matrix * model_matrix;

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glm::mat4 lightMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(light.lightRevol.z), AXIS_Z);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.y), AXIS_Y);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.x), AXIS_X);

		lightMatrix = glm::translate(lightMatrix, (light.lightpos));

		unsigned int lightLocation = glGetUniformLocation(shaderProgramID, "lightMatix");
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
		
		if (!button_m) {
			glUniform3f(lightColorLocation, light.color.r, light.color.g, light.color.b);
		}
		else
			glUniform3f(lightColorLocation, 0.0, 0.0, 0.0);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		glUniform3f(objColorLocation, color.r, color.g, color.b);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
		glUniform3f(viewPosLocation, camerapos.x, camerapos.y, camerapos.z);


		if (Dodraw) {
			for (int i = 0; i < 6; ++i) {
				glUniform3f(objColorLocation, color.r, color.g, color.b);
				glDrawArrays(GL_TRIANGLES, 3 * i, 3);
			}
		}
	}

};

vector <Dice> dice;
CAMERA camera;
vector <Sector> Line;
vector <PYRAMID> pyramid;
float light_angle = 1.0f;
void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����

	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(WIDTH, HEIGHT); // �������� ũ�� ����
	glutCreateWindow("OPENGL"); // ������ ����
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderProgramID = make_shaderProgram();
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	ReadObj("pyramid.obj", P_vertex, P_normal, P_uv);
	dice.emplace_back(400, 300);
	dice.emplace_back(400, 300);
	dice.emplace_back(400, 300);
	pyramid.emplace_back(400, 300);
	Line.emplace_back(400, 300);
	dice[0].scale[0] = 0.5f;
	dice[0].scale[1] = 0.5f;
	dice[0].scale[2] = 0.5f;
	dice[1].scale[0] = 0.1f;
	dice[1].scale[1] = 0.1f;
	dice[1].scale[2] = 0.1f;
	dice[1].color.r = 1.0f;
	dice[1].color.g = 1.0f;
	dice[1].color.b = 1.0f;
	dice[1].move_pos = { light.lightpos.x,light.lightpos.y,light.lightpos.z+0.15f };
	dice[2].scale[0] = 0.02f;
	dice[2].scale[1] = 0.02f; 
	glutKeyboardFunc(KeyBoardFunc);
	dice[2].scale[2] = 0.02f; 
	glutTimerFunc(20, Timer, 1);
	glutTimerFunc(20, Timer, 0);
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	camera.Draw(camerapos,shaderProgramID);
	dice[0].Draw();
	dice[1].Draw();
	pyramid[0].Draw();
	Line[0].Draw();
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //ȭ�鿡 ���
}
GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'n':
		pyramid[0].Dodraw = !pyramid[0].Dodraw;
		dice[0].Dodraw = !dice[0].Dodraw;
		break;
	case 'y':
		glutTimerFunc(20, Timer, 2);
		break;
	case 'm':
		button_m = !button_m;
		break;
	case 'r':
		button_r = !button_r;
		glutTimerFunc(20, Timer, 1);
		break;
	case 'z':
		dice[1].move_pos.z += 0.1f;
		light.lightpos.z += 0.1f;
		break;
	case 'Z':
		dice[1].move_pos.z -= 0.1f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
GLvoid Timer(int value)
{
	switch (value)
	{
	case 0:
		glutPostRedisplay();
		glutTimerFunc(20, Timer, 0);
		break;

	case 1:
		if (button_r) {
			light.lightRevol.y += 1.0f;
			dice[1].revolutionAngle[1] += 1.0f;
			glutTimerFunc(20, Timer, 1);
		}
		break;
	case 2:
		dice[0].rotateAngle[1] += 1.0f;
		pyramid[0].rotateAngle[1] += 1.0f;
		glutTimerFunc(20, Timer, 2);
		break;
	default:
		break;
	}
	
}
GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�

	//vertex shader read, save vertexSource
	vertexSource = filetobuf("vertex_light.glsl");

	//vertex shader ���� ���ְ� ������ �����͸� vertexShader��� ������ ����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//�����ϸ� 0�� �ƴѰ�

	//���̴� ��ü, 1, ���̴� �ҽ� �ڵ�, NULL
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//shader, ���ڿ����� ����, ���ڿ����� �迭, ���ڿ����� ���̸� ������ int�迭

	//������ �ؾ�����.
	glCompileShader(vertexShader);

	//���̴� ��� ��� �ƴ���? ���� Ȯ���ϴ� �ڵ�
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result) {//result == 0�̸� ������ �ִ�.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment_light.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: frag_shader ������ ����\n" << errorLog << endl;
		return;
	}
}
GLuint make_shaderProgram() {
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�

	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

