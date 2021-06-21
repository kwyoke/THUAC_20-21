//
// Created by Tong Gao on 2020/3/28.
//

#ifndef ASSIGNMENT1_OBJ_LOADER_H
#define ASSIGNMENT1_OBJ_LOADER_H

#include <iostream>
#include <fstream>
#include <array>

// GLAD
#include <glad/glad.h>

class ObjLoader {
public:
    ObjLoader() = default;

    ~ObjLoader() {
        if (isData) {
            delete[] vertices;
        }
    }

    int load(const char* filename) {
        std::ifstream input(filename);
        if (!input.is_open())
            return 1;

        std::string s;
        int nv;
        input >> s >> s >> nv;
        input >> s >> s >> nf;

        vertices = new GLfloat[18 * nf];
        isData = true;

        std::vector<std::array<GLfloat, 3>> v;
        GLfloat x, y, z;
        for (size_t i = 0; i < nv; ++i) {
            input >> s >> x >> y >> z;
            std::array<GLfloat, 3> pos = {x, y, z};
            v.push_back(pos);
        }

        std::vector<std::array<int, 3>> f;
        int a, b, c;
        for (size_t i = 0; i < nf; ++i) {
            input >> s >> a >> b >> c;
            std::array<int, 3> triangle = {a, b, c};
            f.push_back(triangle);
        }

        input.close();

        for (size_t i = 0; i < nf; ++i) {
            vertices[18 * i]      = v[f[i][0]-1][0];
            vertices[18 * i + 1]  = v[f[i][0]-1][1];
            vertices[18 * i + 2]  = v[f[i][0]-1][2];

            vertices[18 * i + 6]  = v[f[i][1]-1][0];
            vertices[18 * i + 7]  = v[f[i][1]-1][1];
            vertices[18 * i + 8]  = v[f[i][1]-1][2];

            vertices[18 * i + 12] = v[f[i][2]-1][0];
            vertices[18 * i + 13] = v[f[i][2]-1][1];
            vertices[18 * i + 14] = v[f[i][2]-1][2];

            vertices[18 * i + 3] = vertices[18 * i + 9]  = vertices[18 * i + 15] =
                    (vertices[18 * i]     + vertices[18 * i + 6] + vertices[18 * 12]) / 3;
            vertices[18 * i + 4] = vertices[18 * i + 10] = vertices[18 * i + 16] =
                    (vertices[18 * i + 1] + vertices[18 * i + 7] + vertices[18 * 13]) / 3;
            vertices[18 * i + 5] = vertices[18 * i + 11] = vertices[18 * i + 17] =
                    (vertices[18 * i + 2] + vertices[18 * i + 8] + vertices[18 * 14]) / 3;
        }

        return 0;
    }

    GLfloat* getData() { return vertices; }
    GLint getNf() { return nf; }

private:
    bool isData;
    GLfloat *vertices;
    GLint nf;
};


#endif //ASSIGNMENT1_OBJ_LOADER_H
