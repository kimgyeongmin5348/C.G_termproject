#define _CRT_SECURE_NO_WARNINGS //--- ÇÁ·Î±×·¥ ¸Ç ¾Õ¿¡ ¼±¾ðÇÒ °Í
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define width 800
#define height 600
#define h_vertex 0.5f

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid Setting();

GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLenum hexa_mode{ GL_FILL };
GLint g_window_w, g_window_h;

typedef struct object {
    float x_rotate{}, y_rotate{};
    float x_trans{}, y_trans{};
}O;

typedef struct function {
    bool cull{ false };

    bool y_rotate{ false };
    bool Y_rotate{ false };
    bool x_rotate{ false };
    bool X_rotate{ false };
}F;

O h[6], q[6];
O l;
F h_f;
int count{};
bool change_type{ false };

float line[]
{
   -2,0,0,
   2,0,0,
   0,-2,0,
   0,2,0
};

float line_color[]
{
   1,1,1,
   1,1,1,
   1,1,0,
   1,1,0
};

float hexa[] =
{   //À°¸éÃ¼
   //À­¸é
   -h_vertex, h_vertex, h_vertex,
   h_vertex, h_vertex, h_vertex,
   -h_vertex, h_vertex, -h_vertex,

   h_vertex, h_vertex, h_vertex,
   h_vertex, h_vertex, -h_vertex,
   -h_vertex, h_vertex, -h_vertex,
   //Á¤¸é
   -h_vertex, -h_vertex, h_vertex,
   h_vertex, -h_vertex, h_vertex,
   -h_vertex, h_vertex, h_vertex,

   h_vertex, -h_vertex, h_vertex,
   h_vertex, h_vertex, h_vertex,
   -h_vertex, h_vertex, h_vertex,

   //¾Æ·§¸é
   -h_vertex, -h_vertex, h_vertex,
   -h_vertex, -h_vertex, -h_vertex,
   h_vertex, -h_vertex, -h_vertex,

   -h_vertex, -h_vertex, h_vertex,
   h_vertex, -h_vertex, -h_vertex,
   h_vertex, -h_vertex, h_vertex,

   //µÞ¸é
   h_vertex, -h_vertex, -h_vertex,
   -h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, -h_vertex,

   -h_vertex, -h_vertex, -h_vertex,
   -h_vertex, h_vertex, -h_vertex,
   h_vertex, h_vertex, -h_vertex,


   //ÁÂÃø¸é
   -h_vertex, -h_vertex, -h_vertex,
   -h_vertex, h_vertex, h_vertex,
   -h_vertex, h_vertex, -h_vertex,

   -h_vertex, -h_vertex, -h_vertex,
   -h_vertex, -h_vertex, h_vertex,
   -h_vertex, h_vertex, h_vertex,

   //¿ìÃø¸é
   h_vertex, -h_vertex, h_vertex,
   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex,

   h_vertex, -h_vertex, -h_vertex,
   h_vertex, h_vertex, -h_vertex,
   h_vertex, h_vertex, h_vertex
};

float hexa_color[] =
{     //À°¸éÃ¼
   //À­¸é
   1,0,0,
   0,1,0,
   1,0,1,
   1,0,0,
   0,1,0,
   0,0,1,

   //Á¤¸é
   1,0,0,
   0,1,0,
   0,1,1,
   1,1,0,
   1,1,0,
   0,1,1,

   //¾Æ·§¸é
   1,0,1,
   0,1,1,
   0,1,1,
   0,0,1,
   0,1,1,
   1,0,1,

   //µÞ¸é
   1,1,0,
   0,1,0,
   0,1,1,
   0,1,1,
   0,1,0,
   1,0,0,

   //ÁÂÃø¸é
   1,0,0,
   0,1,0,
   0,0,1,
   1,0,0,
   0,1,0,
   0,0,1,

   //¿ìÃø¸é
   0,0,1,
   0,1,0,
   1,0,0,
   0,0,1,
   0,1,0,
   1,0,0,
};

float quad[] =
{   //»ç°¢»Ô
   -0.5f, -0.5f, -0.5f,
   0.0f, 0.5f, 0.0f,
   0.5f, -0.5f, -0.5f,

   0.5f, -0.5f, -0.5f,
   0.0f, 0.5f, 0.0f,
   0.5f, -0.5f, 0.5f,

   0.5f, -0.5f, 0.5f,
   0.0f, 0.5f, 0.0f,
   -0.5f, -0.5f, 0.5f,

   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   0.0f, 0.5f, 0.0f,

   //¾Æ·¡
  -0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,

  -0.5f, -0.5f, 0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, 0.5f,
};

float quad_color[] =
{   //»ç°¢»Ô
  1,0,0,
  0,1,0,
  0,0,1,

  1,0,0,
  0,1,0,
  0,0,1,

  1,0,0,
  0,1,0,
  0,0,1,

  0,0,1,
  0,1,0,
  1,0,0,

  1,0,0,
  0,1,0,
  0,1,1,

  0,1,0,
  0,0,1,
  1,0,0
};

GLchar* filetobuf(const GLchar* file)
{
    FILE* fptr;
    long length;
    GLchar* buf;

    fopen_s(&fptr, file, "rb");
    if (!fptr)
        return NULL;

    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (GLchar*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;

    return buf;
}

void make_vertexShaders()
{
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
}

void make_fragmentShader()
{
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

}

void InitShader()
{
    make_vertexShaders();
    make_fragmentShader();

    s_program = glCreateProgram();

    glAttachShader(s_program, vertexShader);
    glAttachShader(s_program, fragmentShader);
    glLinkProgram(s_program);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << endl;
        exit(-1);
    }

    else
        cout << "good";
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(s_program);
}

