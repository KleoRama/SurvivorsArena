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
    //colori del personaggio
    const glm::vec4 colorArmor(0.38f, 0.40f, 0.46f, 1.0f);
    const glm::vec4 colorCloth(0.18f, 0.22f, 0.28f, 1.0f);
    const glm::vec4 colorCape(0.68f, 0.15f, 0.18f, 1.0f);
    const glm::vec4 colorVisor(0.75f, 0.92f, 1.0f, 1.0f);
    const glm::vec4 colorWood(0.50f, 0.30f, 0.16f, 1.0f);

    // geometrie base condivise tra le varie parti
    const std::size_t triCapeGeo = scene.addGeometry(
        ShapeGenerator::createTriangle(colorCape, colorCape, colorCape) //3 parametri per colore dei 3 vertici
    );
    const std::size_t triArmorGeo = scene.addGeometry(
        ShapeGenerator::createTriangle(colorArmor, colorArmor, colorArmor)
    );
    const std::size_t rectArmorGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(colorArmor, colorArmor)
    );
    const std::size_t rectClothGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(colorCloth, colorCloth)
    );
    const std::size_t rectVisorGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(colorVisor, colorVisor)
    );
    const std::size_t rectWoodGeo = scene.addGeometry(
        ShapeGenerator::createRectangle(colorWood, colorWood)
    );
    const std::size_t circleArmorGeo = scene.addGeometry(
        ShapeGenerator::createCircle(0.0f, 0.0f, 0.5f, 0.5f, 20, colorArmor, colorArmor)
    );

    const float pi = glm::pi<float>();
    const float halfPi = glm::half_pi<float>();

    //aggiunti in questo ordine per rispettare depth-buffer (profondità)

    //mantello dietro al corpo (3 triangoli)
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(-2.3f, -3.5f), glm::vec2(4.6f, 13.5f), pi - 0.14f);
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(0.0f, -3.8f), glm::vec2(5.0f, 14.0f), pi);
    addPart(m_bodyParts, scene, triCapeGeo, glm::vec2(2.3f, -3.5f), glm::vec2(4.6f, 13.5f), pi + 0.14f);

    // gambe e stivali (divisi in sinistra e destra per l'animazione)
    addPart(m_leftLegParts, scene, circleArmorGeo, glm::vec2(-1.8f, -6.2f), glm::vec2(2.2f, 8.2f));
    addPart(m_leftLegParts, scene, triArmorGeo, glm::vec2(-2.0f, -10.8f), glm::vec2(2.4f, 1.8f));
    addPart(m_rightLegParts, scene, circleArmorGeo, glm::vec2(1.8f, -6.2f), glm::vec2(2.2f, 8.2f));
    addPart(m_rightLegParts, scene, triArmorGeo, glm::vec2(2.0f, -10.8f), glm::vec2(2.4f, 1.8f));

    //busto, cintura, braccia e spallacci
    addPart(m_bodyParts, scene, rectClothGeo, glm::vec2(-3.5f, 0.8f), glm::vec2(1.4f, 4.0f));
    addPart(m_bodyParts, scene, rectClothGeo, glm::vec2(3.5f, 0.8f), glm::vec2(1.4f, 4.0f));
    addPart(m_bodyParts, scene, rectArmorGeo, glm::vec2(0.0f, 0.8f), glm::vec2(6.0f, 6.0f));
    addPart(m_bodyParts, scene, rectClothGeo, glm::vec2(0.0f, -1.8f), glm::vec2(6.2f, 1.1f));
    addPart(m_bodyParts, scene, circleArmorGeo, glm::vec2(-3.2f, 3.0f), glm::vec2(2.6f, 2.4f));
    addPart(m_bodyParts, scene, circleArmorGeo, glm::vec2(3.2f, 3.0f), glm::vec2(2.6f, 2.4f));

    //elmo e visiera
    addPart(m_bodyParts, scene, circleArmorGeo, glm::vec2(0.0f, 7.6f), glm::vec2(4.6f, 2.6f));
    addPart(m_bodyParts, scene, rectArmorGeo, glm::vec2(0.0f, 5.7f), glm::vec2(4.6f, 4.0f));
    addPart(m_bodyParts, scene, rectVisorGeo, glm::vec2(0.0f, 6.25f), glm::vec2(2.8f, 0.7f));
    addPart(m_bodyParts, scene, rectVisorGeo, glm::vec2(0.0f, 5.15f), glm::vec2(0.75f, 2.4f));

    // balestra che ruota verso il mouse
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(8.2f, 0.0f), glm::vec2(7.6f, 1.25f));
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(10.9f, 2.25f), glm::vec2(0.75f, 4.8f), 0.48f);
    addPart(m_weaponParts, scene, rectWoodGeo, glm::vec2(10.9f, -2.25f), glm::vec2(0.75f, 4.8f), -0.48f);
    addPart(m_weaponParts, scene, rectArmorGeo, glm::vec2(9.6f, 0.0f), glm::vec2(7.6f, 0.4f));
    addPart(m_weaponParts, scene, triArmorGeo, glm::vec2(14.0f, 0.0f), glm::vec2(1.5f, 1.4f), -halfPi);

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

void Player::updateTransform(
    Scene& scene,
    const glm::vec2& position,
    float aimAngleRadians,
    bool isMoving,
    float dt)
{
    if (isMoving)
        m_walkPhase += dt * 11.0f;
    else
        m_walkPhase = 0.0f;

    const float stepSin = std::sin(m_walkPhase);
    const float bobY = isMoving ? (std::abs(stepSin) * 0.7f) : 0.0f; //y del movimento del busto
    const float bodyTilt = isMoving ? (stepSin * 0.06f) : 0.0f; //inclinazione busto
    const float legStep = isMoving ? (stepSin * 1.4f) : 0.0f; //passo delle gambe

    const glm::mat4 worldRoot = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(position.x, position.y, 0.0f)
    );

    //gambe che si muovono alternate mentre cammina
    const glm::mat4 worldLeftLeg = glm::translate(
        worldRoot,
        glm::vec3(0.0f, legStep, 0.0f)
    );
    for (const Part& part : m_leftLegParts)
        scene.instance(part.instanceIndex).modelMatrix = worldLeftLeg * part.localTransform;

    const glm::mat4 worldRightLeg = glm::translate(
        worldRoot,
        glm::vec3(0.0f, -legStep, 0.0f)
    );
    for (const Part& part : m_rightLegParts)
        scene.instance(part.instanceIndex).modelMatrix = worldRightLeg * part.localTransform;

    // leggero movimento del busto mentre cammina
    glm::mat4 worldBody = glm::translate(worldRoot, glm::vec3(0.0f, bobY, 0.0f));
    if (bodyTilt != 0.0f)
        worldBody = glm::rotate(worldBody, bodyTilt, glm::vec3(0.0f, 0.0f, 1.0f));

    for (const Part& part : m_bodyParts)
        scene.instance(part.instanceIndex).modelMatrix = worldBody * part.localTransform;

    //la balestra ruota attorno al petto puntando verso il mouse
    const glm::vec2 shoulderOffset(0.0f, 1.5f + bobY * 0.5f);
    glm::mat4 worldWeapon = glm::translate(
        worldRoot,
        glm::vec3(shoulderOffset.x, shoulderOffset.y, 0.0f)
    );
    worldWeapon = glm::rotate(worldWeapon, aimAngleRadians, glm::vec3(0.0f, 0.0f, 1.0f));

    for (const Part& part : m_weaponParts)
        scene.instance(part.instanceIndex).modelMatrix = worldWeapon * part.localTransform;
}
