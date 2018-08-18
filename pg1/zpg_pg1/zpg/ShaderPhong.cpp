#include "stdafx.h"
#include "ShaderPhong.h"

// https://www.youtube.com/watch?v=L4oAQL_Mv5w

/*
Vector3 ShaderPhong::GetPixel(Ray& ray, int depth)
{
	rtcIntersect(scene, ray); // find the closest hit of a ray segment with the scene
							  // pri filtrovani funkce rtcIntersect jsou pruseciky prochazeny od nejblizsiho k nejvzdalenejsimu
							  // u funkce rtcOccluded jsou nalezene pruseciky vraceny v libovolnem poradi

	Vector3 px = { 0,0,0 };
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Surface * surface = surfaces[ray.geomID];
		Triangle & triangle = surface->get_triangle(ray.primID);
		Material * material = surface->get_material();

		// získání souøadnic prùseèíku, normál, texturovacích souøadnic atd.
		const Vector3 p = ray.eval(ray.tfar);
		//Vector3 geometry_normal = -Vector3(ray.Ng); // Ng je nenormalizovaná normála zasaženého trojúhelníka vypoètená nesouhlasnì s pravidlem pravé ruky o závitu
		//geometry_normal.Normalize(); // normála zasaženého trojúhelníka vypoètená souhlasnì s pravidlem pravé ruky o závitu
		const Vector3 normal = triangle.normal(ray.u, ray.v);
		const Vector2 texture_coord = triangle.texture_coord(ray.u, ray.v);

		Texture * texture_diffuze = material->get_texture(material->kDiffuseMapSlot);
		Vector3 diffuze;
		if (texture_diffuze) {
			Color4 col = texture_diffuze->get_texel(texture_coord.x, texture_coord.y);
			diffuze = Vector3(col.r, col.g, col.b);
		}
		else
			diffuze = material->diffuse;


		Vector3 lightDir = light.position - p;
		lightDir.Normalize();
		Vector3 rayDir = Vector3(ray.dir);


		if (depth > 0 && material->get_name() == "green_plastic_transparent") {
			material->ior = 1.55f;
			//px *= 0.1f;

			float n1 = ray.ior;
			float n2 = material->ior == 1.55f && material->ior == ray.ior ? 1.0f : 1.55f;
			float n = n1 / n2;

			Vector3 norm = normal;

			//refraction			
			float Q2 = rayDir.DotProduct(-norm);
			if (Q2 < 0) {
				Q2 = rayDir.DotProduct(norm);
				norm = -norm;
			}
			float Q1 = 1 - SQR(n) * (1 - SQR(Q2));
			if (Q1 > 0) Q1 = sqrt(Q1);

			Vector3 rr = Vector3(-n * rayDir - (n * Q2 + Q1) * norm);
			Vector3 l = rr - (2.0f * rr.DotProduct(norm)) * norm;
			Vector3 lr = -l;

			auto refractRay = Ray(p, rr, 0.01f);
			refractRay.ior = n2;
			auto refractColor = GetPixel(refractRay, depth - 1);

			//reflection
			//auto reflectRay = Ray(p, lr, 0.01f);
			//reflectRay.ior = n1;
			//auto reflectColor = Trace(reflectRay, depth - 1) * material->specular;

			//reflection
			auto vr = MAX((2.0f * normal.DotProduct(-rayDir)), 0.0f) * normal + rayDir;
			vr.Normalize();
			auto reflectRay = Ray(p, vr, 0.01f);
			reflectRay.ior = n1;
			auto reflectColor = GetPixel(reflectRay, depth - 1) * material->specular;

			float R, T;
			Fresnel(n1, n2, Q1, Q2, T, R);

			px += (reflectColor * 0.15f + refractColor * 0.85f) * diffuze;
		}
		else
			px = GetDiffuzeColor(normal, lightDir, diffuze)
			+ material->ambient
			+ GetSpecular(normal, rayDir, lightDir, material->specular, material->shininess);

	}
	else {
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}
*/

