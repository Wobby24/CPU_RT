#pragma once
#include <scene/scene.hpp>
#include <renderer/renderOptions.hpp>

#include <nlohmann/json.hpp>
#include <string>

namespace Aero::RT::Utils {
    class sceneJSONReader {
    public:
        sceneJSONReader() = default;
        ~sceneJSONReader() = default;

        void extractSceneFromJSON(const std::string& jsonPath, Scene::scene&, Aero::RT::renderOptions& options);
    private:
        std::string jsonPath_;
        std::vector<Scene::sceneObject> extractSceneObjects();
        void extractRenderOptions(Aero::RT::renderOptions& options);
        Camera::camera extractCamera();
        nlohmann::json data;
    };
}