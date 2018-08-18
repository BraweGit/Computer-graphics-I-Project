#include "stdafx.h"
#include "ShaderWhite.h"

Vector3 ShaderWhite::GetPixel(Ray& ray, int depth)
{
	rtcIntersect(scene, ray);

	Vector3 px = { 0, 0, 0 };
	if(ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		px = Vector3(1, 1, 1);
	}
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}


ShaderWhite::~ShaderWhite()
{
}
