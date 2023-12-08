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
#include <string>

#define width 1200
#define height 800
#define h_vertex 0.2f
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
    float x_trans{}, y_trans{}, z_trans{};     
    float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
    float x_rotate{}, y_rotate{}, z_rotate{};
    float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};
    float x_scale{ }, y_scale{}, z_scale{};
    float x_scale_aoc{}, y_scale_aoc{}, z_scale_aoc{};
    float color_r{}, color_g{}, color_b{};


    float r{};
}O;
typedef struct function {
    bool x_is_trans{ false }, y_is_trans{ false }, z_is_trans{ false };
    int x_max{}, z_max{};
    bool left_walk{ false };
    bool right_walk{ false };
    bool front_walk{ false };
    bool back_walk{ false };
    bool left_turn{ false };
    bool first_see{ false };

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

void ReadObj(const std::string objfilename, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& normalVertex, std::vector<glm::vec2>& vtVertex)
{
    int lineCount = 0;
    std::string line;
    std::string check[6];
    int vertexNum = 0;
    int normalNum = 0;
    int cordiNum = 0;
    std::ifstream inFile(objfilename);
    std::vector<glm::vec4>face;
    std::vector<glm::vec4>Noramlface;
    std::vector<glm::vec4>vtface;
    while (std::getline(inFile, line)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertexNum++;
        }
        if (line[0] == 'v' && line[1] == 'n') {
            normalNum++;
        }
        if (line[0] == 'v' && line[1] == 't') {
            cordiNum++;
        }
        std::cout << line << std::endl;
    }
    glm::vec4* vertexData = new glm::vec4[vertexNum];
    glm::vec4* normalData = new glm::vec4[normalNum];
    glm::vec2* cordinaterData = new glm::vec2[cordiNum];

    inFile.clear();
    inFile.seekg(0, std::ios::beg);
    vertexNum = 0;
    normalNum = 0;
    cordiNum = 0;
    char head[2];
    int faceNum[3];
    int vnNum[3];
    int vtNum[3];
    std::string nt;
    char n;
    char s;
    while (inFile >> std::noskipws >> head[0]) {
        if (head[0] == 'v') {
            inFile >> std::noskipws >> head[1];
            if (head[1] == ' ') {
                inFile >> std::skipws >> vertexData[vertexNum].x >> vertexData[vertexNum].y >> vertexData[vertexNum].z;
                vertexNum++;
            }
            else if (head[1] == 'n') {
                inFile >> std::skipws >> normalData[normalNum].x >> normalData[normalNum].y >> normalData[normalNum].z;
                normalNum++;
            }
            else if (head[1] == 't') {
                float trash;
                inFile >> std::skipws >> cordinaterData[cordiNum].x >> cordinaterData[cordiNum].y >> trash;
                cordiNum++;
            }
            head[1] = '\0';
        }
        if (head[0] == 'f') {
            inFile >> std::noskipws >> head[1];
            if (head[1] == ' ') {
                for (int i = 0; i < 3; ++i) {
                    inFile >> std::skipws >> faceNum[i] >> std::noskipws >> n >> vtNum[i] >> std::noskipws >> s >> vnNum[i];
                    // 4개의 인덱스를 처리하려면 위 줄을 주석 해제하고, 아래 주석 처리된 줄을 사용하세요.
                    // inFile >> std::skipws >> faceNum[i] >> std::noskipws >> n >> vtNum[i] >> std::noskipws >> s >> vnNum[i] >> std::noskipws >> trash;
                }

                glm::vec4 temp = glm::vec4(faceNum[0], faceNum[1], faceNum[2], 1);//faceNum[3]
                glm::vec4 vttemp = glm::vec4(vtNum[0], vtNum[1], vtNum[2], 1); //vtNum[3]
                glm::vec4 vntemp = glm::vec4(vnNum[0], vnNum[1], vnNum[2], 1);//vnNum[3]
                face.push_back(temp);
                vtface.push_back(vttemp);
                Noramlface.push_back(vntemp);
            }
            head[1] = '\0';
        }
    }
    for (auto iter = face.begin(); iter < face.end(); iter++) {
        vertex.push_back(vertexData[(int)(iter->x) - 1]);
        vertex.push_back(vertexData[(int)(iter->y) - 1]);
        vertex.push_back(vertexData[(int)(iter->z) - 1]);                     //버텍스 좌표
        //vertex.push_back(vertexData[(int)(iter->w) - 1]);
    }
    for (auto iter = vtface.begin(); iter < vtface.end(); iter++) {
        vtVertex.push_back(cordinaterData[(int)(iter->x) - 1]);
        vtVertex.push_back(cordinaterData[(int)(iter->y) - 1]);
        vtVertex.push_back(cordinaterData[(int)(iter->z) - 1]);                //텍스쳐 좌표
        //ve``rtex.push_back(vertexData[(int)(iter->w) - 1]);
    }
    for (auto iter = Noramlface.begin(); iter < Noramlface.end(); iter++) {
        normalVertex.push_back(normalData[(int)(iter->x) - 1]);
        normalVertex.push_back(normalData[(int)(iter->y) - 1]);
        normalVertex.push_back(normalData[(int)(iter->z) - 1]);               //노멀 좌표
        //normalVertex.push_back(normalData[(int)(iter->w) - 1]);
    }
    delete[] vertexData;
    delete[] cordinaterData;
    delete[] normalData;
    inFile.close();
}

