#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>

#include <opencv/cv.h>
#include <opencv\cxcore.h>

void init_opengl(int argc, char** argv);

extern int glutwin;
extern std::vector<double> rv, tv;
extern cv::Mat rvec, tvec;
extern cv::Mat rmat;
extern cv::Mat op;
extern cv::Mat background_image;
extern cv::Mat cam_mat;

class ShaderProgram_t;
extern ShaderProgram_t head_program;
extern ShaderProgram_t background_program;
extern ShaderProgram_t hair_program;