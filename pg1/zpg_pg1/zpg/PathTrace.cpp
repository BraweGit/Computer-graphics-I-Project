#include "stdafx.h"
#include "PathTrace.h"


/*
Vector3 PathTrace::GetPixel(Ray& ray, int depth)
{
	Vector3 px = { 0,0,0 };

	if (depth <= 0)
		return px;

	// We hit something.
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Surface * surface = surfaces[ray.geomID];
		Triangle & triangle = surface->get_triangle(ray.primID);
		Material * material = surface->get_material();

		// Intersection point.
		const Vector3 p = ray.eval(ray.tfar);

		Vector3 geometry_normal = -Vector3(ray.Ng);
		geometry_normal.Normalize();
		const Vector3 normal = triangle.normal(ray.u, ray.v);
		const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

		Texture* texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
		Vector3 diffuze;

		// Random vector, 34
		Vector3 randDir = { 0,0,0 };

		const float r1 = Random(0.0f, 1.0f);
		const float r2 = Random(0.0f, 1.0f);
		const float x = cos(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
		const float y = sin(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
		const float z = r2;

		randDir = Vector3(x, y, z);
		randDir.Normalize();

		// If not on the right hemispere ...
		// reverse it.
		if (normal.DotProduct(randDir) < 0.0f)
		{
			randDir = -randDir;
		}

		Ray randRay(p, randDir, 0.01f);
		px = Vector3(1, 1, 1) * this->GetPixel(randRay, depth - 1) * randDir * this->Brdf() * 2 * M_PI;
	}
	// We didn't hit anything.
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}
*/

// Path trace #1
//Vector3 PathTrace::GetPixel(Ray & ray, int depth, int x, int y)
//{
//	Vector3 px = { 0.0f, 0.0f, 0.0f };
//
//	for (int i = 0; i < this->samples; i++)
//	{
//		float x_ = (x + Random()) - 0.5f;
//		float y_ = (y + Random()) - 0.5f;
//
//		px += this->GetPixel(camera.GenerateRay(x_, y_), depth);
//	}
//
//	return px / (float) this->samples;
//}

// totální odraz, kvazi nahodna cisla, total compendium book
// fr lambertovsky povrch - albedo / pi (albedo <0, 1>)
// path tracing - vzdycky jenom jeden paprsek, supersampling
// kdyz dojde rekurze, vratit 0
// kdyz vyletime ze sceny, vratit 1 (globalni svetlo) nebo cube mapu

