#pragma once

#include <cstdio>
#include <iostream>
#include <vector>

#include "context.hpp"
#include "shader.hpp"
#include "trirast.hpp"

#include "threadpool.hpp"

class pipeline {
public:
	pipeline(context *c_ptr, fragmentShader *frag_ptr, vertexShader *vert_ptr, TriangleRasterizer *rast_ptr, Mesh *mesh_ptr, thread_pool *_pool)
		: c(c_ptr),
		  fShader(frag_ptr),
		  vShader(vert_ptr),
		  trRast(rast_ptr),
		  mesh(mesh_ptr),
		  depth(new float[c->_width * c->_height]),
          pool(),
		  mutexes(new std::mutex[1080])
	{
	}

	~pipeline()
	{
		delete[] depth;
		delete[] mutexes;
	}

	virtual void run();

protected:
	Mesh::vertex mix(Mesh::vertex const v[3], float const b, float const c);

	context *c;
	fragmentShader *fShader;
	vertexShader *vShader;
    TriangleRasterizer *trRast;
	Mesh *mesh;
	float *depth;
    thread_pool pool;

	std::mutex *mutexes;
};

void pipeline::run()
{
	for (auto y = 0u; y < c->_height; y++)
		for (auto x = 0u; x < c->_width; x++)
            depth[y * c->_width + x] = 1.f;

    pool.start_threads(8);

    int tasksNum = pool.workers.size() * 20;

	for (unsigned start = 0u; start < mesh->inds.size(); start += mesh->inds.size() / tasksNum) {
		auto work = [&, start]() {
			for (unsigned i = start * 3; i < start * 3 + mesh->inds.size() / tasksNum * 3 && i < mesh->inds.size(); i += 3) {

				std::vector<TriangleRasterizer::output> out;

				Mesh::vertex const v[3] = {
                    mesh->verts[mesh->inds[i]],
                    mesh->verts[mesh->inds[i + 1]],
                    mesh->verts[mesh->inds[i + 2]]};

				vector4f p[3] = {};

				for (int i = 0; i < 3; ++i)
					p[i] = vShader->vertex(v[i]);

                trRast->rasterize(p, out);

				for (TriangleRasterizer::output el : out) {
                    if (el.depth > depth[el.y * 1920 + el.x] || el.depth < -1.f)
						continue;

					Mesh::vertex v0 = mix(v, el.b, el.c);

					color col = fShader->fragment(v0);

					{
						std::unique_lock<std::mutex> l(mutexes[el.y]);
						depth[el.y * 1920 + el.x] = el.depth;
						(*c)[el.y][el.x] = col;
					}
				}

				out.clear();
			}
		};

        pool.queue(work);
	}

    pool.finish();
}

Mesh::vertex pipeline::mix(Mesh::vertex const v[3], float const b, float const c)
{
	float const a = 1.f - b - c;

	Mesh::vertex out;
	float *f = reinterpret_cast<float *>(&out);
	float const *const f0 = reinterpret_cast<float const *>(v);
	float const *const f1 = reinterpret_cast<float const *>(v + 1);
	float const *const f2 = reinterpret_cast<float const *>(v + 2);
	for (unsigned int i = 0u; i < sizeof(Mesh::vertex) / sizeof(float); ++i)
		f[i] = a * f0[i] + b * f1[i] + c * f2[i];
	return out;
}
