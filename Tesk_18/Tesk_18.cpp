#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#pragma comment (lib, "glu32.lib")
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

// --- GLU 원뿔용 쿼드릭 핸들 ---
GLUquadric* gQuadric = nullptr;

GLuint axisVAO = 0, axisVBO = 0;
GLuint cubeVAO[6] = { 0, }, cubeVBO[6] = { 0 };   // 정육면체

// 축 데이터 (위치, 색상)
GLfloat axisVertices[] =
{
	// x축
	-0.7f, 0.0f, 0.0f,   1,0,0,
	0.7f, 0.0f, 0.0f,   1,0,0,
	// y축
	0.0f, -0.7f, 0.0f,   0,1,0,
	0.0f, 0.7f, 0.0f,   0,1,0,
	// z축
	0.0f, 0.0f, -0.7f,   0,0,1,
	0.0f, 0.0f, 0.8f,   0,0,1,
};

// 정육면체 vertex 좌표값
float cube[8][3] =
{
	{-0.25f, 0, -0.25f}, {-0.5f, 0, -0.25f}, {-0.5f, 0, 0}, {-0.25f, 0, 0},
	{-0.25f, 0.25f, -0.25f}, {-0.5f, 0.25f, -0.25f}, {-0.5f, 0.25f, 0}, {-0.25f, 0.25f, 0}
};
int faces[6][4] = {
	{0, 1, 2, 3},   // 아래
	{4, 7, 6, 5},   // 윗
	{1, 5, 6, 2},   // 안쪽옆면
	{0, 3, 7, 4},   // 밖옆면
	{0, 4, 5, 1},   // 앞
	{3, 2, 6, 7}    // 뒤
};
// 정육면체 꼭짓점별 색상
float cubeColors[8][3] = {
	{1,0,0},    // 0
	{0,1,0},    // 1
	{0,0,1},    // 2
	{1,1,0},    // 3
	{1,0,1},    // 4
	{0,1,1},    // 5
	{0.5f,0.5f,0.5f}, // 6
	{1,0.5f,0}  // 7
};

int objectMode = -1;   // -1: left, 0: 둘 다, 1: right
bool rotatingX = false;
bool dirX = true; // +1: 시계, -1: 반시계
float angleX_1 = -30.0f;
float angleX_2 = -30.0f;
bool rotatingY = false;
bool dirY = true; // +1: 시계, -1: 반시계
float angleY_1 = 30.0f;
float angleY_2 = 30.0f;

bool rotatingCenter = false;  // 중앙 y축 기준 회전
float angleC_1 = 0.0f;
float angleC_2 = 0.0f;
bool dirC = true; // +1: 시계, -1: 반시계

float curScale = 1.0f;
int dirS = +1;           // +1: 증가, -1: 감소
bool scaling = false;
bool scalingCenter = false;

float moveX_1 = 0.0f;
float moveX_2 = 0.0f;
float dirMoveX_1 = +1;
float dirMoveX_2 = -1;
bool translatingX = false;

