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

float curScale_1 = 1.0f;
float curScale_2 = 1.0f;
int dirS_1 = +1;           // +1: 증가, -1: 감소
int dirS_2 = -1;           // +1: 증가, -1: 감소
bool scaling = false;
bool scalingCenter = false;

float moveX_1 = 0.0f;
float moveX_2 = 0.0f;
float dirMoveX_1 = +1;
float dirMoveX_2 = -1;
bool translatingX = false;

float moveY_1 = 0.0f;
float moveY_2 = 0.0f;
float dirMoveY_1 = +1;
float dirMoveY_2 = -1;
bool translatingY = false;

bool posChange = false;
glm::vec3 cubeCenter = { -0.375f, 0.125f, -0.125f };
glm::vec3 coneCenter = { 0.35f, 0.0f, 0.0f };

glm::vec3 cubeTarget = cubeCenter;
glm::vec3 coneTarget = coneCenter;

bool animationV1 = false;   // 사각형이 작아지며 자전, 원뿔이 커지며 공전
bool animationV2 = false;   // 사각형이 커지며 공전, 원뿔이 작아지며 자전

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
}

void Timer(int value)
{
	if (animationV1)
	{
		angleY_1 += 1.0f; angleC_2 += 1.0f;

		curScale_1 += dirS_1 * 0.01f;
		if (curScale_1 >= 1.5f) { curScale_1 = 1.5f; dirS_1 = -1; }
		else if (curScale_1 <= 0.2f) { curScale_1 = 0.2f; dirS_1 = +1; }

		curScale_2 += dirS_2 * 0.01f;
		if (curScale_2 >= 1.5f) { curScale_2 = 1.5f; dirS_2 = -1; }
		else if (curScale_2 <= 0.2f) { curScale_2 = 0.2f; dirS_2 = +1; }
		glutPostRedisplay();
	}
	
	if (animationV2)
	{
		angleY_2 += 1.0f; angleC_1 += 1.0f;

		curScale_1 += dirS_1 * 0.01f;
		if (curScale_1 >= 1.5f) { curScale_1 = 1.5f; dirS_1 = -1; }
		else if (curScale_1 <= 0.2f){ curScale_1 = 0.2f; dirS_1 = +1; }

		curScale_2 += dirS_2 * 0.01f;
		if (curScale_2 >= 1.5f) { curScale_2 = 1.5f; dirS_2 = -1; }
		else if (curScale_2 <= 0.2f) { curScale_2 = 0.2f; dirS_2 = +1;}
		glutPostRedisplay();
	}

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
				angleX_1 += 1.0f; angleX_2 += 1.0f;
			}
			else
			{
				angleX_1 -= 1.0f; angleX_2 -= 1.0f;
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
				angleY_1 += 1.0f; angleY_2 += 1.0f;
			}
			else
			{
				angleY_1 -= 1.0f; angleY_2 -= 1.0f;
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
				angleC_1 += 1.0f;  angleC_2 += 1.0f;
			}
			else
			{
				angleC_1 -= 1.0f; angleC_2 -= 1.0f;
			}
		}
		glutPostRedisplay();
	}
	if (scaling)
	{
		if (objectMode == -1 || objectMode == 0)
		{
			curScale_1 += dirS_1 * 0.01f;
			if (curScale_1 >= 1.5f)
			{
				curScale_1 = 1.5f; dirS_1 = -1;
			}
			else if (curScale_1 <= 0.2f)
			{
				curScale_1 = 0.2f; dirS_1 = +1;
			}
		}
		if (objectMode == 1 || objectMode == 0)
		{
			curScale_2 += dirS_2 * 0.01f;
			if (curScale_2 >= 1.5f)
			{
				curScale_2 = 1.5f; dirS_2 = -1;
			}
			else if (curScale_2 <= 0.2f)
			{
				curScale_2 = 0.2f; dirS_2 = +1;
			}
		}
		glutPostRedisplay();
	}
	if (scalingCenter)
	{
		if (objectMode == -1 || objectMode == 0)
		{
			curScale_1 += dirS_1 * 0.01f;
			if (curScale_1 >= 1.5f)
			{
				curScale_1 = 1.5f; dirS_1 = -1;
			}
			else if (curScale_1 <= 0.2f)
			{
				curScale_1 = 0.2f; dirS_1 = +1;
			}
			glutPostRedisplay();
		}
		if (objectMode == 1 || objectMode == 0)
		{
			curScale_2 += dirS_2 * 0.01f;
			if (curScale_2 >= 1.5f)
			{
				curScale_2 = 1.5f; dirS_2 = -1;
			}
			else if (curScale_2 <= 0.2f)
			{
				curScale_2 = 0.2f; dirS_2 = +1;
			}
			glutPostRedisplay();
		}
		glutPostRedisplay();
	}
	if (translatingX && (objectMode == -1 || objectMode == 0))
	{
		moveX_1 += dirMoveX_1 * 0.01f;

		if (moveX_1 >= 1.3f)
		{
			moveX_1 = 1.3f; dirMoveX_1 = -1;
		}
		else if (moveX_1 <= -0.7f)
		{
			moveX_1 = -0.7f; dirMoveX_1 = +1;
		}
		glutPostRedisplay();
	}
	if (translatingX && (objectMode == 1 || objectMode == 0))
	{
		moveX_2 += dirMoveX_2 * 0.01f;
		if (moveX_2 >= 0.7f)
		{
			moveX_2 = 0.7f; dirMoveX_2 = -1;
		}
		else if (moveX_2 <= -1.0f)
		{
			moveX_2 = -1.0f; dirMoveX_2 = +1;
		}
		glutPostRedisplay();
	}

	if (translatingY && (objectMode == -1 || objectMode == 0))
	{
		moveY_1 += dirMoveY_1 * 0.01f;
		if (moveY_1 >= 0.7f)
		{
			moveY_1 = 0.7f; dirMoveY_1 = -1;
		}
		else if (moveY_1 <= -0.7f)
		{
			moveY_1 = -0.7f; dirMoveY_1 = +1;
		}
		glutPostRedisplay();
	}
	if (translatingY && (objectMode == 1 || objectMode == 0))
	{
		moveY_2 += dirMoveY_2 * 0.01f;
		if (moveY_2 >= 0.7f)
		{
			moveY_2 = 0.7f; dirMoveY_2 = -1;
		}
		else if (moveY_2 <= -0.7f)
		{
			moveY_2 = -0.7f; dirMoveY_2 = +1;
		}
		glutPostRedisplay();
	}
	if (posChange)
	{
		float lerpSpeed = 0.08f;

		glm::vec3 cubeWorld = cubeCenter + glm::vec3(moveX_1, moveY_1, 0.0f);
		cubeWorld = glm::mix(cubeWorld, cubeTarget, lerpSpeed);

		glm::vec3 coneWorld = coneCenter + glm::vec3(moveX_2, moveY_2, 0.0f);
		coneWorld = glm::mix(coneWorld, coneTarget, lerpSpeed);

		moveX_1 = cubeWorld.x - cubeCenter.x;
		moveY_1 = cubeWorld.y - cubeCenter.y;

		moveX_2 = coneWorld.x - coneCenter.x;
		moveY_2 = coneWorld.y - coneCenter.y;
		
		// 가까워지면 타겟 변경
		if (fabs(cubeWorld.x - cubeTarget.x) < 0.0005f &&
			fabs(cubeWorld.y - cubeTarget.y) < 0.0005f &&
			fabs(coneWorld.x - coneTarget.x) < 0.0005f &&
			fabs(coneWorld.y - coneTarget.y) < 0.0005f)
		{
			cubeWorld = cubeTarget;
			coneWorld = coneTarget;

			// 타겟 스왑
			glm::vec3 temp = cubeTarget;
			cubeTarget = coneTarget;
			coneTarget = temp;
		}

		glutPostRedisplay();
	}
	glutTimerFunc(16, Timer, 0);
}

