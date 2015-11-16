#pragma once

#include <glm/matrix.hpp>

namespace glm
{
	template <class T>
	class aabb
	{
	public:
		aabb() : minp(1e12f), maxp(-1e24f){};
		aabb(T p) : minp(p), maxp(p) {};
		aabb(T minp, T maxp) : minp(minp), maxp(maxp) {};
		//! Copy constructor
		aabb(const aabb<T>& v) : minp(v.minp), maxp(v.maxp) {};
		void Setminp(const T& x) { minp = x; };
		void Setmaxp(const T& x) { maxp = x; };
		void Set(const T& minp_, const T& maxp_) { minp = minp_; maxp = maxp_; };
		void SetZero() { minp = T(0.0f); maxp = T(0.0f); };
		void Reset() { minp = T(1e12f, 1e12f); maxp = T(-1e12f, -1e12f); };

		// operators

		aabb<T>& operator = (const aabb<T>& v)	{ minp = v.minp; maxp = v.maxp; return *this; };

		T GetSize() const { return maxp - minp; };
		T GetCenter() const { return 0.5f*(maxp + minp); };

		bool AddPoint(const T& p);
		
		T minp, maxp;
	};

	template <>
	inline bool aabb<vec2>::AddPoint(const vec2& p)
	{
		bool changed = false;
		minp.x = p.x < minp.x ? changed = true, p.x : minp.x;
		minp.y = p.y < minp.y ? changed = true, p.y : minp.y;
		maxp.x = p.x > maxp.x ? changed = true, p.x : maxp.x;
		maxp.y = p.y > maxp.y ? changed = true, p.y : maxp.y;
		return changed;
	}
	template <>
	inline bool aabb<vec3>::AddPoint(const vec3& p)
	{
		bool changed = false;
		minp.x = p.x < minp.x ? changed = true, p.x : minp.x;
		minp.y = p.y < minp.y ? changed = true, p.y : minp.y;
		maxp.x = p.x > maxp.x ? changed = true, p.x : maxp.x;
		maxp.y = p.y > maxp.y ? changed = true, p.y : maxp.y;
		maxp.z = p.z > maxp.z ? changed = true, p.z : maxp.z;
		minp.z = p.z < minp.z ? changed = true, p.z : minp.z;
		return changed;
	}

	template <class T>
	inline aabb<T> operator + (const aabb<T>& a, const aabb<T>& b)
	{
		aabb<T> box(a);
		box.AddPoint(b.maxp);
		box.AddPoint(b.minp);
		return box;
	}
	/*
	/// Multiply a matrix by a aabb.
	template <template< typename > typename T, typename A>
	inline aabb<T, A> TMul(const Mat44<A>& m, const aabb<T, A>& v) {
		aabb<T, A> box(TMul(m, vector4d<A>(v.maxp, 1.0f)));
		box.AddPoint(TMul(m, vector4d<A>(v.minp, 1.0f)));
		return box;
	}
	*/
	template <class T>
	inline bool Overlapping(aabb<T> bodyA, aabb<T> bodyB)
	{
		for (int i = 0; i < T::GetSize(); ++i)
		{
			if ((bodyA.maxp[i] < bodyB.minp[i]) || (bodyA.minp[i] > bodyB.maxp[i]))
			{
				return false;
			}
		}
		return true;
	}

	template <class T>
	inline bool Overlapping(aabb<T> bodyA, T point)
	{
		for (int i = 0; i < T::GetSize(); ++i)
		{
			if ((bodyA.maxp[i] < point[i]) && (bodyA.minp[i] > point[i]))
			{
				return false;
			}
		}
		return true;
	}

	typedef aabb<vec2> aabb2df;
	typedef aabb<vec3> aabb3df;

}