GLvoid InitBuffer()
{
    glGenVertexArrays(3, VAO);

    glBindVertexArray(VAO[0]);
    glGenBuffers(2, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
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
            B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].x_trans, 0.f, build[i][j].z_trans));
            B_Matrix = glm::scale(B_Matrix, glm::vec3(4.0f, build[i][j].y_scale, 4.0f));
            B_Matrix = glm::translate(B_Matrix, glm::vec3(0.f, 0.2f, 0.f));
            unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
            glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(B_Matrix));
            qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, obj_type);
            int objColorLocation = glGetUniformLocation(s_program, "objectColor");
            unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
            glUniform1f(isCheck, false);
            glUniform4f(objColorLocation, 0.f, 0.5f, 0.5f, 1.0);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            B_Matrix = glm::mat4(1.0f);
            B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].x_trans, 0.0f, build[i][j].z_trans));
            B_Matrix = glm::scale(B_Matrix, glm::vec3(4.0f, build[i][j].y_scale, 4.0f));
            B_Matrix = glm::translate(B_Matrix, glm::vec3(0.f, 0.2f, 0.f));
            StransformLocation = glGetUniformLocation(s_program, "transform");
            glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(B_Matrix));
            qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, obj_type);
            objColorLocation = glGetUniformLocation(s_program, "objectColor");
            isCheck = glGetUniformLocation(s_program, "isCheck");
            glUniform1f(isCheck, false);
            glUniform4f(objColorLocation, 0.7f, 0.7f, 0.4f, 1.0);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLES, 30, 6);

            B_Matrix = glm::mat4(1.0f);
            B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].x_trans, 0.0f, build[i][j].z_trans));
            B_Matrix = glm::scale(B_Matrix, glm::vec3(4.0f, build[i][j].y_scale, 4.0f));
            B_Matrix = glm::translate(B_Matrix, glm::vec3(0.f, 0.2f, 0.f));
            StransformLocation = glGetUniformLocation(s_program, "transform");
            glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(B_Matrix));
            qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, obj_type);
            objColorLocation = glGetUniformLocation(s_program, "objectColor");
            isCheck = glGetUniformLocation(s_program, "isCheck");
            glUniform1f(isCheck, false);
            glUniform4f(objColorLocation, 0.7f, 0.7f, 0.4f, 1.0);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLES, 6, 24);
        }
    }
}
GLvoid Building_Setting()  // i'am 빌딩들 랜덤 생성이에요
{

    uniform_int_distribution<> dis{ 900,1000 };   // 숫자 수정 필요
    uniform_int_distribution<> disx_z{ 0,2000 };
    h_f.x_max, h_f.z_max = dis(gen);


}


GLfloat rot;
GLfloat rot_t;
GLfloat camera_rot;
GLfloat C_R;
GLfloat arm_rot;
GLfloat limit;
GLvoid Pilot() // i'am 헬기(조종사) 에요
{
    //엔진
    glm::mat4 H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.5f, 0.2f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.5f, 0.3f, 0.5f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //날개 1
    glm::mat4 H_Matrix1 = glm::mat4(1.0f);
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix1 = glm::scale(H_Matrix1, glm::vec3(4.5f, 0.2f, 0.2f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix1));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //날개2
    glm::mat4 H_Matrix2 = glm::mat4(1.0f);
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix2 = glm::scale(H_Matrix2, glm::vec3(0.2f, 0.2f, 4.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix2));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 중간
    glm::mat4 H_Matrix3 = glm::mat4(1.0f);
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, 0.7f, -0.2f));
    H_Matrix3 = glm::scale(H_Matrix3, glm::vec3(1.1f, 1.1f, 3.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix3));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 앞
    glm::mat4 H_Matrix4 = glm::mat4(1.0f);
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, 0.65f, 0.3f));
    H_Matrix4 = glm::scale(H_Matrix4, glm::vec3(0.3f, 0.3f, 0.4f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix4));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    gluSphere(qobj, 1.0, 20, 30);

    // 몸통 뒤
    glm::mat4 H_Matrix5 = glm::mat4(1.0f);
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, 0.7f, -1.0f));
    H_Matrix5 = glm::scale(H_Matrix5, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix5));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    gluCylinder(qobj, 0.3f, 1.1f, 0.5, 100, 1);

    // 몸통 뒤(꼬리앞)
    glm::mat4 H_Matrix6 = glm::mat4(1.0f);
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, 0.7f, -1.7f));
    H_Matrix6 = glm::scale(H_Matrix6, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix6));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    gluCylinder(qobj, 0.3f, 0.3f, 1.5, 100, 1);

    // 몸통 뒤(꼬리 날개)
    glm::mat4 H_Matrix7 = glm::mat4(1.0f);
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, 0.8f, -1.6f));
    H_Matrix7 = glm::scale(H_Matrix7, glm::vec3(0.1f, 0.7f, 0.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix7));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 0.0f, 0.0f, 1.0f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥(왼쪽)
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 1.0f, 0.3f, 0.7f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥2
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, 0.f, pilot.z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(-0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 1.0f, 0.3f, 0.7f, 1.0);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    





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
    /*glm::mat4 Bottom = glm::mat4(1.0f);
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
    glDrawArrays(GL_TRIANGLES, 0, 6);*/
}
//

