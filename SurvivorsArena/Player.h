#pragma once

#include <cstddef>
#include <vector>

#include <glm/glm.hpp>

class Scene;

// rappresenta il player assemblando
// forme primitive tramite trasformazioni geometriche.
class Player
{
public:
    explicit Player(Scene& scene);

    //aggiorna posizione del player, animazione passi e rotazione della balestra
    void updateTransform(
        Scene& scene,
        const glm::vec2& position,
        float aimAngleRadians,
        bool isMoving = false,
        float dt = 0.0f
    );

private:
    struct Part
    {
        std::size_t instanceIndex = 0;
        glm::mat4 localTransform = glm::mat4(1.0f);
    };

    void addPart(
        std::vector<Part>& targetList,
        Scene& scene,
        std::size_t geometryIndex,
        const glm::vec2& offset,
        const glm::vec2& scale,
        float rotationRadians = 0.0f
    );

    std::vector<Part> m_bodyParts;
    std::vector<Part> m_leftLegParts;
    std::vector<Part> m_rightLegParts;
    std::vector<Part> m_weaponParts;
    float m_walkPhase = 0.0f;
};

