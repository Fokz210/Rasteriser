#ifndef OBJHPP
#define OBJHPP

#include "geometry.hpp"
#include <fstream>
#include <sstream>
#include <vector>

struct Mesh {
	struct vertex {
        vector3f pos;
        vector2f tex;
        vector3f norm;
	};

	using uint = unsigned;
    std::vector<vertex> verts;
    std::vector<uint> inds;
};

inline Mesh import_obj(char const *filename)
{
	std::ifstream in(filename);
	Mesh out;
	if (!in.is_open())
		throw std::invalid_argument("can not find file " + std::string(filename));
    std::string line;
    std::vector<vector3f> pos;
    std::vector<vector2f> tex;
    std::vector<vector3f> norm;
    while (std::getline(in, line)) {
		std::istringstream iss(line);
		std::string word;
		iss >> word;
		if (word == "v") {
            vector3f v;
			if (!(iss >> v.x >> v.y >> v.z))
				throw std::runtime_error("could not parse line: " + line);
			pos.push_back(v);
		} else if (word == "vt") {
            vector2f v;
			if (!(iss >> v.x >> v.y))
				throw std::runtime_error("could not parse line: " + line);
			tex.push_back(v);
		} else if (word == "vn") {
            vector3f v;
            if (!(iss >> v.x >> v.y >> v.z))
				throw std::runtime_error("could not parse line: " + line);
			norm.push_back(v);
		} else if (word == "f") {
			Mesh::uint idx[3];
			char trash;
			auto const vsize = out.verts.size();
			while (iss >> idx[0] >> trash >> idx[1] >> trash >> idx[2])
				out.verts.push_back({pos[idx[0] - 1], tex[idx[1] - 1], norm[idx[2] - 1]});
			auto const vcount = out.verts.size() - vsize;
			for (auto i = 1u; i < vcount - 1; i++) {
				out.inds.push_back(vsize);
				out.inds.push_back(vsize + i);
				out.inds.push_back(vsize + i + 1);
			}
		}
	}
	return out;
}

#endif //OBJHPP
