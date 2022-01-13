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
#include <cinttypes>

namespace rtiow {

RayTracer::RayTracer(const RayTracerConfig &config) : m_config(config) {
	assert(m_config.m_render_resolution_x > 0);
	assert(m_config.m_render_resolution_y > 0);
	m_output = std::make_unique<RGBBuffer>(m_config.m_render_resolution_x, m_config.m_render_resolution_y);
}

RayTracer::~RayTracer() {
}

static inline color_t environment_color(const Ray &ray) {
	auto unit_direction = glm::normalize(ray.direction());
	auto t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f-t) * color_t(1.0f, 1.0f, 1.0f) + t * color_t(0.5f, 0.7f, 1.0f);
}

static inline float reflectance(float cosine, float ref_idx) {
	// Use Schlick's approximation for reflectance.
	auto r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * glm::pow((1.0f - cosine), 5.0f);
}

static inline color_t specular_reflect(Ray &ray, const HitRecord &hit, const Material &mat) {

	auto reflected_dir = glm::reflect(ray.direction(), hit.m_normal);
	ray = Ray(hit.m_point, glm::normalize(reflected_dir + mat.m_specular_roughness * random_vector_in_unit_sphere()));

	if (glm::dot(ray.direction(), hit.m_normal) > 0) {
		return mat.m_specular_color;
	} else {
		return {0.0f, 0.0f, 0.0f};
	}

}

color_t ray_color(const Scene &scene, const Ray &start_ray, int32_t max_ray_bounces) {

	auto result = color_t{0.0f, 0.0f, 0.0f};
	auto attenuation = color_t{1.0f, 1.0f, 1.0f};
	auto ray = start_ray;

	for (int32_t bounce = 0; bounce <= max_ray_bounces; ++bounce) {

		// shoot the ray into the scene
		HitRecord hit;

		// stop tracing if the ray didn't hit anything
		if (!scene.hit_detection(ray, hit)) {
			result += environment_color(ray) * attenuation;
			break;
		}

		auto mat = scene.material(hit.m_material);

		// absorption if hit is from the inside of the object
		if (!hit.m_front_face) {
			attenuation *= glm::exp(-mat.m_refraction_color * hit.m_at_t);
		}

		// decide how this ray will be reflected
		float ray_probability = 1.0f;
		bool  choose_specular = false;
		bool  choose_refraction = false;

		float random_chance = random_float();

		if (random_chance < mat.m_specular_chance) {
			choose_specular = true;
			ray_probability = mat.m_specular_chance;
		} else if (random_chance < mat.m_specular_chance + mat.m_refraction_chance) {
			choose_refraction = true;
			ray_probability = mat.m_refraction_chance;
		} else {
			ray_probability = 1.0f - mat.m_specular_chance - mat.m_refraction_chance;
		}
		ray_probability = glm::max(ray_probability, 0.001f);

		if (choose_specular) {
			// specular reflection
			attenuation *= specular_reflect(ray, hit, mat);
		} else if (choose_refraction) {
			// refraction
			float refraction_ratio = hit.m_front_face ? 1.0f / mat.m_index_of_refraction : mat.m_index_of_refraction;
			float cos_theta = glm::min(glm::dot(-ray.direction(), hit.m_normal), 1.0f);
            float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);

            if (refraction_ratio * sin_theta > 1.0f || reflectance(cos_theta, refraction_ratio) > ray_probability) {
				// refraction not possible or looking at steep angle so material becomes reflective
				attenuation *= specular_reflect(ray, hit, mat);
			} else {
				auto refraction_dir = glm::refract(ray.direction(), hit.m_normal, refraction_ratio);
				ray = Ray(hit.m_point, glm::normalize(refraction_dir + mat.m_refraction_roughness * random_vector_in_unit_sphere()));
			}
		} else {
			// diffuse reflection
			auto diffuse_dir = glm::normalize(hit.m_normal + random_unit_vector());
			if (glm::all(glm::epsilonEqual(diffuse_dir, vector_t(0.0f, 0.0f, 0.0f), 1e-6f))) {
				diffuse_dir = hit.m_normal;
			}
			ray = Ray(hit.m_point, diffuse_dir);
			attenuation *= mat.m_albedo;
		}

		// divide attentuation by the probability that this ray-type was chosen to make sure
		// they count the same in the final average
		attenuation /= ray_probability;
	}

	return result;
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
							pixel_color += ray_color(scene, ray, m_config.m_max_ray_bounces);
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
	std::printf("Rendering took %" PRIu64 "ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time).count());
}

} // namespace rtiow
