#include "physics/spherebody.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "scene/sphere.hpp"
#include <iostream>
#include <exception>
#include <algorithm>

#define EPSILON_VEL 0.01
#define EPSILON_ANGVEL 0.005

namespace _462 {

struct State
{
    Vector3 x;      // position
    Vector3 v;      // velocity
};

struct Derivative
{
    Vector3 dx;      // dx/dt = velocity
    Vector3 dv;      // dv/dt = acceleration
};

SphereBody::SphereBody( Sphere* geom )
{
    sphere = geom;
    position = sphere->position;
    radius = sphere->radius;
    orientation = sphere->orientation;
    mass = 0.0;
    velocity = Vector3::Zero;
    angular_velocity = Vector3::Zero;
    force = Vector3::Zero;
    torque = Vector3::Zero;
}

Vector3 SphereBody::calc_acceleration(int angular)
{
	Vector3 acceleration;
	if (angular == 0) {
		acceleration = force / mass;
	}
	else {
		real_t I = 2.0/5.0 * mass * radius * radius;
		acceleration = torque / I;
	}
	return acceleration;
}


Derivative SphereBody::evaluate( const State &initial, real_t t, real_t dt, const Derivative &d, int angular )
{
    State state;
    state.x = initial.x + d.dx*dt;
    state.v = initial.v + d.dv*dt;

    Derivative output;
    output.dx = state.v;
    output.dv = calc_acceleration( angular );
    return output;
}


Vector3 SphereBody::integrate( State &state, real_t t, real_t dt, int angular )
{
    Derivative a,b,c,d;

    a = evaluate( state, t, 0.0f, Derivative(), angular);
    b = evaluate( state, t, dt*0.5f, a, angular );
    c = evaluate( state, t, dt*0.5f, b, angular );
    d = evaluate( state, t, dt, c, angular );

    Vector3 dxdt = 1.0f / 6.0f * ( a.dx + 2.0f * ( b.dx + c.dx ) + d.dx );
    
    Vector3 dvdt = 1.0f / 6.0f * ( a.dv + 2.0f * ( b.dv + c.dv ) + d.dv );

    state.x = state.x + dxdt * dt;
    state.v = state.v + dvdt * dt;

	return dxdt * dt;
}


Vector3 SphereBody::step_position( real_t dt, real_t motion_damping )
{
    // Note: This function is here as a hint for an approach to take towards
    // programming RK4, you should add more functions to help you or change the
    // scheme
    // TODO return the delta in position dt in the future
    
    Vector3 deltaV = dt * calc_acceleration(0);
    velocity += deltaV;

    if (length(velocity) < EPSILON_VEL) {
        velocity = Vector3::Zero;
        return Vector3::Zero;
    }

	State curr_state;
	curr_state.x = position;
	curr_state.v = velocity;

	real_t t = 0; //arbitrary

	//update position and velocity with RK4
	Vector3 deltaX = integrate(curr_state, t, dt, 0);

	sphere->position = curr_state.x; //change pos of graphical object
	position = curr_state.x;
	velocity = curr_state.v;

	return deltaX;
}

Vector3 SphereBody::step_orientation( real_t dt, real_t motion_damping )
{
    // Note: This function is here as a hint for an approach to take towards
    // programming RK4, you should add more functions to help you or change the
    // scheme
    // TODO return the delta in orientation dt in the future
    // vec.x = rotation along x axis
    // vec.y = rotation along y axis
    // vec.z = rotation along z axis
    Vector3 delta_angvel = dt * calc_acceleration(1);
    angular_velocity += delta_angvel;

    if (length(angular_velocity) < EPSILON_ANGVEL) {
        angular_velocity = Vector3::Zero;
        return Vector3::Zero;
    }

	Vector3 deltaTheta;
		
	// dummy orient vector for rk4 processing
	Vector3 orient_vec = Vector3::Zero;
	
	State now_state;
	now_state.x = orient_vec;
	now_state.v = angular_velocity;

	real_t t = 0; //arbitrary

	//update orientation and angular velocity with RK4
	deltaTheta = integrate(now_state, t, dt, 1);

	Quaternion r = Quaternion(deltaTheta, length(deltaTheta));
    orientation = r * orientation;
    sphere->orientation = orientation; //change orient of graphical object
	angular_velocity = now_state.v;

    return deltaTheta;

}

void SphereBody::apply_force( const Vector3& f, const Vector3& offset )
{
    // TODO apply force/torque to sphere

	// increment force and torque so that multiple forces (e.g. spring + gravity) can be addeded
	force += f;
    torque += cross(offset, f);
}

}
