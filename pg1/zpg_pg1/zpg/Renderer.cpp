#include "stdafx.h"


//Renderer::Renderer(Camera& cam, RTCScene& sc, std::vector<Surface*>& surfs, OmniLight& olight) : camera(cam), scene(sc), surfaces(surfs), light(olight)
//{
//}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Render(Shader& shader, bool superSampling)
{
	printf("Start ray tracing...\n");

	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
	cv::moveWindow("Image", 0, 0);

	Camera& camera = shader.camera;
	RTCScene& scene = shader.scene;
	int depth = 15;

	// Ray trace.
	// Divide each pixel by.
	const int divideBy = 15;
	const float step = 1.0f / (float)divideBy;


	// obrázek tøí barevných kanálù, jeden pixel tvoøí 3 subpixely; Blue, Green, Red
	cv::Mat image = cv::Mat(
		cv::Size(camera.width(), camera.height()), CV_32FC3);
	cv::resizeWindow("Image", image.cols, image.rows);

#pragma omp parallel for
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; ++x) {

			Ray rtc_ray = camera.GenerateRay(x, y);
			Vector3 px = { 0,0,0 };
			
			if (superSampling) {
				for (int r = 0; r < divideBy; ++r) {
					for (int c = 0; c < divideBy; ++c) {

						const float randX = Random(x + (c*step), x + ((c + 1) * step));
						const float randY = Random(y + (r*step), y + ((r + 1) * step));

						Ray rtc_Randray = camera.GenerateRay(randX, randY);

						px += shader.GetPixel(rtc_Randray, depth);
					}
				}
				px /= (float)(divideBy*divideBy);
			}
			else
			{
				px = shader.GetPixel(rtc_ray, depth);
			}

			// static_cast<float>(pow(px.z, 1.0f/2.2f)), static_cast<float>(pow(px.y, 1.0f/2.2f)), static_cast<float>(pow(px.x, 1.0f/2.2f))
			image.at<cv::Vec3f>(y, x) = { px.z, px.y, px.x };
		}
	}

	printf("Redering done!\n");
	cv::imshow("Image", image);
	//cvWaitKey(0);
}

/*
void Renderer::Render(Shader& shader)
{
printf("Start ray tracing...\n");

cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
cv::moveWindow("Image", 0, 0);

Camera& camera = shader.camera;
RTCScene& scene = shader.scene;
int depth = 5;


// obrázek tøí barevných kanálù, jeden pixel tvoøí 3 subpixely; Blue, Green, Red
cv::Mat image = cv::Mat(
cv::Size(camera.width(), camera.height()), CV_32FC3);
cv::resizeWindow("Image", image.cols, image.rows);
Vector3 px = Vector3{ 0,0,0 };

for (int y = 0; y < image.rows; ++y)
{
for (int x = 0; x < image.cols; ++x)
{
Ray rtc_ray = camera.GenerateRay(x, y);
rtcIntersect(scene, rtc_ray);

// TODO: METHOD
//Surface *surface = surfaces.at(rtc_ray.geomID);
//Triangle triangle = surface->get_triangle(rtc_ray.primID);

//Vector3 normal = triangle.normal(rtc_ray.u, rtc_ray.v);
//normal.Normalize();

// Get normal
// Normal to RGB
// (n+(1,1,1))/2

//if (rtc_ray.geomID != RTC_INVALID_GEOMETRY_ID)
//{
//	image.at<cv::Vec3f>(y,x) = cv::Vec3f(1, 1, 1);
//}
px = shader.GetPixel(rtc_ray, depth);
image.at<cv::Vec3f>(y, x) = { px.z, px.y, px.x };
}
}

cv::imshow("Image", image);
cvWaitKey(0);
}*/