Vector3 ShaderPhong::GetPixel(Ray& ray, int depth)
{
	rtcIntersect(scene, ray);

	Vector3 px = { 0, 0, 0 };
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Surface* surface = surfaces[ray.geomID];
		Triangle& triangle = surface->get_triangle(ray.primID);
		Material* material = surface->get_material();

		// Intersect point.
		const Vector3 p = ray.eval(ray.tfar);

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

		Vector3 lightDir = light.position - p;
		lightDir.Normalize();
		Vector3 rayDir = Vector3(ray.dir);

		// We done here
		if(depth <= 0)
		{
			px = GetDiffuzeColor(normal, lightDir, diffuze)
				+ material->ambient
				+ GetSpecular(normal, rayDir, lightDir, material->specular, material->shininess);

			return px;
		}

		// Glass - reflect + refract
		if(material->get_name() == "green_plastic_transparent" || material->get_name() == "wire_214229166")
		{
			//material->ior = 1.55f;

			// Ior.
			float n1 = ray.ior;
			float n2 = material->ior == 1.3f && material->ior == ray.ior ? 1.0f : 1.3f;//SwitchIor(n1);
			float n = n1 / n2;

			Vector3 norm = normal;

			// Calculate cos2.
			float Q2 = (-norm).DotProduct(rayDir);

			if (Q2 < 0) {
				norm = -norm;
				Q2 = (-norm).DotProduct(rayDir);
			}

			// Calculate cos1.
			float Q1 = 1 - SQR(n) * (1 - SQR(Q2));

			// Not absolute reflection.
			// If < 0, then it is...
			if (Q1 > 0)
				Q1 = sqrt(Q1);

			// Vector rr.
			Vector3 rr = Vector3(-n * rayDir - (n * Q2 + Q1) * norm);
			rr.Normalize();
			//Vector3 l = rr - (2.0f * rr.DotProduct(norm)) * norm;
			//Vector3 lr = -l;

			auto refractedRay = Ray(p, rr, 0.01f);
			refractedRay.ior = n2;
			auto refractColor = GetPixel(refractedRay, depth - 1);

			// Reflected ray.
			auto refDir = MAX((2.0f * normal.DotProduct(-rayDir)), 0.0f) * normal + rayDir;
			refDir.Normalize();
			auto reflectedRay = Ray(p, refDir, 0.01f);
			reflectedRay.ior = n1;
			auto reflectColor = GetPixel(reflectedRay, depth - 1) * material->specular;

			float R, T;
			Fresnel(n1, n2, Q1, Q2, T, R);
			float coefRefract = 1.0f - R;
			
			// Add refraction and reflection.
			px += (reflectColor * reflectionIntensity + refractColor * refractionIntensity) * diffuze;
		}
		// reflect only
		else
		{
			auto refDir = MAX((2.0f * normal.DotProduct(-rayDir)), 0.0f) * normal + rayDir;
			refDir.Normalize();
			auto reflectedRay = Ray(p, refDir, 0.01f);
			auto reflectColor = GetPixel(reflectedRay, depth - 1) * material->specular;

			px = reflectColor * reflectionIntensity + GetDiffuzeColor(normal, lightDir, diffuze)
				+ material->ambient
				+ GetSpecular(normal, rayDir, lightDir, material->specular, material->shininess);
		}
	}
	// Enviroment
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}


ShaderPhong::~ShaderPhong()
{
}


/*
Vector3 ShaderPhong::GetPixel(Ray& ray, int depth)
{
rtcIntersect(scene, ray);

Vector3 px = { 0, 0, 0 };
if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
{
Surface* surface = surfaces[ray.geomID];
Triangle& triangle = surface->get_triangle(ray.primID);
Material* material = surface->get_material();


const Vector3 p = ray.eval(ray.tfar);
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

Vector3 lightDir = light.position - p;
lightDir.Normalize();
Vector3 rayDir = Vector3(ray.dir);

// TODO: Shadows.
/*auto shadowRay = Ray(p, lightDir, 0.01f, lightDir.Length());
rtcOccluded(scene, shadowRay);
float shadowed = shadowRay.geomID == 0 ? 0.0f : 1.0f;


px = GetDiffuzeColor(normal, lightDir, diffuze) /** shadowed
+ material->ambient
+ GetSpecular(normal, rayDir, lightDir, material->specular, material->shininess) /** shadowed ;
	}
	else
	{
		auto mapPx = cubeMap.get_texel(Vector3(ray.dir));
		px = { mapPx.r, mapPx.g, mapPx.b };
	}

	return px;
}
*/
