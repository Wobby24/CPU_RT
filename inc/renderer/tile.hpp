#pragma once

#include <vector>
#include <algorithm>

namespace Aero::RT::Renderer {
	struct tile {
        tile() {};
		int x_start = 0;
		int y_start = 0;
		int x_end = 0;
		int y_end = 0;
	};

    inline std::vector<tile> dispatchTiles(int width, int height, int tile_size) {
        std::vector<tile> buffer;

        // Start at 0 (the bottom) and step up by tile_size
        for (int j = 0; j < height; j += tile_size) {

            // Start at 0 (the left) and step right by tile_size
            for (int i = 0; i < width; i += tile_size) {
                tile t;

                t.x_start = i;
                t.y_start = j;

                // Clamp to screen boundaries so we don't overflow memory
                t.x_end = std::min(i + tile_size, width);
                t.y_end = std::min(j + tile_size, height);

                buffer.push_back(t);
            }
        }

        return buffer;
    }
}