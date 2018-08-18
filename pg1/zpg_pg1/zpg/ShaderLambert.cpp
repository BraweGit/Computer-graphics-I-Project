#include "stdafx.h"
#include "ShaderLambert.h"

Vector3 ShaderLambert::GetPixel(Ray& ray, int depth)
{
	rtcIntersect(scene, ray);

	// Check hit.
	if (!(ray.geomID != RTC_INVALID_GEOMETRY_ID))
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		return Vector3(mapPx.r, mapPx.g, mapPx.b);
	}

	// Intersect point.
	const Vector3 p = ray.eval(ray.tfar);

	Surface* surface = surfaces[ray.geomID];
	Triangle& triangle = surface->get_triangle(ray.primID);
	Material* material = surface->get_material();

	Vector3 lightDir = light.position - p;
	lightDir.Normalize();

	const Vector3 normal = triangle.normal(ray.u, ray.v);
	const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

	Texture* texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
	Vector3 diffuze;

	if (texture_diffuze)
	{
		Color4 col = texture_diffuze->get_texel(texture_coord.x, texture_coord.y);
		diffuze = Vector3(col.r, col.g, col.b);
	}
	else
		diffuze = material->diffuse;

	return GetDiffuzeColor(normal, lightDir, diffuze);
}

ShaderLambert::~ShaderLambert()
{
}
