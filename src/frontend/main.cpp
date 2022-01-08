#include <chrono>
#include <initializer_list>
#include <thread>

#include <raytrace/raytrace.h>
#include <argh/argh.h>

#include "output_opengl.h"

static constexpr int MAX_FPS = 60;
static constexpr int MIN_FRAMETIME_MS = 1000 / MAX_FPS;

using argh_list_t = std::initializer_list<const char *const>;
static constexpr argh_list_t ARG_RESOLUTION_X = {"-x", "--resolution-x"};
static constexpr argh_list_t ARG_RESOLUTION_Y = {"-y", "--resolution-y"};
static constexpr argh_list_t ARG_SAMPLES_PER_PIXEL = {"-s", "--samples-per-pixel"};
static constexpr argh_list_t ARG_MAX_RAY_BOUNCES = {"-b", "--max-ray-bounces"};
static constexpr argh_list_t ARG_RENDER_WORKERS = {"-w", "--render-workers"};
static constexpr argh_list_t ARG_THREADS_IGNORE = {"--threads-ignore"};
static constexpr argh_list_t ARG_THREADS_PERCENT = {"--threads-percentage"};

int main(int argc, char *argv[]) {

	// parameter parsing
	argh::parser cmd_line;
	cmd_line.add_params(ARG_RESOLUTION_X);
	cmd_line.add_params(ARG_RESOLUTION_Y);
	cmd_line.add_params(ARG_SAMPLES_PER_PIXEL);
	cmd_line.add_params(ARG_MAX_RAY_BOUNCES);
	cmd_line.add_params(ARG_RENDER_WORKERS);
	cmd_line.add_params(ARG_THREADS_IGNORE);
	cmd_line.add_params(ARG_THREADS_PERCENT);
	cmd_line.parse(argc, argv);

	// fill out configuration structure
	rtiow::RayTracerConfig raytracer_config;

	cmd_line(ARG_RESOLUTION_X, raytracer_config.m_render_resolution_x) >> raytracer_config.m_render_resolution_x;
	cmd_line(ARG_RESOLUTION_Y, raytracer_config.m_render_resolution_y) >> raytracer_config.m_render_resolution_y;
	cmd_line(ARG_SAMPLES_PER_PIXEL, raytracer_config.m_samples_per_pixel) >> raytracer_config.m_samples_per_pixel;
	cmd_line(ARG_MAX_RAY_BOUNCES, raytracer_config.m_max_ray_bounces) >> raytracer_config.m_max_ray_bounces;
	cmd_line(ARG_RENDER_WORKERS, raytracer_config.m_num_render_workers) >> raytracer_config.m_num_render_workers;
	cmd_line(ARG_THREADS_IGNORE, raytracer_config.m_threads_ignore) >> raytracer_config.m_threads_ignore;
	cmd_line(ARG_THREADS_PERCENT, raytracer_config.m_threads_use_percent) >> raytracer_config.m_threads_use_percent;

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