Vector3 PathTrace::GetPixel(Ray& ray, int depth)
{
	Vector3 rayDir = Vector3(ray.dir);
	rayDir.Normalize();

	rtcIntersect(scene, ray);

	// Check hit.
	if (!(ray.geomID != RTC_INVALID_GEOMETRY_ID))
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		Vector3 vec{ 0,0,0 };
		return vec;//Vector3(mapPx.r, mapPx.g, mapPx.b);
	}

	Surface * surface = surfaces[ray.geomID];
	Triangle & triangle = surface->get_triangle(ray.primID);
	Material * material = surface->get_material();
	// Intersection point.
	const Vector3 p = ray.eval(ray.tfar);
	//Vector3 geometry_normal = -Vector3(ray.Ng);
	//geometry_normal.Normalize();
	Vector3 normal = triangle.normal(ray.u, ray.v);

	// Check normal orientation.
	if (-rayDir.DotProduct(normal) < 0.0f)
	{
		normal = -normal;
	}

	const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

	// Random vector, 34
	Vector3 randDir = { 0,0,0 };

	const float r1 = Random(0.0f, 1.0f);
	const float r2 = Random(0.0f, 1.0f);
	const float x = cos(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float y = sin(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float z = r2;

	randDir = Vector3(x, y, z);
	randDir.Normalize();

	Vector3 px(0.0f, 0.0f, 0.0f);

	// If not on the right hemispere ...
	// reverse it.
	if (normal.DotProduct(randDir) < 0.0f)
	{
		randDir = -randDir;
	}

	Texture* texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
	Vector3 diffuze;

	if (texture_diffuze)
	{
		Color4 col = texture_diffuze->get_texel(texture_coord.x, texture_coord.y);
		diffuze = Vector3(col.r, col.g, col.b);
	}
	else
		diffuze = material->diffuse;

	Vector3 lightDir = light.position - p;
	lightDir.Normalize();

	// http://slideplayer.com/slide/6399542/
	Vector3 Le = { 0, 0, 0};
	
	// black_plastic
	// white_plastic
	if (material->get_name() == "green_plastic_transparent")
	{
		Le = { 5, 5, 5 };
	}
	//else if (material->get_name() == "black_plastic")
	//{
	//	Le = { 5, 5, 5 };
	//}
	//else if (material->get_name() == "white_plastic")
	//{
	//	Le = { 5, 5, 5 };
	//}


	//px = Brdf() * normal.DotProduct(randDir) * GetDiffuzeColor(normal, lightDir, diffuze);

	//Ray randomRay(p, randDir, 0.01f);
	//px += GetPixel(randomRay, depth - 1);

	if (depth >= 0)
	{
		Ray randomRay(p, randDir, 0.01f);
		float cosTheta = normal.DotProduct(randDir);
		px = Le 
			+ diffuze 
			* this->GetPixel(randomRay, depth - 1) 
			* normal.DotProduct(randDir)
			* this->Brdf()
			* 1.0f / (cosTheta / M_PI);
	}

	return px;
}



// Path trace #2
/*
Vector3 PathTrace::GetPixel(Ray& ray, int depth)
{
	Vector3 rayDir = Vector3(ray.dir);
	rayDir.Normalize();

	rtcIntersect(scene, ray);

	// Check hit.
	if (!(ray.geomID != RTC_INVALID_GEOMETRY_ID))
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		return Vector3(mapPx.r, mapPx.g, mapPx.b);
	}

	Surface * surface = surfaces[ray.geomID];
	Triangle & triangle = surface->get_triangle(ray.primID);
	Material * material = surface->get_material();
	// Intersection point.
	const Vector3 p = ray.eval(ray.tfar);
	//Vector3 geometry_normal = -Vector3(ray.Ng);
	//geometry_normal.Normalize();
	Vector3 normal = triangle.normal(ray.u, ray.v);

	// Check normal orientation.
	if (-rayDir.DotProduct(normal) < 0.0f)
	{
		normal = -normal;
	}

	const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

	// Random vector, 34
	Vector3 randDir = { 0,0,0 };

	const float r1 = Random(0.0f, 1.0f);
	const float r2 = Random(0.0f, 1.0f);
	const float x = cos(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float y = sin(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float z = r2;

	randDir = Vector3(x, y, z);
	randDir.Normalize();

	Vector3 px(0.0f, 0.0f, 0.0f);

	// If not on the right hemispere ...
	// reverse it.
	if (normal.DotProduct(randDir) < 0.0f)
	{
		randDir = -randDir;
	}

	Texture* texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
	Vector3 diffuze;

	if (texture_diffuze)
	{
		Color4 col = texture_diffuze->get_texel(texture_coord.x, texture_coord.y);
		diffuze = Vector3(col.r, col.g, col.b);
	}
	else
		diffuze = material->diffuse;

	Vector3 lightDir = light.position - p;
	lightDir.Normalize();

	//px = Brdf() * normal.DotProduct(randDir) * GetDiffuzeColor(normal, lightDir, diffuze);

	//Ray randomRay(p, randDir, 0.01f);
	//px += GetPixel(randomRay, depth - 1);

	if (depth >= 0)
	{
		auto diff = this->GetDiffuzeColor(normal, lightDir, diffuze);
		Ray randomRay(p, randDir, 0.01f);
		px = Vector3(1.0f, 1.0f, 1.0f) * diff * this->GetPixel(randomRay, depth - 1) * normal.DotProduct(randDir)
			* this->Brdf() * 2 * M_PI;
	}


	return px;
}
*/



// Path trace #1
/*
Vector3 PathTrace::GetPixel(Ray & ray, int depth)
{
	Vector3 px = { 0.0f, 0.0f, 0.0f };

	if (depth <= 0)
		return px;

	Vector3 rayDir = Vector3(ray.dir);
	rayDir.Normalize();

	rtcIntersect(scene, ray);

	// We hit something.
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Surface * surface = surfaces[ray.geomID];
		Triangle & triangle = surface->get_triangle(ray.primID);
		Material * material = surface->get_material();
		// Intersection point.
		const Vector3 p = ray.eval(ray.tfar);
		//Vector3 geometry_normal = -Vector3(ray.Ng);
		//geometry_normal.Normalize();
		Vector3 normal = triangle.normal(ray.u, ray.v);

		// Check normal orientation.
		if (-rayDir.DotProduct(normal) < 0.0f)
		{
			normal = -normal;
		}

		const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

		// Random vector, 34
		Vector3 randDir = { 0,0,0 };

		const float r1 = Random(0.0f, 1.0f);
		const float r2 = Random(0.0f, 1.0f);
		const float x = cos(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
		const float y = sin(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
		const float z = r2;

		randDir = Vector3(x, y, z);
		randDir.Normalize();

		// If not on the right hemispere ...
		// reverse it.
		if (normal.DotProduct(randDir) < 0.0f)
		{
			randDir = -randDir;
		}

		Texture* texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
		Vector3 diffuze;

		if (texture_diffuze)
		{
			Color4 col = texture_diffuze->get_texel(texture_coord.x, texture_coord.y);
			diffuze = Vector3(col.r, col.g, col.b);
		}
		else
			diffuze = material->diffuse;

		Vector3 lightDir = light.position - p;
		lightDir.Normalize();

		px = Brdf() * normal.DotProduct(randDir) * GetDiffuzeColor(normal, lightDir, diffuze);

		Ray randomRay(p, randDir, 0.01f);
		px += GetPixel(randomRay, depth - 1);

	}
	// We didn't hit anything.
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}
*/


PathTrace::~PathTrace()
{
}


/*
Vector3 PathTrace::GetPixel(Ray & ray, int depth)
{
	Surface * surface = surfaces[ray.geomID];
	Triangle & triangle = surface->get_triangle(ray.primID);
	Material * material = surface->get_material();

	// Intersection point.
	const Vector3 p = ray.eval(ray.tfar);

	Vector3 geometry_normal = -Vector3(ray.Ng);
	geometry_normal.Normalize();
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

	Vector3 emittance = material->emission;
	Vector3 rayDir = Vector3(ray.dir);

	Vector3 randDir = { 0,0,0 };

	const float r1 = Random(0.0f, 1.0f);
	const float r2 = Random(0.0f, 1.0f);
	const float x = cos(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float y = sin(2.0f * M_PI * r1) * sqrt(1.0f - SQR(r2));
	const float z = r2;

	randDir = Vector3(x, y, z);
	randDir.Normalize();

	// If not on the right hemispere ...
	// reverse it.
	if (normal.DotProduct(randDir) < 0.0f)
	{
		randDir = -randDir;
	}

	// Vector random to intersection point.
	randDir = randDir - p;
	randDir.Normalize();
	auto sphereRay = Ray(p, randDir, 0.01f);

	Vector3 lightDir = light.position - p;
	lightDir.Normalize();

	float cos_theta = normal.DotProduct(rayDir);
	float BRDF = 1.0 / M_PI;
	float pdf = cos_theta / M_PI;
	Vector3 reflected = GetPixel(sphereRay, depth - 1);
	reflected = reflected * GetDiffuzeColor(normal, lightDir, diffuze) * cos_theta;


	return reflected;//(emittance + (BRDF * reflected)) / (cos_theta / M_PI);
}

*/
