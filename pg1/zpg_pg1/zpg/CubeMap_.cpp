#include "stdafx.h"
#include "CubeMap_.h"

CubeMap::CubeMap(std::vector<std::string> & paths)
{
	for (int i = 0; i < paths.size(); ++i) {
		maps.push_back(LoadTexture(paths[i].c_str()));
	}
}

CubeMap::~CubeMap()
{
	SafeDeleteVectorItems<Texture *>(maps);
}

Color4 CubeMap::get_texel(Vector3 & dir)
{
	float u, v, tmp;
	int idx = dir.LargestComponent(true);
	int map = dir.data[idx] >= 0 ? idx + 1 : -(idx + 1);

	switch (map)
	{
	case CubeMap::POS_X:
		tmp = 1.0f / abs(dir.x);
		u = (dir.y * tmp + 1) * 0.5f;
		u = 1 - u;
		v = (dir.z * tmp + 1) * 0.5f;
		idx = 0;
		break;
	case CubeMap::NEG_X:
		tmp = 1.0f / abs(dir.x);
		u = (dir.y * tmp + 1) * 0.5f;
		v = (dir.z * tmp + 1) * 0.5f;
		idx = 1;
		break;
	case CubeMap::POS_Y:
		tmp = 1.0f / abs(dir.y);
		u = (dir.x * tmp + 1) * 0.5f;
		v = (dir.z * tmp + 1) * 0.5f;
		idx = 2;
		break;
	case CubeMap::NEG_Y:
		tmp = 1.0f / abs(dir.y);
		u = (dir.x * tmp + 1) * 0.5f;
		u = 1 - u;
		v = (dir.z * tmp + 1) * 0.5f;
		idx = 3;
		break;
	case CubeMap::POS_Z:
		tmp = 1.0f / abs(dir.z);
		u = (dir.x * tmp + 1) * 0.5f;
		v = (-dir.y * tmp + 1) * 0.5f;
		idx = 4;
		break;
	case CubeMap::NEG_Z:
		tmp = 1.0f / abs(dir.z);
		u = (dir.x * tmp + 1) * 0.5f;
		v = (-dir.y * tmp + 1) * 0.5f;
		v = 1 - v;
		idx = 5;
		break;
	}

	return maps[idx]->get_texel(u, v);
}
