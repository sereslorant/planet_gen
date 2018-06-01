#ifndef NOISE_UTILS_H
#define NOISE_UTILS_H

#include <vector>
#include <random>

#include <functional>

namespace NoiseUtils
{

void GenerateGaussianNoise(std::vector<float> &gauss_map,unsigned int resolution,unsigned int seed = std::mt19937::default_seed)
{
	gauss_map.resize(resolution * resolution);
	
	std::mt19937 mt_random_device;
	mt_random_device.seed(seed);
	std::normal_distribution<float> normal_dist;
	
	for(int i=0;i < resolution * resolution;i++)
	{
		gauss_map[i] = normal_dist(mt_random_device);
	}
}

void GenerateCovarianceMatrix(std::vector<float> &covariance_matrix_data,unsigned int resolution,std::function<float(const vec4&,const vec4&)> covariance_function)
{
	covariance_matrix_data.resize(resolution * resolution);
	
	//vec4 first_vec = {1.0f/resolution,1.0f/resolution};
	vec4 first_vec = {0.5,0.5};
	
	for(int i=0;i < resolution;i++)
	{
		for(int j=0;j < resolution;j++)
		{
			vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
			
			covariance_matrix_data[j*resolution + i] = covariance_function(current_vec,first_vec);
		}
	}
}

}

#endif // NOISE_UTILS_H
