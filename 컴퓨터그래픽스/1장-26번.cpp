#include "stdafx.h"
#include "camera.h"
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
using namespace std;
using namespace glm;

enum planet {
	sun = 0,
	earth1,
	earth2,
	lighting ,
};
float WIDTH = 1000;
float HEIGHT = 1000;
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
std::vector <glm::vec3> S_vertex;
std::vector <glm::vec3> S_normal;
std::vector <glm::vec2> S_uv;
pair<float, float> ConvertWinToGL(int x, int y) {
	float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl좌표계로 변경
	float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl좌표계로 변경
	return { mx, my };
}
vec3 pos = { 0.0,0.0,15.0f };
std::vector <glm::vec3> C_vertex;
GLvoid Timer(int value);
struct Light {
	vec3 lightpos = { 1.0,0,3.0 };
	vec3 lightRevol = { 0,0,0.0 };
	vec3 color = { 1,1,1 };
};
Light light;

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
		glUniform3f(viewPosLocation, pos.x, pos.y, pos.z);

		glDrawArrays(GL_TRIANGLES, 0, S_vertex.size());


	}


};


CAMERA camera;

vector<SPHERE> sphere;
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
	ReadObj("sphere.obj", S_vertex, S_normal, S_uv);

	sphere.emplace_back(WIDTH / 2, HEIGHT / 2);

	for (int i = 0; i < 3; ++i) {
		sphere.emplace_back(WIDTH / 2, HEIGHT / 2);	//지구
	}
	sphere[sun].scale = 0.1f;
	sphere[earth1].scale = 0.1f; sphere[earth1].move_pos.x = -1.5;
	sphere[earth2].scale = 0.1f; sphere[earth2].move_pos.x = -3.0f;
	sphere[lighting].scale = 0.05f; sphere[lighting].move_pos = light.lightpos;

	glutKeyboardFunc(KeyBoardFunc);
	glutTimerFunc(20, Timer, 0);
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop(); // 이벤트 처리 시작
}
vec3 camerapos{ 0.50f,0.7f,1.7f };
void drawScene()
{
	
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	camera.Draw(pos,shaderProgramID);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	//for (int i = 0; i < sphere.size(); ++i) {
	sphere[lighting].color = light.color;
	sphere[0].Draw();
	sphere[1].Draw();
	sphere[2].Draw();
	sphere[3].Draw();
	//}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glutSwapBuffers(); //화면에 출력
}
bool button_r = FALSE;
bool button_R = FALSE;
GLvoid Timer(int value)
{
	switch (value) {
	case 0:
		glutPostRedisplay();
		glutTimerFunc(17, Timer, 0);
		break;
	case 1:
		if (button_r) {
			sphere[lighting].revolutionAngle[1] += 1.5f;
			light.lightRevol.y += 1.5f;
			glutTimerFunc(17, Timer, 1);
		}
		break;
	case 2:
		if (button_R) {
			sphere[lighting].revolutionAngle[1] -= 1.5f;
			light.lightRevol.y -= 1.5f;
			glutTimerFunc(17, Timer, 2);
		}
		break;

	}
}

GLvoid KeyBoardFunc(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'c':
		if (light.color.r == 1 && light.color.g == 1 && light.color.b == 1) {
			light.color.g = 0; light.color.b = 0;
		}
		else if (light.color.r == 1 && light.color.g == 0 && light.color.b == 0)
		{
			light.color.g = 1;
		}
		else if (light.color.r == 1 && light.color.g == 1 && light.color.b == 0)
			light.color.b = 1;
		break;
	case 'w':
		light.lightpos.y += 0.1f;
		break;
	case 's':
		light.lightpos.y -= 0.1f;
		break;
	case 'd':
		light.lightpos.x += 0.1f;
		break;
	case 'a':
		light.lightpos.x -= 0.1f;
		break;
	case '=':
		light.lightpos.z += 0.1f;
		break;
	case '-':
		light.lightpos.z -= 0.1f;
		break;
	case 'r':
		if (!button_r) {
			button_r = !button_r;
			glutTimerFunc(17, Timer, 1);
		}
		else
			button_r = FALSE;

		break;
	case 'R':
		if (!button_R) {
			button_R = !button_R;
			glutTimerFunc(17, Timer, 2);
		}
		else
			button_R = FALSE;
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

