#pragma once
#include <scene/scene.hpp>
#include <renderer/renderOptions.hpp>

// geometry
#include <geometry//quad.hpp>
#include <geometry/sphere.hpp>
#include <geometry/triangle.hpp>

// materials
#include <materials/diffuse_light.hpp>
#include <materials/isotropic.hpp>
#include <materials/lambertian.hpp>
#include <materials/metal.hpp>
#include <materials/dielectric.hpp>

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
        // parse each geometry implementation
        Geometry::quad parseQuad();
        Geometry::sphere parseSphere();
        Geometry::triangle parseTriangle();

        // parse each material implementation
        Materials::dielectric parseDielectric();
        Materials::metal parseMetal();
        Materials::diffuse_light parseDiffuseLight();
        Materials::lambertian parseLambertian();
        Materials::isotropic parseIsotropic();

        void extractRenderOptions(Aero::RT::renderOptions& options);
        std::vector<Camera::camera> extractCameras(Scene::scene& scene);
        nlohmann::json data;
    };
}