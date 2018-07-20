//
// Created by vlado on 20-Jul-18.
//

#include "Cube.h"
#include <gl/glew.h>
#include <iostream>

elements::Cube::Cube(const glm::vec3 &at, float edge_length) {

    glm::vec3 positions[8] = {
            // bottom square
            (glm::vec3(at.x, at.y, at.z)),                                              // 0
            (glm::vec3(at.x + edge_length, at.y, at.z)),                                // 1
            (glm::vec3(at.x + edge_length, at.y, at.z + edge_length)),                  // 2
            (glm::vec3(at.x, at.y, at.z + edge_length)),                                // 3

            // top square
            (glm::vec3(at.x, at.y + edge_length, at.z)),                                // 4
            (glm::vec3(at.x + edge_length, at.y + edge_length, at.z)),                  // 5
            (glm::vec3(at.x + edge_length, at.y + edge_length, at.z + edge_length)),    // 6
            (glm::vec3(at.x, at.y + edge_length, at.z + edge_length)),                  // 7
    };

    glm::vec2 uv[4]{
            glm::vec2(0.f, 0.f),
            glm::vec2(1.f, 0.f),
            glm::vec2(1.f, 1.f),
            glm::vec2(0.f, 1.f),
    };

    vertices = new util::Vertex[vertexCount]{
            // bottom square
            util::Vertex(positions[0], uv[3]),     // 0
            util::Vertex(positions[1], uv[2]),     // 1
            util::Vertex(positions[2], uv[1]),     // 2
            util::Vertex(positions[3], uv[0]),     // 3
            // top square
            util::Vertex(positions[4], uv[0]),     // 4
            util::Vertex(positions[5], uv[1]),     // 5
            util::Vertex(positions[6], uv[2]),     // 6
            util::Vertex(positions[7], uv[3]),     // 7
            // front square
            util::Vertex(positions[0], uv[0]),     // 8
            util::Vertex(positions[1], uv[1]),     // 9
            util::Vertex(positions[5], uv[2]),     // 10
            util::Vertex(positions[4], uv[3]),     // 11
            // back square
            util::Vertex(positions[2], uv[0]),     // 12
            util::Vertex(positions[6], uv[3]),     // 13
            util::Vertex(positions[7], uv[2]),     // 14
            util::Vertex(positions[3], uv[1]),     // 15
            // left square
            util::Vertex(positions[0], uv[1]),     // 16
            util::Vertex(positions[4], uv[2]),     // 17
            util::Vertex(positions[7], uv[3]),     // 18
            util::Vertex(positions[3], uv[0]),     // 19
            // right square
            util::Vertex(positions[1], uv[0]),     // 20
            util::Vertex(positions[5], uv[3]),     // 21
            util::Vertex(positions[6], uv[2]),     // 22
            util::Vertex(positions[2], uv[1]),     // 23
    };

    indices = new unsigned[indexCount]{
            // bottom
            0, 1, 2,
            0, 2, 3,

            // top
            4, 5, 6,
            4, 6, 7,

            // right
            20, 21, 22,
            20, 22, 23,

            // left
            16, 17, 18,
            16, 18, 19,

            // front
            8, 9, 10,
            8, 10, 11,

            // back
            12, 13, 14,
            12, 14, 15,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
}

elements::Cube::~Cube() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] this->vertices;
    delete[] this->indices;
}

void elements::Cube::setColors(const glm::vec3 *colors) {
    for (int i = 0; i < vertexCount; ++i) {
        this->vertices[i].m_col = colors[i];
    }
}

void elements::Cube::setColors(const glm::vec3 &color) {
    for (int i = 0; i < vertexCount; ++i) {
        this->vertices[i].m_col = color;
    }
}

void elements::Cube::draw() const {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, util::Vertex::size, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, util::Vertex::size, (void *) util::Vertex::color_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, util::Vertex::size, (void *) util::Vertex::uv_offset);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *) 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}