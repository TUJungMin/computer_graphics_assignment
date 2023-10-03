#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>

#pragma warning(disable:4996)

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitBuffer();
void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
char* filetobuf(const char* file);

enum
{
    Point,
    Line,
    Tri,
    Rect
};

#define winWidth 800
#define winHeight 800
#define MAX_NUM 10

GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vao[10], vbo[2];
int shape_flag = 0;
int num = -1;

int l_num = 0;
int t_num = 0;
int r_num = 0;
int p_num = 0;
int v_num = 0;

int draw_num = 0;
int shape_num[10] = {};

GLfloat PointShape[10][1][3] = {}; //--- �ﰢ�� ��ġ ��
GLfloat TriShape[10][3][3] = {}; //--- �ﰢ�� ��ġ ��
GLfloat LineShape[10][2][3] = {}; //--- �ﰢ�� ��ġ ��
GLfloat RectShape[10][4][3] = {}; //--- �ﰢ�� ��ġ ��
GLfloat PointColor[1][3] = { {0,0,1} }; //--- �ﰢ�� ��ġ ��
GLfloat TriColor[1][3] = { {1,0,0} }; //--- �ﰢ�� ��ġ ��
GLfloat LineColor[1][3] = { {0,0,0} }; //--- �ﰢ�� ��ġ ��
GLfloat RectColor[1][3] = { {1,0,0} }; //--- �ﰢ�� ��ġ ��

GLfloat Change_GLcoord(int n, int lenth) // x�� �״��, y�� �Լ� �տ� - ���̱�
{
    GLfloat coord = (GLfloat)(n - lenth / 2) * (GLfloat)(1.0 / (GLfloat)(lenth / 2.0));
    return coord;
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'p':
    case 'P':
        shape_flag = Point; // ��
        break;
    case 'l':
    case 'L':
        shape_flag = Line; // ��
        break;
    case 't':
    case 'T':
        shape_flag = Tri; // �ﰢ��
        break;
    case 'r':
    case 'R':
        shape_flag = Rect; // �ﰢ�� �ΰ�
        break;
    case 'w':
    case 'W':
        break;
    case 'S':
    case 's':
        break;
    case 'd':
    case 'D':
        break;
    case 'a':
    case 'A':
        break;
    case 'c':
    case 'C':
        num = 0;
        break;

    }
    glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}

void Mouse(int button, int state, int x, int y)
{
    GLfloat _x = Change_GLcoord(x, winWidth);
    GLfloat _y = -Change_GLcoord(y, winHeight);

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (num != 9)
            {
                switch (shape_flag)
                {
                case Point:
                    num++;
                    shape_num[num] = Point;

                    PointShape[p_num][0][0] = _x;
                    PointShape[p_num++][0][1] = _y;
                    break;
                case Tri:
                    num++;
                    shape_num[num] = Tri;

                    TriShape[t_num][0][0] = _x;
                    TriShape[t_num++][0][1] = _y + 0.05;

                    TriShape[t_num][1][0] = _x - 0.03;
                    TriShape[t_num++][1][1] = _y - 0.02;

                    TriShape[t_num][2][0] = _x + 0.03;
                    TriShape[t_num++][2][1] = _y - 0.02;
                    break;
                case Line:
                    num++;
                    shape_num[num] = Line;

                    LineShape[l_num][0][0] = _x;
                    LineShape[l_num++][0][1] = _y;

                    LineShape[l_num][1][0] = _x + 0.1;
                    LineShape[l_num++][1][1] = _y + 0.1;
                    break;
                case Rect:
                    num++;
                    shape_num[num] = Rect;

                    RectShape[r_num][0][0] = _x - 0.05;
                    RectShape[r_num++][0][1] = _y + 0.05;

                    RectShape[r_num][1][0] = _x - 0.05;
                    RectShape[r_num++][1][1] = _y - 0.05;

                    RectShape[r_num][2][0] = _x + 0.05;
                    RectShape[r_num++][2][1] = _y - 0.05;

                    RectShape[r_num][3][0] = _x + 0.05;
                    RectShape[r_num++][3][1] = _y + 0.05;
                    break;
                }
            }
        }
    }
}

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
    //--- ������ �����ϱ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");
    //--- GLEW �ʱ�ȭ�ϱ�
    glewExperimental = GL_TRUE;
    glewInit();
    make_shaderProgram();
    InitBuffer();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMainLoop();
}

