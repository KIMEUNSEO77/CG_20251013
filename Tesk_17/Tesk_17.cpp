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

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint axisVAO = 0, axisVBO = 0;   // 축들
GLuint cubeVAO[6] = { 0, }, cubeVBO[6] = { 0 };   // 정육면체
GLuint pyramidVAO[6] = { 0, }, pyramidVBO[6] = { 0 };   // 삼각뿔

bool cubeMode = true;         // 정육면체 모드, fasle사각뿔
bool cullMode = false;         // 면 제거 모드

float angleX = -30, angleY = 30; // 회전 각도
bool rotatingX = false;
bool rotatingY = false;
bool directionX = true; // 회전 방향 (true: 시계, false: 반시계)
bool directionY = true;

bool backfaceCulling = true;
bool frontAnimation = false;   // 육면체 앞면 열렸다가 닫혔다가
float frontAngle = 0.0f; // 육면체 앞면 회전 각도
int dir = +1;           // +1: 증가, -1: 감소
float speed = 1.0f; // 회전 속도

// 윗면 z축 기준 회전
bool topAnimation = false;    // 육면체 z축 회전
float topAngle = 0.0f; // 육면체 윗면 회전 각도

// 옆면 x축 기준 회전
bool sideAnimation = false;    // 육면체 x축 회전
float sideAngle = 0.0f; // 육면체 옆면 회전 각도

// 뒷면 크기 변화
bool backAnimation = false;    // 육면체 뒷면 크기 변화
float backScale = 1.0f; // 육면체 뒷면 크기
int backDir = +1;           // +1: 증가, -1: 감소

// 사각뿔 애니메이션
bool pyramidAnimation = false;
float pyramidAngle = 0.0f; // 삼각뿔 회전 각도
float dirP = +1;           // +1: 증가, -1: 감소

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
	{0.25f, 0, -0.25f}, {0, 0, -0.25f}, {0, 0, 0}, {0.25f, 0, 0},
	{0.25f, 0.25f, -0.25f}, {0, 0.25f, -0.25f}, {0, 0.25f, 0}, {0.25f, 0.25f, 0}
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

// 삼각뿔 vertex 좌표값
float pyramid[5][3] =
{
	{0, 0.3f, 0}, {0.15f, 0, -0.15f}, {-0.15f, 0, -0.15f}, {-0.15f, 0, 0.15f},
	{0.15f, 0, 0.15f}
};
int pyramidFaces[6][3] = {
	{1, 2, 3}, {1, 3, 4}, {0, 3, 2}, {0, 2, 1}, {0, 4, 3}, {0, 1, 4}  // 0, 1은 한 면
};
// 삼각뿔 꼭짓점별 색상
float pyramidColors[5][3] = {
	{1,0,0},    // 0
	{0,1,0},    // 1
	{0,0,1},    // 2
	{1,1,0},    // 3
	{1,0,1}     // 4
};

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

