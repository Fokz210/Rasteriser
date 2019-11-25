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
          pool(),
          mutexes(new std::mutex[1080])
    {
	}

	~pipeline()
    {
	}

    virtual void run(const Mesh & mesh);

protected:


	context *c;
	fragmentShader *fShader;
	vertexShader *vShader;
    TriangleRasterizer *trRast;
    thread_pool pool;

    std::mutex *mutexes;
};

void pipeline::run(const Mesh & mesh)
{
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

                trRast->rasterize(p, v, out);

				for (TriangleRasterizer::output el : out) {
                    if (el.depth > c->zbuffer[el.y * 1920 + el.x] || el.depth < -1.f)
                        continue;

					Mesh::vertex v0 = mix(v, el.b, el.c);

					color col = fShader->fragment(v0);
					{
						std::unique_lock<std::mutex> l(mutexes[el.y]);

                        c->zbuffer[el.y * 1920 + el.x] = el.depth;
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


