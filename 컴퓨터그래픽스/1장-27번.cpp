#include "stdafx.h"
#include "camera.h"
#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
using namespace std;
using namespace glm;

float WIDTH = 1000;
float HEIGHT = 1000;
int g_rotate = 0;

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
vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };

bool button_m = FALSE;  bool button_f = FALSE; bool button_F = FALSE;
bool button_e = FALSE; bool button_E = FALSE; bool button_t = FALSE; bool button_T = FALSE;
bool button_y = FALSE; bool button_Y = FALSE; bool button_r = FALSE; bool button_R = FALSE;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
	return { mx, my };
}

struct Light {
	vec3 lightpos = { 1.0,0.5,-0.5 };
	vec3 lightRevol = { 0,0,0.0 };
	vec3 color = { 1,1,1 };
};
Light light;
std::vector <glm::vec3> D_vertex; std::vector <glm::vec3> D_normal; std::vector <glm::vec2> D_uv;
std::vector <glm::vec3> S_vertex; std::vector <glm::vec3> S_normal; std::vector <glm::vec2> S_uv;

vec3 camerapos = { 0.0f,0.5f,3.0f };
GLvoid Timer(int value);
struct Sector {
	vec3 color[3];
	GLuint m_vbo;
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1; color[0].g = 0; color[0].b = 0;
		color[1].r = 0; color[1].g = 1; color[1].b = 0;
		color[2].r = 0; color[2].g = 0; color[2].b = 1;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO ����
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO�� ���ε�
		// VBO�� ������ ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(POS), POS, GL_STATIC_DRAW);
		//Line_T_matrix = glm::translate(Line_R_matrix, glm::vec3(0.0, 0.0, 0.0));

	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);
		int colorUniformLoc = glGetUniformLocation(shaderProgramID, "fColor");

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));


		for (int i = 0; i < 3; ++i) {
			glUniform3f(colorUniformLoc, color[i].r, color[i].g, color[i].b);
			glDrawArrays(GL_LINES, 2 * i, 2);
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
struct SPHERE {
	GLuint g_vbo;
	GLuint g_vbo_normal;
	float rotateAngle[3] = { 0 };
	float revolutionAngle[3] = { 0 };
	float scale = 0.1f;
	vec3 color;
	vec3 move_pos = { 0.0f,0.0f,0.0f };
	float gimbalAngle[3] = { 0 };
	glm::mat4 model_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ
	glm::mat4 parents_matrix = glm::mat4(1.0f); //������ķ� �ʱ�ȭ

	SPHERE(int x, int y)
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * S_vertex.size(), &S_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal); // VBO�� ���ε�
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * S_normal.size(), &S_normal[0], GL_STATIC_DRAW);
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

		model_matrix = glm::scale(model_matrix, glm::vec3(scale, scale, scale));
		model_matrix = parents_matrix * model_matrix;

		unsigned int transLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glm::mat4 lightMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(light.lightRevol.z), AXIS_Z);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.y), AXIS_Y);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(light.lightRevol.x), AXIS_X);

		lightMatrix = glm::translate(lightMatrix, (light.lightpos));

		unsigned int lightLocation = glGetUniformLocation(shaderProgramID, "lightMatix");
		glUniformMatrix4fv(lightLocation, 1, GL_FALSE, glm::value_ptr(lightMatrix));

		//unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
		//glUniform3f(lightPosLocation, lightpos.x, lightpos.y, lightpos.z);
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
		glUniform3f(lightColorLocation, light.color.r, light.color.g, light.color.b);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��
		glUniform3f(objColorLocation, color.r, color.g, color.b);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
		glUniform3f(viewPosLocation, camerapos.x, camerapos.y, camerapos.z);

		glDrawArrays(GL_TRIANGLES, 0, S_vertex.size());


	}


};

