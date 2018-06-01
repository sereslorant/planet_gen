#ifndef LINE_STRIP_H
#define LINE_STRIP_H

#include <glmath.hpp>

#include <list>
#include <vector>

struct Vertex
{
	vec4 position;
	float height;
	float width;
	
	Vertex(const vec4 &p_position,float p_height,float p_width)
		:position(p_position),height(p_height),width(p_width)
	{}
};

class LineStrip
{
private:
	std::vector<Vertex> vertex_list;
	float end_len = 0.15;
	
public:
	
	float GetTotalLength()
	{
		float total_length = 2.0*end_len;
		
		auto front_iterator = vertex_list.begin();
		Vertex *previous = &(*front_iterator++);
		
		for(auto I = front_iterator;I != vertex_list.end();I++)
		{
			Vertex *current = &(*I);
			
			vec4 segment_vec = current->position - previous->position;
			total_length += length(segment_vec);
			
			previous = current;
		}
		
		return total_length;
	}
	
	Vertex &PushFrontPoint(const vec4 &position,float height,float width)
	{
		//vertex_list.emplace_front(position,height,width);
		vertex_list.insert(vertex_list.begin(),{position,height,width});
		return vertex_list.front();
	}
	
	Vertex &GetFront()
	{
		return vertex_list.front();
	}
	
	Vertex &PushBackPoint(const vec4 &position,float height,float width)
	{
		vertex_list.emplace_back(position,height,width);
		return vertex_list.back();
	}
	
	Vertex &GetBack()
	{
		return vertex_list.back();
	}
	
	Vertex &GetPoint(unsigned int id)
	{
		return vertex_list[id];
	}
	
	void RemovePoint(unsigned int id)
	{
		vertex_list.erase(vertex_list.begin() + id);
	}
	
	void ExtractCtrlPoints(std::vector<vec4> &vertices,std::vector<vec4> &velocities,std::vector<unsigned int> &indices) const
	{
		auto front_iterator = vertex_list.cbegin();
		
		const Vertex &first = *(front_iterator++);
		const Vertex &second = *(front_iterator);
		
		vec4 front_vel = second.position - first.position;
		vec4 front_dir = normalize(front_vel);
		
		vertices.push_back(first.position - front_dir * end_len);
		vertices.back()[2] = 0.0;
		vertices.back()[3] = end_len;
		//velocities.push_back(front_dir * end_len);
		velocities.push_back(front_dir * end_len);
		velocities.back()[2] = first.height;
		velocities.back()[3] = first.width - end_len;
		
		vertices.push_back(first.position);
		vertices.back()[2] = first.height;
		vertices.back()[3] = first.width;
		//velocities.push_back(front_vel);
		velocities.push_back((front_vel + front_dir * end_len) / 2.0);
		velocities.back()[2] = second.height - first.height;
		velocities.back()[3] = second.width - first.width;
		
		for(auto prev_iterator = std::prev(front_iterator),next_iterator = std::next(front_iterator);
			next_iterator != vertex_list.end();
			prev_iterator++,front_iterator++,next_iterator++
		)
		{
			const Vertex &previous = *prev_iterator;
			const Vertex &current = *front_iterator;
			const Vertex &next = *next_iterator;
			
			vec4 direction = (next.position - previous.position);
			
			vertices.push_back(current.position);
			vertices.back()[2] = current.height;
			vertices.back()[3] = current.width;
			velocities.push_back(direction/2.0);
			velocities.back()[2] = next.height - current.height;
			velocities.back()[3] = next.width - current.width;
		}
		
		const Vertex &last = vertex_list.back();
		const Vertex &almost_last = *(++vertex_list.rbegin());
		
		vec4 back_vel = last.position - almost_last.position;
		vec4 back_dir = normalize(back_vel);
		
		vertices.push_back(last.position);
		vertices.back()[2] = last.height;
		vertices.back()[3] = last.width;
		//velocities.push_back(back_vel);
		velocities.push_back((back_vel + back_dir * end_len) / 2.0);
		velocities.back()[2] = -1.0 * last.height;
		velocities.back()[3] = end_len - last.width;
		
		vertices.push_back(last.position + back_dir * end_len);
		vertices.back()[2] = 0.0;
		vertices.back()[3] = end_len;
		velocities.push_back(back_dir * end_len);
		velocities.back()[2] = 0.0;
		velocities.back()[3] = 0.0;
		
		for(int i=0;i < vertices.size() - 1;i++)
		{
			indices.push_back(i);
			indices.push_back(i+1);
		}
	}
	
