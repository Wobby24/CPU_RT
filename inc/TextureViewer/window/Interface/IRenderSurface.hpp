#pragma once

#include "util/Vec.hpp"

namespace Aero::GL::Window {
    class IRenderSurface {
    public:
        virtual ~IRenderSurface() = default;

        virtual Vec2<int> GetSize() const = 0;
        virtual void Bind() = 0;   // make framebuffer current
        virtual void Present() = 0;   // end frame
    };
}