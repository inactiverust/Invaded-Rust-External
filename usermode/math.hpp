#pragma once
#include <math.h>
#define M_PI 3.14159265358979323846f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))

class Vector2
{
public:
	float x, y;

	Vector2() { };

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	};

	bool is_valid()
	{
		return (x != 0 && y != 0);
	}


	void normalize()
	{

		if (x < -89)
			x = -89;


		else if (x > 89)
			x = 89;


		if (y < -360)
			y += 360;


		else if (y > 360)
			y -= 360;
	}

	Vector2& operator*=(float input)
	{
		x *= input;
		y *= input;
		return *this;
	}
};

class Vector3
{
public:
	Vector3()
	{
		x = y = z = 0.f;
	}

	Vector3(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	float x, y, z;

	Vector3 operator+(const Vector3& input) const
	{
		return Vector3{ x + input.x, y + input.y, z + input.z };
	}

	Vector3 operator-(const Vector3& input) const
	{
		return Vector3{ x - input.x, y - input.y, z - input.z };
	}

	Vector3 operator/(float input) const
	{
		return Vector3{ x / input, y / input, z / input };
	}

	Vector3 operator*(float input) const
	{
		return Vector3{ x * input, y * input, z * input };
	}

	Vector3& operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector3& operator/=(float input)
	{
		x /= input;
		y /= input;
		z /= input;
		return *this;
	}

	Vector3& operator*=(float input)
	{
		x *= input;
		y *= input;
		z *= input;
		return *this;
	}

	bool operator==(const Vector3& input) const
	{
		return x == input.x && y == input.y && z == input.z;
	}

	float length() {
		return sqrtf(x * x + y * y + z * z);
	}
};

class Vector4
{
public:
	Vector4()
	{
		x = y = z = w = 0.f;
	}

	Vector4(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	float x, y, z, w;

	Vector4 operator+(const Vector4& input) const
	{
		return Vector4{ x + input.x, y + input.y, z + input.z, w + input.w };
	}

	Vector4 operator-(const Vector4& input) const
	{
		return Vector4{ x - input.x, y - input.y, z - input.z, w - input.w };
	}

	Vector4 operator/(float input) const
	{
		return Vector4{ x / input, y / input, z / input, w / input };
	}

	Vector4 operator*(float input) const
	{
		return Vector4{ x * input, y * input, z * input, w * input };
	}

	Vector4& operator-=(const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	Vector4& operator/=(float input)
	{
		x /= input;
		y /= input;
		z /= input;
		w /= input;
		return *this;
	}

	Vector4& operator*=(float input)
	{
		x *= input;
		y *= input;
		z *= input;
		w *= input;
		return *this;
	}

	bool operator==(const Vector4& input) const
	{
		return x == input.x && y == input.y && z == input.z && w == input.w;
	}

};

struct Matrix4x4 {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		}; float m[4][4];
	};
};

float calculate_bullet_drop(float height, float distance, float velocity, float gravity)
{
	float pitch = std::atan2(height, distance);
	float bullet_velocity = velocity * std::cos(pitch);
	float time = distance / bullet_velocity;

	return (0.5f * gravity * time * time) * 10;
}

__forceinline float Calc3D_Dist(const Vector3& Src, const Vector3& Dst) {
	return sqrtf(pow((Src.x - Dst.x), 2) + pow((Src.y - Dst.y), 2) + pow((Src.z - Dst.z), 2));
}

__forceinline Vector2 CalcAngle(const Vector3& Src, const Vector3& Dst) {
	Vector3 dir = Src - Dst;
	return Vector2{ RAD2DEG(asin(dir.y / dir.length())), RAD2DEG(-atan2(dir.x, -dir.z)) };
}

__forceinline float Dot(const Vector3& Vec1, const Vector3& Vec2) {
	return Vec1.x * Vec2.x + Vec1.y * Vec2.y + Vec1.z * Vec2.z;
}

double to_rad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

Vector4 to_quat(Vector3 Euler) {

	double heading = to_rad(Euler.x);
	double attitude = to_rad(Euler.y);
	double bank = to_rad(Euler.z);

	double c1 = cos(heading / 2);
	double s1 = sin(heading / 2);
	double c2 = cos(attitude / 2);
	double s2 = sin(attitude / 2);
	double c3 = cos(bank / 2);
	double s3 = sin(bank / 2);
	double c1c2 = c1 * c2;
	double s1s2 = s1 * s2;
	Vector4 Quat;
	Quat.w = c1c2 * c3 - s1s2 * s3;
	Quat.x = c1c2 * s3 + s1s2 * c3;
	Quat.y = s1 * c2 * c3 + c1 * s2 * s3;
	Quat.z = c1 * s2 * c3 - s1 * c2 * s3;
	Vector4 Quat2;
	Quat2 = { Quat.y, Quat.z, (Quat.x * -1), Quat.w };
	return Quat2;
}

__forceinline float Calc2D_Dist(const Vector2& Src, const Vector2& Dst) {
	return sqrt(powf(Src.x - Dst.x, 2) + powf(Src.y - Dst.y, 2));
}