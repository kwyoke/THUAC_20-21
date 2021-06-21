#include <stdlib.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265359

class Snowflake
{
public:
	double size;
	glm::vec3 position;
	glm::vec3 speed;
	glm::vec4 color;

	int width;
	int height;

public:
	Snowflake(int width, int height, glm::vec3 speed)
	{
		this->width = width;
		this->height = height;
		this->speed = speed;
	}

	void init()
	{
		this->size = rand() % 50 + 5; //random size
		this->position = glm::vec3(GLfloat(rand()%width),height,0);
		this->color = glm::vec4(1.0, 1.0, 1.0, 0.0); //not visible
		this->speed.y = speed.y + rand() % 20 - 10;
	}

	void update(double dt)
	{
		position -= speed * GLfloat(dt);

		if (position.y < 0) {
			init();
		}
	}
};


class BunchofSnow
{
protected:
	int snowNum;
	Snowflake** snowflakes;
	double numSec = 0.0;
	int w;
	int h;
	int numVis = 0;
	double density;

public:
	BunchofSnow(int snowNum, int width, int height, glm::vec3 speed, double density)
	{
		this->snowNum = snowNum;
		this->w = width;
		this->h = height;
		this->density = density;
		snowflakes = new Snowflake * [snowNum];

		if (snowflakes)
		{
			for (int i = 0; i < snowNum; ++i)
				snowflakes[i] = new Snowflake(width, height, speed);
		}
	}

	~BunchofSnow()
	{
		if (snowflakes)
		{
			for (int i = 0; i < snowNum; ++i)
				delete[] snowflakes[i];
			delete[] snowflakes;
		}
	}

	Snowflake* getSnowflake(int idx)
	{
		return snowflakes[idx];
	}

	void init()
	{
		for (int i = 0; i < snowNum; ++i)
			snowflakes[i]->init(); 
	}


	void update(double dt)
	{
		for (int i = 0; i < snowNum; ++i)
			snowflakes[i]->update(dt); //update position

		numSec += dt;
		if (numSec > 1/density && numVis < snowNum) {
			snowflakes[numVis]->color.a = 1.0;
			snowflakes[numVis]->position.y = this->h;
			numSec = 0.0;
			numVis++;
		}
	}

};