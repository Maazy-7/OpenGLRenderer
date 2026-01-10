#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "Physics/Collision/CollisionManifold.h"

class ContactSolverSystem
{
	const int m_solverIterations = 8;

public:
	ContactSolverSystem();

	int getSolverIterationCount() const;
	//warm starts contacts and calculates the contact's common terms ie effective mass, tangents etc
	void preSolveManifolds(CollisionManifold& manifold, float dt);
	//solves the contact constraints
	void solveManifolds(CollisionManifold& manifold);
};

