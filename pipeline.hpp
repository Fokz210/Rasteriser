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
    pipeline(context *c_ptr, fragmentShader *frag_ptr, vertexShader *vert_ptr, TriangleRasterizer *rast_ptr)
		: c(c_ptr),
		  fShader(frag_ptr),
		  vShader(vert_ptr),
          trRast(rast_ptr),
          depth(new float[static_cast<size_t>(c->width() * c->height())]),
          pool(),
          mutexes(new std::mutex[1080]),
          camPos(),
          faces_out (true)
	{
	}

	~pipeline()
	{
		delete[] depth;
		delete[] mutexes;
	}

    virtual void run(const Mesh & mesh);

    vector3f camPos;
    vector3f localPos;
    bool faces_out;
protected:
	Mesh::vertex mix(Mesh::vertex const v[3], float const b, float const c);

	context *c;
	fragmentShader *fShader;
	vertexShader *vShader;
    TriangleRasterizer *trRast;
	float *depth;
    thread_pool pool;

	std::mutex *mutexes;
};

void pipeline::run(const Mesh & mesh)
{
    for (auto y = 0u; y < c->height(); y++)
        for (auto x = 0u; x < c->width(); x++)
            depth[static_cast<int>(y * c->width() + x)] = 1.f;

    pool.start_threads(8);

    int tasksNum = static_cast<int>(pool.workers.size() * 20);

    for (unsigned start = 0u; start < mesh.inds.size(); start += mesh.inds.size() / static_cast<unsigned>(tasksNum)) {
		auto work = [&, start]() {
            std::vector<TriangleRasterizer::output> out;
            for (unsigned i = start * 3; i < start * 3 + mesh.inds.size() / static_cast<unsigned>(tasksNum) * 3 && i < mesh.inds.size(); i += 3) {

				Mesh::vertex const v[3] = {
                    mesh.verts[mesh.inds[i]],
                    mesh.verts[mesh.inds[i + 1]],
                    mesh.verts[mesh.inds[i + 2]]};

                vector4f p[3] = {};

				for (int i = 0; i < 3; ++i)
					p[i] = vShader->vertex(v[i]);

                trRast->rasterize(p, out);

				for (TriangleRasterizer::output el : out) {
                    if (el.depth > depth[el.y * 1920 + el.x] || el.depth < -1.f)
                        continue;

					Mesh::vertex v0 = mix(v, el.b, el.c);

                    if ((faces_out ? 1 : -1) * dot (v0.norm, camPos - localPos - v0.pos) < 0)
                        continue;

					color col = fShader->fragment(v0);
					{
						std::unique_lock<std::mutex> l(mutexes[el.y]);
						depth[el.y * 1920 + el.x] = el.depth;


                        (*c)[static_cast<unsigned>(el.y)][static_cast<unsigned>(el.x)] = col;
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
