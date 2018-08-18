#ifndef CUBEMAP__H_
#define CUBEMAP__H_
#include <string>
class CubeMap
{
public:
	enum SIDE
	{
		POS_X = 1,
		NEG_X = -1,

		POS_Y = 2,
		NEG_Y = -2,

		POS_Z = 3,
		NEG_Z = -3
	};

	CubeMap(std::vector<std::string> & paths);
	~CubeMap();

	Color4 get_texel(Vector3 & dir);

private:

	std::vector<Texture *> maps;

};
#endif