void InitAxis()
{
	glGenVertexArrays(1, &axisVAO);
	glGenBuffers(1, &axisVBO);

	glBindVertexArray(axisVAO);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

	// 위치
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 색상
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void InitCube()
{
	for (int i = 0; i < 6; i++)   // 6면 각각
	{
		std::vector<GLfloat> vertices;
		int v0 = faces[i][0];
		int v1 = faces[i][1];
		int v2 = faces[i][2];
		int v3 = faces[i][3];

		// 삼각형 1: v0, v1, v2
		vertices.push_back(cube[v0][0]); vertices.push_back(cube[v0][1]); vertices.push_back(cube[v0][2]);
		vertices.push_back(cubeColors[v0][0]); vertices.push_back(cubeColors[v0][1]); vertices.push_back(cubeColors[v0][2]);
		vertices.push_back(cube[v1][0]); vertices.push_back(cube[v1][1]); vertices.push_back(cube[v1][2]);
		vertices.push_back(cubeColors[v1][0]); vertices.push_back(cubeColors[v1][1]); vertices.push_back(cubeColors[v1][2]);
		vertices.push_back(cube[v2][0]); vertices.push_back(cube[v2][1]); vertices.push_back(cube[v2][2]);
		vertices.push_back(cubeColors[v2][0]); vertices.push_back(cubeColors[v2][1]); vertices.push_back(cubeColors[v2][2]);

		// 삼각형 2: v0, v2, v3
		vertices.push_back(cube[v0][0]); vertices.push_back(cube[v0][1]); vertices.push_back(cube[v0][2]);
		vertices.push_back(cubeColors[v0][0]); vertices.push_back(cubeColors[v0][1]); vertices.push_back(cubeColors[v0][2]);
		vertices.push_back(cube[v2][0]); vertices.push_back(cube[v2][1]); vertices.push_back(cube[v2][2]);
		vertices.push_back(cubeColors[v2][0]); vertices.push_back(cubeColors[v2][1]); vertices.push_back(cubeColors[v2][2]);
		vertices.push_back(cube[v3][0]); vertices.push_back(cube[v3][1]); vertices.push_back(cube[v3][2]);
		vertices.push_back(cubeColors[v3][0]); vertices.push_back(cubeColors[v3][1]); vertices.push_back(cubeColors[v3][2]);

		glGenVertexArrays(1, &cubeVAO[i]);
		glGenBuffers(1, &cubeVBO[i]);
		glBindVertexArray(cubeVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		// 위치
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 색상
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
	// Open file for reading 
	// Return NULL on failure 
	// Seek to the end of the file 
	// Find out how many bytes into the file we are 
	// Allocate a buffer for the entire length of the file and a null terminator 
	// Go back to the beginning of the file 
	// Read the contents of the file in to the buffer 
	// Close the file 
	// Null terminator 
	// Return the buffer 
}

void Timer(int value)
{
	if (rotatingX)
	{
		if (objectMode == -1) // left
		{
			if (dirX) angleX_1 += 1.0f;  // 회전 속도
			else angleX_1 -= 1.0f;
		}
		else if (objectMode == 1) // right
		{
			if (dirX) angleX_2 += 1.0f;  // 회전 속도
			else angleX_2 -= 1.0f;
		}
		else // both
		{
			if (dirX)
			{
				angleX_1 += 1.0f;  // 회전 속도
				angleX_2 += 1.0f;
			}
			else
			{
				angleX_1 -= 1.0f;
				angleX_2 -= 1.0f;
			}
		}
		glutPostRedisplay();
	}
	else if (rotatingY)
	{
		if (objectMode == -1) // left
		{
			if (dirY) angleY_1 += 1.0f;  // 회전 속도
			else angleY_1 -= 1.0f;
		}
		else if (objectMode == 1) // right
		{
			if (dirY) angleY_2 += 1.0f;  // 회전 속도
			else angleY_2 -= 1.0f;
		}
		else // both
		{
			if (dirY)
			{
				angleY_1 += 1.0f;  // 회전 속도
				angleY_2 += 1.0f;
			}
			else
			{
				angleY_1 -= 1.0f;
				angleY_2 -= 1.0f;
			}
		}
		glutPostRedisplay();
	}
	if (rotatingCenter)
	{
		if (objectMode == -1) // left
		{
			if (dirC) angleC_1 += 1.0f;  // 회전 속도
			else angleC_1 -= 1.0f;
		}
		else if (objectMode == 1) // right
		{
			if (dirC) angleC_2 += 1.0f;  // 회전 속도
			else angleC_2 -= 1.0f;
		}
		else // both
		{
			if (dirC)
			{
				angleC_1 += 1.0f;  // 회전 속도
				angleC_2 += 1.0f;
			}
			else
			{
				angleC_1 -= 1.0f;
				angleC_2 -= 1.0f;
			}
		}
		glutPostRedisplay();
	}
	if (scaling)
	{
		curScale += dirS * 0.01f;

		if (curScale >= 1.5f)
		{
			curScale = 1.5f;
			dirS = -1;
		}
		else if (curScale <= 0.2f)
		{
			curScale = 0.2f;
			dirS = +1;
		}
		glutPostRedisplay();
	}
	if (scalingCenter)
	{
		curScale += dirS * 0.01f;
		if (curScale >= 1.5f)
		{
			curScale = 1.5f;
			dirS = -1;
		}
		else if (curScale <= 0.2f)
		{
			curScale = 0.2f;
			dirS = +1;
		}
		glutPostRedisplay();
	}
	if (translatingX && (objectMode == -1 || objectMode == 0))
	{
		moveX_1 += dirMoveX_1 * 0.01f;

		if (moveX_1 >= 0.8f)
		{
			moveX_1 = 0.8f;
			dirMoveX_1 = -1;
		}
		else if (moveX_1 <= -0.8f)
		{
			moveX_1 = -0.8f;
			dirMoveX_1 = +1;
		}
		glutPostRedisplay();
	}
	if (translatingX && (objectMode == 1 || objectMode == 0))
	{
		moveX_2 += dirMoveX_2 * 0.01f;
		if (moveX_2 >= 0.8f)
		{
			moveX_2 = 0.8f;
			dirMoveX_2 = -1;
		}
		else if (moveX_2 <= -0.8f)
		{
			moveX_2 = -0.8f;
			dirMoveX_2 = +1;
		}
		glutPostRedisplay();
	}
	glutTimerFunc(16, Timer, 0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		objectMode = -1; // left
		glutPostRedisplay();
		break;
	case '3':
		objectMode = 0; // both
		glutPostRedisplay();
		break;
	case '2':
		objectMode = 1; // right
		glutPostRedisplay();
		break;
	case 'x':
		rotatingX = !rotatingX;
		dirX = true;
		rotatingY = false;
		break;
	case 'X':
		rotatingX = !rotatingX;
		dirX = false;
		rotatingY = false;
		break;
	case 'y':
		rotatingY = !rotatingY;
		dirY = true;
		rotatingX = false;
		break;
	case 'Y':
		rotatingY = !rotatingY;
		dirY = false;
		rotatingX = false;
		break;
	case 'r':
		rotatingCenter = !rotatingCenter;
		dirC = true;
		break;
	case 'R':
		rotatingCenter = !rotatingCenter;
		dirC = false;
		break;
	case 'a':
		scaling = !scaling;
		scalingCenter = false;
		break;
	case 'b':
		scalingCenter = !scalingCenter;
		scaling = false;
		break;
	case 'd':
		translatingX = !translatingX;
		break;
	case 'q':
		exit(0);
		break;
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // 깊이 버퍼 추가
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Tesk_18");

	glewExperimental = GL_TRUE;
	glewInit();

	gQuadric = gluNewQuadric();  // GLU 쿼드릭 객체 생성

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	InitAxis();   // 축 초기화
	InitCube();   // 정육면체 초기화

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화

	glutTimerFunc(0, Timer, 0); // 타이머 콜백 등록
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "Error: vertex shader            \n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader            \n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program          \n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

// 축 그리는 함수
GLvoid DrawAxis(GLuint shaderProgramID)
{
	GLuint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");

	glm::mat4 axisModel = glm::mat4(1.0f);
	axisModel = glm::rotate(axisModel, glm::radians(-30.0f), glm::vec3(1, 0, 0));
	axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0, 1, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &axisModel[0][0]);

	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
}

// 정육면체 그리는 함수
GLvoid DrawCube(GLuint shaderProgramID)
{
	GLuint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");

	glm::vec3 center(-0.375f, 0.125f, -0.125f);

	// 모델 변환 적용 (30도씩 회전)
	glm::mat4 model = glm::mat4(1.0f);

	if (rotatingCenter)
	{
		model = glm::rotate(model, glm::radians(angleC_1), glm::vec3(0, 1, 0));
	}

	model = glm::translate(model, center); // 중심을 원점으로 이동

	// 회전 행렬 생성
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angleY_1), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(angleX_1), glm::vec3(1, 0, 0));
	glm::vec3 movedir = glm::vec3(rot * glm::vec4(1, 0, 0, 0));

	// 회전 적용
	model = rot;

	// 이동 적용
	model = glm::translate(model, movedir * moveX_1);

	model = glm::translate(model, -center); // 다시 원래 위치로 이동

	if (objectMode == 0 || objectMode == -1) // both
	{
		if (!scalingCenter)
		{
			model = glm::translate(model, center); // 위치
			model = glm::scale(model, glm::vec3(curScale, curScale, curScale)); // 스케일링
			model = glm::translate(model, -center); // 다시 원래 위치로 이동
		}
		else
			{
			model = glm::scale(model, glm::vec3(curScale, curScale, curScale)); // 스케일링
		}
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
	// 각 면별로 그리기
	for (int i = 0; i < 6; ++i)
	{
		glm::mat4 M = model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(cubeVAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

GLvoid DrawCone(GLuint shaderProgramID)
{
	if (!gQuadric) return;  // 안전 장치

	// --- 모델 행렬 구성 (위치 + 회전) ---
	glm::mat4 M(1.0f);

	if (rotatingCenter)
	{
		M = glm::rotate(M, glm::radians(angleC_2), glm::vec3(0, 1, 0));
		// M = glm::translate(M, glm::vec3(0.35f, 0.0f, 0.0f));
	}
	if (scalingCenter && (objectMode == 0 || objectMode == 1))
	{
		M = glm::scale(M, glm::vec3(curScale, curScale, curScale)); // 스케일링
		// M = glm::translate(M, glm::vec3(0.35f, 0.0f, 0.0f));
	}
	
	M = glm::translate(M, glm::vec3(0.35f, 0.0f, 0.0f)); // 위치
	M = glm::rotate(M, glm::radians(angleY_2), glm::vec3(0, 1, 0));
	M = glm::rotate(M, glm::radians(angleX_2), glm::vec3(1, 0, 0)); // x축 회전
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angleY_2), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(angleX_2), glm::vec3(1, 0, 0));

	glm::vec3 movedir = glm::vec3(rot * glm::vec4(1, 0, 0, 0));
	M = glm::translate(M, movedir * moveX_2);

	if (scaling && (objectMode == 0 || objectMode == 1)) // both
		M = glm::scale(M, glm::vec3(curScale, curScale, curScale)); // 스케일링

	// --- 셰이더에 uModel 전달 ---
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);

	// --- 원뿔 그리기 ---
	// baseRadius=0.18, topRadius=0 → 원뿔
	gluCylinder(gQuadric,
		(GLdouble)0.1,  // 밑면 반지름
		0.0,             // 윗면 반지름 (0이면 원뿔)
		(GLdouble)0.3,  // 높이
		(GLint)20,       // 둘레 분할
		(GLint)8);       // 세로 분할

	// --- 바닥 막기 (옵션) ---
	//gluDisk(gQuadric, 0.0, (GLdouble)0.18, (GLint)48, 1);
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 이제 깊이 버퍼도 초기화

	glUseProgram(shaderProgramID);

	DrawAxis(shaderProgramID); // 축 그리기	
	DrawCube(shaderProgramID); // 정육면체 그리기

	DrawCone(shaderProgramID);   // 원뿔 그리기

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}