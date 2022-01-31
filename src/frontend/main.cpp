#include <chrono>
#include <initializer_list>
#include <thread>
#include <cstdio>

#include <raytrace/raytrace.h>
#include <raytrace/utils.h>
#include <argh/argh.h>

#include "output_opengl.h"

static constexpr int MAX_FPS = 60;
static constexpr int MIN_FRAMETIME_MS = 1000 / MAX_FPS;

// command line arguments
using argh_list_t = std::initializer_list<const char *const>;
static constexpr argh_list_t ARG_RESOLUTION_X = {"-x", "--resolution-x"};
static constexpr argh_list_t ARG_RESOLUTION_Y = {"-y", "--resolution-y"};
static constexpr argh_list_t ARG_SAMPLES_PER_PIXEL = {"-s", "--samples-per-pixel"};
static constexpr argh_list_t ARG_MAX_RAY_BOUNCES = {"-b", "--max-ray-bounces"};
static constexpr argh_list_t ARG_RENDER_WORKERS = {"-w", "--render-workers"};
static constexpr argh_list_t ARG_THREADS_IGNORE = {"--threads-ignore"};
static constexpr argh_list_t ARG_THREADS_PERCENT = {"--threads-percentage"};
static constexpr argh_list_t ARG_SCENE = {"--scene"};
static constexpr argh_list_t ARG_HELP = {"-h", "--help"};

static rtiow::RayTracerConfig raytracer_config;

namespace rtiow {

void construct_scene_01(Scene &scene) {

	auto material_ground = scene.material_create_diffuse({0.8f, 0.8f, 0.0f});
	auto material_center = scene.material_create_diffuse({0.7f, 0.3f, 0.3f});
	auto material_left   = scene.material_create(	{0.8f, 0.8f, 0.8f},							// albedo
													0.0f, {1.0f, 1.0f, 1.0f}, 0.01f,			// specular
													1.5f, 1.00f, {1.0f, 1.0f, 1.0f}, 0.01f);	// refraction
	auto material_right  = scene.material_create_specular({0.8f, 0.6f, 0.2f}, 1.00f, {0.8f, 0.6f, 0.2f}, 0.20f);

	scene.sphere_add({ 0.0f, -100.5f, -1.0f}, 100.0f, material_ground);
	scene.sphere_add({ 0.0f,    0.0f, -1.0f},   0.5f, material_center);
	scene.sphere_add({-1.0f,    0.0f, -1.0f},   0.5f, material_left);
	scene.sphere_add({-1.0f,    0.0f, -1.0f},  -0.45f, material_left);
	scene.sphere_add({ 1.0f,    0.0f, -1.0f},   0.5f, material_right);

	scene.setup_camera( float(raytracer_config.m_render_resolution_x) / float(raytracer_config.m_render_resolution_y),
						20.0f,
						{3.0f, 3.0f, 2.0f},
						{0.0f, 0.0f, -1.0f},
						{0.0f, 1.0f, 0.0f},
						1.0f
	);
}

void construct_scene_02(Scene &scene) {

	auto mat_ground = scene.material_create_diffuse({0.5f, 0.5f, 0.5f});
    scene.sphere_add({0.0f, -1000.0f, 0.0f}, 1000.0f, mat_ground);

    for (float a = -11; a < 11; a++) {
        for (float b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            point_t center(a + 0.9f*random_float(), 0.2f, b + 0.9f * random_float());

            if ((center - point_t(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8f) {
                    // diffuse
                    auto albedo = random_vector() * random_vector();
					auto mat_sphere = scene.material_create_diffuse(albedo);
					scene.sphere_add(center, 0.2f, mat_sphere);
                } else if (choose_mat < 0.95f) {
                    // metal
                    auto albedo = random_vector(0.5f, 1.0f);
                    auto fuzz = random_float(0.0f, 0.5f);
					auto mat_sphere = scene.material_create_specular(albedo, 1.0f, {1.0f, 1.0f, 1.0f}, fuzz);
					scene.sphere_add(center, 0.2f, mat_sphere);
                } else {
                    // glass
					auto mat_sphere = scene.material_create({0.0f, 0.0, 0.0f},							// albedo,
															0.0f, {1.0f, 1.0f, 1.0f}, 0.00f,			// specular
															1.5f, 1.0f, {1.0f, 1.0f, 1.0f}, 0.00f);		// refraction
					scene.sphere_add(center, 0.2f, mat_sphere);
                }
            }
        }
    }

	auto material_1 = scene.material_create({0.0f, 0.0, 0.0f},							// albedo,
										   0.0f, {1.0f, 1.0f, 1.0f}, 0.00f,				// specular
										   1.5f, 1.0f, {1.0f, 1.0f, 1.0f}, 0.00f);		// refraction
    scene.sphere_add({0.0f, 1.0f, 0.0f}, 1.0f, material_1);

	auto material_2 = scene.material_create_diffuse({0.4f, 0.2f, 0.1f});
    scene.sphere_add({-4.0f, 1.0f, 0.0f}, 1.0f, material_2);

	auto material_3 = scene.material_create_specular({0.7f, 0.6f, 0.5f}, 1.0f, {0.7f, 0.6f, 0.5f}, 0.0f);
    scene.sphere_add({4.0f, 1.0f, 0.0f}, 1.0f, material_3);

	scene.setup_camera( float(raytracer_config.m_render_resolution_x) / float(raytracer_config.m_render_resolution_y),
						20.0f,
						{13.0f, 2.0f, 3.0f},
						{0.0f, 0.0f, 0.0f},
						{0.0f, 1.0f, 0.0f},
						0.1f
	);
}

std::string format_argh_list(const argh_list_t &args) {
	std::string result;
	const char *sepa = "";

	for (const auto &a : args) {
		result.append(sepa);
		result.append(a);
		sepa = ", ";
	}

	return result;
}

void print_help() {
	printf("Usage:\n\n");
	printf("rtiow_gl [options]\n\n");
	printf("Options\n");
	printf(" %-25s set horizontal resolution in pixels of output (%d)\n",
			format_argh_list(ARG_RESOLUTION_X).c_str(), raytracer_config.m_render_resolution_x);
	printf(" %-25s set vertical resolution in pixels of output (%d)\n",
			format_argh_list(ARG_RESOLUTION_Y).c_str(), raytracer_config.m_render_resolution_y);
	printf(" %-25s number of sample points per pixel (%d)\n",
			format_argh_list(ARG_SAMPLES_PER_PIXEL).c_str(), raytracer_config.m_samples_per_pixel);
	printf(" %-25s maximum number of ray-bounces (%d)\n",
			format_argh_list(ARG_MAX_RAY_BOUNCES).c_str(), raytracer_config.m_max_ray_bounces);
	printf(" %-25s id of the scene to render [(1),2]\n", format_argh_list(ARG_SCENE).c_str());
	printf(" %-25s manually set number of render threads (0 = use available hardware threads)\n",
			format_argh_list(ARG_RENDER_WORKERS).c_str());
	printf(" %-25s number of hardware threads to ignore and leave available for others (%d)\n",
			format_argh_list(ARG_THREADS_IGNORE).c_str(), raytracer_config.m_threads_ignore);
	printf(" %-25s percentage of available (non-ignored) hardware threads to use (%d)\n",
			format_argh_list(ARG_THREADS_PERCENT).c_str(), raytracer_config.m_threads_use_percent);
}

} // namespace rtiow

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
	cmd_line.add_params(ARG_SCENE);
	cmd_line.add_params(ARG_HELP);
	cmd_line.parse(argc, argv);

