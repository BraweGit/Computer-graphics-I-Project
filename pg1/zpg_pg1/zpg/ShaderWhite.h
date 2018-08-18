#ifndef SHADERWHITE_H_
#define SHADERWHITE_H_
class ShaderWhite : public Shader
{
public:
	ShaderWhite(RTCScene& sc, Camera& cam, std::vector<Surface*>& surf, OmniLight& li, CubeMap& cube) : Shader(sc, cam, surf, li, cube){}
	
	Vector3 GetPixel(Ray& ray, int depth) override;

	~ShaderWhite();
};
#endif