void drawScene()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
    glUseProgram(s_program);
    for (int i = 0; i < 2; ++i) {

        if (i == 0) {
            glViewport(0, 0, width, height);
            if (!h_f.first_see) {  // 이건 3인칭 (기본값) => 헬기랑 같이 움직이게 하려 했는데 안된다... 흠... 다시 시도해보자
                glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -0.3f);			//위치
                glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);	//바라보는 방향
                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);			//카메라 상향
                glm::mat4 view = glm::mat4(1.0f);
                view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

                view = glm::rotate(view, glm::radians(-30.f), glm::vec3(1.0f, 0.0f, 0.0f));

                view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));

                view = glm::translate(view, glm::vec3(pilot.x_trans_aoc, pilot.y_trans_aoc, pilot.z_trans_aoc));


                unsigned int viewLocation = glGetUniformLocation(s_program, "view"); //--- 뷰잉 변환 설정
                glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
            }
            else {  // 이거 1인칭 하는건데 왜 안되는 것일까..? 수정 필요 
                glm::vec3 cameraPos = glm::vec3(0.0f, 0.1f, 4.95f);			//위치
                glm::vec3 cameraDirection = glm::vec3(0.0f, 0.05f, 5.95f);	//바라보는 방향
                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);			//카메라 상향
                glm::mat4 view = glm::mat4(1.0f);
                view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

                view = glm::rotate(view, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                view = glm::rotate(view, glm::radians(-pilot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
                view = glm::translate(view, glm::vec3(-pilot.x_trans - pilot.x_trans_aoc, 0.0f, -pilot.z_trans - pilot.z_trans_aoc));
                unsigned int viewLocation = glGetUniformLocation(s_program, "view");
                glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
            }
           

            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
            projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
            unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        }
        else {
            glViewport(1050, 550, 150, 150);
            glm::vec3 cameraPos = glm::vec3(pilot.x_trans, 5.0f, pilot.z_trans);         //위치
            glm::vec3 cameraDirection = glm::vec3(pilot.x_trans, 0.0f, pilot.z_trans);   //바라보는 방향
            glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 10.0f);         //카메라 상향
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
        Building_Setting();
    }

    glutSwapBuffers();
}
void Reshape(int w, int h) {

    g_window_w = w;
    g_window_h = h;
    glViewport(0, 0, w, h);

}
GLvoid KeyBoardUp(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'a':
        h_f.left_walk = false;
        break;
    case 'd':
        h_f.right_walk = false;
        break;
    case 'w':
        h_f.front_walk = false;
        pilot.x_rotate = 0.f;
        break;
    case 's':
        h_f.back_walk = false;
        break;
    }
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
    case 'd':
        h_f.right_walk = true;
        cout << "오른쪽으로" << endl;
        break;
    case 'a':
        h_f.left_walk = true;
        cout << "왼쪽으로" << endl;
        break;
    case 'w':
        h_f.front_walk = true;
        pilot.x_rotate = 15.0f;
        cout << "앞으로" << endl;
        break;
    case 's':
        h_f.back_walk = true;
        cout << "뒤로" << endl;
        break;
    case 'm':
        h_f.left_turn = !h_f.left_turn;
        break;
    case 'c':
        memcpy(&pilot, &temp, sizeof(pilot));
        memcpy(&camera, &temp, sizeof(camera));
        memcpy(&h_f, &temp_f, sizeof(h_f));
        break;
    case '1':
        h_f.first_see = true;
        cout << "1인칭" << endl;
    case '3':
        h_f.first_see = false;
        cout << "3인칭" << endl;
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
        pilot.x_trans += 0.1;
    }
    if (h_f.right_walk) {
        if (pilot.x_trans_aoc > -2.5)
            pilot.x_trans_aoc -= 0.01f;
    }
    if (h_f.left_walk) {
        if (pilot.x_trans_aoc < 2.5)
            pilot.x_trans_aoc += 0.01f;
    }
    if (h_f.back_walk) {
        pilot.z_trans_aoc -= 0.01f;
    }
    if (h_f.front_walk) {
        pilot.z_trans_aoc += 0.01f;
    }


    if (h_f.left_turn)
        pilot.y_rotate_aoc++;





    glutPostRedisplay();
    glutTimerFunc(5, Timer, 1);
}
