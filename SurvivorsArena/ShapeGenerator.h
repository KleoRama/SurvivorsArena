#pragma once

#include "MeshData.h"

// ============================================================================
// ShapeGenerator
// ----------------------------------------------------------------------------
// Generatore statico di geometrie 2D.
// ============================================================================

class ShapeGenerator
{
public:
    ShapeGenerator() = delete;

    static MeshData createTriangle(
        const glm::vec4& color1,
        const glm::vec4& color2,
        const glm::vec4& color3
    );

    static MeshData createCircle(
        float cx, float cy,
        float radiusX, float radiusY,
        int segments,
        const glm::vec4& centerColor,
        const glm::vec4& borderColor
    );

    static MeshData createButterfly(
        float cx, float cy,
        float radiusX, float radiusY,
        int segments,
        const glm::vec4& centerColor,
        const glm::vec4& borderColor
    );

    static MeshData createHeart(
        float cx, float cy,
        float radiusX, float radiusY,
        int segments,
        const glm::vec4& centerColor,
        const glm::vec4& borderColor
    );

    static MeshData createMoon(
        float cx, float cy,
        float scaleX, float scaleY,
        int segments,
        const glm::vec4& color
    );

    static MeshData createSierpinski(
        const glm::vec3& T0,
        const glm::vec3& T1,
        const glm::vec3& T2,
        int nPoints,
        const glm::vec4& color
    );
};

