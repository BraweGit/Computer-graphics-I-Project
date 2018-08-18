#ifndef SHADER_H_
#define SHADER_H_
#include "CubeMap_.h"

//class Shader
//{
//public:
//	Shader(RTCScene& sc);
//	Vector3 GetPixel(Ray& ray);
//
//	RTCScene& scene;
//
//	~Shader(){}
//};

class Shader
{
public:
	Shader(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& li, CubeMap& cube) : 
		scene(sc), camera(cam), surfaces(surf), light(li), cubeMap(cube){}

	Vector3 GetDiffuzeColor(const Vector3& normal, const Vector3& lightNormal, const Vector3& diffuzeColor);
	Vector3 GetSpecular(const Vector3& normal, const Vector3& viewNormal, const Vector3& lightNormal, const Vector3& specularColor, float shininess);
	void Fresnel(const float n1, const float n2, const float Q1, const float Q2, float & T, float & R);
	float SwitchIor(float ior);
	bool Refract(std::string name);
	float Brdf();

	int samples = 1;
	
	
	virtual Vector3 GetPixel(Ray& ray, int depth) = 0;

	RTCScene& scene;
	Camera& camera;
	std::vector<Surface*>& surfaces;
	OmniLight& light;
	CubeMap& cubeMap;

	const float reflectionIntensity = 0.15f;
	const float refractionIntensity = 0.85f;

	const float glassIor = 1.55f;
	const float airIor = 1.0f;

	virtual ~Shader(){}
};


#endif

