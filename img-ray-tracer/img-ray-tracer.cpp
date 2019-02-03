#include "pch.h"

#include "vec3f.h"

struct Ray {
	Vec3f origin, direction;

	Ray(const Vec3f &origin, const Vec3f &direction) : origin(origin), direction(direction) {}
};

struct Sphere {
	Vec3f center;
	float radius;

	Sphere(const Vec3f &center, const float &radius) : center(center), radius(radius) {}

	bool check_ray_intersection(const Ray& ray) const {
		constexpr float direction_error_threshold = 0.001;
		assert(ray.direction.length_squared() - 1.0f <= direction_error_threshold);
		const Vec3f relative_center = center - ray.origin;
		const float direction_center_dot_product = dot_product(ray.direction, center);

		if (direction_center_dot_product <= 0)
			// Sphere is behind the ray source
			return false;

		const Vec3f center_projection = direction_center_dot_product * ray.direction;
		const Vec3f center_rejection = center - center_projection;

		// If the rejection is smaller than the radius, the ray intersects the sphere
		return (center_rejection.length_squared() < radius * radius);
	}
};

void render(const Sphere &sphere) {
	constexpr int width = 1024;
	constexpr int height = 768;
	constexpr float fov = 2.5f;
	std::vector<Vec3f> framebuffer(width*height);

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float x = (2 * (i + 0.5) / (float)width - 1)*tan(fov / 2.)*width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1)*tan(fov / 2.);
			const Ray ray{ {0.f, 0.f, 0.f}, Vec3f(x, y, -1).unit_vector() };
			if (sphere.check_ray_intersection(ray))
				framebuffer[i + j * width] = { 1.f, 1.f, 1.f };
			else
				framebuffer[i + j * width] = { 0.f, 0.f, 0.f };
		}
	}

	std::ofstream ofs; // save the framebuffer to file
	ofs.open("./out.ppm");
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height*width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {
	Sphere sphere{ {0.f, 0.f, -2.f}, 1.f };
	render(sphere);
	return 0;
}