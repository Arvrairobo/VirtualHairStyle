#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "PoseEstimation.h"
#include "HeadPos.h"
#include "OGL_OCV_common.h"
#include <iostream>
#include <stdio.h>
#include "Obj3DModel.h"
#include <thread>
using namespace std;
using namespace cv;

GLMmodel* head_obj;
GLMmodel* hair_obj;

std::vector<double> rv(3), tv(3);
cv::Mat rvec(rv), tvec(tv);
cv::Mat rmat;
OpenCVGLTexture imgTex;
cv::Mat cam_mat;
cv::Mat op;
VideoCapture capture;

dlib::frontal_face_detector detector;
dlib::shape_predictor pose_model;

int glutwin;

class VideoCapture_t {
public:
	VideoCapture_t(int cam_idx = 0) : _thr(&VideoCapture_t::run, this), _cam_idx(cam_idx) {}
	~VideoCapture_t() {
		_thr.join();
	}
private:
	std::thread _thr;
	int _cam_idx;
	void run() {
		cv::VideoCapture _capture;
		//_capture.open(_cam_idx);
		_capture.open("C:\\Users\\iantv\\Videos\\i.avi");
		cv::Mat _frame;
		while (_capture.isOpened()) {
			if (_capture.read(_frame)) {
				if (!_frame.empty()) {
					cv::flip(_frame, _frame, 1);
					detect_2d_points(_frame);
					glutPostWindowRedisplay(glutwin);
				} else {
					printf(" --(!) No captured frame -- Break!");
					break;
				}
			}
			int c = waitKey(10);
		}
	}
};

GLuint background_texture;

GLuint init_texture() {
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return t;
}

static GLfloat background_vbuf_data[] = {
	-1.0, -1.0,  0.0,
	1.0, -1.0,  0.0,
	1.0,  1.0,  0.0,
	-1.0,  1.0,  0.0,
};

GLuint background_array_id;
GLuint background_buffer;

GLuint VertexArrayID;
GLuint vertexbuffer;

GLint uniform_background_texture;
GLint attribute_coord3d, attribute_v_color, attribute_texcoord;

#include "Shader.h"
ShaderProgram_t head_program;
ShaderProgram_t background_program;

GLfloat background_texcoords[] = {
	// front
	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,
};

GLuint background_texcoords_buffer;
Obj3DModel_t head, hair;

int main(int argc, char **argv)
{
	init_opengl(argc, argv);

	vector<Point3f > modelPoints;
	modelPoints.push_back(Point3f(0.002619, 1.64075, 0.10742)); // chin
	modelPoints.push_back(Point3f(0.002906, 1.71028, 0.12765)); // nose
	modelPoints.push_back(Point3f(-0.01894, 1.68333, 0.10709)); // left lip
	modelPoints.push_back(Point3f(0.02458, 1.68255, 0.1049)); // right lip
	modelPoints.push_back(Point3f(-0.04733, 1.74132, 0.08684)); // left eye
	modelPoints.push_back(Point3f(0.05289, 1.74154, 0.08526)); // right eye
	modelPoints.push_back(Point3f(-0.08431, 1.75617, -0.008755)); // left ear
	modelPoints.push_back(Point3f(0.0849, 1.75458, -0.005749)); // right ear
	op = Mat(modelPoints);

	detector = dlib::get_frontal_face_detector();
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
	cout << "shape_predictor_68_face_landmark.dat deserialized" << endl;
	if (glewInit() != GLEW_OK)
		throw std::exception("Failed to initialize GLEW\n");

	load_obj("3D models\\head_v3.obj", head);

	head_program = ShaderProgram_t("shaders\\head.vert", "shaders\\head.frag");
	head_program.create_shader_program();
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, head.vertices.size()*sizeof(glm::vec3), &head.vertices[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	background_program = ShaderProgram_t("shaders\\background.vert", "shaders\\background.frag");
	background_program.create_shader_program();

	glGenVertexArrays(1, &background_array_id);
	glBindVertexArray(background_array_id);

	glGenBuffers(1, &background_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, background_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_vbuf_data), background_vbuf_data, GL_STATIC_DRAW);

	
	VideoCapture_t cap;
	background_texture = init_texture();
	
	attribute_texcoord = glGetAttribLocation(background_program.get_program_id(), "texcoord");
	if (attribute_texcoord == -1) {
		cerr << "Could not bind attribute texcoord" << endl;
		return false;
	}

	glGenBuffers(1, &background_texcoords_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, background_texcoords_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_texcoords), background_texcoords, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rvec = cv::Mat(rv);
	double _d[9] = { 1,0,0,
		0,-1,0,
		0,0,-1 };
	rmat = cv::Mat(3, 3, CV_64FC1, _d);
	Rodrigues(rmat, rvec);
	tvec = cv::Mat(tv);

	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &vertexbuffer);

	glDeleteVertexArrays(1, &background_array_id);
	glDeleteBuffers(1, &background_buffer);
	glDeleteBuffers(1, &background_texcoords_buffer);
}
/*
int main(int argc, char **argv) {
	

	head_obj = glmReadOBJ("head-obj.obj");
	hair_obj = glmReadOBJ("hair.obj");
	op = Mat(modelPoints);
	/*Scalar m = mean(Mat(modelPoints));
	//op = op - m;

	//assert(norm(mean(op)) < 1e-05); //make sure is centered

	//op = op + Scalar(0,0,25);

	cout << "model points " << op << endl;*

	rvec = Mat(rv);
	double _d[9] = { 1,0,0,
		0,-1,0,
		0,0,-1 };
	Rodrigues(Mat(3, 3, CV_64FC1, _d), rvec);
	tv[0] = 0; tv[1] = 0; tv[2] = 0;
	tvec = Mat(tv);

	cam_mat = Mat(3, 3, CV_64FC1);

	init_opengl(argc, argv);
	// prepare OpenCV-OpenGL images
	imgTex = MakeOpenCVGLTexture(Mat());

	//capture.open(1);
	capture.open("C:\\Users\\iantv\\Videos\\i.avi");

	detector = dlib::get_frontal_face_detector();
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

	glutMainLoop();
	return 0;
}*/
