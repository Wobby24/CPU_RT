#pragma once
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include <memory>

// sceneObject should contain a hittable and a material. we can set them via the constructor or set methods and then do all of the stuff in an internal function
// keep shared pointer for the geo and mats, as some of them might be reused overtime to save memory
namespace Aero::RT::Scene {
	class sceneObject {
	public:
		sceneObject() = default;
		sceneObject(std::shared_ptr<Geometry::hittable> geo, std::shared_ptr<Materials::material> mat) : hittable_(geo), material_(mat) {}
		sceneObject(std::shared_ptr<Geometry::hittable> geo) : hittable_(geo) {}

		~sceneObject() = default;

		std::shared_ptr<Geometry::hittable> getHittable() { return hittable_; }
		std::shared_ptr<Materials::material>getMaterial() { return material_; }

		void setHittable(std::shared_ptr<Geometry::hittable> hittable) { hittable_ = std::move(hittable); }
		void setMaterial(std::shared_ptr<Materials::material> material) { material_ = std::move(material); }

		uint32_t getID() const { return id_; }
		uint32_t getID() { return id_; }
	private:
		std::shared_ptr<Geometry::hittable> hittable_;
		std::shared_ptr<Materials::material> material_;
		uint32_t id_;
	};
}