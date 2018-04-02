#ifndef	MATH_3D_H
#define MATH_3D_H

#include <math.h>
#include <vcruntime_string.h> // memcpy

namespace Math3D
{
	struct Vec2
	{
		float x, y;

		Vec2() {};
		Vec2(float _x, float _y)
		{
			x = _x;
			y = _y;
		};

		Vec2 operator-(const Vec2 _source)
		{
			Vec2 result;

			result.x = this->x - _source.x;
			result.y = this->y - _source.y;

			return result;
		}
		Vec2 operator*(const float _source)
		{
			Vec2 result;

			result.x = this->x * _source;
			result.y = this->y * _source;

			return result;
		}

		bool IsZero()
		{
			if (x == 0.0f && y == 0.0f)
				return true;
			return false;
		}

		void Rotate(float _angle)
		{
			float tempX = (x * cosf(_angle)) - (y * sinf(_angle));
			float tempY = (y * cosf(_angle)) + (x * sinf(_angle));
			x = tempX;
			y = tempY;
		}

		static float Distance(Vec2 _sourceA, Vec2 _sourceB)
		{
			return sqrtf(powf(_sourceA.x - _sourceB.x, 2) + powf(_sourceA.y - _sourceB.y, 2));
		}
		static float Lenght(Vec2 _source)
		{
			return sqrtf((_source.x * _source.x) + (_source.y * _source.y));
		}
		static Vec2 Normalize(Vec2 _source)
		{
			float length = Lenght(_source);
			return Vec2(_source.x / length, _source.y / length);
		}
		static float cross(Vec2 _sourceA, Vec2 _sourceB)
		{
			return (_sourceA.x * _sourceB.y) - (_sourceA.y * _sourceB.x);
		}
		static float Dot(const Vec2 _sourceA, const Vec2 _sourceB)
		{
			return _sourceA.x * _sourceB.x + _sourceA.y * _sourceB.y;
		}
		static Vec2 Reflect(Vec2 _velocity, Vec2 _normal)
		{
			Math3D::Vec2 velN = _normal * Math3D::Vec2::Dot(_velocity, _normal); // Normal component
			Math3D::Vec2 velT = _velocity - velN; // Tangential component

			return { velT - velN };
		}
	};
	struct Vec3
	{
		float x, y, z;

		Vec3() {};
		Vec3(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
		};

		Vec3& operator-(const Vec3 _source)
		{
			x -= _source.x;
			y -= _source.y;
			z -= _source.z;

			return *this;
		}
		Vec3& operator+(const Vec3 _source)
		{
			x += _source.x;
			y += _source.y;
			z += _source.z;

			return *this;
		}
		Vec3& operator*(const float _source)
		{
			x *= _source;
			y *= _source;
			z *= _source;

			return *this;
		}

		static float Lenght(Vec3 _source)
		{
			return sqrtf((_source.x * _source.x) + (_source.y * _source.y) + (_source.z * _source.z));
		}
		static Vec3 Normalize(Vec3 _source)
		{
			float length = Lenght(_source);
			return Vec3(_source.x / length, _source.y / length, _source.z / length);
		}
		static Vec3 Cross(const Vec3 _sourceA, const Vec3 _sourceB)
		{
			return Vec3(
				(_sourceA.y * _sourceB.z) - (_sourceB.y * _sourceA.z),
				(_sourceA.z * _sourceB.x) - (_sourceB.z * _sourceA.x),
				(_sourceA.x * _sourceB.y) - (_sourceB.x * _sourceA.y));
		}
		static float Dot(const Vec3 _sourceA, const Vec3 _sourceB)
		{
			return _sourceA.x * _sourceB.x + _sourceA.y * _sourceB.y + _sourceA.z * _sourceB.z;
		}
	};
	struct Vec4
	{
		float x, y, z, w;

		Vec4& operator-(const Vec4 _source)
		{
			x -= _source.x;
			y -= _source.y;
			z -= _source.z;
			w -= _source.w;

			return *this;
		}
		Vec4& operator+(const Vec4 _source)
		{
			x += _source.x;
			y += _source.y;
			z += _source.z;
			w += _source.w;

			return *this;
		}
		Vec4& operator*(const float _source)
		{
			x *= _source;
			y *= _source;
			z *= _source;
			w *= _source;

			return *this;
		}
	};

	struct Mat4
	{
		float
			xx, xy, xz, xw,
			yx, yy, yz, yw,
			zx, zy, zz, zw,
			wx, wy, wz, ww;

