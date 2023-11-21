#include "stdafx.h"
#include "camera.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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
//--- 필요한 변수 선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];
vec3 POS[6] = { { -1,0,0 }, { 1,0,0 },{ 0,-1,0 }, { 0,1,0 },{ 0,0,-1 }, { 0,0,1 } };

bool button_m = FALSE;  bool button_f = FALSE; bool button_F = FALSE;
bool button_e = FALSE; bool button_E = FALSE; bool button_t = FALSE; bool button_T = FALSE;
bool button_y = FALSE; bool button_Y = FALSE; bool button_r = FALSE; bool button_R = FALSE;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
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
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	Sector(int x, int y)
	{
		auto center = ConvertWinToGL(x, y);
		color[0].r = 1; color[0].g = 0; color[0].b = 0;
		color[1].r = 0; color[1].g = 1; color[1].b = 0;
		color[2].r = 0; color[2].g = 0; color[2].b = 1;
		InitVbo();
	}
	void InitVbo() {
		glGenBuffers(1, &m_vbo); // VBO 생성
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // VBO를 바인딩
		// VBO에 데이터 복사
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
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화

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
		glGenBuffers(1, &g_vbo); // VBO 생성
		glGenBuffers(1, &g_vbo_normal); // VBO 생성
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO를 바인딩
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * D_vertex.size(), &D_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal); // VBO를 바인딩
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
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색

		if (!button_m) {
			glUniform3f(lightColorLocation, light.color.r, light.color.g, light.color.b);
		}
		else
			glUniform3f(lightColorLocation, 0.0, 0.0, 0.0);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
		glUniform3f(objColorLocation, color.r, color.g, color.b);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos 값 전달: 카메라 위치
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
	glm::mat4 model_matrix = glm::mat4(1.0f); //단위행렬로 초기화
	glm::mat4 parents_matrix = glm::mat4(1.0f); //단위행렬로 초기화

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
		glGenBuffers(1, &g_vbo); // VBO 생성
		glGenBuffers(1, &g_vbo_normal); // VBO 생성
	}
	void Draw() {
		glBindBuffer(GL_ARRAY_BUFFER, g_vbo); // VBO를 바인딩
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * S_vertex.size(), &S_vertex[0], GL_STATIC_DRAW);
		int vertexAttribLoc = glGetAttribLocation(shaderProgramID, "vPos");
		glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(vertexAttribLoc);

		glBindBuffer(GL_ARRAY_BUFFER, g_vbo_normal); // VBO를 바인딩
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

		//unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
		//glUniform3f(lightPosLocation, lightpos.x, lightpos.y, lightpos.z);
		unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
		glUniform3f(lightColorLocation, light.color.r, light.color.g, light.color.b);
		unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
		glUniform3f(objColorLocation, color.r, color.g, color.b);
		unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos 값 전달: 카메라 위치
		glUniform3f(viewPosLocation, camerapos.x, camerapos.y, camerapos.z);

		glDrawArrays(GL_TRIANGLES, 0, S_vertex.size());


	}


};

vector <Sector> Line;
vector <Dice> dice;
vector <SPHERE> sphere;
CAMERA camera;

void main(int argc, char** argv) {//--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정

	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WIDTH, HEIGHT); // 윈도우의 크기 지정

	glutCreateWindow("OPENGL"); // 윈도우 생성
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized" << endl;
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	Line.emplace_back(WIDTH / 2, HEIGHT / 2);
	ReadObj("cube.obj", D_vertex, D_normal, D_uv);
	ReadObj("sphere.obj", S_vertex, S_normal, S_uv);

	camera.camerapos[3].x = -0.5f; camera.camerapos[3].y = 0.5f; camera.camerapos[3].z = 3.0f;
	dice.emplace_back(500, 500);		//밑바닥
	dice.emplace_back(500, 500);		//크레인 몸통
	dice.emplace_back(500, 500);		//크레인 머리
	dice.emplace_back(500, 500);		//크레인 팔1
	dice.emplace_back(500, 500);		//크레인 팔2
	dice.emplace_back(500, 500);		//크레인 팔3
	dice.emplace_back(500, 500);		//크레인 팔4
	sphere.emplace_back(500, 500);		//크레인 팔4

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
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
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
	glutSwapBuffers(); //화면에 출력
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


GLvoid Reshape(int w, int h) {//--- 콜백 함수: 다시 그리기 콜백 함수
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
	if (!result) {//result == 0이면 오류가 있다.
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
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
		cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << endl;
		return;
	}
}
GLuint make_shaderProgram() {
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기 - 두 세이더 붙어야됨, vertex - fragment는 짝이 맞아야됨
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