void InitPyramid()
{
	for (int i = 0; i < 6; i++)   // 6면 각각
	{
		std::vector<GLfloat> vertices;
		int v0 = pyramidFaces[i][0];
		int v1 = pyramidFaces[i][1];
		int v2 = pyramidFaces[i][2];

		// 삼각형 1: v0, v1, v2
		vertices.push_back(pyramid[v0][0]); vertices.push_back(pyramid[v0][1]); vertices.push_back(pyramid[v0][2]);
		vertices.push_back(pyramidColors[v0][0]); vertices.push_back(pyramidColors[v0][1]); vertices.push_back(pyramidColors[v0][2]);
		vertices.push_back(pyramid[v1][0]); vertices.push_back(pyramid[v1][1]); vertices.push_back(pyramid[v1][2]);
		vertices.push_back(pyramidColors[v1][0]); vertices.push_back(pyramidColors[v1][1]); vertices.push_back(pyramidColors[v1][2]);
		vertices.push_back(pyramid[v2][0]); vertices.push_back(pyramid[v2][1]); vertices.push_back(pyramid[v2][2]);
		vertices.push_back(pyramidColors[v2][0]); vertices.push_back(pyramidColors[v2][1]); vertices.push_back(pyramidColors[v2][2]);

		glGenVertexArrays(1, &pyramidVAO[i]);
		glGenBuffers(1, &pyramidVBO[i]);
		glBindVertexArray(pyramidVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO[i]);
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
	if (rotatingY)
	{
		if (directionY) angleY += 1.0f;  // 회전 속도
		else angleY -= 1.0f;
		glutPostRedisplay();
	}
	if (frontAnimation)
	{
		frontAngle += dir * speed;

		if (frontAngle >= 90.0f) 
		{
			frontAngle = 90.0f;   // ★ 경계에서 고정
			dir = -1;             // ★ 방향 반전
		}
		else if (frontAngle <= 0.0f) 
		{
			frontAngle = 0.0f;    // ★ 경계에서 고정
			dir = +1;             // ★ 방향 반전
		}
		glutPostRedisplay();
	}
	if (topAnimation)
	{
		topAngle += 2.0f;
		glutPostRedisplay();
	}
	if (sideAnimation)
	{
		sideAngle += 2.0f;
		glutPostRedisplay();
	}
	if (backAnimation)
	{
		backScale += backDir * 0.01f;
		if (backScale >= 1.0f)
		{
			backScale = 1.0;
			backDir = -1;
		}
		else if (backScale <= 0.2f)
		{
			backScale = 0.2f;
			backDir = +1;
		}
		glutPostRedisplay();
	}
	if (pyramidAnimation)
	{
		pyramidAngle += 2.0f * dirP;
		if (pyramidAngle >= 120.0f)
		{
			pyramidAngle = 120.0f;
			dirP = -1;
		}
		else if (pyramidAngle <= 0.0f)
		{
			pyramidAngle = 0.0f;
			dirP = 1;
		}
		glutPostRedisplay();
	}
	glutTimerFunc(16, Timer, 0);
}

void Reset()
{
	angleX = -30; angleY = 30;
	rotatingX = false; rotatingY = false;
	directionX = true; directionY = true;
	cubeMode = true;
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		cubeMode = !cubeMode;
		glutPostRedisplay();
		break;
	case 'h':
		cullMode = !cullMode;
		if (cullMode) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		glutPostRedisplay();
		break;
		// 뒷면제거 적용, 해제
		/*
	case 'u':
		backfaceCulling = !backfaceCulling;
		if (backfaceCulling)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		glutPostRedisplay();
		break;
		*/
	case 'y':
		rotatingY = !rotatingY;
		directionY = true;
		break;
	case 'c':
		Reset();
		break;
	case 'f':
		if (cubeMode) frontAnimation = !frontAnimation;
		break;
	case 't':
		if (cubeMode) topAnimation = !topAnimation;
		break;
	case 's':
		if (cubeMode) sideAnimation = !sideAnimation;
		break;
	case 'b':
		if (cubeMode) backAnimation = !backAnimation;
		break;
	case 'o':
		if (!cubeMode) pyramidAnimation = !pyramidAnimation;
		break;
	case 'r':
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
	glutCreateWindow("Tesk_17");

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	InitAxis();   // 축 초기화
	InitCube();   // 정육면체 초기화
	InitPyramid();   // 삼각뿔 초기화

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	//glEnable(GL_CULL_FACE); // 면 제거
	//glDepthFunc(GL_ALWAYS);   // 항상 통과

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

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 이제 깊이 버퍼도 초기화

	glUseProgram(shaderProgramID);

	GLuint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");

	// 2. 좌표축 그릴 때: 단위행렬(변환 없음) 적용
	glm::mat4 axisModel = glm::mat4(1.0f);
	axisModel = glm::rotate(axisModel, glm::radians(-30.0f), glm::vec3(1, 0, 0));
	axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0, 1, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &axisModel[0][0]);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);

	// 모델 변환 적용 (30도씩 회전)
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(moveX, moveY, 0.0f));  // 이동

	model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

	int front = 4;
	int top = 1;
	if (cubeMode)
	{
		// 각 면별로 그리기
		for (int i = 0; i < 6; ++i)
		{
			glm::mat4 M = model;

			if (i == front) 
			{
				// 회전축 설정
				glm::vec3 p0 = { cube[4][0], cube[4][1], cube[4][2] };   // v4
				glm::vec3 p1 = { cube[5][0], cube[5][1], cube[5][2] };   // v7
				glm::vec3 axis = glm::normalize(p0-p1);               // 모서리 방향 (여긴 (0,0,1))
				glm::vec3 pivot = p0;                                     // v4 기준 (중점 쓰려면 (p0+p1)*0.5f)

				// 모델 공간 피벗 기준 회전: base * T(pivot) * R(axis, θ) * T(−pivot)
				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(frontAngle), axis);
				M = glm::translate(M, -pivot);
			}

			if (i == top)
			{
				glm::vec3 pivot = { (cube[4][0] + cube[6][0]) * 0.5f, cube[4][1], (cube[4][2] + cube[6][2]) * 0.5f }; // v4, v7 중점
				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(topAngle), glm::vec3(0, 0, 1));
				M = glm::translate(M, -pivot);
			}
			if (i == 3)
			{
				// 회전축 설정
				glm::vec3 p0 = { cube[3][0], cube[3][1], cube[3][2] };   // v4
				glm::vec3 p1 = { cube[4][0], cube[4][1], cube[4][2] };   // v7
				glm::vec3 axis = glm::normalize(p0 - p1);               // 모서리 방향 (여긴 (0,0,1))
				glm::vec3 pivot = (p0 + p1) * 0.5f;                                     // v4 기준 (중점 쓰려면 (p0+p1)*0.5f)

				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(sideAngle), glm::vec3(1, 0, 0));
				M = glm::translate(M, -pivot);
			}

			if (i == 2)
			{
				// 회전축 설정
				glm::vec3 p0 = { cube[2][0], cube[2][1], cube[2][2] };   // v4
				glm::vec3 p1 = { cube[5][0], cube[5][1], cube[5][2] };   // v7
				glm::vec3 axis = glm::normalize(p0 - p1);               // 모서리 방향 (여긴 (0,0,1))
				glm::vec3 pivot = (p0 + p1) * 0.5f;                                     // v4 기준 (중점 쓰려면 (p0+p1)*0.5f)

				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(sideAngle), glm::vec3(1, 0, 0));
				M = glm::translate(M, -pivot);
			}
			if (i == 5)
			{
				// 뒷면의 네 꼭짓점 in object space
				glm::vec3 p3 = { cube[3][0], cube[3][1], cube[3][2] };
				glm::vec3 p2 = { cube[2][0], cube[2][1], cube[2][2] };
				glm::vec3 p6 = { cube[6][0], cube[6][1], cube[6][2] };
				glm::vec3 p7 = { cube[7][0], cube[7][1], cube[7][2] };

				// 면 중심(피벗)
				glm::vec3 pivot = (p3 + p2 + p6 + p7) * 0.25f;

				// 피벗 기준으로 스케일
				M = glm::translate(M, pivot);
				M = glm::scale(M, glm::vec3(backScale, backScale, backScale));
				M = glm::translate(M, -pivot);
			}
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);

			glBindVertexArray(cubeVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	else if (!cubeMode)
	{
		// 삼각뿔 각 면 그리기
		for (int i = 0; i < 6; ++i)
		{
			glm::mat4 M = model;

			if (i == 2)
			{
				// 회전축 설정
				glm::vec3 A = { pyramid[3][0], pyramid[3][1], pyramid[3][2] }; // 밑변 한쪽
				glm::vec3 B = { pyramid[2][0], pyramid[2][1], pyramid[2][2] }; // 밑변 다른쪽

				glm::vec3 axis = glm::normalize(A - B);  // 경첩 방향(3→2)
				glm::vec3 pivot = A;                      // A를 피벗으로 (중점 쓰려면 (A+B)*0.5f)

				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(pyramidAngle), axis); // pyramidAngle은 네가 타이머에서 ±로 제어
				M = glm::translate(M, -pivot);
			}
			if (i == 3)
			{
				// 회전축 설정
				glm::vec3 A = { pyramid[2][0], pyramid[2][1], pyramid[2][2] }; // 밑변 한쪽
				glm::vec3 B = { pyramid[1][0], pyramid[1][1], pyramid[1][2] }; // 밑변 다른쪽
				glm::vec3 axis = glm::normalize(A - B);  // 경첩 방향(4→1)
				glm::vec3 pivot = A;                      // A를 피벗으로 (중점 쓰려면 (A+B)*0.5f)
				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(pyramidAngle), axis); // pyramidAngle은 네가 타이머에서 ±로 제어
				M = glm::translate(M, -pivot);
			}
			if (i == 4)
			{
				// 회전축 설정
				glm::vec3 A = { pyramid[4][0], pyramid[4][1], pyramid[4][2] }; // 밑변 한쪽
				glm::vec3 B = { pyramid[3][0], pyramid[3][1], pyramid[3][2] }; // 밑변 다른쪽
				glm::vec3 axis = glm::normalize(A - B);  // 경첩 방향(1→4)
				glm::vec3 pivot = A;                      // A를 피벗으로 (중점 쓰려면 (A+B)*0.5f)
				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(pyramidAngle), axis); // pyramidAngle은 네가 타이머에서 ±로 제어
				M = glm::translate(M, -pivot);
			}
			if (i == 5)
			{
				// 회전축 설정
				glm::vec3 A = { pyramid[1][0], pyramid[1][1], pyramid[1][2] }; // 밑변 한쪽
				glm::vec3 B = { pyramid[4][0], pyramid[4][1], pyramid[4][2] }; // 밑변 다른쪽
				glm::vec3 axis = glm::normalize(A - B);  // 경첩 방향(2→3)
				glm::vec3 pivot = A;                      // A를 피벗으로 (중점 쓰려면 (A+B)*0.5f)
				M = glm::translate(M, pivot);
				M = glm::rotate(M, glm::radians(pyramidAngle), axis); // pyramidAngle은 네가 타이머에서 ±로 제어
				M = glm::translate(M, -pivot);
			}
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);

			glBindVertexArray(pyramidVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3); // 각 면은 삼각형 1개(3정점)
			glBindVertexArray(0);
		}
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}