		Mat4& operator*(const Mat4 _source)
		{
			Mat4 result;

			result.xx = xx*_source.xx + xy*_source.yx + xz*_source.zx + xw*_source.wx;
			result.yx = yx*_source.xx + yy*_source.yx + yz*_source.zx + yw*_source.wx;
			result.zx = zx*_source.xx + zy*_source.yx + zz*_source.zx + zw*_source.wx;
			result.wx = wx*_source.xx + wy*_source.yx + wz*_source.zx + ww*_source.wx;

			result.xy = xx*_source.xy + xy*_source.yy + xz*_source.zy + xw*_source.wy;
			result.yy = yx*_source.xy + yy*_source.yy + yz*_source.zy + yw*_source.wy;
			result.zy = zx*_source.xy + zy*_source.yy + zz*_source.zy + zw*_source.wy;
			result.wy = wx*_source.xy + wy*_source.yy + wz*_source.zy + ww*_source.wy;

			result.xz = xx*_source.xz + xy*_source.yz + xz*_source.zz + xw*_source.wz;
			result.yz = yx*_source.xz + yy*_source.yz + yz*_source.zz + yw*_source.wz;
			result.zz = zx*_source.xz + zy*_source.yz + zz*_source.zz + zw*_source.wz;
			result.wz = wx*_source.xz + wy*_source.yz + wz*_source.zz + ww*_source.wz;

			result.xw = xx*_source.xw + xy*_source.yw + xz*_source.zw + xw*_source.ww;
			result.yw = yx*_source.xw + yy*_source.yw + yz*_source.zw + yw*_source.ww;
			result.zw = zx*_source.xw + zy*_source.yw + zz*_source.zw + zw*_source.ww;
			result.ww = wx*_source.xw + wy*_source.yw + wz*_source.zw + ww*_source.ww;

			memcpy(&this->xx, &result.xx, sizeof(Mat4));

			return *this;
		}
		void operator*=(const Mat4 _source)
		{
			Mat4 result;

			result.xx = xx*_source.xx + xy*_source.yx + xz*_source.zx + xw*_source.wx;
			result.yx = yx*_source.xx + yy*_source.yx + yz*_source.zx + yw*_source.wx;
			result.zx = zx*_source.xx + zy*_source.yx + zz*_source.zx + zw*_source.wx;
			result.wx = wx*_source.xx + wy*_source.yx + wz*_source.zx + ww*_source.wx;

			result.xy = xx*_source.xy + xy*_source.yy + xz*_source.zy + xw*_source.wy;
			result.yy = yx*_source.xy + yy*_source.yy + yz*_source.zy + yw*_source.wy;
			result.zy = zx*_source.xy + zy*_source.yy + zz*_source.zy + zw*_source.wy;
			result.wy = wx*_source.xy + wy*_source.yy + wz*_source.zy + ww*_source.wy;

			result.xz = xx*_source.xz + xy*_source.yz + xz*_source.zz + xw*_source.wz;
			result.yz = yx*_source.xz + yy*_source.yz + yz*_source.zz + yw*_source.wz;
			result.zz = zx*_source.xz + zy*_source.yz + zz*_source.zz + zw*_source.wz;
			result.wz = wx*_source.xz + wy*_source.yz + wz*_source.zz + ww*_source.wz;

			result.xw = xx*_source.xw + xy*_source.yw + xz*_source.zw + xw*_source.ww;
			result.yw = yx*_source.xw + yy*_source.yw + yz*_source.zw + yw*_source.ww;
			result.zw = zx*_source.xw + zy*_source.yw + zz*_source.zw + zw*_source.ww;
			result.ww = wx*_source.xw + wy*_source.yw + wz*_source.zw + ww*_source.ww;

			memcpy(&this->xx, &result.xx, sizeof(Mat4));
		}

