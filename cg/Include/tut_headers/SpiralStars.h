#include <stdlib.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265359


class Star
{
public:
	glm::vec3 position;
	glm::vec3 centre;
	glm::vec4 color;
	double r;
	double theta; //degrees
	double a; //spiral const
	double b; //spiral const (only for logarithmic)
	int type; //0: archi, 1: fermat, 2: log
	double life;
	double num_sec = 0;

public:
	Star() {}
	Star(glm::vec3 centre, double theta, double a, double b, int type, double life)
	{
		this->centre = centre;
		this->theta = theta;
		this->a = a;
		this->b = b;
		this->type = type;
		this->life = life;
	}

	//0 <= scalar <= 1
	glm::vec4 scalar2col(double scalar)
	{
		glm::vec4 out_col;
		double H, S, V, C, X, m, R, G, B;

		H = scalar * 360;
		S = 1.0;
		V = 1.0;

		C = V * S;
		//X = C * (1 - std::abs((H / 60) % 2 - 1));
		X = C * (1 - H / 360);
		m = V - C;

		if (H < 60) {
			R = C;
			G = X;
			B = 0;
		}
		else if (H < 120) {
			R = X;
			G = C;
			B = 0;
		}
		else if (H < 180) {
			R = 0;
			G = C;
			B = X;
		}
		else if (H < 240) {
			R = 0;
			G = X;
			B = C;
		}
		else if (H < 300) {
			R = X;
			G = 0;
			B = C;
		}
		else if (H < 360) {
			R = C;
			G = 0;
			B = X;
		}

		out_col = glm::vec4(GLfloat(R + m), GLfloat(G + m), GLfloat(B + m), 1.0);
		

		/*if (scalar < 0.25) {
			out_col = glm::vec4(1.0, GLfloat(scalar), 0.0, 1.0);
		}
		else if (scalar < 0.5) {
			out_col = glm::vec4(GLfloat(scalar), 1.0, 0.0, 1.0);
		}
		else if (scalar < 0.75) {
			out_col = glm::vec4(0.0, 1.0, GLfloat(scalar), 1.0);
		}
		else {
			out_col = glm::vec4(0.0, GLfloat(scalar), 1.0, 1.0);
		}*/

		return out_col;
	}

	//calc position according to spiral formula
	void calc_pos()
	{
		GLfloat theta_rad = glm::radians(GLfloat(theta));
		//archimedes spiral
		if (type == 0) {
			r = a * theta / 180.0 * PI;

		}
		//fermat spiral
		else if (type == 1) {
			r = a * sqrt(theta / 180.0 * PI);
		}
		//logarithmic spiral
		else if (type == 2) {
			r = a * exp(b * theta / 180.0 * PI);
		}
		position = glm::vec3(GLfloat(r) * glm::cos(theta_rad), GLfloat(r) * glm::sin(theta_rad), 0) + centre;
	}

	//wrap theta angle to range 0 to 360
	double wrapAngle() {
		double val;
		if (theta < 0) {
			val = theta + 360;
			val = val / 360.0;
		}
		else if (theta < 360) {
			val = theta / 360.0;
		}
		else if (theta < 720) {
			val = theta - 360;
			val = val / 360.0;
		}
		else if (theta < 1080) {
			val = theta - 2 * 360;
			val = val / 360.0;
		}

		return val;
	}

	void init()
	{
		//map scalar to rainbow colormap
		double val = wrapAngle();
		color = scalar2col(val);

		//calculate initial positions of spiral points
		calc_pos();

	}

	void update(double dt)
	{
		num_sec += dt;
		if (num_sec > 0.1) { //change display every 0.1s
			life -= 1;
			num_sec = 0;
		}

		// set visibility of points based on life remaining
		if (life > 5) {
			color.a = 0.0;
		}
		else if (life == 5) {
			color.a = 1.0;
		}
		else if (life <= 0) {
			life = 10;
		}
		else {
			color.a = 0.0;
		}
		
			
	}


};

class BunchofStars
{
protected:
	int starNum;
	Star** stars;
	int width;
	int height;
	int numSpirals;

public:
	BunchofStars(int starNum, glm::vec3 centre, double a, double b, int type, int numSpirals)
	{
		this->starNum = starNum;
		this->width = centre.x * 2;
		this->height = centre.y * 2;
		this->numSpirals = numSpirals;
		stars = new Star * [numSpirals*starNum];
		double a_new;
		double theta;
		double life;
		double maxtheta;
		double mintheta;

		if (type == 0) {
			mintheta = -45.0;
			maxtheta = 1080.0;
		}
		else if (type == 1) {
			mintheta = 0;
			maxtheta = 1080.0;
		}
		else if (type == 2) {
			mintheta = -180.0;
			maxtheta = 1080.0;
		}


		if (stars)
		{
			for (int j = 0; j < numSpirals; ++j) {
				a_new = (j + 1.0) * a; //set a params for expanding spirals
				life = 6.0 + j; //set life params for expanding spirals
				for (int i = 0; i < starNum; ++i)
				{
					theta = i * (maxtheta-mintheta) / starNum + mintheta; //assign angles for each particle
					stars[j*starNum + i] = new Star(centre, theta, a_new, b, type, life);
				}
			}
		}
	}

	~BunchofStars()
	{
		if (stars)
		{
			for (int i = 0; i < numSpirals*starNum; ++i)
				delete[] stars[i];
			delete[] stars;
		}
	}

	Star* getStar(int idx)
	{
		return stars[idx];
	}

	int getStarNum()
	{
		return starNum;
	}

	void init()
	{
		for (int i = 0; i < numSpirals*starNum; ++i)
			stars[i]->init(); //set initial position, set colour
	}

	virtual void apply() {}

	virtual void update(double dt)
	{
		for (int i = 0; i < numSpirals*starNum; ++i) {
			stars[i]->update(dt); 
		}
								  
	}
};