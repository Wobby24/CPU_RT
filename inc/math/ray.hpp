#pragma once 

#include "math/vec3.hpp" 
#include "hwy/highway.h"

namespace Aero::RT::Math {
	class ray {
	public:
		ray();

		ray(const point3& origin, const vec3<double>& direction, double time);

		ray(const point3& origin, const vec3<double>& direction);

		const point3& origin() const { return orig; }
		const vec3<double>& direction() const { return dir; }

		double time() const { return tm; }

		point3 at(double t) const { return orig + t * dir; }

	private:
		point3 orig;
		vec3<double> dir;
		double tm;
	};
}

/* For animation:
Before continuing, let's think about time, and how we might manage it across one or more successive renders. 
There are two aspects of shutter timing to think about: the time from one shutter opening to the next shutter opening, 
and how long the shutter stays open for each frame. Standard movie film used to be shot at 24 frames per second. 
Modern digital movies can be 24, 30, 48, 60, 120 or however many frames per second director wants.
Each frame can have its own shutter speed. This shutter speed need not be — and typically isn't — the maximum duration of the entire frame. 
You could have the shutter open for 1/1000th of a second every frame, or 1/60th of a second.
If you wanted to render a sequence of images, you would need to set up the camera with the appropriate shutter timings: 
frame-to-frame period, shutter/render duration, and the total number of frames (total shot time). 
If the camera is moving and the world is static, you're good to go. 
However, if anything in the world is moving, you would need to add a method to hittable so that every object could be made aware of the current frame's time period. 
This method would provide a way for all animated objects to set up their motion during that frame.
This is fairly straight-forward, and definitely a fun avenue for you to experiment with if you wish. 
However, for our purposes right now, we're going to proceed with a much simpler model. We will render only a single frame, implicitly assuming a start at time = 0 and ending at time = 1. 
Our first task is to modify the camera to launch rays with random times in [0,1], and our second task will be the creation of an animated sphere class.
*/