#pragma once

template<typename T> 
struct Vec2 {
	Vec2(T x, T y) {};
	Vec2() {};
	T x, y;
	T X() const { return x; }
	T Y() const { return y; }
};

template<typename T>
struct Vec3 {
	Vec3(T x, T y, T z) {};
	Vec3() {};
	T x, y, z;
	T X() const { return x; }
	T Y() const { return y; }
	T Z() const { return z; }
};

