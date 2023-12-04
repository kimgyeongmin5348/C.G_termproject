#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
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
#include <random>

#define width 1200
#define height 800
#define h_vertex 0.1f
#define pi 3.141592
using namespace std;

random_device rd;
mt19937 gen(rd());

bool test = true;
GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid KeyBoardUp(unsigned char, int, int);
GLvoid SpecialKeyBoard(int, int, int);
GLvoid SpecialKeyBoardUp(int, int, int);
GLvoid Timer(int);

float bottom[] =
{
    1, 0, -1,
    -1, 0, 1,
    1, 0, 1,
    1, 0, -1,
    -1, 0, -1,
    -1, 0, 1
};

typedef struct object {
    float trans_x{}, trans_y{}, trans_z{};               // 기본 좌표값
    float trans_x_aoc{}, trans_y_aoc{}, trans_z_aoc{};      // 좌표 변화량 aoc - amount of change

    float rotate_x{}, rotate_y{}, rotate_z{};            // trans(x_trans + x_trans_aoc, 0.0f, 0.0f)
    float rotate_x_aoc{}, rotate_y_aoc{}, rotate_z_aoc{};   // You know what im say??

    float scale_x{ }, scale_y{}, scale_z{};
    float scale_x_aoc{}, scale_y_aoc{}, scale_z_aoc{};

    float color_r{}, color_g{}, color_b{};


    float r{};
}O;
typedef struct function {
    bool x_is_trans{ false }, y_is_trans{ false }, z_is_trans{ false };
    int x_max{}, z_max{};
}F;

O pilot, build[1000][1000], temp_build[1000][1000];
O temp, camera;
F h_f, temp_f;
GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
GLenum obj_type{ GL_FILL };
GLint g_window_w, g_window_h;
struct Camera_Option {
    float camera_distance = 3.0;

    float mouse_sensitivity = 200;

    float camera_drgree_x{};
    float camera_drgree_y{};
    float camera_delta_y = 0;

    float camera_fov = 60.0;

    float camera_moving_timer = 0;
};
Camera_Option CO, C1;

GLvoid Setting();
GLint Collision(float first_x1, float first_x2, float last_x1, float last_x2)  // i'am 충돌체크에요
{
    if (first_x1 <= last_x1 && last_x1 <= first_x2)
        return 1;
    if (first_x1 <= last_x2 && last_x2 <= first_x2)
        return 1;
    if (last_x1 <= first_x1 && first_x1 <= last_x2)
        return 1;
    if (last_x1 <= first_x2 && first_x2 <= last_x2)
        return 1;
    return 0;
}
GLvoid Building_Mat()  // i'am 빌딩 만들기이에요
{
    glm::mat4 B_Matrix = glm::mat4(1.0f);
    for (int i = 0; i < h_f.x_max; ++i) {
        for (int j = 0; j < h_f.z_max; ++j) {
            //윗면
            B_Matrix = glm::mat4(1.0f);
            B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].trans_x, 0.f, build[i][j].trans_z));
        }
    }
}
GLvoid Building_Setting()  // i'am 빌딩들 랜덤 생성이에요
{

    uniform_int_distribution<> dis{ 5,70 };   // 숫자 수정 필요
    uniform_int_distribution<> disx_z{ 0,2000 };
 
}


GLfloat rot;
GLfloat rot_t;
GLfloat camera_rot;
GLfloat C_R;
GLfloat arm_rot;
GLfloat limit;
GLvoid Pilot() // i'am 헬기(조종사) 에요
{

   

}

GLvoid Pilot_collison()  // i'am 헬기 충돌 체크에요 (vs 건물) 총알 발사 하면 이김 -> 총알 충돌체크도 해야할듯?
{
    
}

GLvoid Gun() //i'am 총알이에요
{

}