	if (cmd_line[ARG_HELP]) {
		rtiow::print_help();
		exit(EXIT_SUCCESS);
	}

	// fill out configuration structure
	cmd_line(ARG_RESOLUTION_X, raytracer_config.m_render_resolution_x) >> raytracer_config.m_render_resolution_x;
	cmd_line(ARG_RESOLUTION_Y, raytracer_config.m_render_resolution_y) >> raytracer_config.m_render_resolution_y;
	cmd_line(ARG_SAMPLES_PER_PIXEL, raytracer_config.m_samples_per_pixel) >> raytracer_config.m_samples_per_pixel;
	cmd_line(ARG_MAX_RAY_BOUNCES, raytracer_config.m_max_ray_bounces) >> raytracer_config.m_max_ray_bounces;
	cmd_line(ARG_RENDER_WORKERS, raytracer_config.m_num_render_workers) >> raytracer_config.m_num_render_workers;
	cmd_line(ARG_THREADS_IGNORE, raytracer_config.m_threads_ignore) >> raytracer_config.m_threads_ignore;
	cmd_line(ARG_THREADS_PERCENT, raytracer_config.m_threads_use_percent) >> raytracer_config.m_threads_use_percent;

	int choose_scene;
	cmd_line(ARG_SCENE, 1) >> choose_scene;

	// create scene
	rtiow::Scene scene;

	if (choose_scene == 2) {
		rtiow::construct_scene_02(scene);
	} else {
		rtiow::construct_scene_01(scene);
	}

	// create output window
	rtiow::gui::OutputOpenGL window;
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
