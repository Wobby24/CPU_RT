#include "textures/image_texture.hpp"
#include "math/interval.hpp"
#include "math/vec3.hpp"
#include "util/color.hpp"

namespace Aero::RT::Textures {
    image_texture::image_texture(const char* filename) : image_(filename) {}

    Utils::color image_texture::value(double u, double v, const Math::point3&) const {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image_.height() <= 0) return Utils::color(0, 1, 1);

        // Clamp input texture coordinates to [0, 1] x [1, 0]
        u = Math::interval(0, 1).clamp(u);
        v = 1.0 - Math::interval(0, 1).clamp(v); // Flip V to image coordinates

        auto i = int(u * image_.width());
        auto j = int(v * image_.height());
        auto pixel = image_.pixel_data(i, j);

        auto color_scale = 1.0 / 255.0; 
        return Utils::color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }
}