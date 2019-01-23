#pragma once

#include <unordered_map>
#include "Vector.hpp"
#include <vector>

class IcosphereCreator
{
private:

	struct TriangleIndices
	{
		int v[3];

		TriangleIndices(int v1, int v2, int v3)
		{
			v[0] = v1;
			v[1] = v2;
			v[2] = v3;
		}
	};

	//MeshGeometry3D geometry;
	int mIndex;
	std::vector<Vector> mVertices;
	std::vector<uint16_t> mIndices;
	std::unordered_map<int64_t, int> mMiddlePointIndexCache;

	// add vertex to mesh, fix position to be on unit sphere, return index
	int addVertex(Vector p)
	{
		//double length = sqrt(p.f[0] * p.f[0] + p.f[1] * p.f[1] + p.Z * p.Z);
		//float len = p.Length3();
		mVertices.push_back(p.Normalized3());
		return mIndex++;
	}

	// return index of point in the middle of p1 and p2
	int getMiddlePoint(int p1, int p2)
	{
		// first check if we have it already
		bool firstIsSmaller = p1 < p2;
		int64_t smallerIndex = firstIsSmaller ? p1 : p2;
		int64_t greaterIndex = firstIsSmaller ? p2 : p1;
		int64_t key = (smallerIndex << 32) + greaterIndex;

		int ret;
		if (mMiddlePointIndexCache.count(key) > 0)
		{
			return mMiddlePointIndexCache.at(key);
		}

		// not in cache, calculate it
		Vector point1 = mVertices[p1];
		Vector point2 = mVertices[p2];
		Vector middle(
			(point1.f[0] + point2.f[0]) / 2.0,
			(point1.f[1] + point2.f[1]) / 2.0,
			(point1.f[2] + point2.f[2]) / 2.0);

		// add vertex makes sure point is on unit sphere
		int idx = addVertex(middle);

		// store it, return index
		mMiddlePointIndexCache[key] = idx;
		return idx;
	}
public:
	void Create(int recursionLevel)
	{
		mVertices.clear();
		mMiddlePointIndexCache.clear();
		mIndex = 0;

		// create 12 vertices of a icosahedron
		//float t = (1.0 + sqrt(5.0)) / 2.0;
		const float s = sqrt((5.0 - sqrt(5.0)) / 10.0);
		const float t = sqrt((5.0 + sqrt(5.0)) / 10.0);

		addVertex(Vector(-s, t, 0));
		addVertex(Vector(s, t, 0));
		addVertex(Vector(-s, -t, 0));
		addVertex(Vector(s, -t, 0));

		addVertex(Vector(0, -s, t));
		addVertex(Vector(0, s, t));
		addVertex(Vector(0, -s, -t));
		addVertex(Vector(0, s, -t));

		addVertex(Vector(t, 0, -s));
		addVertex(Vector(t, 0, s));
		addVertex(Vector(-t, 0, -s));
		addVertex(Vector(-t, 0, s));


		// create 20 triangles of the icosahedron
		std::vector<TriangleIndices> faces;

		// 5 faces around point 0
		faces.push_back(TriangleIndices(0, 11, 5));
		faces.push_back(TriangleIndices(0, 5, 1));
		faces.push_back(TriangleIndices(0, 1, 7));
		faces.push_back(TriangleIndices(0, 7, 10));
		faces.push_back(TriangleIndices(0, 10, 11));

		// 5 adjacent faces 
		faces.push_back(TriangleIndices(1, 5, 9));
		faces.push_back(TriangleIndices(5, 11, 4));
		faces.push_back(TriangleIndices(11, 10, 2));
		faces.push_back(TriangleIndices(10, 7, 6));
		faces.push_back(TriangleIndices(7, 1, 8));

		// 5 faces around point 3
		faces.push_back(TriangleIndices(3, 9, 4));
		faces.push_back(TriangleIndices(3, 4, 2));
		faces.push_back(TriangleIndices(3, 2, 6));
		faces.push_back(TriangleIndices(3, 6, 8));
		faces.push_back(TriangleIndices(3, 8, 9));

		// 5 adjacent faces 
		faces.push_back(TriangleIndices(4, 9, 5));
		faces.push_back(TriangleIndices(2, 4, 11));
		faces.push_back(TriangleIndices(6, 2, 10));
		faces.push_back(TriangleIndices(8, 6, 7));
		faces.push_back(TriangleIndices(9, 8, 1));


		// refine triangles
		for (int i = 0; i < recursionLevel; i++)
		{
			std::vector<TriangleIndices> faces2;
			for(auto &tri : faces)
			{
				// replace triangle by 4 triangles
				int a = getMiddlePoint(tri.v[0], tri.v[1]);
				int b = getMiddlePoint(tri.v[1], tri.v[2]);
				int c = getMiddlePoint(tri.v[2], tri.v[0]);

				faces2.push_back(TriangleIndices(tri.v[0], a, c));
				faces2.push_back(TriangleIndices(tri.v[1], b, a));
				faces2.push_back(TriangleIndices(tri.v[2], c, b));
				faces2.push_back(TriangleIndices(a, b, c));
			}
			faces = faces2;
		}

		// done, now add triangles to mesh
		for (auto &tri : faces)
		{
			mIndices.push_back(tri.v[0]);
			mIndices.push_back(tri.v[1]);
			mIndices.push_back(tri.v[2]);
		}
	}

	std::vector<Vector> GetVertices()
	{
		return mVertices;
	}

	std::vector<uint16_t> GetIndices()
	{
		return mIndices;
	}

	size_t GetVerticesNumber()
	{
		return mVertices.size();
	}

	size_t GetIndicesNumber()
	{
		return mIndices.size();
	}
};