#pragma once

#include <unordered_map>
#include "Vector.hpp"
#include <vector>

class IcosphereCreator
{
public:
	void Create(int recursionLevel);

	std::vector<Vector> GetVertices() const;
	std::vector<uint16_t> GetIndices() const;

	size_t GetVerticesNumber() const;
	size_t GetIndicesNumber() const;

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

	int mIndex;
	std::vector<Vector> mVertices;
	std::vector<uint16_t> mIndices;
	std::unordered_map<int64_t, int> mMiddlePointIndexCache;

	// add vertex to mesh, fix position to be on unit sphere, return index
	int AddVertex(Vector p);

	// return index of point in the middle of p1 and p2
	int GetMiddlePoint(int p1, int p2);
};