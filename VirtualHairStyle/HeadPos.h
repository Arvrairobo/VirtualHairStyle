#pragma once
#include <opencv\cxcore.h>
#include <vector>
#include "glm.h"
#include <opencv/cv.h>
#include "OGL_OCV_common.h"
class ShaderProgram_t;

extern int glutwin;
extern std::vector<double> rv, tv;
extern cv::Mat rvec, tvec;
extern cv::Mat cam_mat;
extern GLMmodel* head_obj;
extern GLMmodel* hair_obj;
extern cv::Mat op;
extern OpenCVGLTexture imgTex;
extern cv::Mat background_image;
extern GLuint background_texture;

extern cv::VideoCapture capture;
extern cv::Point2d camera_center;

void init_opengl(int argc, char** argv);
void solve_head_pos(cv::Mat& ip, cv::Mat* img);

extern GLuint vertexbuffer;
extern ShaderProgram_t head_program;

extern GLuint background_buffer;
extern ShaderProgram_t background_program;

extern GLint uniform_background_texture;
extern GLuint background_texcoords_buffer;
extern GLint attribute_coord3d, attribute_v_color, attribute_texcoord;

extern cv::Mat rmat;