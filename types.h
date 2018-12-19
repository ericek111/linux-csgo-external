#ifndef __TYPES_H_
#define __TYPES_H_

struct Vector2D {
	float x, y;
	struct Vector2D& operator+=(const Vector2D& a) { x += a.x; y += a.y; return *this; }
	struct Vector2D& operator+=(const float& k) { x += k; y += k; return *this; }
	struct Vector2D& operator-=(const Vector2D& a) { x -= a.x; y -= a.y; return *this; }
	struct Vector2D& operator-=(const float& k) { x -= k; y -= k; return *this; }
	struct Vector2D& operator*=(const Vector2D& a) { x *= a.x; y *= a.y; return *this; }
	struct Vector2D& operator*=(const float& k) { x *= k; y *= k; return *this; }
	struct Vector2D& operator=(const Vector2D& a) { x = a.x; y = a.y; return *this; }
	struct Vector2D& operator=(const float& k) { x = k; y = k; return *this; }
};
inline Vector2D operator+(Vector2D a, const Vector2D& b) { return a += b; }
inline Vector2D operator+(Vector2D a, const float k) { return a += k; }
inline Vector2D operator-(Vector2D a, const Vector2D& b) { return a -= b; }
inline Vector2D operator-(Vector2D a, const float k) { return a -= k; }
inline Vector2D operator*(Vector2D a, const Vector2D& b) { return a *= b; }
inline Vector2D operator*(Vector2D a, const float k) { return a *= k; }

struct Vector {
	float x, z, y;
	struct Vector& operator+=(const Vector& a) { x += a.x; y += a.y; z += a.z; return *this; }
	struct Vector& operator+=(const float& k) { x += k; y += k; z += k; return *this; }
	struct Vector& operator-=(const Vector& a) { x -= a.x; y -= a.y; z -= a.z; return *this; }
	struct Vector& operator-=(const float& k) { x -= k; y -= k; z -= k; return *this; }
	struct Vector& operator*=(const Vector& a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
	struct Vector& operator*=(const float& k) { x *= k; y *= k; z *= k; return *this; }
	struct Vector& operator=(const Vector& a) { x = a.x; y = a.y; z = a.z; return *this; }
	struct Vector& operator=(const float& k) { x = k; y = k; z = k; return *this; }
};
inline Vector operator+(Vector a, const Vector& b) { return a += b; }
inline Vector operator+(Vector a, const float k) { return a += k; }
inline Vector operator-(Vector a, const Vector& b) { return a -= b; }
inline Vector operator-(Vector a, const float k) { return a -= k; }
inline Vector operator*(Vector a, const Vector& b) { return a *= b; }
inline Vector operator*(Vector a, const float k) { return a *= k; }

struct QAngle {
	float x; // Pitch
	float y; // Yaw
	float z; // Roll
};

#endif
