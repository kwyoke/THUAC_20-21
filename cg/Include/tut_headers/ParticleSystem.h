#include <stdlib.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265359

class Mass
{
public:
    double mass;
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 force;
    glm::vec4 color;
    double fadeSpeed;
    double life;
    
public:
    Mass() {}
    Mass(double mass, glm::vec3 position, glm::vec3 speed, double life)
    {
        this->mass = mass;
        this->position = position;
        this->speed = speed;
        this->life = life;
    }
    
    void init()
    {
        force = glm::vec3(0.0, 0.0, 0.0);
    }
    
    void update(double dt)
    {
        speed += force / GLfloat(mass) * GLfloat(dt);
        position += speed * GLfloat(dt);
    }
    
    void applyForce(glm::vec3 newForce)
    {
        this->force += newForce;
    }
    
    virtual void apply() {}
    
    void process(double dt)
    {
        init();
        apply();
        update(dt);
    }
};

class BunchofMass
{
protected:
	int massNum;
	Mass** masses;

public:
	BunchofMass(int massNum, double m, glm::vec3 position, glm::vec3 speed, double life)
	{
		this->massNum = massNum;
		masses = new Mass*[massNum];
		if(masses)
		{
			for (int i = 0;i < massNum;++i)
				masses[i] = new Mass(m, position, speed, life);
		}
	}

	~BunchofMass()
	{
		if (masses)
		{
			for (int i = 0; i < massNum; ++i)
				delete[] masses[i];
			delete[] masses;
		}
	}

	Mass* getMass(int idx)
	{
		return masses[idx];
	}

	void init()
	{
		for(int i = 0;i < massNum;++i)
			masses[i]->init(); //set force to zero
	}

	virtual void apply() {}

	virtual void update(double dt, glm::vec3 newpos)
	{
		for(int i = 0;i < massNum;++i)
			masses[i]->update(dt); //update speed and position
								   // speed += (F/m) * dt
								   // position += speed * dt
	}

	void process(double dt, glm::vec3 newpos)
	{
		init(); // set force to zero
		apply(); // apply force (add force)
		update(dt, newpos); // update speed and position
	}
};

class FireWork : public BunchofMass
{
    glm::vec3 gravAcceleratio, constGravAcceleratio;
	double lifeLeft;

	glm::vec3 initialSpeed;
	glm::vec3 initialPosition;
	double initialLife;

public:
	bool hasExploded;

public:
	FireWork(int massNum, double m, glm::vec3 position, glm::vec3 speed,
		glm::vec3 gravity, double lifeValue)
		: BunchofMass(massNum, m, position, speed, lifeValue), hasExploded(false)
	{
		gravAcceleratio = gravity;
		constGravAcceleratio = gravity;
		lifeLeft = lifeValue;

		initialPosition = position;
		initialSpeed = speed;
		initialLife = lifeLeft;

		double r = rand()%255;
		double g = rand()%255;
		double b = rand()%255;
		for (int i = 0;i < massNum;++i)
		{
            masses[i]->color = glm::vec4(r/255.,g/255.,b/255.,1.0);
			masses[i]->fadeSpeed = rand() % 2 ? 50/255. : 40/255.;
            masses[i]->life = lifeLeft;
		}
	}

	~FireWork()
	{
	}

	int getMassNum()
	{
		return massNum;
	}

	void apply()
	{
		for (int i = 0; i < massNum; ++i) // apply gravity
		{
			masses[i]->applyForce(gravAcceleratio*GLfloat(masses[i]->mass));
		}

		if (!hasExploded && masses[0]->speed[1] < 10) // time to explode
		{
			for (int i = 0;i < massNum; ++i) // simulate uniform spray
			{
				double radius = rand() % 30;
				double angle1 = rand() % 360;
				double angle2 = rand() % 360;

				//a random direction
                glm::vec3 dir = glm::vec3(radius * cos(angle2 * PI/180) * cos(angle1 * PI/180),
					radius * cos(angle2 * PI/180) * sin(angle1 * PI/180),
					radius * sin(angle2 * PI/180));
                dir = glm::normalize(dir);
				dir *= 10;

				gravAcceleratio = glm::vec3(0, 0, 0);
				masses[i]->speed = glm::vec3(0, 0, 0);
				masses[i]->speed += dir;
				hasExploded = true;
			}
		}
	}

	void update(double dt, glm::vec3 newPos)
	{
		BunchofMass::update(dt, newPos); // update speed and position
		lifeLeft -= dt;

		if (hasExploded)
		{
			for (int i = 0; i < massNum; ++i){
				masses[i]->color.a -= masses[i]->fadeSpeed * dt; // here alpha is transparency
				if(masses[i]->color.a < 0) masses[i]->color.a = 0;
			}
		}
		if (lifeLeft < 1e-2) // revive
		{
			double r = rand()%255;
			double g = rand()%255;
			double b = rand()%255;
			for (int i = 0;i < massNum;++i){
				masses[i]->speed = initialSpeed;
                masses[i]->color = glm::vec4(r/255.,g/255.,b/255.,1.0);
				masses[i]->position = newPos;
			}
			gravAcceleratio = constGravAcceleratio;
			lifeLeft = initialLife;
			hasExploded = false;
		}
	}
};
