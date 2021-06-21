#define PI 3.14159265359
#include <vector>
#include <cmath>

class Sphere {
public:
    std::vector<double> vertices;
    std::vector<double> normals;
    std::vector<double> texCoords;
    std::vector<unsigned int> indices;

    int nv, nf;
    
    float radius;
    int stackCount;
    int sectorCount;
    
    Sphere(double radius, unsigned int stackCount, unsigned int sectorCount)
    {
        constructSphere(radius, stackCount, sectorCount);
        constructElIndices(radius, stackCount, sectorCount);

        this->nv = this->vertices.size() / 3.0; //3 pos coords
        this->nf = this->indices.size() / 3.0; //3 vertices in triangle
        this->radius = radius;
        this->stackCount = stackCount;
        this->sectorCount = sectorCount;
    }

private:
    void constructSphere(double radius, int stackCount, int sectorCount) 
    {
        double x, y, z, xy;                              // vertex position
        double nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
        double s, t;                                     // vertex texCoord

        double sectorStep = 2 * PI / sectorCount;
        double stackStep = PI / stackCount;
        double sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cos(stackAngle);             // r * cos(u)
            z = radius * sin(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cos(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sin(sectorAngle);             // r * cos(u) * sin(v)
                this->vertices.push_back(x);
                this->vertices.push_back(y);
                this->vertices.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                this->normals.push_back(nx);
                this->normals.push_back(ny);
                this->normals.push_back(nz);

                // vertex tex coord (s, t) range between [0, 1]
                s = (double)j / sectorCount;
                t = (double)i / stackCount;
                this->texCoords.push_back(s);
                this->texCoords.push_back(t);
            }
        }
    }


    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    void constructElIndices(double radius, int stackCount, int sectorCount)
    {
        unsigned int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = (unsigned int)(i * (sectorCount + 1));     // beginning of current stack
            k2 = (unsigned int)(k1 + sectorCount + 1);      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0)
                {
                    this->indices.push_back(k1);
                    this->indices.push_back(k2);
                    this->indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1))
                {
                    this->indices.push_back(k1 + 1);
                    this->indices.push_back(k2);
                    this->indices.push_back(k2 + 1);
                }
            }
        }
    }
};