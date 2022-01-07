#include <chrono>
#include <thread>

#include <raytrace/raytrace.h>
#include <argh/argh.h>

#include "output_opengl.h"

static constexpr int MAX_FPS = 60;
static constexpr int MIN_FRAMETIME_MS = 1000 / MAX_FPS;

static rtiow::RayTracerConfig raytracer_config;

int main() {


	// create scene
	rtiow::Scene scene;
	auto material_ground = scene.material_create(rtiow::color_t(0.8f, 0.8f, 0.0f), 0.0f);
	auto material_center = scene.material_create(rtiow::color_t(0.7f, 0.3f, 0.3f), 0.0f);
	auto material_left = scene.material_create(rtiow::color_t(0.8f, 0.8f, 0.8f), 1.0f);
	auto material_right = scene.material_create(rtiow::color_t(0.8f, 0.6f, 0.2f), 0.25f);

	scene.sphere_add(rtiow::point_t(0.0f, -100.5f, -1.0f), 100.0f, material_ground);
	scene.sphere_add(rtiow::point_t(0.0f, 0.0f, -1.0f), 0.5f, material_center);
	scene.sphere_add(rtiow::point_t(-1.0f, 0.0f, -1.0f), 0.5f, material_left);
	scene.sphere_add(rtiow::point_t(1.0f, 0.0f, -1.0f), 0.5f, material_right);

	// create output window
	rtiow::gui::OutputOpengl window;
	window.setup(int32_t(raytracer_config.m_render_resolution_x), int32_t(raytracer_config.m_render_resolution_y));


	// kick of renderer
	rtiow::RayTracer ray_tracer(raytracer_config);

	std::thread render_thread([&ray_tracer, &scene] {
		ray_tracer.render(scene);
	});

	while (!window.should_exit()) {

		auto clock_start = clock();

		window.display(ray_tracer.output_ptr());

		// fps limiter
		auto clock_end = clock();
		auto sleep_time = MIN_FRAMETIME_MS - ((clock_end - clock_start) * 1000) / CLOCKS_PER_SEC;
		if (sleep_time > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		}
	}

	window.teardown();
	exit(EXIT_SUCCESS);
}
