#ifndef PATHTRACE_H_
#define PATHTRACE_H_
class PathTrace : public Shader
{
public:
	PathTrace(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& li, CubeMap& cube) : Shader(sc, cam, surf, li, cube) {}

	Vector3 GetPixel(Ray& ray, int depth) override;
	Vector3 GetPixel(Ray& ray, int depth, int x, int y);

	~PathTrace();
};
#endif

