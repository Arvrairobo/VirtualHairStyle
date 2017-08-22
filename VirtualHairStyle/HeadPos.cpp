#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <opencv2\opencv.hpp>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <glut.h>

#include "HeadPos.h"
#include "PoseEstimation.h"
#include "Obj3DModel.h"
#include "Shader.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\transform.hpp>

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

int __w=250,__h=250;
double scale = 1.0f;

void key(unsigned char key, int x, int y)
{

    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': 
		exit(0);
		break;
	case 'w':
	case 'W':
		__w++;
		__w = __w%251;
		break;
	case 'h':
	case 'H':
		__h++;
		__h = __h%250;
		break;
    default:
		printf("pressed %c\n", key);
        break;
    }

    glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}

void myGLinit()
{
//    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int width, height;

static void update_background_texture()
{
	if (background_image.data != NULL) {
		cv::Mat tmp = cv::Mat(cv::Size(1024, 512), CV_8UC3);
		cv::resize(background_image, tmp, cv::Size(1024, 512));
		cv::Mat ttmp = tmp(cv::Range(0, tmp.rows), cv::Range(0, tmp.cols));

		if (tmp.step == tmp.cols) {
			cvtColor(tmp, ttmp, CV_GRAY2RGB);
		} else if (tmp.step == tmp.cols * 3) {
			cvtColor(tmp, ttmp, CV_BGR2RGB);
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, background->get_texture_id());
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp.data);
	} else {
		printf("background_image is NULL\n");
	}
}

static glm::mat4 calc_mvp_mat()
{
	double rot[9] = { 0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			rot[i * 3 + j] = rmat.at<double>(i, j);
		}
	}
	double m[16] = {
		-rot[0], -rot[3], -rot[6], 0.0f,
		-rot[1], -rot[4], -rot[7], 0.0f,
		-rot[2], -rot[5], -rot[8], 0.0f,
		-tv[0], -tv[1], -tv[2], 1.0f
	};

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::make_mat4(m);
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	glm::mat4 mvp = projection * view * model;

	return mvp;
}

static void background_draw()
{
	background_program.use();
	update_background_texture();
	background->bind_vao();
	background_program.draw_vertices(0, background->get_vbo(VBO_VERTICES), 3, GL_QUADS, 0, 4);
	background_program.enable_attr(background_program.get_attrib_location("texcoord"));
	background_program.bind_array_buffer(background_program.get_attrib_location("texcoord"), background->get_vbo(VBO_TEXCOORDS), 2);
}

void display()
{
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	background_draw();

	glm::mat4 mvp = calc_mvp_mat();
	GLuint matrix_id;	
	
	// Render head
	glDrawBuffer(GL_NONE);
	head_program.use();
	head->bind_vao();
	matrix_id = glGetUniformLocation(head_program.get_program_id(), "mvp");
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
	head_program.draw_vertices(0, head->get_vbo(VBO_VERTICES), 3, GL_TRIANGLES, 0, head->get_vsize());
	glDrawBuffer(GL_FRONT_AND_BACK);
	
	// Render hair
	hair_program.use();
	glBindTexture(GL_TEXTURE_2D, hair->get_texture_id());
	hair->bind_vao();
	matrix_id = glGetUniformLocation(hair_program.get_program_id(), "mvp");
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
	hair_program.draw_vertices(0, hair->get_vbo(VBO_VERTICES), 3, GL_TRIANGLES, 0, hair->get_vsize());
	hair_program.enable_attr(hair_program.get_attrib_location("vertexUV"));
	hair_program.bind_array_buffer(hair_program.get_attrib_location("vertexUV"), hair->get_vbo(VBO_TEXCOORDS), 2);
	
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	width = w; height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void init_opengl(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ); // | GLUT_MULTISAMPLE
	glutInitWindowSize(800, 600);
	
    glutwin = glutCreateWindow("Virtual Hairstyle");
	
	myGLinit();
	
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
}
