#ifndef SHADERLAMBERT_H_
#define SHADERLAMBERT_H_
class ShaderLambert : public Shader
{
public:
	ShaderLambert(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& light, CubeMap& cube) : Shader(sc, cam, surf, light, cube) {}

	Vector3 GetPixel(Ray& ray, int depth) override;

	~ShaderLambert();
};
#endif

