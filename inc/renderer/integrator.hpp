#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"

// so i think we should pass a raypacket to the function, then have it return a pack of colors, and we just for loop and compute it all in here.
namespace Aero::RT::Renderer {
	Utils::color ray_color(const Math::ray& r, int depth, const Geometry::hittable& world, Utils::color backgroundColor) {
		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
			return Utils::color(0, 0, 0);

		Geometry::hit_record rec;
		
		// If the ray hits nothing, return background color
		if (!world.hit(r, Math::interval(0.001, infinity), rec)) 
			return backgroundColor;
		
		Math::ray scattered;
		Utils::color attenuation;
		Utils::color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

		if (!rec.mat->scatter(r, rec, attenuation, scattered))
			return color_from_emission;

		Utils::color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, backgroundColor);

		return color_from_emission + color_from_scatter;
	}
}