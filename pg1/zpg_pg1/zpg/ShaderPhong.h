#ifndef SHADERPHONG_H_
#define SHADERPHONG_H_
class ShaderPhong : public Shader
{
public:
	ShaderPhong(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& li, CubeMap& cube) : Shader(sc, cam, surf, li, cube) {}

	Vector3 GetPixel(Ray& ray, int depth) override;

	~ShaderPhong();
};
#endif

