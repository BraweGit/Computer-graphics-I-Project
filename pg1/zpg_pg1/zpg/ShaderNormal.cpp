#include "stdafx.h"
#include "ShaderNormal.h"

Vector3 ShaderNormal::GetPixel(Ray& ray, int depth)
{
	rtcIntersect(scene, ray);

	Vector3 px = { 0, 0, 0 };
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Surface* surface = surfaces[ray.geomID];
		Triangle& triangle = surface->get_triangle(ray.primID);
		//Material* material = surface->get_material();


		const Vector3 normal = triangle.normal(ray.u, ray.v);
		//const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

		Vector3 diffuze = normal / 2.0f + Vector3(0.5f, 0.5f, 0.5f);


		px = diffuze;
	}
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}


ShaderNormal::~ShaderNormal()
{
}