		static Mat4 GetIdentity()
		{
			Mat4 mat;

			mat.xx = 1.0f; mat.xy = 0.0f; mat.xz = 0.0f; mat.xw = 0.0f;
			mat.yx = 0.0f; mat.yy = 1.0f; mat.yz = 0.0f; mat.yw = 0.0f;
			mat.zx = 0.0f; mat.zy = 0.0f; mat.zz = 1.0f; mat.zw = 0.0f;
			mat.wx = 0.0f; mat.wy = 0.0f; mat.wz = 0.0f; mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetTranslateMatrix(Vec3 _translate)
		{
			Mat4 mat;

			mat.xx = 1.0f; mat.xy = 0.0f; mat.xz = 0.0f; mat.xw = 0.0f;
			mat.yx = 0.0f; mat.yy = 1.0f; mat.yz = 0.0f; mat.yw = 0.0f;
			mat.zx = 0.0f; mat.zy = 0.0f; mat.zz = 1.0f; mat.zw = 0.0f;
			mat.wx = _translate.x; mat.wy = _translate.y; mat.wz = _translate.z; mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetRotateXMatrix(float _angle)
		{
			Mat4 mat;

			mat.xx = 1.0f; mat.xy = 0.0f;			mat.xz = 0.0f;			mat.xw = 0.0f;
			mat.yx = 0.0f; mat.yy = cosf(_angle);	mat.yz = -sinf(_angle);	mat.yw = 0.0f;
			mat.zx = 0.0f; mat.zy = sinf(_angle);	mat.zz = cosf(_angle);	mat.zw = 0.0f;
			mat.wx = 0.0f; mat.wy = 0.0f;			mat.wz = 0.0f;			mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetRotateYMatrix(float _angle)
		{
			Mat4 mat;

			mat.xx = cosf(_angle);	mat.xy = 0.0f; mat.xz = sinf(_angle);	mat.xw = 0.0f;
			mat.yx = 0.0f;			mat.yy = 1.0f; mat.yz = 0.0f;			mat.yw = 0.0f;
			mat.zx = -sinf(_angle);	mat.zy = 0.0f; mat.zz = cosf(_angle);	mat.zw = 0.0f;
			mat.wx = 0.0f;			mat.wy = 0.0f; mat.wz = 0.0f;			mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetRotateZMatrix(float _angle)
		{
			Mat4 mat;

			mat.xx = cosf(_angle);	mat.xy = -sinf(_angle);	mat.xz = 0.0f; mat.xw = 0.0f;
			mat.yx = sinf(_angle);	mat.yy = cosf(_angle);	mat.yz = 0.0f; mat.yw = 0.0f;
			mat.zx = 0.0f;			mat.zy = 0.0f;			mat.zz = 1.0f; mat.zw = 0.0f;
			mat.wx = 0.0f;			mat.wy = 0.0f;			mat.wz = 0.0f; mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetScaleMatrix(Vec3 _scale)
		{
			Mat4 mat;

			mat.xx = _scale.x;	mat.xy = 0.0f;		mat.xz = 0.0f;		mat.xw = 0.0f;
			mat.yx = 0.0f;		mat.yy = _scale.y;	mat.yz = 0.0f;		mat.yw = 0.0f;
			mat.zx = 0.0f;		mat.zy = 0.0f;		mat.zz = _scale.z;	mat.zw = 0.0f;
			mat.wx = 0.0f;		mat.wy = 0.0f;		mat.wz = 0.0f;		mat.ww = 1.0f;

			return mat;
		}
		static Mat4 GetLookAt(Vec3 _origin, Vec3 _target, Vec3 _up)
		{
			Mat4 mat;

			const Vec3 front(Vec3::Normalize(_target - _origin));
			const Vec3 side(Vec3::Normalize(Vec3::Cross(front, _up)));
			const Vec3 up(Vec3::Cross(side, front));

			mat.xw = 0.0f;
			mat.yw = 0.0f;
			mat.zw = 0.0f;
			mat.ww = 1.0f;

			mat.xx = side.x;
			mat.xy = up.x;
			mat.xz = -front.x;
			mat.yx = side.y;
			mat.yy = up.y;
			mat.yz = -front.y;
			mat.zx = side.z;
			mat.zy = up.z;
			mat.zz = -front.z;
			mat.wx = -Vec3::Dot(side, _origin);
			mat.wy = -Vec3::Dot(up, _origin);
			mat.wz = Vec3::Dot(front, _origin);

			return mat;
		}
		static Mat4 GetPerspectiveProjection(const float _fov, const float _width, const float _height, const float _near, const float _far)
		{
			Mat4 mat;

			float aspect = _width / _height;
			if (aspect < 0.0f)
				aspect *= -1;

			const float tanHalfFov = tanf(_fov / 2.0f);

			mat.xx = 1.0f / (aspect * tanHalfFov);
			mat.xy = 0.0f;
			mat.xz = 0.0f;
			mat.xw = 0.0f;

			mat.yx = 0.0f;
			mat.yy = 1.0f / tanHalfFov;
			mat.yz = 0.0f;
			mat.yw = 0.0f;


			mat.zx = 0.0f;
			mat.zy = 0.0f;
			mat.zz = _far / (_near - _far);
			mat.zw = -1.0f;

			mat.wx = 0.0f;
			mat.wy = 0.0f;
			mat.wz = -(_far * _near) / (_far - _near);
			mat.ww = 0.0f;

			return mat;
		}
		static Mat4 GetPerspectiveProjection(const float _fov, float _aspect, const float _near, const float _far)
		{
			Mat4 mat;

			if (_aspect < 0.0f)
				_aspect *= -1;

			const float tanHalfFov = tanf(_fov / 2.0f);

			mat.xx = 1.0f / (_aspect * tanHalfFov);
			mat.xy = 0.0f;
			mat.xz = 0.0f;
			mat.xw = 0.0f;

			mat.yx = 0.0f;
			mat.yy = 1.0f / tanHalfFov;
			mat.yz = 0.0f;
			mat.yw = 0.0f;


			mat.zx = 0.0f;
			mat.zy = 0.0f;
			mat.zz = _far / (_near - _far);
			mat.zw = -1.0f;

			mat.wx = 0.0f;
			mat.wy = 0.0f;
			mat.wz = -(_far * _near) / (_far - _near);
			mat.ww = 0.0f;

			return mat;
		}

		static Vec3 ExtractPosition(Mat4 _source)
		{
			return { _source.xz * _source.wz, _source.yz * _source.wz, _source.zz * _source.wz };
		}
	};
}

#endif