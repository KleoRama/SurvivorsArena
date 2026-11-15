#include "Player.h"

#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scene.h"
#include "ShapeGenerator.h"

namespace
{
    glm::mat4 makeLocalTransform(
        const glm::vec2& offset,
        const glm::vec2& scale,
        float rotationRadians)
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(offset.x, offset.y, 0.0f));
        if (rotationRadians != 0.0f)
            m = glm::rotate(m, rotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, glm::vec3(scale.x, scale.y, 1.0f));
        return m;
    }
}

Player::Player(Scene& scene)
{
    // Colori 
    const glm::vec4 steelDark(0.22f, 0.23f, 0.26f, 1.0f);
    const glm::vec4 steelLight(0.46f, 0.48f, 0.53f, 1.0f);
    const glm::vec4 crimsonDark(0.32f, 0.08f, 0.09f, 1.0f);
    const glm::vec4 crimsonLight(0.52f, 0.14f, 0.15f, 1.0f);
    const glm::vec4 capeDark(0.14f, 0.06f, 0.07f, 1.0f);
    const glm::vec4 capeTop(0.28f, 0.11f, 0.11f, 1.0f);
    const glm::vec4 visorRed(1.0f, 0.16f, 0.12f, 1.0f);
    const glm::vec4 visorOrange(1.0f, 0.30f, 0.18f, 1.0f);
    const glm::vec4 woodDark(0.24f, 0.13f, 0.13f, 1.0f);
    const glm::vec4 woodLight(0.42f, 0.22f, 0.21f, 1.0f);

    // Poche geometrie base condivise tra tutte le parti del personaggio
    const std::size_t triCapeGeo = scene.addGeometry(
        ShapeGenerator::createTriangle(capeTop, capeTop, capeDark)
    );
    const std::size_t triSteelGeo = scene.addGeometry(
        ShapeGenerator::createTriangle(steelDark, steelDark, steelLight)
    );
    const std::size_t rectSteelGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(steelDark, steelLight)
    );
    const std::size_t circleSteelGeo = scene.addGeometry(
        ShapeGenerator::createCircle(0.0f, 0.0f, 0.5f, 0.5f, 20, steelLight, steelDark)
    );
    const std::size_t rectCrimsonGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(crimsonDark, crimsonLight)
    );
    const std::size_t rectVisorGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(visorRed, visorOrange)
    );
    const std::size_t rectWoodGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(woodDark, woodLight)
    );

    const float pi = glm::pi<float>();
    const float halfPi = glm::half_pi<float>();

    // 1. Mantello dietro al corpo (3 triangoli puntati verso il basso)
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(-2.3f, -3.5f), glm::vec2(4.6f, 13.5f), pi - 0.14f);
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(0.0f, -3.8f), glm::vec2(5.0f, 14.0f), pi);
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(2.3f, -3.5f), glm::vec2(4.6f, 13.5f), pi + 0.14f);

    // 2. Gambe e stivali
    addPart(m_bodyParts, scene, circleSteelGeo, glm::vec2(-1.8f, -6.2f), glm::vec2(2.2f, 8.2f));
    addPart(m_bodyParts, scene, circleSteelGeo, glm::vec2(1.8f, -6.2f), glm::vec2(2.2f, 8.2f));
    addPart(m_bodyParts, scene, triSteelGeo, glm::vec2(-2.0f, -10.8f), glm::vec2(2.4f, 1.8f));
    addPart(m_bodyParts, scene, triSteelGeo, glm::vec2(2.0f, -10.8f), glm::vec2(2.4f, 1.8f));

    // 3. Busto, cintura, braccia statiche e spallacci
    addPart(m_bodyParts, scene, rectCrimsonGeo, glm::vec2(-3.5f, 0.8f), glm::vec2(1.4f, 4.0f));
    addPart(m_bodyParts, scene, rectCrimsonGeo, glm::vec2(3.5f, 0.8f), glm::vec2(1.4f, 4.0f));
    addPart(m_bodyParts, scene, rectSteelGeo, glm::vec2(0.0f, 0.8f), glm::vec2(6.0f, 6.0f));
    addPart(m_bodyParts, scene, rectCrimsonGeo, glm::vec2(0.0f, -1.8f), glm::vec2(6.2f, 1.1f));
    addPart(m_bodyParts, scene, circleSteelGeo, glm::vec2(-3.2f, 3.0f), glm::vec2(2.6f, 2.4f));
    addPart(m_bodyParts, scene, circleSteelGeo, glm::vec2(3.2f, 3.0f), glm::vec2(2.6f, 2.4f));

    // 4. Elmo e visiera a "T" rossa
    addPart(m_bodyParts, scene, circleSteelGeo, glm::vec2(0.0f, 7.6f), glm::vec2(4.6f, 2.6f));
    addPart(m_bodyParts, scene, rectSteelGeo, glm::vec2(0.0f, 5.7f), glm::vec2(4.6f, 4.0f));
    addPart(m_bodyParts, scene, rectVisorGeo, glm::vec2(0.0f, 6.25f), glm::vec2(2.8f, 0.7f));
    addPart(m_bodyParts, scene, rectVisorGeo, glm::vec2(0.0f, 5.15f), glm::vec2(0.75f, 2.4f));

    // 5. Balestra (orbita intorno al player puntando verso il cursore del mouse)
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(8.2f, 0.0f), glm::vec2(7.6f, 1.25f));
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(10.9f, 2.25f), glm::vec2(0.75f, 4.8f), 0.48f);
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(10.9f, -2.25f), glm::vec2(0.75f, 4.8f), -0.48f);
    addPart(m_weaponParts, scene, rectSteelGeo, glm::vec2(9.6f, 0.0f), glm::vec2(7.6f, 0.4f));
    addPart(m_weaponParts, scene, triSteelGeo, glm::vec2(14.0f, 0.0f), glm::vec2(1.5f, 1.4f), -halfPi);

    updateTransform(scene, glm::vec2(0.0f, 0.0f), 0.0f);
}

void Player::addPart(
    std::vector<Part>& targetList,
    Scene& scene,
    std::size_t geometryIndex,
    const glm::vec2& offset,
    const glm::vec2& scale,
    float rotationRadians)
{
    Part part;
    part.instanceIndex = scene.addInstance(geometryIndex);
    part.localTransform = makeLocalTransform(offset, scale, rotationRadians);
    targetList.push_back(part);
}

void Player::updateTransform(Scene& scene, const glm::vec2& position, float aimAngleRadians)
{
    const glm::mat4 worldBody = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(position.x, position.y, 0.0f)
    );

    for (const Part& part : m_bodyParts)
        scene.instance(part.instanceIndex).modelMatrix = worldBody * part.localTransform;

    // La balestra ruota attorno al petto; quando miro a sinistra si specchia in Y per non capovolgersi
    const glm::vec2 shoulderOffset(0.0f, 1.5f);
    glm::mat4 worldWeapon = glm::translate(
        worldBody,
        glm::vec3(shoulderOffset.x, shoulderOffset.y, 0.0f)
    );
    worldWeapon = glm::rotate(worldWeapon, aimAngleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    if (std::cos(aimAngleRadians) < 0.0f)
        worldWeapon = glm::scale(worldWeapon, glm::vec3(1.0f, -1.0f, 1.0f));

    for (const Part& part : m_weaponParts)
        scene.instance(part.instanceIndex).modelMatrix = worldWeapon * part.localTransform;
}