GLvoid InitBuffer()
{
    glGenVertexArrays(3, VAO);

    // ¼±
    glBindVertexArray(VAO[0]);
    glGenBuffers(2, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // ¼± »ö
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // »ö»ó
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_color), line_color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Á¤À°¸éÃ¼
    glBindVertexArray(VAO[1]);
    glGenBuffers(2, &VBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // Á¤À°¸éÃ¼ »ö
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]); // »ö»ó
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexa_color), hexa_color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Á¤»ç¸éÃ¼
    glBindVertexArray(VAO[2]);
    glGenBuffers(2, &VBO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // Á¤»ç¸éÃ¼ »ö
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]); // »ö»ó
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_color), quad_color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

}

void drawScene()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //±íÀÌ Ã¼Å© (ÄÃ¸µ)
    glUseProgram(s_program);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    unsigned int viewLocation = glGetUniformLocation(s_program, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);
    unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


    glm::mat4 L_Matrix = glm::mat4(1.0f);
    unsigned int HtransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(L_Matrix));
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_LINES, 0, 6);

    for (int i = 0; i < 6; ++i) {
        if (!change_type) {
            glm::mat4 H_Matrix = glm::mat4(1.0f);

            // H_Matrix¸¦ ÁÖ¾îÁø º¤ÅÍ °ª¸¸Å­ ÀÌµ¿½ÃÅµ´Ï´Ù.
            H_Matrix = glm::translate(H_Matrix, glm::vec3(l.x_trans, l.y_trans, 0.0f));
            H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].x_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
            H_Matrix = glm::rotate(H_Matrix, glm::radians(h[i].y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
            qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, hexa_mode);
            glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
            glBindVertexArray(VAO[1]);
            glDrawArrays(GL_TRIANGLES, i * 6, 6);
        }

        else {
            glm::mat4 Q_Matrix = glm::mat4(1.0f);

            Q_Matrix = glm::translate(Q_Matrix, glm::vec3(l.x_trans, l.y_trans, 0.0f));
            Q_Matrix = glm::rotate(Q_Matrix, glm::radians(h[i].x_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
            Q_Matrix = glm::rotate(Q_Matrix, glm::radians(h[i].y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(HtransformLocation, 1, GL_FALSE, glm::value_ptr(Q_Matrix));
            qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, hexa_mode);
            glBindVertexArray(VAO[2]);
            glDrawArrays(GL_TRIANGLES, i * 3, 3);
        }
    }

    glutSwapBuffers();
}

void Reshape(int w, int h) {

    g_window_w = w;
    g_window_h = h;
    glViewport(0, 0, w, h);

}

GLvoid KeyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'y':
        h_f.y_rotate = !h_f.y_rotate;
        break;
    case 'Y':
        h_f.Y_rotate = !h_f.Y_rotate;
        break;
    case 'x':
        h_f.x_rotate = !h_f.x_rotate;
        break;
    case 'X':
        h_f.X_rotate = !h_f.X_rotate;
        break;
    case 'h':
        h_f.cull = !h_f.cull;
        if (h_f.cull)glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        break;
    case 'p':
        change_type = !change_type;
        break;
    case 'c':
        change_type = !change_type;
    case 'w':
        if (!(hexa_mode == GL_LINE))
            hexa_mode = GL_LINE;
        else
            hexa_mode = GL_FILL;
        break;
    }
    glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        l.x_trans -= 0.1f;
    }
    else if (key == GLUT_KEY_RIGHT) {
        l.x_trans += 0.1f;
    }
    else if (key == GLUT_KEY_UP) {
        l.y_trans += 0.1f;
    }
    else if (key == GLUT_KEY_DOWN) {
        l.y_trans -= 0.1f;
    }
    glutSwapBuffers();
}

GLvoid Timer(int value)
{

    if (h_f.y_rotate)
        for (O& i : h)
            i.y_rotate += 0.2f;

    if (h_f.Y_rotate)
        for (O& i : h)
            i.y_rotate -= 0.2f;

    if (h_f.x_rotate)
        for (O& i : h)
            i.x_rotate += 0.2f;

    if (h_f.X_rotate)
        for (O& i : h)
            i.x_rotate -= 0.2f;


    glutPostRedisplay();
    glutTimerFunc(5, Timer, 1);
}

GLvoid Motion(int x, int y)
{
    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
    glutPostRedisplay();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("C.G_NO15");
    Setting();

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "NOT INIT" << endl;
    }
    else
        cout << "INIT<<endl" << endl;
        cout << "y/Y yÃàÀ¸·Î ¾ç,À½ È¸Àü" << endl;
        cout << "x/X xÃàÀ¸·Î ¾ç,À½ È¸Àü" << endl;
        cout << "h µÞ¸é º¸ÀÌ°Ô" << endl;
        cout << "p,c »ç°¢»Ô, Á¤À°¸éÃ¼ ·Î º¯°æ" << endl;
        cout << "¹æÇâÅ° ÀÌµ¿" << endl;

    InitShader();
    InitBuffer();
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(KeyBoard);
    glutSpecialFunc(SpecialKeyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutReshapeFunc(Reshape);
    glutTimerFunc(5, Timer, 1);
    glutMainLoop();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLvoid Setting()
{
    for (int i = 0; i < 6; ++i) {
        h[i].x_rotate = 30.f;
        h[i].y_rotate = -30.f;
    }
    l.x_rotate = h[0].x_rotate;
    l.y_rotate = h[0].y_rotate;
}