GLvoid drawScene()
{
    draw_num = 0;
    //--- ����� ���� ����
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    //glClearColor(1.0, 1.0, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //--- ������ ���������ο� ���̴� �ҷ�����
    glUseProgram(shaderProgramID);
    //--- ����� VAO �ҷ�����
    for (int i = 0; i < num; i++)
    {
        glBindVertexArray(vao[i]);
        switch (shape_num[i])
        {
        case Point:
            glDrawArrays(GL_POINT, draw_num, 1);
            draw_num++;
            break;
        case Tri:
            glDrawArrays(GL_TRIANGLES, draw_num, 3);
            draw_num += 3;
            break;
        case Line:
            glDrawArrays(GL_LINE, draw_num, 2);
            draw_num += 2;
            break;
        case Rect:
            glDrawArrays(GL_TRIANGLE_STRIP, draw_num, 4);
            draw_num += 4;
            break;
        }

    }
    //--- �ﰢ�� �׸���
    glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void InitBuffer()
{
    v_num = 0;
    for (int i = v_num; i < v_num + p_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
        glBindVertexArray(vao[i]); //--- VAO�� ���ε��ϱ�
        glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
        //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
        //--- triShape �迭�� ������: 40 * float
        glBufferData(GL_ARRAY_BUFFER, p_num * 3 * sizeof(GLfloat), PointShape, GL_STATIC_DRAW);
        //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 0���� ��밡���ϰ� ��
        glEnableVertexAttribArray(0);
        //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
        //--- colors �迭�� ������: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, p_num * 3 * sizeof(GLfloat), PointColor, GL_STATIC_DRAW);
        //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 1���� ��� �����ϰ� ��.
        glEnableVertexAttribArray(1);
    }
    v_num += p_num;
    for (int i = v_num; i < v_num + l_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
        glBindVertexArray(vao[i]); //--- VAO�� ���ε��ϱ�
        glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
        //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
        //--- triShape �迭�� ������: 40 * float
        glBufferData(GL_ARRAY_BUFFER, l_num * 3 * 2 * sizeof(GLfloat), LineShape, GL_STATIC_DRAW);
        //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 0���� ��밡���ϰ� ��
        glEnableVertexAttribArray(0);
        //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
        //--- colors �迭�� ������: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, l_num * 3 * 2 * sizeof(GLfloat), LineColor, GL_STATIC_DRAW);
        //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 1���� ��� �����ϰ� ��.
        glEnableVertexAttribArray(1);
    }
    v_num += l_num;
    for (int i = v_num; i < v_num + r_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
        glBindVertexArray(vao[i]); //--- VAO�� ���ε��ϱ�
        glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
        //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
        //--- triShape �迭�� ������: 40 * float
        glBufferData(GL_ARRAY_BUFFER, r_num * 3 * 4 * sizeof(GLfloat), RectShape, GL_STATIC_DRAW);
        //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 0���� ��밡���ϰ� ��
        glEnableVertexAttribArray(0);
        //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
        //--- colors �迭�� ������: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, r_num * 3 * 4 * sizeof(GLfloat), RectColor, GL_STATIC_DRAW);
        //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 1���� ��� �����ϰ� ��.
        glEnableVertexAttribArray(1);
    }
    v_num += r_num;
    for (int i = v_num; i < v_num + t_num; i++)
    {
        glGenVertexArrays(1, &vao[i]); //--- VAO �� �����ϰ� �Ҵ��ϱ�
        glBindVertexArray(vao[i]); //--- VAO�� ���ε��ϱ�
        glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
        //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
        //--- triShape �迭�� ������: 40 * float
        glBufferData(GL_ARRAY_BUFFER, t_num * 3 * 3 * sizeof(GLfloat), TriShape, GL_STATIC_DRAW);
        //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 0���� ��밡���ϰ� ��
        glEnableVertexAttribArray(0);
        //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
        //--- colors �迭�� ������: 40 *float 
        glBufferData(GL_ARRAY_BUFFER, t_num * 3 * 3 * sizeof(GLfloat), TriColor, GL_STATIC_DRAW);
        //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //--- attribute �ε��� 1���� ��� �����ϰ� ��.
        glEnableVertexAttribArray(1);
    }

}

void make_shaderProgram()
{
    make_vertexShaders(); //--- ���ؽ� ���̴� �����
    make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
    //-- shader Program
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);
    //--- ���̴� �����ϱ�
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //--- Shader Program ����ϱ�
    glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
    vertexSource = filetobuf("vertex.glsl");
    //--- ���ؽ� ���̴� ��ü �����
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- ���ؽ� ���̴� �������ϱ�
    glCompileShader(vertexShader);
    //--- �������� ����� ���� ���� ���: ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragment.glsl");
    //--- �����׸�Ʈ ���̴� ��ü �����
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- �����׸�Ʈ ���̴� ������
    glCompileShader(fragmentShader);
    //--- �������� ����� ���� ���� ���: ������ ���� üũ
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR: fragment shader ������ ����\n" << errorLog << endl;
        return;
    }
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
    {
        cout << file << "���� ��ã��" << endl;
        exit(1);
    }
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0; // Null terminator
    return buf; // Return the buffer
}