vector <Sector> Line;
vector <Dice> dice;
vector <SPHERE> sphere;
CAMERA camera;

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
	Line.emplace_back(WIDTH / 2, HEIGHT / 2);
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	ReadObj("sphere.obj", S_vertex, S_normal, S_uv);

	camera.camerapos[3].x = -0.5f; camera.camerapos[3].y = 0.5f; camera.camerapos[3].z = 3.0f;
	dice.emplace_back(500, 500);		//�عٴ�
	dice.emplace_back(500, 500);		//ũ���� ����
	dice.emplace_back(500, 500);		//ũ���� �Ӹ�
	dice.emplace_back(500, 500);		//ũ���� ��1
	dice.emplace_back(500, 500);		//ũ���� ��2
	dice.emplace_back(500, 500);		//ũ���� ��3
	dice.emplace_back(500, 500);		//ũ���� ��4
	sphere.emplace_back(500, 500);		//ũ���� ��4

	dice[0].scale[0] = 4.0f; dice[0].scale[1] = 4.0f; dice[0].scale[2] = 4.0f;
	dice[0].move_pos.y = -0.5f * dice[0].scale[1];

	dice[1].scale[0] = 0.7f; dice[1].scale[1] = 0.35f; dice[1].scale[2] = 0.7f;
	dice[1].move_pos.y = 0.5f * dice[1].scale[1];

	dice[2].scale[0] = 0.5f; dice[2].scale[1] = 0.5f; dice[2].scale[2] = 0.5f;
	dice[2].move_pos.y = 0.5f;


	dice[3].scale[0] = 0.2f; dice[3].scale[1] = 0.8f; dice[3].scale[2] = 0.2f;
	dice[3].move_pos.y = 0.5f;
	dice[3].move_pos.x -= 0.3f;

	dice[4].scale[0] = 0.2f; dice[4].scale[1] = 0.8f; dice[4].scale[2] = 0.2f;
	dice[4].move_pos.y = 0.5f;
	dice[4].move_pos.x += 0.3f;

	dice[5].scale[0] = 0.1f; dice[5].scale[1] = 0.2f; dice[5].scale[2] = 1.0f;
	dice[5].move_pos.z = 0.7f;
	dice[5].move_pos.x -= 0.3f;

	dice[6].scale[0] = 0.1f; dice[6].scale[1] = 0.2f; dice[6].scale[2] = 1.0f;
	dice[6].move_pos.z = 0.7f;
	dice[6].move_pos.x += 0.3f;
	dice[5].color.r = dice[6].color.r;
	dice[5].color.g = dice[6].color.g;
	dice[5].color.b = dice[6].color.b;

	sphere[0].scale = 0.01f;
	sphere[0].move_pos = { 0.8f, 0.2f,-0.3f };
	//dice[2].move_pos.y = 1.0f * dice[1].scale[1];
	glutTimerFunc(20, Timer, 0);
	glutKeyboardFunc(KeyBoardFunc);
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

void drawScene()
{
	{
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		camera.Draw(camerapos,shaderProgramID);

	}
	dice[2].parents_matrix = dice[1].model_matrix;
	dice[3].parents_matrix = dice[2].model_matrix;
	dice[4].parents_matrix = dice[2].model_matrix;
	dice[5].parents_matrix = dice[1].model_matrix;
	dice[6].parents_matrix = dice[1].model_matrix;
	sphere[0].color = light.color;
	sphere[0].move_pos = light.lightpos;

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	Line[0].Draw();
	for (int i = 0; i < dice.size(); ++i) {
		dice[i].Draw();
	}
	sphere[0].Draw();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glutSwapBuffers(); //ȭ�鿡 ���
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
		if (button_y)
		{
			light.lightRevol.y += 1.0f;
			sphere[0].revolutionAngle[1] += 1.0f;
			
			glutTimerFunc(20, Timer, 1);
			
		}
		else if (button_Y) {

			light.lightRevol.y -= 1.0f;
			sphere[0].revolutionAngle[1] -= 1.0f;
			glutTimerFunc(20, Timer, 1);
		}
		break;
	case 2:
		if (button_r)
		{
			camera.CR_matrix = glm::rotate(camera.CR_matrix, glm::radians(2.0f), AXIS_Y);


			glutTimerFunc(20, Timer, 2);
		}
		else if (button_R) {

			camera.CR_matrix = glm::rotate(camera.CR_matrix, glm::radians(-2.0f), AXIS_Y);
			glutTimerFunc(20, Timer, 2);
		}
		break;
	default:
		break;
	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'z':
		camerapos.z += 0.1f;
		break;
	case 'Z':
		camerapos.z -= 0.1f;
		break;
	case 'x':
		camerapos.x += 0.1f;
		break;
	case 'X':
		camerapos.x -= 0.1f;
		break;

	case 'c':
		if (light.color.r == 1 && light.color.g == 1 && light.color.b == 1) {
			light.color.r = 0; light.color.b = 0;
		}
		else if (light.color.r == 0 && light.color.g == 1 && light.color.b == 0)
		{
			light.color.r = 1;
		}
		else if (light.color.r == 1 && light.color.g == 1 && light.color.b == 0)
			light.color.b = 1;
		break;
	case'm':
		button_m = !button_m;
		break;
	case 'y':
		button_y = !button_y;
		if (button_Y) {
			button_Y = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 1);
		break;
	case 'Y':
		button_Y = !button_Y;
		if (button_y) {
			button_y = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 1);
		break;
	case 'r':
		button_r = !button_r;
		if (button_R) {
			button_R = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 2);
		break;
	case 'R':
		button_R = !button_R;
		if (button_r) {
			button_r = FALSE;
		}
		else
			glutTimerFunc(20, Timer, 2);
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();

}


GLvoid Reshape(int w, int h) {//--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
}
void make_vertexShaders() {
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex_light.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {//result == 0�̸� ������ �ִ�.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
		return;
	}
}
void make_fragmentShaders() {
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment_light.glsl");
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

