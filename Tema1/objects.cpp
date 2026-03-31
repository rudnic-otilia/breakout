#include "objects.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* objects::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* objects::CreateStartButton(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3((float)length / 2, (float)length / 2, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 
                                          0, 3, 4 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* objects::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    const int numSegments = 50;
    const float PI = 3.1415926f;

    //Adaugam centrul cercului
    vertices.push_back(VertexFormat(center, color));

    //Generam punctele de pe circumferinta
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2 * PI * i / numSegments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, y, center.z), color));
        indices.push_back(i + 1);
    }

    Mesh* circle = new Mesh(name);

    //Alegem modul de desen
    if (!fill) {
        // doar conturul cercului
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // facem triunghiuri in evantai (GL_TRIANGLE_FAN)
        for (int i = 1; i <= numSegments; i++) {
            indices.push_back(0);         // centrul
            indices.push_back(i);         // punctul curent
            indices.push_back(i + 1);     // urmatorul punct
        }
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

