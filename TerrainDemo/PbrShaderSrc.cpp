
/*
 * Forrás: https://github.com/sereslorant/logan_engine
 */

/*

Copyright 2017 Lóránt Seres

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "PbrShaderSrc.h"


const char *PbEquationsSource = R"(
	#version 330
	//
	const float PI = 3.14159265359;
	//
	vec3 LambertianDiffuse(vec3 Diffuse,float LightDotNormal)
	{
		return (Diffuse * LightDotNormal) / PI;
	}
	//
	vec3 FresnelSchlick(vec3 Fresnel,float CameraDotHalfway)
	{
		return Fresnel + (1.0 - Fresnel)*pow((1.0 - CameraDotHalfway),5);
	}
	//
	float BeckmannDistribution(float Roughness,float NormalDotHalfway)
	{
		if(Roughness < 1e-4)
		{return 0.0;}
		//
		float PowSqr = NormalDotHalfway*NormalDotHalfway;
		float RoughnessSqr = Roughness * Roughness;
		float Exponent = (PowSqr - 1.0)/(RoughnessSqr * PowSqr);
		//
		return exp(Exponent)/(PI * RoughnessSqr * PowSqr*PowSqr);
	}
	//
	float TrowbridgeReitzDistribution(float Roughness,float NormalDotHalfway)
	{
		if(Roughness < 1e-4)
		{return 0.0;}
		//
		float PowSqr = NormalDotHalfway*NormalDotHalfway;
		float RoughnessSqr = Roughness * Roughness;
		float Tan = (1 - PowSqr) / PowSqr;
		return 1.0 / (PI * RoughnessSqr * (1 + Tan/(RoughnessSqr)));
	}
	//
	float HeavisideFunction(float a)
	{
		if(a > 0)
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	}
	//
	float SmithLambda(float Roughness,float CosAngle)
	{
		float CosSqr = CosAngle * CosAngle;
		float TgSqr = (1.0 - CosSqr)/CosSqr;
		//
		return (-1.0 + sqrt(1 + Roughness * Roughness * TgSqr)) / 2.0;
	}
	//
	float SmithVisibilityFunction(float Roughness,float CameraDotHalfway,float LightDotHalfway,float CameraDotNormal,float LightDotNormal)
	{
		return HeavisideFunction(CameraDotHalfway) * HeavisideFunction(LightDotHalfway) / (1 + SmithLambda(Roughness,CameraDotNormal) + SmithLambda(Roughness,LightDotNormal));
	}
	//
	float CalculateSurfaceRoughness(float Roughness,float NormalDotHalfway,float CameraDotHalfway,float LightDotHalfway,float CameraDotNormal,float LightDotNormal)
	{
		float D = BeckmannDistribution(Roughness,NormalDotHalfway);
		//float D = TrowbridgeReitzDistribution(Roughness,NormalDotHalfway);
		float G = SmithVisibilityFunction(Roughness,CameraDotHalfway,LightDotHalfway,CameraDotNormal,LightDotNormal);
		//
		return (D * G) / (4.0 * CameraDotNormal);
	}
	//
	vec3 CalculateNonMetallicContrib(vec3 DiffuseContrib,float Fresnel,float CameraDotHalfway,float SurfaceRoughness,float Reflectiveness)
	{
		float F = FresnelSchlick(vec3(Fresnel),CameraDotHalfway).x;
		float SpecularContrib = F * SurfaceRoughness;
		//
		return DiffuseContrib * (1.0 - Reflectiveness) + vec3(SpecularContrib) * Reflectiveness;
	}
	//
	vec3 CalculateMetallicContrib(vec3 Fresnel,float CameraDotHalfway,float SurfaceRoughness)
	{
		vec3 F = FresnelSchlick(Fresnel,CameraDotHalfway);
		return F * SurfaceRoughness;
	}
)";

const char *FragmentShaderSource = R"(
	#version 330
	//
	uniform vec4 CameraPosition;
	//
	const float PI = 3.14159265359;
	//
	uniform vec3 LightPosition = vec3(0.0,1800.0,500.0);
	uniform vec3 LightColor = vec3(1.0,1.0,1.0);
	uniform float LightIntensity = 15000000.0;
	//
	in vec4 fs_position;
	in vec4 fs_normal;
	//
	in vec4 fs_albedo;
	in float fs_fresnel;
	in float fs_roughness;
	in float fs_reflectiveness;
	in float fs_metallic;
	//
	out vec4 FragColor;
	//
	vec3 LambertianDiffuse(vec3 Diffuse,float LightDotNormal);
	//
	float CalculateSurfaceRoughness(float Roughness,float NormalDotHalfway,float CameraDotHalfway,float LightDotHalfway,float CameraDotNormal,float LightDotNormal);
	//
	vec3 CalculateNonMetallicContrib(vec3 DiffuseContrib,float Fresnel,float CameraDotHalfway,float SurfaceRoughness,float Reflectiveness);
	//
	vec3 CalculateMetallicContrib(vec3 Fresnel,float CameraDotHalfway,float SurfaceRoughness);
	//
	void main()
	{
		vec3 LightDirection = LightPosition - fs_position.xyz;
		float LightDistance = length(LightDirection);
		vec3 Radiance = LightColor * LightIntensity / (LightDistance*LightDistance);
		//
		LightDirection = normalize(LightDirection);
		vec3 CameraDirection = normalize(CameraPosition.xyz - fs_position.xyz);
		//
		float LightDotNormal = max(1e-4,dot(LightDirection,fs_normal.xyz));
		float CameraDotNormal = max(1e-4,dot(CameraDirection,fs_normal.xyz));
		//
		vec3 HalfwayVector = normalize(CameraDirection + LightDirection);
		//
		float CameraDotHalfway = max(1e-4,dot(CameraDirection,HalfwayVector));
		float LightDotHalfway = max(1e-4,dot(LightDirection,HalfwayVector));
		//
		float NormalDotHalfway = max(1e-4,dot(fs_normal.xyz,HalfwayVector));
		//
		/*
		 * Calculating diffuse
		 */
		vec3 DiffuseContrib = LambertianDiffuse(fs_albedo.rgb,LightDotNormal);
		//
		/*
		 * Calculating non metallic and metallic
		 */
		float SurfaceRoughness = CalculateSurfaceRoughness(fs_roughness,NormalDotHalfway,CameraDotHalfway,LightDotHalfway,CameraDotNormal,LightDotNormal);
		//
		vec3 NonMetallicContrib = CalculateNonMetallicContrib(DiffuseContrib,fs_fresnel,CameraDotHalfway,SurfaceRoughness,fs_reflectiveness);
		vec3 MetallicContrib = CalculateMetallicContrib(fs_albedo.rgb,CameraDotHalfway,SurfaceRoughness);
		//
		/*
		 * Adding it all up
		 */
		vec3 Contrib = NonMetallicContrib * (1.0 - fs_metallic) + MetallicContrib * fs_metallic;
		//
		FragColor = vec4(Contrib * Radiance,1.0);
		//FragColor = vec4(fs_albedo.rgb,1.0);
		//FragColor = vec4(fs_normal/2.0,1.0);
		//FragColor = vec4(CameraDirection,1.0);
		//FragColor = vec4(CameraDotHalfway,CameraDotNormal,0.0,1.0);
		//FragColor = vec4(1.0,0.0,0.0,1.0);
	}
)";