GLvoid Gun_collision() // i'am 총알 충돌체크에요
{

}
GLvoid BackGround() //i'am 지형이에요    < -   이번 숙제를 바탕으로 지형이 올라오게 만들고 요리피하고 총알로 부수면서 가는 게임을 함 만들어 볼까? 미로 찾기 마냥... 흠... 이건 일단 보류
{
    glm::mat4 Bottom = glm::mat4(1.0f);
    Bottom = glm::scale(Bottom, glm::vec3(1000.0f, 0.f, 1000.0f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Bottom));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.7f, 0.7f, 0.4f, 1.0);
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
//
GLfloat hexa[] = {   //육면체
    //윗면
    -h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    //정면
    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //아랫면
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, -h_vertex,

    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, h_vertex,

    //뒷면
    h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,


    //좌측면
    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //우측면
    h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex
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
        cout << "good" << endl;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(s_program);
}
GLvoid InitBuffer()
{
    glGenVertexArrays(3, VAO);

    glBindVertexArray(VAO[0]);
    glGenBuffers(2, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof hexa, hexa, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[(1)]);
    glGenBuffers(2, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);



    glUseProgram(s_program);
    unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    glUniform3f(lightPosLocation, 5.0f, 10.0f, 0.0f);
    unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
    unsigned int objColorLocation = glGetUniformLocation(s_program, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
    glUniform3f(objColorLocation, 0.7f, 0.7f, 0.4f);

}
void drawScene()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
    glUseProgram(s_program);
    for (int i = 0; i < 2; ++i) {

        if (i == 0) {
            glViewport(0, 0, width, height);
            glm::vec3 cameraPos = glm::vec3(pilot.trans_x, pilot.trans_y + 1.5f, pilot.trans_z); //--- 카메라 위치
            glm::vec3 cameraDirection = glm::vec3(pilot.trans_x, 0.3f, pilot.trans_z - 5); //--- 카메라 바라보는 방향
            glm::vec3 cameraUp = glm::vec3(0.0f, 4.0f, 0.0f); //--- 카메라 위쪽 방향
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
            view = glm::translate(view, glm::vec3(pilot.trans_x, pilot.trans_y + 1.0f, pilot.trans_z));
            view = glm::rotate(view, glm::radians(CO.camera_drgree_y), glm::vec3(1.0f, 0.0f, 0.0f));
            view = glm::rotate(view, glm::radians(CO.camera_drgree_x), glm::vec3(0.0f, 1.0f, 0.0f));
            view = glm::translate(view, glm::vec3(-pilot.trans_x, -pilot.trans_y - 1.0f, -pilot.trans_z));

            unsigned int viewLocation = glGetUniformLocation(s_program, "view"); //--- 뷰잉 변환 설정
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
            projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
            unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        }
        else {
            glViewport(1050, 550, 150, 150);
            glm::vec3 cameraPos = glm::vec3(pilot.trans_x, 5.0f, pilot.trans_z);         //위치
            glm::vec3 cameraDirection = glm::vec3(pilot.trans_x, 0.0f, pilot.trans_z);   //바라보는 방향
            glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -10.0f);         //카메라 상향
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

            unsigned int viewLocation = glGetUniformLocation(s_program, "view");
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
            projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
            unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        }
        
       
        Building_Mat();
        Pilot();
        Pilot_collison();
        Gun();
        Gun_collision();
        BackGround();
    }

    glutSwapBuffers();
}
void Reshape(int w, int h) {

    g_window_w = w;
    g_window_h = h;
    glViewport(0, 0, w, h);

}
GLvoid KeyBoardUp(unsigned char key, int x, int y) {
    
    glutPostRedisplay();
}
GLvoid KeyBoard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'q':
        exit(0);
        cout << "exit program" << endl;
        break;
    case 'x':
        h_f.x_is_trans = !h_f.x_is_trans;
        cout << "x축 이동(카메라가)" << endl;
    }
    glutPostRedisplay();
}
GLvoid SpecialKeyBoard(int key, int x, int y)
{

    glutPostRedisplay();
}
GLvoid SpecialKeyBoardUp(int key, int x, int y)
{
    switch (key) {
    }
    glutPostRedisplay();
}
void Mouse(int button, int state, int x, int y)
{
   
}
void Motion(int x, int y)
{
   
}
void MouseChange(int x, int y) {

}
void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("SkyLinw");
    Setting();

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "NOT INIT" << endl;
    }
    else
        cout << "INIT<<endl";
    InitShader();
    InitBuffer();
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(KeyBoard);
    glutSpecialFunc(SpecialKeyBoard);
    glutSpecialUpFunc(SpecialKeyBoardUp);
    glutKeyboardUpFunc(KeyBoardUp);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutReshapeFunc(Reshape);
    glutTimerFunc(5, Timer, 1);
    glutMainLoop();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
GLvoid Setting()
{
   
}
GLvoid Timer(int value)
{
    if (h_f.x_is_trans) {
        pilot.trans_x += 0.1;
    }
    glutPostRedisplay();
    glutTimerFunc(5, Timer, 1);
}