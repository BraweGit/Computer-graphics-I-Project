#ifndef SHADERNORMAL_H_
#define SHADERNORMAL_H_
class ShaderNormal : public Shader
{
public:
	ShaderNormal(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& light, CubeMap& cube) : Shader(sc, cam, surf, light, cube) {}

	Vector3 GetPixel(Ray& ray, int depth) override;

	~ShaderNormal();
};
#endif

