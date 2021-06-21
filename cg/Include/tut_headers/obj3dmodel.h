#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <glad/glad.h>

class obj3dmodel
{
public:
    struct vertex {
        double x;
        double y;
        double z;
    };
    struct face {
        unsigned int v1, v2, v3;
    };
    std::vector<vertex> vertices;
    std::vector<face> faces;

    int nv, nf;

    obj3dmodel(const char* filename) {
        readfile(filename);
    }

private:
    void readfile(const char* filename)
    {
        std::string s;
        std::ifstream fin(filename);
        if (!fin)
            return;
        while (fin >> s)
        {
            switch (*s.c_str())
            {
            case 'v':
            {
                vertex v;
                fin >> v.x >> v.y >> v.z;
                this->vertices.push_back(v);
            }
            break;
            case 'f':
            {
                face f;
                fin >> f.v1 >> f.v2 >> f.v3;
                this->faces.push_back(f);
            }
            break;
            case '#':
            {
                std::string tmp;
                fin >> tmp;
                if (tmp == "vertices:") {
                    fin >> this->nv;
                }
                else if (tmp == "faces:") {
                    fin >> this->nf;
                }
            }
            break;
            }
        }
    }

}; 
