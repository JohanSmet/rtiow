// raytrace/raytrace.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "raytrace.h"

#include "camera.h"
#include "geometry_spheres.h"
#include "ray.h"
#include "utils.h"
#include "scene.h"
#include "thread_pool.h"

#include <algorithm>
#include <cassert>
#include <chrono>

namespace rtiow {

RayTracer::RayTracer(const RayTracerConfig &config) : m_config(config) {
	assert(m_config.m_render_resolution_x > 0);
	assert(m_config.m_render_resolution_y > 0);
	m_output = std::make_unique<RGBBuffer>(m_config.m_render_resolution_x, m_config.m_render_resolution_y);
}

RayTracer::~RayTracer() {
}

void RayTracer::render(Scene &scene) {

	auto start_time = std::chrono::system_clock::now();

	// Creating and destroying the threadpool per render might seem like a bad idea, and it is.
	// But destroying the pool is currently the only way to wait until rendering is done :-(
	// And besides, render() isn't going to be called multiple times in the current setup
	auto num_workers = (m_config.m_num_render_workers > 0) ?
							m_config.m_num_render_workers :
							((ThreadPool::hardware_concurrency() - m_config.m_threads_ignore) * m_config.m_threads_use_percent) / 100;
	m_thread_pool = std::make_unique<ThreadPool>(std::max(1, num_workers));

	// setup camera
	Camera camera(m_output->width(), m_output->height());

	// setup rendering (e.g. precompute material values)
	scene.setup_rendering(m_config);

	// split scene into quads and render them in parallel
	constexpr uint32_t CHUNK_SIZE = 128;

	for (uint32_t y0 = 0; y0 < m_output->height(); y0 += CHUNK_SIZE) {
		for (uint32_t x0 = 0; x0 < m_output->width(); x0 += CHUNK_SIZE) {
			auto x1 = std::min(m_config.m_render_resolution_x, x0 + CHUNK_SIZE);
			auto y1 = std::min(m_config.m_render_resolution_y, y0 + CHUNK_SIZE);

			m_thread_pool->add_task([=, &scene, this] () {
				for (uint32_t y = y0; y < y1; ++y) {
					uint8_t *out = m_output->data() + (3 * (y * m_config.m_render_resolution_x + x0));

					for (uint32_t x = x0; x < x1; ++x) {

						color_t pixel_color(0.0f, 0.0f, 0.0f);

						for (uint32_t sample = 0; sample < m_config.m_samples_per_pixel; ++sample) {

							auto u = (static_cast<float>(x) + random_float()) / static_cast<float>(m_output->width() - 1);
							auto v = (static_cast<float>(y) + random_float()) / static_cast<float>(m_output->height() - 1);

							Ray ray = camera.create_ray(u, v);
							pixel_color += scene.ray_color(ray, sample, m_config.m_max_ray_bounces);
						}
						write_color(&out, pixel_color, m_config.m_samples_per_pixel);
					}
				}
			});
		}
	}

	// destroy the thread pool to wait for work to finish
	m_thread_pool = nullptr;

	auto finish_time = std::chrono::system_clock::now();
	std::printf("Rendering took %ld ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time).count());
}

} // namespace rtiow
