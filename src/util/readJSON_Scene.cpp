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

                if (objects.contains("material") && objects["material"].contains("type")) {
                    Utils::color cl;
                    if (objects["material"].contains("color")) {
                        for (auto& n : objects["material"]["color"]) {
                            cl[i] = n;
                            i++;
                        }
                    }
                    if (objects["material"]["type"] == "lambertian") {
                        mat = std::make_shared<Materials::lambertian>(cl);
                    }
                    else if () {

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
        if (data["aero_rt_scene"].contains("background")) {
            Utils::color background;

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
    }

    Camera::camera sceneJSONReader::extractCamera() {
        Camera::camera cam;

        if (data["aero_rt_scene"].contains("camera") && data["aero_rt_scene"]["camera"].contains("position")) {
            Math::point3 pos;
            for (auto& n : data["aero_rt_scene"]["camera"]["position"]) {
                pos[i] = n;
                i++;
            }

            cam.set_lookfrom(pos);
        }

        if (data["aero_rt_scene"].contains("camera") && data["aero_rt_scene"]["camera"].contains("lookAt")) {
            Math::point3 lookAt;
            for (auto& n : data["aero_rt_scene"]["camera"]["lookAt"]) {
                lookAt[i] = n;
                i++;
            }

            cam.set_lookat(lookAt);
        }

        if (data["aero_rt_scene"].contains("camera") && data["aero_rt_scene"]["camera"].contains("fov")) {
            cam.set_fov(data["aero_rt_scene"]["camera"]["fov"]);
        }

        if (data["aero_rt_scene"].contains("camera") && data["aero_rt_scene"]["camera"].contains("vup")) {
            Math::point3 vup;
            for (auto& n : data["aero_rt_scene"]["camera"]["vup"]) {
                vup[i] = n;
                i++;
            }

            cam.set_lookat(vup);
        }

        if (data["aero_rt_scene"].contains("camera") && data["aero_rt_scene"]["camera"].contains("defocus_angle")) {
            cam.set_defocus_angle(data["aero_rt_scene"]["camera"]["defocus_angle"]);
        }

        return cam;
    }
}
