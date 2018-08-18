#ifndef RENDERER_H_
#define RENDERER_H_

class Renderer
{
public:
	//Renderer(Camera& cam, RTCScene& sc, std::vector<Surface*>& surfs, OmniLight& olight);
	Renderer();

	void Render(Shader& shader, bool superSampling);

	//Camera& camera;
	//RTCScene& scene;
	//std::vector<Surface*>& surfaces;
	//OmniLight& light;
	//CubeMap& cube;


	~Renderer();
};
#endif


