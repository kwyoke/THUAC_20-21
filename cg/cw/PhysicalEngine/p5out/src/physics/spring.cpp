#include "math/math.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "physics/spring.hpp"
#include "physics/body.hpp"
#include "physics/spherebody.hpp"
#include <iostream>

namespace _462 {

Spring::Spring()
{
    body1_offset = Vector3::Zero;
    body2_offset = Vector3::Zero;
    damping = 0.0;
	old_f = Vector3::Zero;
	old_body1_offset = Vector3::Zero;
	old_body2_offset = Vector3::Zero;
}

void Spring::step( real_t dt )
{
    // TODO apply forces to attached bodies

	//offset needs to be rotated along with the sphere
	Matrix3 rot_mat = Matrix3::Identity;
	body1->orientation.to_matrix(&rot_mat);
	Vector3 body1_offset_r = rot_mat * body1_offset;
	body2->orientation.to_matrix(&rot_mat);
	Vector3 body2_offset_r = rot_mat * body2_offset;

	// calculating spring force 
	Vector3 displ = body1->position + body1_offset_r - body2->position - body2_offset_r;
	// Hookes Law
	Vector3 f = - constant * (length(displ) - equilibrium) * normalize(displ);
	// Damping
	f -= damping * (dot(body1->velocity - body2->velocity, normalize(displ))) * normalize(displ);
	
	//subtract previous force and torque, then add new force and torque
	//not same as resetting force and torque to zero, because other forces might be acting on body (e.g. gravity)
	body1->apply_force(-old_f, old_body1_offset);
	body2->apply_force(old_f, old_body2_offset);
	
	body1->apply_force(f, body1_offset_r);
	body2->apply_force(-f, body2_offset_r);

	old_body1_offset = body1_offset_r;
	old_body2_offset = body2_offset_r;
	old_f = f;

}

}


