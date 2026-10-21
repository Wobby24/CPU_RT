#include <util/readJSON_Scene.hpp>
#include <fstream>
#include <string>
#include <scene/scene.hpp>
#include <scene/sceneObject.hpp>
#include <nlohmann/json.hpp>
#include <renderer/renderOptions.hpp>
#include <Geometry/sphere.hpp>
#include <Geometry/quad.hpp>
#include <materials/dielectric.hpp>
#include <materials/dielectric.hpp>
#include <materials/diffuse_light.hpp>
#include <materials/isotropic.hpp>
#include <materials/lambertian.hpp>
#include <materials/metal.hpp>
#include "util/Logger.hpp"

// replace all of the runtime errors with the aero assert, as it uses our logger and can assert with it

namespace Aero::RT::Utils {
    void sceneJSONReader::extractSceneFromJSON(const std::string &jsonPath, Scene::scene &scene, Aero::RT::renderOptions& options) {
        jsonPath_ = jsonPath;
        // first, import file and have json parse it
        std::ifstream file(jsonPath);
        data = nlohmann::json::parse(file);

        if (data.contains("aero_rt_scene") == false) {
            throw std::runtime_error("JSON file does not contain a scene entry! Did you use 'aero_rt_scene'?");
        }
        else {
           // scene = Scene::scene(extractSceneObjects());
        }
    }

    std::vector<Scene::sceneObject> sceneJSONReader::extractSceneObjects() {
        // loop over objects array in the json and construct proper scene objects
        if (data["aero_rt_scene"].contains("objects")) {
            auto objects = data["aero_rt_scene"]["objects"];

            for (auto obj : objects) {
                // first, we check for the position
                Math::point3 pos;
                int i = 0;
                for (auto& n : objects["position"]) {
                    pos[i] = n;
                    i++;
                }
                // for (int i = 0; i < 3; i++) {
                //  pos[i] = objects["position"][i]; // idk if this will work
                //}
                // extract mat properties
                std::shared_ptr<Materials::material> mat;

                // continue by inside of each parsing function checking if there is a texture or not (depending on if the material's constructor can take one) and continue parsing that way
                if (objects.contains("material")) {
                    if (objects["material"] == "lambertian") {
                        mat = std::make_shared<Materials::lambertian>(parseLambertian());
                    }
                    else if (objects["material"] == "diffuse_light") {
                        mat = std::make_shared<Materials::diffuse_light>(parseDiffuseLight());
                    }
                    else if (objects["material"] == "metal") {
                        mat = std::make_shared<Materials::metal>(parseMetal());
                    }
                    else if (objects["material"] == "isotropic") {
                        mat = std::make_shared<Materials::isotropic>(parseIsotropic());
                    }
                    else if (objects["material"] == "dielectric") {
                        mat = std::make_shared<Materials::dielectric>(parseDielectric());
                    }
                }

                // then, we get geo properties
                if (objects.contains("geometry") && objects["geometry"].contains("type")) {
                    // then, we do type checks
                    if (objects["geometry"]["type"] == "sphere") {
                        double radius = 1;
                        // check if radius is there
                        if (objects["geometry"]["type"]["sphere"].contains("radius")) {
                            radius = objects["geometry"]["type"]["sphere"]["radius"];
                        }
                    }
                }
            }
        }
    }

    void sceneJSONReader::extractRenderOptions(Aero::RT::renderOptions &options) {
        if (!data["aero_rt_scene"]["render_options"]) {
            throw std::runtime_error("JSON did not contain 'render_options' entry!");
        }

        auto renderOptions = data["aero_rt_scene"]["render_options"];

        if (renderOptions.contains("background")) {
            Utils::color background;

            int i = 0;
            for (auto& n : data["aero_rt_scene"]["background"]) {
                background[i] = n;
                i++;
            }

            options.background = background;
        }
        else {
            Utils::Logger::instance().log(Logger::Severity::WARNING, "JSON did not contain 'background' entry! defaulting to 50, 50, 50");
            options.background = Utils::color(50, 50, 50);
        }

        if (renderOptions.contains("width")) {
            options.width = renderOptions["width"];
        }
        else {
            Utils::Logger::instance().log(Logger::Severity::WARNING, "JSON did not contain 'width' entry! defaulting to 400");
            options.width = 400;
        }

        if (renderOptions.contains("height")) {
            options.width = renderOptions["height"];
        }
        else {
            Utils::Logger::instance().log(Logger::Severity::WARNING, "JSON did not contain 'height' entry! defaulting to 225");
            options.width = 225;
        }

        if (renderOptions.contains("samples_per_pixel")) {
            options.samples_per_pixel = renderOptions["samples_per_pixel"];
        }
        else {
            Utils::Logger::instance().log(Logger::Severity::WARNING, "JSON did not contain 'samples_per_pixel' entry! defaulting to 100");
            options.samples_per_pixel = 100;
        }

        if (renderOptions.contains("max_depth")) {
            options.max_depth = renderOptions["max_depth"];
        }
        else {
            Utils::Logger::instance().log(Logger::Severity::WARNING, "JSON did not contain 'max_depth' entry! defaulting to 50");
            options.max_depth = 50;
        }
    }

    std::vector<Camera::camera> sceneJSONReader::extractCameras(Scene::scene& scene) {
        if (!data["aero_rt_scene"]["cameras"]) {
            throw std::runtime_error("JSON did not contain 'cameras' entry! At least 1 camera is required to render a scene!");
        }

        std::vector<Camera::camera> cams;

        auto cameras = data["aero_rt_scene"]["cameras"];
        int i = 0;
        int cam_index = 0;

        for (auto& n : cameras) {
            Camera::camera cam;

            if (cameras.contains("position")) {
                Math::point3 pos;
                for (auto& n : cameras["position"]) {
                    pos[i] = n;
                    i++;
                }

                i = 0;

                cam.set_lookfrom(pos);
            }
            else {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a position entry! Defaulting to origin");
                cam.set_lookfrom({0, 0 ,0});
            }

            if (cameras.contains("lookAt")) {
                Math::point3 lookAt;
                for (auto& n : cameras["lookAt"]) {
                    lookAt[i] = n;
                    i++;
                }
                i = 0;

                cam.set_lookat(lookAt);
            }
            else {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a lookAt entry! Defaulting to origin");
                cam.set_lookat({0, 0 ,0});
            }

            if (cameras.contains("fov")) {
                cam.set_fov(cameras["fov"]);
            }
            else {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a fov entry! Defaulting to 20");
                cam.set_fov(20);
            }

            if (cameras.contains("vup")) {
                Math::point3 vup;
                for (auto& n : cameras["vup"]) {
                    vup[i] = n;
                    i++;
                }
                i = 0;

                cam.set_vup(vup);
            }
            else {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a vup entry! Defaulting to [0, 1, 0]");
                cam.set_vup({0, 1, 0});
            }

            if (cameras.contains("defocus_angle")) {
                cam.set_defocus_angle(cameras["defocus_angle"]);
            }
            else {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a defocus_angle entry! Defaulting to 0");
                cam.set_defocus_angle(0);
            }

            if (cameras.contains("current_camera") && !cameras["current_camera"] == false) {
                scene.set_current_camera(cam);
            }
            else if (!cameras.contains("current_camera")) {
                Utils::Logger::instance().log(Utils::Logger::Severity::WARNING,
                    "Camera entry " + std::to_string(cam_index) + " did not contain a current_camera entry! Defaulting to false");
            }

            cam_index++;
            cams.push_back(cam);
        }

        return cams;
    }
}
