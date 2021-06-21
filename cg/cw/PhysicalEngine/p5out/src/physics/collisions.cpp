#include "physics/collisions.hpp"
#include <cstdio>

namespace _462 {

bool collides( SphereBody& body1, SphereBody& body2, real_t collision_damping )
{
    // TODO detect collision. If there is one, update velocity

	Vector3 deltaPos = body1.position - body2.position;
	Vector3 deltaVel = body1.velocity - body2.velocity;

	//check if there is collision: (1) check relative pos and velocity; (2) check distance between objects
	bool collision = (dot(deltaPos, deltaVel) < 0) && ((length(deltaPos)) < body1.radius + body2.radius);

	if (collision) {
		//calculate new velocities after elastic collision
		Vector3 d = deltaPos / length(deltaPos);
		Vector3 tmpv2 = 2 * d * body1.mass /(body1.mass + body2.mass) * dot(deltaVel, d);
		Vector3 u2 = body2.velocity + tmpv2;
		Vector3 u1 = 1 / body1.mass * (body1.mass * body1.velocity + body2.mass * body2.velocity - body2.mass * u2);

		//account for damping
		body1.velocity = u1 - collision_damping*u1;
		body2.velocity = u2 - collision_damping*u2;

		return true;
	}
	else {
		return false;
	}
}

bool pointInTri(Vector3 A, Vector3 B, Vector3 C, Vector3 P) 
{
	// check if point P lies in triangle with vertices A, B, C
	// ----- BARYCENTRIC TECHNIQUE START ---------
	Vector3 v0 = C - A;
	Vector3 v1 = B - A;
	Vector3 v2 = P - A;

	real_t dot00 = dot(v0, v0);
	real_t dot01 = dot(v0, v1);
	real_t dot02 = dot(v0, v2);
	real_t dot11 = dot(v1, v1);
	real_t dot12 = dot(v1, v2);

	// Compute barycentric coordinates
	real_t invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	real_t u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	real_t v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	bool inTri = (u >= 0) && (v >= 0) && (u + v < 1);
	// ------ BARYCENTRIC TECHNIQUE END -----------

	return inTri;
}

bool collides( SphereBody& body1, TriangleBody& body2, real_t collision_damping )
{
    // TODO detect collision. If there is one, update velocity

	// get triangle vertices
	Vector3 A = body2.vertices[0];
	Vector3 B = body2.vertices[1];
	Vector3 C = body2.vertices[2];

	Vector3 normalUnit = normalize(cross(B - A, C - A));
	Vector3 deltaPosDiag = body1.position - A;
	if (dot(deltaPosDiag, normalUnit) < 0){ //ensure normal is pointing towards sphere
		normalUnit *= -1; 
	}

	real_t d = dot(deltaPosDiag, normalUnit); //distance between sphere and plane containing triangle
	Vector3 deltaPos = d * normalUnit; //shortest vector pointing from plane to sphere
	Vector3 deltaVel = body1.velocity - body2.velocity;
	Vector3 projectedPoint = body1.position - deltaPos; //point projected by sphere on plane

	//check if there is collision: (1) check relative pos and velocity; (2) check distance between objects 
	//(3) check if point projected by sphere on plane lies in triangle
	bool collision = (dot(deltaPos, deltaVel) < 0) && (std::abs(d) < body1.radius) && (pointInTri(A, B, C, projectedPoint));

	if (collision) {
		Vector3 u = body1.velocity - 2 * dot(body1.velocity, normalUnit) * normalUnit;
		body1.velocity = u - collision_damping*u;
		
		return true;
	}
	else {
		return false;
	}
}

bool collides( SphereBody& body1, PlaneBody& body2, real_t collision_damping )
{
    // TODO detect collision. If there is one, update velocity

	Vector3 deltaPosDiag = body1.position - body2.position; // not the shortest perpendicular distance from sphere to plane
	Vector3 planeNormUnit = normalize(body2.normal); // unit normal vector
	if (dot(deltaPosDiag, planeNormUnit) < 0){ //ensure normal is pointing towards sphere
		planeNormUnit *= -1; 
	}
	
	Vector3 deltaVel = body1.velocity - body2.velocity;

	real_t d = dot(deltaPosDiag, planeNormUnit); //distance between sphere and plane
	Vector3 deltaPos = d * planeNormUnit; //shortest vector pointing from plane to sphere

	//check if there is collision: (1) check relative pos and velocity; (2) check distance between objects
	bool collision = (dot(deltaPos, deltaVel) < 0) && (std::abs(d) < body1.radius);

	if (collision) {
		Vector3 u = body1.velocity - 2 * dot(body1.velocity, planeNormUnit) * planeNormUnit;
		body1.velocity = u - collision_damping*u;
		
		return true;
	}
	else {
		return false;
	}
}

}