	void ExtractMesh(std::vector<vec4> &vertices,std::vector<vec4> &properties,std::vector<unsigned int> &indices)
	{
		float total_length = GetTotalLength();
		float current_dist = end_len;
		
		auto front_iterator = vertex_list.begin();
		
		Vertex &first = *(front_iterator++);
		Vertex &second = *(front_iterator);
		
		vec4 front_dir = normalize(first.position - second.position);
		vec4 front_side_dir = normalize(vec4{front_dir[1],-front_dir[0]});
		
		vertices.push_back(first.position + front_dir * end_len);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5,0.0});
		
		vertices.push_back(first.position + front_side_dir * first.width);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5f - first.width/total_length,current_dist/total_length});
		
		vertices.push_back(first.position - front_side_dir * first.width);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5f + first.width/total_length,current_dist/total_length});
		
		vertices.push_back(first.position);
		vertices.back()[2] = first.height;
		properties.push_back({0.5f,current_dist/total_length});
		
		unsigned int i = 2;
		for(auto prev_iterator = std::prev(front_iterator),next_iterator = std::next(front_iterator);
			next_iterator != vertex_list.end();
			prev_iterator++,front_iterator++,next_iterator++
		)
		{
			Vertex &previous = *prev_iterator;
			Vertex &current = *front_iterator;
			Vertex &next = *next_iterator;
			
			vec4 direction = (next.position - previous.position);
			vec4 side_dir = normalize(vec4{-direction[1],direction[0]});
			
			vec4 segment_vec = current.position - previous.position;
			current_dist += length(segment_vec);
			
			vertices.push_back(current.position + side_dir * current.width);
			vertices.back()[2] = 0.0;
			properties.push_back({0.5f - current.width/total_length,current_dist/total_length});
			
			vertices.push_back(current.position - side_dir * current.width);
			vertices.back()[2] = 0.0;
			properties.push_back({0.5f + current.width/total_length,current_dist/total_length});
			
			vertices.push_back(current.position);
			vertices.back()[2] = current.height;
			properties.push_back({0.5f,current_dist/total_length});
			
			i++;
		}
		
		Vertex &last = vertex_list.back();
		Vertex &almost_last = *(++vertex_list.rbegin());
		
		vec4 segment_vec = last.position - almost_last.position;
		current_dist += length(segment_vec);
		
		vec4 back_dir = normalize(last.position - almost_last.position);
		vec4 back_side_dir = normalize(vec4{-back_dir[1],back_dir[0]});
		
		vertices.push_back(last.position + back_side_dir * last.width);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5f - last.width/total_length,current_dist/total_length});
		
		vertices.push_back(last.position - back_side_dir * last.width);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5f + last.width/total_length,current_dist/total_length});
		
		vertices.push_back(last.position);
		vertices.back()[2] = last.height;
		properties.push_back({0.5f,current_dist/total_length});
		
		current_dist += end_len;
		
		vertices.push_back(last.position + back_dir * end_len);
		vertices.back()[2] = 0.0;
		properties.push_back({0.5,current_dist/total_length});
		
		indices.push_back(0);
		
		for(int i=0;i < vertex_list.size();i++)
		{
			indices.push_back(1 + i*3 + 0);
			indices.push_back(1 + i*3 + 2);
		}
		
		indices.push_back(1 + (vertex_list.size() - 1)*3 + 3);
		
		indices.push_back(1 + (vertex_list.size() - 1)*3 + 3);
		
		for(int i=vertex_list.size() - 1;i >= 0;i--)
		{
			indices.push_back(1 + i*3 + 1);
			indices.push_back(1 + i*3 + 2);
		}
		
		indices.push_back(0);
	}
};

#endif // LINE_STRIP_H
