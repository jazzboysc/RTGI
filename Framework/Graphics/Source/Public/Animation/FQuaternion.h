#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>
#include <math.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#define M_PI 3.14159265358979323846

class FQuaternion
{
	float x, y, z, w;

public:

	FQuaternion()
	{
		x = y = z = 0;
		w = 1;
	}

	FQuaternion(const glm::vec3& _v, float _w)
	{
		x = _v.x;
		y = _v.y;
		z = _v.z;
		w = _w;
	}

	FQuaternion(const glm::vec4& _v)
	{
		x = _v.x;
		y = _v.y;
		z = _v.z;
		w = _v.w;
	}

	FQuaternion(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	FQuaternion conjugate(void) const
	{
		return FQuaternion(-x, -y, -z, w);
	}

	FQuaternion product(const FQuaternion& rhs) const
	{
		return FQuaternion(y*rhs.z - z*rhs.y + x*rhs.w + w*rhs.x,
			z*rhs.x - x*rhs.z + y*rhs.w + w*rhs.y,
			x*rhs.y - y*rhs.x + z*rhs.w + w*rhs.z,
			w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z);
	}

	FQuaternion operator*(const FQuaternion& rhs) const
	{
		return product(rhs);
	}

	FQuaternion operator*(float s) const
	{
		return FQuaternion(glm::vec4(x, y, z, w) * s);
	}

	FQuaternion operator+(const FQuaternion& rhs) const
	{
		return FQuaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	FQuaternion operator-(const FQuaternion& rhs) const
	{
		return FQuaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	FQuaternion operator-() const
	{
		return FQuaternion(-x, -y, -z, -w);
	}

	FQuaternion operator/(float s) const
	{
		return FQuaternion(glm::vec4(x, y, z, w) / s);
	}

	glm::mat4 rotationMatrix() const
	{
		glm::mat3 m = {
			1 - 2 * y*y - 2 * z*z, 2 * x*y - 2 * z*w, 2 * x*z + 2 * y*w,
			2 * x*y + 2 * z*w, 1 - 2 * x*x - 2 * z*z, 2 * y*z - 2 * x*w,
			2 * x*z - 2 * y*w, 2 * y*z + 2 * x*w, 1 - 2 * x*x - 2 * y*y
		};
		return glm::mat4(m);
	}

	/**
	* @brief Returns the norm ("magnitude") of the quaternion.
	* @return The 2-norm of [ w, x, y, z ]<sup>T</sup>.
	*/
	float norm() const
	{
		return ::sqrt(x * x + y * y +
			z * z + w * w);
	}

	/**
	* @brief Returns a vector rotated by this quaternion.
	*
	* Functionally equivalent to:  (rotationMatrix * v)
	* or (q * Quaternion(0, v) * q.inverse()).
	*
	* @warning conjugate() is used instead of inverse() for better
	* performance, when this quaternion must be normalized.
	*/
	glm::vec3 rotatedVector(const glm::vec3& v) const
	{
		FQuaternion r = (*this) * FQuaternion(v, 0) * conjugate();
		return glm::vec3(r.x, r.y, r.z);
	}

	/**
	* @brief Computes the quaternion that is equivalent to a given
	* euler angle rotation.
	* @param euler A 3-vector in order:  roll-pitch-yaw.
	*/
	void euler(const glm::vec3& euler)
	{
		float c1 = float(cos(euler[2] * M_PI / 180 * 0.5));
		float c2 = float(cos(euler[1] * M_PI / 180 * 0.5));
		float c3 = float(cos(euler[0] * M_PI / 180 * 0.5));
		float s1 = float(sin(euler[2] * M_PI / 180 * 0.5));
		float s2 = float(sin(euler[1] * M_PI / 180 * 0.5));
		float s3 = float(sin(euler[0] * M_PI / 180 * 0.5));

		x = c1*c2*s3 - s1*s2*c3;
		y = c1*s2*c3 + s1*c2*s3;
		z = s1*c2*c3 - c1*s2*s3;
		w = c1*c2*c3 + s1*s2*s3;
	}

	/** @brief Returns an equivalent euler angle representation of
	* this quaternion.
	* @return Euler angles in roll-pitch-yaw order.
	*/
	glm::vec3 euler(void) const
	{
		glm::vec3 euler;
		const static float PI_OVER_2 = float(M_PI * 0.5);
		const static float EPSILON = float(1e-10);
		float sqw, sqx, sqy, sqz;

		// quick conversion to Euler angles to give tilt to user
		sqw = w * w;
		sqx = x * x;
		sqy = y * y;
		sqz = z * z;

		euler[1] = float(asin(2.0 * (w * y - x * z)));
		if (PI_OVER_2 - fabs(euler[1]) > EPSILON)
		{
			euler[2] = float(atan2(2.0 * (x * y + w * z),
				sqx - sqy - sqz + sqw));
			euler[0] = float(atan2(2.0 * (w * x + y * z),
				sqw - sqx - sqy + sqz));
		}
		else
		{
			// compute heading from local 'down' vector
			euler[2] = atan2(2 * y * z - 2 * x * w,
				2 * x * z + 2 * y * w);
			euler[0] = 0.0;

			// If facing down, reverse yaw
			if (euler[1] < 0)
				euler[2] = float(M_PI - euler[2]);
		}
		return euler;
	}

	/**
	* @brief Returns the quaternion slerped between this and q1 by fraction 0 <= t <= 1.
	*/
	FQuaternion slerp(const FQuaternion& q1, float t)
	{
		return slerp(*this, q1, t);
	}

	/// Returns quaternion that is slerped by fraction 't' between q0 and q1.
	static FQuaternion slerp(const FQuaternion& q0, const FQuaternion& q1, float t)
	{
		float theta, it;
		theta = acos(q0.w*q1.w + q0.x*q1.x + q0.y*q1.y + q0.z*q1.z);

		if (fabs(theta) > 1e-6)
		{
			it = sin((1 - t)*theta) / sin(theta);
			t = sin(t*theta) / sin(theta);
		}
		// Precision considerations
		else
		{
			it = 1 - t;
		}

		return FQuaternion(
			it*q0.x + t*q1.x,
			it*q0.y + t*q1.y,
			it*q0.z + t*q1.z,
			it*q0.w + t*q1.w);
	}
};

/**
* @brief Global operator allowing left-multiply by scalar.
*/
FQuaternion operator*(float s, const FQuaternion& q);


#endif /* QUATERNION_H */
