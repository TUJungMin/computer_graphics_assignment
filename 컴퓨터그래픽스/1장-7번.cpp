#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#pragma comment(lib, "freeglut")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")


#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading 
    if (!fptr) // Return NULL on failure 
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
    length = ftell(fptr); // Find out how many bytes into the file we are 
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
    fclose(fptr); // Close the file 
    buf[length] = 0; // Null terminator 
    return buf; // Return the buffer 
}

enum DRAW_CASE {
    D_SOLID,//fill
    D_POINT,//
    D_LINE//
};
float WIDTH = 800;
float HEIGHT = 600;
using namespace std;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
float r = 0, g = 0, b = 1.0;


void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid ClickFunc(int button, int state, int x, int y);

//--- �ʿ��� ���� ����
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader;
GLint result;
GLchar errorLog[512];

std::pair<float, float> ConvertWinToGL(int x, int y) {
    float mx = ((float)x - (WIDTH / 2)) / (WIDTH / 2); //gl��ǥ��� ����
    float my = -((float)y - (HEIGHT / 2)) / (HEIGHT / 2); //gl��ǥ��� ����
    return { mx, my };
}
GLvoid Keyboard(unsigned char key, int x, int y);
struct Triangle {
    glm::vec3 m_vertex[3];
    glm::vec3 m_color[3];
    GLuint m_VBOVertex;
    GLuint m_VBOColor;
    GLuint m_VAO;

    Triangle(int x, int y)
    {
        auto center = ConvertWinToGL(x, y);
        random_device rd;
        default_random_engine dre(rd());
        uniform_real_distribution<float> color(0, 1);
        float r = color(dre);
        float g = color(dre);
        float b = color(dre);
        m_color[0].r = r;
        m_color[1].r = r;
        m_color[2].r = r;
        m_color[0].g = g;
        m_color[1].g = g;
        m_color[2].g = g;
        m_color[0].b = b;
        m_color[1].b = b;
        m_color[2].b = b;
        center;
        m_vertex[0].x = center.first;
        m_vertex[0].y = center.second + 0.15f;

        m_vertex[1].x = center.first - 0.11f;
        m_vertex[1].y = center.second - 0.11f;

        m_vertex[2].x = center.first + 0.11f;
        m_vertex[2].y = center.second - 0.11f;

        m_vertex[0].z = m_vertex[1].z = m_vertex[2].z = 0.0f;
            
        SetVAO();
    }

    void SetVAO()
    {
        glGenVertexArrays(1, &m_VAO);//vao ��ü ����, params(����, GLuint*)//n���� �޾Ƶ� �����ǰ��Ϸ���
        glBindVertexArray(m_VAO);//���ε��ҰŶ�� �˷��ֱ�

        //vbo��ü ����
        glGenBuffers(1, &m_VBOVertex);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_vertex, GL_STATIC_DRAW);
        int positionAttrib = glGetAttribLocation(shaderProgramID, "vPos");
        //� ��Ʈ��������, ���̴����� vec3�� 3(���� ����), ������ Ÿ��, ����ȭ, �ϳ��� ��� ũ��?, ���� ��ġ
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(positionAttrib);

        glGenBuffers(1, &m_VBOColor);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOColor);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, m_color, GL_STATIC_DRAW);
        int colorAttrib = glGetAttribLocation(shaderProgramID, "vColor");
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(colorAttrib);
    }

    void Draw(int type)
    {
        switch (type)
        {
        case D_SOLID:
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        case D_POINT:
            glBindVertexArray(m_VAO);
            glPointSize(15.0);
            glDrawArrays(GL_POINTS, 0, 3);
            //glLineWidth(15.0f);
            //glDrawArrays(GL_LINES, 1, 3);
            break;
        case D_LINE:
            glBindVertexArray(m_VAO);
            glLineWidth(15.0f);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
            break;
        default:
            break;
        }
    }
};

vector<Triangle> g_triangles;

void main(int argc, char** argv) {//--- ������ ����ϰ� �ݹ��Լ� ����
    glutInit(&argc, argv); // glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
    glutInitWindowPosition(0, 0); // �������� ��ġ ����
    glutInitWindowSize(WIDTH, HEIGHT); // �������� ũ�� ����
    glutCreateWindow("OPENGL"); // ������ ����
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized" << std::endl;

    make_vertexShaders(); //--- ���ؽ� ���̴� �����
    make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
    shaderProgramID = make_shaderProgram();
    glutDisplayFunc(drawScene); // ��� �Լ��� ����
    glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(ClickFunc);

    glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid drawScene() { //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
    glClearColor(r, g, b, 1);
    glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
    glUseProgram(shaderProgramID);
    for (auto& t : g_triangles)t.Draw(D_SOLID);

    for (auto& t : g_triangles)t.Draw(D_LINE);

    for (auto& t : g_triangles)t.Draw(D_POINT);
    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
    vertexSource = filetobuf("vertex.glsl");

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
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders() {
    GLchar* fragmentSource;
    //--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
    fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}

GLuint make_shaderProgram() {
    //vertex, frament shader�� �̹� �������� �� ��Ȳ
    GLuint shaderID;
    shaderID = glCreateProgram(); //--- ���̴� ���α׷� ����� - �� ���̴� �پ�ߵ�, vertex - fragment�� ¦�� �¾ƾߵ�
    glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
    glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
    glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�

    glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
    glDeleteShader(fragmentShader);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
        return false;
    }
    glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
    //--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
    //--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
    //--- ����ϱ� ������ ȣ���� �� �ִ�.
    return shaderID;
}

GLvoid ClickFunc(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        g_triangles.emplace_back(x, y);
    }
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'p':
        
        break;
    }
}