void Reset()
{
	 objectMode = -1;   // -1: left, 0: 둘 다, 1: right
	 rotatingX = false; dirX = true;
	 angleX_1 = -30.0f; angleX_2 = -30.0f;
	 rotatingY = false; dirY = true; 
	 angleY_1 = 30.0f; angleY_2 = 30.0f;

	 rotatingCenter = false;  // 중앙 y축 기준 회전
	 angleC_1 = 0.0f; angleC_2 = 0.0f;
	 dirC = true; 

	 curScale_1 = 1.0f; curScale_2 = 1.0f;
	 dirS_1 = +1; dirS_2 = -1;        
	 scaling = false;
	 scalingCenter = false;

	 moveX_1 = 0.0f; moveX_2 = 0.0f;
	 dirMoveX_1 = +1; dirMoveX_2 = -1;
	 translatingX = false;

	moveY_1 = 0.0f; moveY_2 = 0.0f;
	dirMoveY_1 = +1; dirMoveY_2 = -1;
	translatingY = false;

	posChange = false;
	cubeCenter = { -0.375f, 0.125f, -0.125f }; coneCenter = { 0.35f, 0.0f, 0.0f };
	cubeTarget = cubeCenter; coneTarget = coneCenter;

	animationV1 = false; animationV2 = false;   

	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		objectMode = -1; glutPostRedisplay(); break;
	case '3':
		objectMode = 0; glutPostRedisplay(); break;
	case '2':
		objectMode = 1; glutPostRedisplay(); break;
	case 'x':
		rotatingX = !rotatingX; dirX = true; rotatingY = false; break;
	case 'X':
		rotatingX = !rotatingX; dirX = false; rotatingY = false; break;
	case 'y':
		rotatingY = !rotatingY; dirY = true; rotatingX = false; break;
	case 'Y':
		rotatingY = !rotatingY; dirY = false; rotatingX = false; break;
	case 'r':
		rotatingCenter = !rotatingCenter; dirC = true; break;
	case 'R':
		rotatingCenter = !rotatingCenter; dirC = false; break;
	case 'a':
		scaling = !scaling; scalingCenter = false; break;
	case 'b':
		scalingCenter = !scalingCenter; scaling = false; break;
	case 'd':
		translatingX = !translatingX; break;
	case 'e':
		translatingY = !translatingY; break;
	case 't':
		posChange = !posChange;
		glm::vec3 temp = cubeTarget;
		cubeTarget = coneTarget;
		coneTarget = temp;
		break;
	case 'v':
		animationV1 = !animationV1; dirS_1 = +1; dirS_2 = -1; animationV2 = false;
		break;
	case 'V':
		animationV2 = !animationV2; dirS_1 = -1; dirS_2 = +1; animationV1 = false;
		break;
	case 's': Reset(); break;
	case 'q': exit(0); break;
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

	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 center = cubeCenter;

	if (rotatingCenter || animationV2)
	{
		model = glm::rotate(model, glm::radians(angleC_1), glm::vec3(0, 1, 0));
	}

	glm::mat4 viewRot = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));
	viewRot = glm::rotate(viewRot, glm::radians(-30.0f), glm::vec3(1, 0, 0));

	glm::mat3 camRot = glm::mat3(glm::inverse(viewRot));  // 카메라 회전 행렬의 역행렬
	glm::vec3 screenX = glm::normalize(camRot[0]);        // 카메라 기준 x축을 월드 좌표계로 변환 
	glm::vec3 screenY = glm::normalize(camRot[1]);        // 카메라 기준 y축을 월드 좌표계로 변환

	model = glm::translate(model, screenX * moveX_1 + screenY * moveY_1);  // 그려진 좌표축 기준 움직이기 위해

	glm::mat4 R = glm::mat4(1.0f);
	R = glm::rotate(R, glm::radians(angleY_1), glm::vec3(0, 1, 0));
	R = glm::rotate(R, glm::radians(angleX_1), glm::vec3(1, 0, 0)); 

	model = glm::translate(model, center);
	model = model * R;
	model = glm::translate(model, -center); 

	if (!scalingCenter)
	{
		model = glm::translate(model, center);
		model = glm::scale(model, glm::vec3(curScale_1, curScale_1, curScale_1));
		model = glm::translate(model, -center); 
	}
	else
		model = glm::scale(model, glm::vec3(curScale_1, curScale_1, curScale_1)); 
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

	for (int i = 0; i < 6; ++i)
	{
		glm::mat4 M = model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(cubeVAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

// 원뿔 그리는 함수
GLvoid DrawCone(GLuint shaderProgramID)
{
	if (!gQuadric) return; 

	glm::mat4 M(1.0f);

	if (rotatingCenter || animationV1)
		M = glm::rotate(M, glm::radians(angleC_2), glm::vec3(0, 1, 0));
	
	if (scalingCenter && (objectMode == 0 || objectMode == 1))
		M = glm::scale(M, glm::vec3(curScale_2, curScale_2, curScale_2)); 
	

	glm::mat4 viewRot = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));
	viewRot = glm::rotate(viewRot, glm::radians(-30.0f), glm::vec3(1, 0, 0));

	glm::mat3 camRot = glm::mat3(glm::inverse(viewRot)); 
	glm::vec3 screenX = glm::normalize(camRot[0]);       
	glm::vec3 screenY = glm::normalize(camRot[1]);      

	M = glm::translate(M, screenX * moveX_2 + screenY * moveY_2);

	M = glm::translate(M, coneCenter);
	M = glm::rotate(M, glm::radians(angleY_2), glm::vec3(0, 1, 0));
	M = glm::rotate(M, glm::radians(angleX_2), glm::vec3(1, 0, 0));

	M = glm::scale(M, glm::vec3(curScale_2, curScale_2, curScale_2));

	GLint modelLoc = glGetUniformLocation(shaderProgramID, "uModel");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &M[0][0]);

	gluCylinder(gQuadric,
		(GLdouble)0.1,   // 밑면 반지름
		0.0,             // 윗면 반지름
		(GLdouble)0.3,   // 높이
		(GLint)20,       // 둘레 분할
		(GLint)8);       // 세로 분할
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