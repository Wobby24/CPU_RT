#pragma once

#include "util/image_loader.hpp"
#include "texture.hpp"
#include "util/color.hpp"

namespace Aero::RT::Textures {
    class image_texture : public texture {
    public:
        image_texture(const char* filename);

        Utils::color value(double u, double v, const Math::point3&) const override;
    private:
        Utils::imageLoader image_;
    };
}