#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

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
        int v1, v2, v3;
    };
    struct vec {
        double x;
        double y;
        double z;
    };

    std::vector<vertex> vertices; //nv
    std::vector<face> faces; //nf
    std::vector<vec> face_norms; //nf
    std::vector<vec> vertex_norms; //nv

    int nv, nf;

    obj3dmodel(const char* filename) {
        readfile(filename);
        calc_face_normals();
        calc_vertex_normals();
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

    void calc_face_normals()
    {
        for (int i=0; i < this->nf; i++) {
            face curr_f = this->faces[i];
            vertex v1 = this->vertices[curr_f.v1 - 1];
            vertex v2 = this->vertices[curr_f.v2 - 1];
            vertex v3 = this->vertices[curr_f.v3 - 1];

            vec e1, e2;
            e1.x = v2.x - v1.x;
            e1.y = v2.y - v1.y;
            e1.z = v2.z - v1.z;

            e2.x = v3.x - v1.x;
            e2.y = v3.y - v1.y;
            e2.z = v3.z - v1.z;

            vec facenorm;
            facenorm.x = (e1.y * e2.z) - (e1.z * e2.y);
            facenorm.y = (e1.z * e2.x) - (e1.x * e2.z);
            facenorm.z = (e1.x * e2.y) - (e1.y * e2.x);

            double mag = sqrt(facenorm.x*facenorm.x + facenorm.y*facenorm.y + facenorm.z*facenorm.z);
            facenorm.x /= mag;
            facenorm.y /= mag;
            facenorm.z /= mag;

            this->face_norms.push_back(facenorm);
        }
    }

    void calc_vertex_normals()
    {
        for (int v=0; v < this->nv; v++) {
            //get all the face norms associated with vertex v
            std::vector<vec> vnorms;
            for (int f=0; f < nf; f++) {
                face curr_f = this->faces[f];
                int v1_ind = curr_f.v1 - 1;
                int v2_ind = curr_f.v2 - 1;
                int v3_ind = curr_f.v3 - 1;

                if (v1_ind == v || v2_ind == v || v3_ind == v) {
                    vnorms.push_back(this->face_norms[f]);
                }
            }

            //calculate average norm for vertex v
            vec vnorm;
            vnorm.x = 0;
            vnorm.y = 0;
            vnorm.z = 0;
            for (int i=0; i < vnorms.size(); i++) {
                vnorm.x += vnorms[i].x;
                vnorm.y += vnorms[i].y;
                vnorm.z += vnorms[i].z;
            }

            //normalise v norm
            double mag = sqrt(vnorm.x*vnorm.x + vnorm.y*vnorm.y + vnorm.z*vnorm.z);
            vnorm.x /= mag;
            vnorm.y /= mag;
            vnorm.z /= mag;

            this->vertex_norms.push_back(vnorm);
        }
    }

}; 
