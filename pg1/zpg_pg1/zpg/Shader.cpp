#include "stdafx.h"
#include "Shader.h"


/* Poznamky
 * dotproduct (skalarni soucin) - vzdalenost mezi vektory, vraci -1 az 1, podle toho jak blizko je prvni vektor ke druhemu (-1 == daleko, 1 == blizko)
 *								- hodnoty mensi nez 0 nas vetsinou nezajimaji je tam proste cerna barva
 * lightdirection - smer odkud jde svetlo
 * 
 * 
 * 
 * 
 * 
 */

Vector3 Shader::GetDiffuzeColor(const Vector3& normal, const Vector3& lightNormal, const Vector3& diffuzeColor)
{
	return MAX(lightNormal.DotProduct(normal), 0.0f) * diffuzeColor;
}

Vector3 Shader::GetSpecular(const Vector3& normal, const Vector3& viewNormal, const Vector3& lightNormal, const Vector3& specularColor, float shininess)
{
	auto vr = 2.0f * viewNormal.DotProduct(normal) * normal - viewNormal;
	float vrLd = MAX(std::powf(vr.DotProduct(lightNormal), shininess), 0.0f);
	return vrLd * specularColor;
}

void Shader::Fresnel(const float n1, const float n2, const float Q1, const float Q2, float & T, float & R) {
	auto Rs = SQR((n1 * Q2 - n2 * Q1) / (n1 * Q2 + n2 * Q1));
	auto Rp = SQR((n1 * Q1 - n2 * Q2) / (n1 * Q1 + n2 * Q2));

	//float Ts = 1.0f - Rs;
	//float Tp = 1.0f - Rp;

	//R = (Rs + Rp) * 0.5f;
	//T = (Ts + Tp) * 0.5f;
	R = 0.5f * (Rs + Rp);
}

float Shader::Brdf()
{
	return 0.5f / M_PI; 
}


float Shader::SwitchIor(float ior)
{
	return ior == airIor ? glassIor : airIor;
}

bool Shader::Refract(std::string name)
{
	std::string sphereName = "wire_214229166";
	std::string avengerName = "green_plastic_transparent";

	if (name == sphereName || name == avengerName)
		return true;

	return false;
}