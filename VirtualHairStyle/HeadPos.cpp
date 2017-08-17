// HeadPose.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <ctime>
#include <cmath>
#include "stdafx.h"

#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <glut.h>
#include <vector>
#include <iostream>
#include <fstream>

//#include "glm.h"
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
bool readFrame;
bool stopReadFrame;
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
	case 't':
	case 'T': {
		readFrame = true;
		break;
	}
	case 'p':
	case 'P':
	{
		stopReadFrame = !stopReadFrame;
		break;
	}
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

/*void display(void)
{	
	Mat frame;
	bool wasReadNewFrame = false;
	clock_t t1 = clock();
	if (capture.isOpened()) {
		if (!stopReadFrame &&/* readFrame &&*//* capture.read(frame)) { // while
			readFrame = false;
			wasReadNewFrame = true;
			if (!frame.empty()) {
				Mat flipFrame;
				cv::flip(frame, flipFrame, 1);
				detect_2d_points(flipFrame);
			} else {
				printf(" --(!) No captured frame -- Break!"); //break;
			}
		}
		int c = waitKey(10);
	}
	
	// draw the image in the back
	int vPort[4]; glGetIntegerv(GL_VIEWPORT, vPort);
	glEnable2D();
	drawOpenCVImageInGL(imgTex);
	glDisable2D();
	
	glClear(GL_DEPTH_BUFFER_BIT); // we want to draw stuff over the image
	glViewport(0, 0, vPort[2], vPort[3]);
	
	glPushMatrix();
	
	gluLookAt(0,0,0,0,0,1,0,-1,0);

	// put the object in the right position in space
	Vec3d tvv(tv[0],tv[1],tv[2]);
	glTranslated(tvv[0], tvv[1], tvv[2]);

	// rotate it
	double _d[16] = {	rot[0],rot[1],rot[2],0,
						rot[3],rot[4],rot[5],0,
						rot[6],rot[7],rot[8],0,
						0,	   0,	  0		,1};
	
	glMultMatrixd(_d);
	
	// draw the 3D head model
	glColor4f(1, 1, 1, 0);*/
	/*glmDraw(head_obj, GLM_SMOOTH);
	glColor4f(1, 1, 0.8, 1); // color for hair + alfa == 1
	glmDraw(hair_obj, GLM_SMOOTH | GLM_TEXTURE);

	glPopMatrix();
	
	// restore to looking at complete viewport
	glViewport(0, 0, vPort[2], vPort[3]); 
	
	glutSwapBuffers();
	if (wasReadNewFrame) {
		printf("fps %f ", 1/(float(clock() - t1) / CLOCKS_PER_SEC));
	}
}*/
int width, height;

static void update_background_texture()
{
	if (background_image.data != NULL) {
		cv::Mat tmp = Mat(Size(1024, 512), CV_8UC3);
		cv::resize(background_image, tmp, Size(1024, 512));
		Mat ttmp = tmp(Range(0, tmp.rows), Range(0, tmp.cols));

		if (tmp.step == tmp.cols) {
			cvtColor(tmp, ttmp, CV_GRAY2RGB);
		} else if (tmp.step == tmp.cols * 3) {
			cvtColor(tmp, ttmp, CV_BGR2RGB);
		}

		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp.data);
		glBindTexture(GL_TEXTURE_2D, background_texture);
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
	float m[16] = {
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
	update_background_texture();
	background_program.use();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_background_texture, /*GL_TEXTURE*/0);
	glBindTexture(GL_TEXTURE_2D, background_texture);

	background_program.draw_vertices(0, background_buffer, 3, GL_QUADS, 0, 4);
	background_program.enable_attr(attribute_texcoord);
	background_program.bind_array_buffer(attribute_texcoord, background_texcoords_buffer, 2);
}

void display()
{
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	background_draw();

	head_program.use();
	glm::mat4 mvp = calc_mvp_mat();
	GLuint matrix_id = glGetUniformLocation(head_program.get_program_id(), "mvp");
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
	head_program.draw_vertices(0, vertexbuffer, 3, GL_TRIANGLES, 0, head.vertices.size());

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	width = w; height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	//glOrtho(0, w, 0, h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glutPostRedisplay();
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

void solve_head_pos(Mat& ip, Mat *img) {
	int max_d = MAX(img->rows, img->cols);
	cam_mat = (Mat_<double>(3, 3) << max_d, 0, img->cols / 2.0,
		0, max_d, img->rows / 2.0,
		0, 0, 1.0);
	double _dc[] = { 0,0,0,0 };
	
	if (!solvePnP(op, ip, cam_mat, Mat(1, 4, CV_64FC1, _dc), rvec, tvec, true, SOLVEPNP_EPNP)) {
		std::cout << "solvePnP was fail" << std::endl;
	}
	/* UNRESOLVED: Sometimes blink effect. TODO
		
	cv::Point2d *img_points = ip.ptr<cv::Point2d>();
	std::vector<Point3d> nose_end_point3D;
	std::vector<Point2d> nose_end_point2D;
	nose_end_point3D.push_back(Point3d(0, 0, 10.0));
	nose_end_point3D.push_back(Point3d(0, 10.0, 0));
	nose_end_point3D.push_back(Point3d(10.0, 0, 0));
	std::cout << "begin ... ";
	projectPoints(nose_end_point3D, rvec, tvec, cam_mat, Mat(1, 4, CV_64FC1, _dc), nose_end_point2D);
	cv::line(*img, img_points[1], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2); //blue z
	cv::line(*img, img_points[1], nose_end_point2D[1], cv::Scalar(0, 255, 0), 2); //green y
	cv::line(*img, img_points[1], nose_end_point2D[2], cv::Scalar(0, 0, 255), 2); //red x
	std::cout << "end" << std::endl;*/
	
	cv::Rodrigues(rvec, rmat);
}
