#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "BackgroundRenderer.h"
#include "Gui.h"
#include "Input.h"
#include "Player.h"
#include "PlayerShapeLoader.h"
#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"
#include "ShapeGenerator.h"

namespace
{
    constexpr int INITIAL_WIDTH = 1100;
    constexpr int INITIAL_HEIGHT = 700;
    constexpr float WORLD_HALF_HEIGHT = 100.0f;

    // limiti del mondo e raggi per le collisioni
    constexpr float PLAYER_MARGIN_X = 6.0f;
    constexpr float PLAYER_MARGIN_Y = 12.0f;
    constexpr float PLAYER_RADIUS = 7.5f;
    constexpr int MAX_LIVES = 5;
    constexpr float HEART_HUD_SCALE = 5.5f;

    constexpr int MAX_PROJECTILES = 30;
    constexpr float PROJECTILE_LENGTH = 5.0f;
    constexpr float PROJECTILE_THICKNESS = 1.1f;
    constexpr float PROJECTILE_RADIUS = 2.5f;
    constexpr float MUZZLE_DISTANCE = 14.5f;

    constexpr int MAX_BATS = 15;
    constexpr int BAT_MAX_HP = 2; //muoiono con 2 colpi
    constexpr float BAT_OBJ_SCALE = 22.0f;
    constexpr float BAT_RADIUS = 7.0f;

    constexpr int MAX_GHOSTS = 10;
    constexpr int GHOST_MAX_HP = 2;
    constexpr float GHOST_OBJ_SCALE = 24.0f;
    constexpr float GHOST_RADIUS = 7.0f;
    constexpr float GHOST_MIN_DIST = 55.0f;
    constexpr float GHOST_MAX_DIST = 78.0f;
    constexpr float GHOST_SHOOT_INTERVAL = 2.2f;

    constexpr int MAX_ENEMY_PELLETS = 40;
    constexpr float PELLET_RADIUS = 2.2f;
    constexpr float PELLET_SPEED = 62.0f;

    constexpr float ENEMY_RESPAWN_TIME = 3.0f; //tempo di respawn dei nemici

    constexpr int MAX_GEMS = 40;
    constexpr float GEM_SCALE = 5.5f;
    constexpr float GEM_RADIUS = 3.6f;
    constexpr float GEM_LIFETIME = 8.0f;      // secondi prima che la gemma sparisca
    constexpr float GEM_WARNING_TIME = 2.5f;  //negli ultimi secondi pulsa piu' veloce

    struct Projectile
    {
        std::size_t instanceIndex = 0;
        glm::vec2 position{0.0f};
        glm::vec2 velocity{0.0f};
        float angleRadians = 0.0f;
        bool active = false;
    };

    struct Bat
    {
        std::size_t instanceIndex = 0;
        glm::vec2 position{0.0f};
        float respawnTimer = 0.0f;
        int hp = BAT_MAX_HP;
        bool alive = true;
    };

    struct Ghost
    {
        std::size_t instanceIndex = 0;
        glm::vec2 position{0.0f};
        float shootTimer = 1.5f;
        float respawnTimer = 0.0f;
        int hp = GHOST_MAX_HP;
        bool alive = true;
    };

    struct EnemyPellet
    {
        std::size_t instanceIndex = 0;
        glm::vec2 position{0.0f};
        glm::vec2 velocity{0.0f};
        bool active = false;
    };

    struct Gem
    {
        std::size_t instanceIndex = 0;
        glm::vec2 position{0.0f};
        float timeLeft = 0.0f;
        float animTime = 0.0f;
        bool active = false;
    };

    std::mt19937 randomEngine(static_cast<unsigned int>(std::time(nullptr))); //seed per lo spawn casuale

    float randomBetween(float lo, float hi)
    {
        std::uniform_real_distribution<float> dist(lo, hi);
        return dist(randomEngine);
    }

    //mette un pipistrello lontano dal player e dagli altri
    void respawnBat(
        Bat& b,
        const std::vector<Bat>& bats,
        const Scene& scene,
        const glm::vec2& playerPos)
    {
        const float xmin = -135.0f;
        const float xmax = 135.0f;
        const float ymin = -88.0f;
        const float ymax = 88.0f;
        const float minPlayerDist = 60.0f;
        const float minBatDist = BAT_RADIUS * 2.0f + 2.0f;

        b.hp = BAT_MAX_HP;
        b.alive = true;
        b.respawnTimer = 0.0f;

        for (int attempt = 0; attempt < 400; ++attempt)
        {
            const glm::vec2 candidate(randomBetween(xmin, xmax), randomBetween(ymin, ymax));
            if (glm::distance(candidate, playerPos) < minPlayerDist)
                continue;

            bool overlapsOther = false;
            for (const Bat& other : bats)
            {
                if (&other == &b || other.instanceIndex == b.instanceIndex)
                    continue;
                if (!other.alive || !scene.instance(other.instanceIndex).visible)
                    continue;
                if (glm::distance(candidate, other.position) < minBatDist)
                {
                    overlapsOther = true;
                    break;
                }
            }

            if (!overlapsOther)
            {
                b.position = candidate;
                return;
            }
        }

        b.position = glm::vec2(xmin, ymax);
    }

    //posiziona un fantasma a distanza dal player
    void respawnGhost(
        Ghost& g,
        const std::vector<Ghost>& ghosts,
        const Scene& scene,
        const glm::vec2& playerPos)
    {
        const float xmin = -130.0f;
        const float xmax = 130.0f;
        const float ymin = -82.0f;
        const float ymax = 82.0f;
        const float minPlayerDist = 65.0f;
        const float minGhostDist = GHOST_RADIUS * 2.0f + 4.0f;

        g.hp = GHOST_MAX_HP;
        g.alive = true;
        g.respawnTimer = 0.0f;
        g.shootTimer = randomBetween(0.8f, GHOST_SHOOT_INTERVAL);

        for (int attempt = 0; attempt < 400; ++attempt)
        {
            const glm::vec2 candidate(randomBetween(xmin, xmax), randomBetween(ymin, ymax));
            if (glm::distance(candidate, playerPos) < minPlayerDist)
                continue;

            bool overlapsOther = false;
            for (const Ghost& other : ghosts)
            {
                if (&other == &g || other.instanceIndex == g.instanceIndex)
                    continue;
                if (!other.alive || !scene.instance(other.instanceIndex).visible)
                    continue;
                if (glm::distance(candidate, other.position) < minGhostDist)
                {
                    overlapsOther = true;
                    break;
                }
            }

            if (!overlapsOther)
            {
                g.position = candidate;
                return;
            }
        }

        g.position = glm::vec2(xmax, ymax);
    }

    // attiva un pallino bianco sparato dal fantasma
    void fireEnemyPellet(
        std::vector<EnemyPellet>& pellets,
        const glm::vec2& origin,
        const glm::vec2& direction)
    {
        for (EnemyPellet& pellet : pellets)
        {
            if (pellet.active)
                continue;

            pellet.active = true;
            pellet.position = origin;
            pellet.velocity = direction * PELLET_SPEED;
            return;
        }
    }

    //cerca il primo proiettile libero nel pool e lo fa partire
    void fireProjectile(
        std::vector<Projectile>& projectiles,
        int maxAllowed,
        const glm::vec2& origin,
        const glm::vec2& direction,
        float angleRadians,
        float speed)
    {
        int activeCount = 0;
        for (const Projectile& p : projectiles)
        {
            if (p.active)
                ++activeCount;
        }
        if (activeCount >= maxAllowed)
            return;

        for (Projectile& p : projectiles)
        {
            if (p.active)
                continue;

            p.active = true;
            p.position = origin;
            p.velocity = direction * speed;
            p.angleRadians = angleRadians;
            return;
        }
    }

    // muove i pipistrelli verso il player e controlla il contatto
    void updateBats(
        Scene& scene,
        std::vector<Bat>& bats,
        int maxActiveBats,
        float batSpeed,
        const glm::vec2& playerPos,
        int& playerLives,
        float dt)
    {
        for (std::size_t i = 0; i < bats.size(); ++i)
        {
            Bat& b = bats[i];
            GeometryInstance& instance = scene.instance(b.instanceIndex);

            if (static_cast<int>(i) >= maxActiveBats)
            {
                instance.visible = false;
                continue;
            }

            if (!b.alive)
            {
                b.respawnTimer -= dt;
                if (b.respawnTimer <= 0.0f)
                {
                    respawnBat(b, bats, scene, playerPos);
                }
                else
                {
                    instance.visible = false;
                    continue;
                }
            }

            instance.visible = true;

            if (playerLives > 0)
            {
                const glm::vec2 toPlayer = playerPos - b.position;
                const float dist = glm::length(toPlayer);
                if (dist > 0.001f)
                {
                    const glm::vec2 dir = toPlayer / dist;
                    b.position += dir * batSpeed * dt;
                }

                //se tocca il player toglie un cuore
                const glm::vec2 diffPlayer = b.position - playerPos;
                const float hitRadius = BAT_RADIUS + PLAYER_RADIUS;
                if (glm::dot(diffPlayer, diffPlayer) <= hitRadius * hitRadius)
                {
                    playerLives = std::max(0, playerLives - 1);

                    //rimbalza indietro dopo aver colpito
                    if (dist > 0.001f)
                        b.position -= (toPlayer / dist) * 38.0f;
                    else
                        b.position.x += 38.0f;
                }
            }
        }

        // evita che i pipistrelli si sovrappongano tra loro
        for (std::size_t i = 0; i < bats.size(); ++i)
        {
            if (!bats[i].alive || !scene.instance(bats[i].instanceIndex).visible)
                continue;

            for (std::size_t j = i + 1; j < bats.size(); ++j)
            {
                if (!bats[j].alive || !scene.instance(bats[j].instanceIndex).visible)
                    continue;

                const glm::vec2 diff = bats[j].position - bats[i].position;
                const float dist = glm::length(diff);
                const float minDist = BAT_RADIUS * 2.0f;
                if (dist < minDist)
                {
                    const glm::vec2 normal = (dist > 0.0001f) ? (diff / dist) : glm::vec2(1.0f, 0.0f);
                    const float overlap = 0.5f * (minDist - dist);
                    bats[i].position -= normal * overlap;
                    bats[j].position += normal * overlap;
                }
            }
        }

        //aggiorna la model matrix (se ha 1 solo hp diventa un po' piu piccolo)
        for (const Bat& b : bats)
        {
            GeometryInstance& instance = scene.instance(b.instanceIndex);
            if (!instance.visible)
                continue;

            const float scale = (b.hp == 1) ? (BAT_OBJ_SCALE * 0.80f) : BAT_OBJ_SCALE;
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(b.position, 0.0f));
            model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
            instance.modelMatrix = model;
        }
    }

    //movimento lento dei fantasmi e sparo dei pallini bianchi
    void updateGhosts(
        Scene& scene,
        std::vector<Ghost>& ghosts,
        std::vector<EnemyPellet>& pellets,
        int maxActiveGhosts,
        float ghostSpeed,
        const glm::vec2& playerPos,
        int playerLives,
        float halfWidth,
        float halfHeight,
        float dt)
    {
        for (std::size_t i = 0; i < ghosts.size(); ++i)
        {
            Ghost& g = ghosts[i];
            GeometryInstance& instance = scene.instance(g.instanceIndex);

            if (static_cast<int>(i) >= maxActiveGhosts)
            {
                instance.visible = false;
                continue;
            }

            if (!g.alive)
            {
                g.respawnTimer -= dt;
                if (g.respawnTimer <= 0.0f)
                {
                    respawnGhost(g, ghosts, scene, playerPos);
                }
                else
                {
                    instance.visible = false;
                    continue;
                }
            }

            instance.visible = true;

            if (playerLives > 0)
            {
                const glm::vec2 toPlayer = playerPos - g.position;
                const float dist = glm::length(toPlayer);
                const glm::vec2 dirToPlayer = (dist > 0.001f) ? (toPlayer / dist) : glm::vec2(1.0f, 0.0f);

                //se e' lontano si avvicina, se il player si avvicina troppo scappa indietro
                if (dist > GHOST_MAX_DIST)
                {
                    g.position += dirToPlayer * ghostSpeed * dt;
                }
                else if (dist < GHOST_MIN_DIST)
                {
                    g.position -= dirToPlayer * ghostSpeed * dt;
                }
                else
                {
                    //alla giusta distanza si muove un po' di lato
                    const glm::vec2 tangent(-dirToPlayer.y, dirToPlayer.x);
                    g.position += tangent * (ghostSpeed * 0.35f) * dt;
                }

                // non esce dallo schermo
                g.position.x = std::clamp(g.position.x, -halfWidth + GHOST_RADIUS + 2.0f, halfWidth - GHOST_RADIUS - 2.0f);
                g.position.y = std::clamp(g.position.y, -halfHeight + GHOST_RADIUS + 2.0f, halfHeight - GHOST_RADIUS - 2.0f);

                //sparo del pallino bianco
                g.shootTimer -= dt;
                if (g.shootTimer <= 0.0f)
                {
                    fireEnemyPellet(pellets, g.position, dirToPlayer);
                    g.shootTimer = GHOST_SHOOT_INTERVAL;
                }
            }
        }

        //separazione tra fantasmi vicini
        for (std::size_t i = 0; i < ghosts.size(); ++i)
        {
            if (!ghosts[i].alive || !scene.instance(ghosts[i].instanceIndex).visible)
                continue;

            for (std::size_t j = i + 1; j < ghosts.size(); ++j)
            {
                if (!ghosts[j].alive || !scene.instance(ghosts[j].instanceIndex).visible)
                    continue;

                const glm::vec2 diff = ghosts[j].position - ghosts[i].position;
                const float dist = glm::length(diff);
                const float minDist = GHOST_RADIUS * 2.2f;
                if (dist < minDist)
                {
                    const glm::vec2 normal = (dist > 0.0001f) ? (diff / dist) : glm::vec2(1.0f, 0.0f);
                    const float overlap = 0.5f * (minDist - dist);
                    ghosts[i].position -= normal * overlap;
                    ghosts[j].position += normal * overlap;
                }
            }
        }

        // aggiorna la model matrix dei fantasmi
        for (const Ghost& g : ghosts)
        {
            GeometryInstance& instance = scene.instance(g.instanceIndex);
            if (!instance.visible)
                continue;

            const float scale = (g.hp == 1) ? (GHOST_OBJ_SCALE * 0.80f) : GHOST_OBJ_SCALE;
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(g.position, 0.0f));
            model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
            instance.modelMatrix = model;
        }
    }

    //aggiorna i pallini sparati dai fantasmi
    void updateEnemyPellets(
        Scene& scene,
        std::vector<EnemyPellet>& pellets,
        const glm::vec2& playerPos,
        int& playerLives,
        float halfWidth,
        float halfHeight,
        float dt)
    {
        for (EnemyPellet& pellet : pellets)
        {
            if (!pellet.active)
                continue;

            pellet.position += pellet.velocity * dt;

            if (std::abs(pellet.position.x) > halfWidth + 5.0f ||
                std::abs(pellet.position.y) > halfHeight + 5.0f)
            {
                pellet.active = false;
                continue;
            }

            if (playerLives > 0)
            {
                const glm::vec2 diff = pellet.position - playerPos;
                const float hitRadius = PELLET_RADIUS + PLAYER_RADIUS;
                if (glm::dot(diff, diff) <= hitRadius * hitRadius)
                {
                    pellet.active = false;
                    playerLives = std::max(0, playerLives - 1);
                }
            }
        }

        for (const EnemyPellet& pellet : pellets)
        {
            GeometryInstance& instance = scene.instance(pellet.instanceIndex);
            instance.visible = pellet.active;
            if (pellet.active)
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(pellet.position, 0.0f));
                instance.modelMatrix = model;
            }
        }
    }

    //fa cadere una gemma dove e' morto il nemico
    void spawnGem(std::vector<Gem>& gems, const glm::vec2& position)
    {
        for (Gem& gem : gems)
        {
            if (gem.active)
                continue;

            gem.active = true;
            gem.position = position;
            gem.timeLeft = GEM_LIFETIME;
            gem.animTime = 0.0f;
            return;
        }
    }

    // gestisce le gemme a terra (animazione, timer e raccolta)
    void updateGems(
        Scene& scene,
        std::vector<Gem>& gems,
        const glm::vec2& playerPos,
        int playerLives,
        float dt,
        int& score)
    {
        for (Gem& gem : gems)
        {
            if (!gem.active)
                continue;

            gem.timeLeft -= dt;
            if (gem.timeLeft <= 0.0f)
            {
                gem.active = false;
                continue;
            }

            //quando sta per sparire pulsa piu velocemente
            const float pulseSpeed = (gem.timeLeft <= GEM_WARNING_TIME) ? 20.0f : 6.0f;
            gem.animTime += dt * pulseSpeed;

            if (playerLives > 0)
            {
                const glm::vec2 diff = gem.position - playerPos;
                const float pickupRadius = GEM_RADIUS + PLAYER_RADIUS;
                if (glm::dot(diff, diff) <= pickupRadius * pickupRadius)
                {
                    gem.active = false;
                    ++score;
                }
            }
        }

        for (const Gem& gem : gems)
        {
            GeometryInstance& instance = scene.instance(gem.instanceIndex);
            instance.visible = gem.active;
            if (gem.active)
            {
                const float amp = (gem.timeLeft <= GEM_WARNING_TIME) ? 0.30f : 0.18f;
                const float scale = GEM_SCALE * (1.0f + amp * std::sin(gem.animTime));
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(gem.position, 0.0f));
                model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
                instance.modelMatrix = model;
            }
        }
    }

    //aggiorna i proiettili del player e controlla se colpiscono pipistrelli o fantasmi
    void updateProjectiles(
        Scene& scene,
        std::vector<Projectile>& projectiles,
        std::vector<Bat>& bats,
        std::vector<Ghost>& ghosts,
        std::vector<Gem>& gems,
        float halfWidth,
        float halfHeight,
        float dt)
    {
        for (Projectile& p : projectiles)
        {
            if (!p.active)
                continue;

            p.position += p.velocity * dt;

            if (std::abs(p.position.x) > halfWidth + 5.0f ||
                std::abs(p.position.y) > halfHeight + 5.0f)
            {
                p.active = false;
                continue;
            }

            // collisione proiettile - pipistrelli
            for (Bat& b : bats)
            {
                GeometryInstance& batInstance = scene.instance(b.instanceIndex);
                if (!b.alive || !batInstance.visible)
                    continue;

                const glm::vec2 difference = p.position - b.position;
                const float radiusSum = PROJECTILE_RADIUS + BAT_RADIUS;
                if (glm::dot(difference, difference) <= radiusSum * radiusSum)
                {
                    p.active = false;
                    b.hp -= 1;

                    //piccolo rinculo quando viene colpito
                    if (glm::length(p.velocity) > 0.001f)
                        b.position += glm::normalize(p.velocity) * 5.0f;

                    //al secondo colpo muore e lascia una gemma
                    if (b.hp <= 0)
                    {
                        b.alive = false;
                        b.respawnTimer = ENEMY_RESPAWN_TIME;
                        batInstance.visible = false;
                        spawnGem(gems, b.position);
                    }
                    break;
                }
            }

            if (!p.active)
                continue;

            // collisione proiettile - fantasmi
            for (Ghost& g : ghosts)
            {
                GeometryInstance& ghostInstance = scene.instance(g.instanceIndex);
                if (!g.alive || !ghostInstance.visible)
                    continue;

                const glm::vec2 difference = p.position - g.position;
                const float radiusSum = PROJECTILE_RADIUS + GHOST_RADIUS;
                if (glm::dot(difference, difference) <= radiusSum * radiusSum)
                {
                    p.active = false;
                    g.hp -= 1;

                    if (glm::length(p.velocity) > 0.001f)
                        g.position += glm::normalize(p.velocity) * 4.5f;

                    //al secondo colpo muore e lascia una gemma
                    if (g.hp <= 0)
                    {
                        g.alive = false;
                        g.respawnTimer = ENEMY_RESPAWN_TIME;
                        ghostInstance.visible = false;
                        spawnGem(gems, g.position);
                    }
                    break;
                }
            }
        }

        for (const Projectile& p : projectiles)
        {
            GeometryInstance& instance = scene.instance(p.instanceIndex);
            instance.visible = p.active;
            if (p.active)
            {
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(p.position, 0.0f));
                model = glm::rotate(model, p.angleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(PROJECTILE_LENGTH, PROJECTILE_THICKNESS, 1.0f));
                instance.modelMatrix = model;
            }
        }
    }

    //disegna i 5 cuori in alto a sinistra
    void updateHeartsHUD(
        Scene& scene,
        const std::vector<std::size_t>& heartInstances,
        int playerLives,
        float halfWidth,
        float halfHeight)
    {
        for (std::size_t i = 0; i < heartInstances.size(); ++i)
        {
            GeometryInstance& instance = scene.instance(heartInstances[i]);
            instance.visible = (static_cast<int>(i) < playerLives);
            if (!instance.visible)
                continue;

            const float x = -halfWidth + 12.0f + static_cast<float>(i) * 11.5f;
            const float y = halfHeight - 10.0f;
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(x, y, 0.0f));
            model = glm::scale(model, glm::vec3(HEART_HUD_SCALE, HEART_HUD_SCALE, 1.0f));
            instance.modelMatrix = model;
        }
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Errore inizializzazione GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        INITIAL_WIDTH,
        INITIAL_HEIGHT,
        "Survivors Arena",
        nullptr,
        nullptr
    );
    if (!window)
    {
        std::cerr << "Errore creazione finestra GLFW\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Registra i callback di input prima di ImGui, cosi' ImGui fa chaining corretto.
    glfwSetFramebufferSizeCallback(window, Input::framebufferSizeCallback);
    glfwSetKeyCallback(window, Input::keyCallback);
    glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Errore inizializzazione GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Blocco interno: assicura che Shader e Scene rilascino le risorse OpenGL
    // prima di distruggere il contesto GLFW.
    {
        Shader shader;
        if (!shader.load("shader/vertexshaderC.glsl", "shader/fragmentshaderC.glsl"))
        {
            std::cerr << "Errore caricamento shader\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        //secondo shader per lo sfondo
        Shader backgroundShader;
        if (!backgroundShader.load("shader/background.vert", "shader/background.frag"))
        {
            std::cerr << "Errore caricamento shader sfondo\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        Renderer renderer(shader);
        BackgroundRenderer background(backgroundShader);
        Scene scene;
        Player player(scene);

        //caricamento pipistrello fatto con curve di Hermite
        MeshData batMesh;
        std::string batLoadError;
        bool batLoaded = false;
        for (const char* path : { "assets/pipistrello.obj", "pipistrello.obj" })
        {
            if (PlayerShapeLoader::load(path, batMesh, batLoadError))
            {
                batLoaded = true;
                break;
            }
        }
        if (!batLoaded)
        {
            std::cerr << "ERRORE caricamento pipistrello.obj: " << batLoadError << '\n';
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        const std::size_t batGeometry = scene.addGeometry(batMesh);
        std::vector<Bat> bats(MAX_BATS);
        for (Bat& b : bats)
        {
            b.instanceIndex = scene.addInstance(batGeometry);
            scene.instance(b.instanceIndex).visible = false;
        }
        for (Bat& b : bats)
        {
            respawnBat(b, bats, scene, glm::vec2(0.0f, 0.0f));
            scene.instance(b.instanceIndex).visible = true;
        }

        //caricamento fantasma (sempre con Hermite)
        MeshData ghostMesh;
        std::string ghostLoadError;
        bool ghostLoaded = false;
        for (const char* path : { "assets/fantasma.obj", "fantasma.obj" })
        {
            if (PlayerShapeLoader::load(path, ghostMesh, ghostLoadError))
            {
                ghostLoaded = true;
                break;
            }
        }
        if (!ghostLoaded)
        {
            std::cerr << "ERRORE caricamento fantasma.obj: " << ghostLoadError << '\n';
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }

        const std::size_t ghostGeometry = scene.addGeometry(ghostMesh);
        std::vector<Ghost> ghosts(MAX_GHOSTS);
        for (Ghost& g : ghosts)
        {
            g.instanceIndex = scene.addInstance(ghostGeometry);
            scene.instance(g.instanceIndex).visible = false;
        }
        for (Ghost& g : ghosts)
        {
            respawnGhost(g, ghosts, scene, glm::vec2(0.0f, 0.0f));
            scene.instance(g.instanceIndex).visible = true;
        }

        //pallini bianchi sparati dai fantasmi
        const MeshData pelletMesh = ShapeGenerator::createCircle(
            0.0f, 0.0f, PELLET_RADIUS, PELLET_RADIUS, 18,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            glm::vec4(0.85f, 0.94f, 1.0f, 1.0f)
        );
        const std::size_t pelletGeometry = scene.addGeometry(pelletMesh);
        std::vector<EnemyPellet> enemyPellets(MAX_ENEMY_PELLETS);
        for (EnemyPellet& pellet : enemyPellets)
        {
            pellet.instanceIndex = scene.addInstance(pelletGeometry);
            scene.instance(pellet.instanceIndex).visible = false;
        }

        // gemme rilasciate dai nemici quando muoiono
        const MeshData gemMesh = ShapeGenerator::createGem(
            glm::vec4(0.95f, 1.0f, 1.0f, 1.0f),
            glm::vec4(0.10f, 0.88f, 0.72f, 1.0f)
        );
        const std::size_t gemGeometry = scene.addGeometry(gemMesh);
        std::vector<Gem> gems(MAX_GEMS);
        for (Gem& gem : gems)
        {
            gem.instanceIndex = scene.addInstance(gemGeometry);
            scene.instance(gem.instanceIndex).visible = false;
        }

        //geometria condivisa per i dardi della balestra
        const MeshData boltMesh = ShapeGenerator::createRectangle(
            glm::vec4(0.85f, 0.22f, 0.18f, 1.0f),
            glm::vec4(1.0f, 0.82f, 0.45f, 1.0f)
        );
        const std::size_t projectileGeometry = scene.addGeometry(boltMesh);
        std::vector<Projectile> projectiles(MAX_PROJECTILES);
        for (Projectile& p : projectiles)
        {
            p.instanceIndex = scene.addInstance(projectileGeometry);
            scene.instance(p.instanceIndex).visible = false;
        }

        // 5 cuori di vita in alto a sinistra
        const MeshData heartMesh = ShapeGenerator::createHeart(
            0.0f, 0.0f, 0.05f, 0.05f, 80,
            glm::vec4(1.0f, 0.85f, 0.85f, 1.0f),
            glm::vec4(0.95f, 0.12f, 0.32f, 1.0f)
        );
        const std::size_t heartGeometry = scene.addGeometry(heartMesh);
        std::vector<std::size_t> heartInstances(MAX_LIVES);
        for (int i = 0; i < MAX_LIVES; ++i)
            heartInstances[i] = scene.addInstance(heartGeometry);

        Gui gui;
        gui.initialize(window);

        glm::vec2 playerPos(0.0f, 0.0f);
        int playerLives = MAX_LIVES;
        int score = 0;
        bool previousFire = false;
        float fireCooldown = 0.0f;
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            //f1 apre/chiude il menu debug
            if (Input::consumeDebugMenuToggle())
                gui.toggleDebugMenu();

            const double now = glfwGetTime();
            const float dt = std::min(0.05f, std::max(0.0f, static_cast<float>(now - lastTime)));
            lastTime = now;

            int width = 0;
            int height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            if (width <= 0 || height <= 0)
                continue;

            const float aspect = static_cast<float>(width) / static_cast<float>(height);
            const float halfWidth = WORLD_HALF_HEIGHT * aspect;
            const glm::mat4 projection = glm::ortho(
                -halfWidth, halfWidth,
                -WORLD_HALF_HEIGHT, WORLD_HALF_HEIGHT,
                -1.0f, 1.0f
            );
            renderer.setProjectionMatrix(projection);

            //il gioco va avanti solo se siamo in partita e il player e' vivo
            const bool activePlay = gui.gameStarted() && (playerLives > 0);

            glm::vec2 moveDir(0.0f, 0.0f);
            if (activePlay)
            {
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                    moveDir.x -= 1.0f;
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                    moveDir.x += 1.0f;
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                    moveDir.y -= 1.0f;
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                    moveDir.y += 1.0f;
            }

            const bool isMoving = glm::length(moveDir) > 0.0f;
            if (isMoving)
                moveDir = glm::normalize(moveDir);

            playerPos += moveDir * gui.playerSpeed() * dt;
            playerPos.x = std::clamp(playerPos.x, -halfWidth + PLAYER_MARGIN_X, halfWidth - PLAYER_MARGIN_X);
            playerPos.y = std::clamp(playerPos.y, -WORLD_HALF_HEIGHT + PLAYER_MARGIN_Y, WORLD_HALF_HEIGHT - PLAYER_MARGIN_Y);

            //coordinate del mouse nel mondo e rotazione balestra
            const float worldMouseX =
                (static_cast<float>(Input::mouseX) / static_cast<float>(width)) * (2.0f * halfWidth) - halfWidth;
            const float worldMouseY =
                (static_cast<float>(Input::mouseY) / static_cast<float>(height)) * (2.0f * WORLD_HALF_HEIGHT) - WORLD_HALF_HEIGHT;
            const glm::vec2 shoulderPos = playerPos + glm::vec2(0.0f, 1.5f);
            const float aimAngle = std::atan2(worldMouseY - shoulderPos.y, worldMouseX - shoulderPos.x);
            const glm::vec2 aimDirection(std::cos(aimAngle), std::sin(aimAngle));
            if (activePlay)
                player.updateTransform(scene, playerPos, aimAngle, isMoving, dt);

            // sparo con click sinistro o spazio
            const ImGuiIO& io = ImGui::GetIO();
            const bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            const bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
            const bool firePressed =
                activePlay &&
                ((mousePressed && !io.WantCaptureMouse) ||
                 (spacePressed && !io.WantCaptureKeyboard));

            fireCooldown = std::max(0.0f, fireCooldown - dt);
            if (firePressed && !previousFire && fireCooldown <= 0.0f)
            {
                const glm::vec2 muzzle = shoulderPos + aimDirection * MUZZLE_DISTANCE;
                fireProjectile(
                    projectiles,
                    gui.maxProjectiles(),
                    muzzle,
                    aimDirection,
                    aimAngle,
                    gui.bulletSpeed()
                );
                fireCooldown = gui.fireCooldown();
            }
            previousFire = firePressed;

            //aggiornamento nemici, proiettili, gemme e cuori
            if (activePlay)
            {
                updateBats(scene, bats, gui.batCount(), gui.batSpeed(), playerPos, playerLives, dt);
                updateGhosts(
                    scene,
                    ghosts,
                    enemyPellets,
                    gui.ghostCount(),
                    gui.ghostSpeed(),
                    playerPos,
                    playerLives,
                    halfWidth,
                    WORLD_HALF_HEIGHT,
                    dt
                );
                updateEnemyPellets(scene, enemyPellets, playerPos, playerLives, halfWidth, WORLD_HALF_HEIGHT, dt);
                updateProjectiles(scene, projectiles, bats, ghosts, gems, halfWidth, WORLD_HALF_HEIGHT, dt);
                updateGems(scene, gems, playerPos, playerLives, dt, score);
            }
            updateHeartsHUD(scene, heartInstances, playerLives, halfWidth, WORLD_HALF_HEIGHT);

            // aggiorna i dati per la gui
            int activeCount = 0;
            for (const Projectile& p : projectiles)
            {
                if (p.active)
                    ++activeCount;
            }
            gui.setActiveProjectiles(activeCount);
            gui.setLives(playerLives);
            gui.setScore(score);

            //pulisce lo schermo e disegna lo sfondo
            renderer.beginFrame(glm::vec4(0.01f, 0.01f, 0.015f, 1.0f));
            renderer.setWireframe(false);
            background.draw(static_cast<float>(now));

            //disegna la scena solo durante la partita
            if (activePlay)
            {
                renderer.setWireframe(gui.wireframe());
                renderer.drawScene(scene);
            }

            gui.beginFrame();
            gui.draw();
            if (gui.quitRequested())
            {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if (gui.resetRequested())
            {
                playerPos = glm::vec2(0.0f, 0.0f);
                player.updateTransform(scene, playerPos, 0.0f);
                playerLives = MAX_LIVES;
                score = 0;
                for (Projectile& p : projectiles)
                {
                    p.active = false;
                    scene.instance(p.instanceIndex).visible = false;
                }
                for (EnemyPellet& pellet : enemyPellets)
                {
                    pellet.active = false;
                    scene.instance(pellet.instanceIndex).visible = false;
                }
                for (Gem& gem : gems)
                {
                    gem.active = false;
                    scene.instance(gem.instanceIndex).visible = false;
                }
                for (Bat& b : bats)
                {
                    scene.instance(b.instanceIndex).visible = false;
                }
                for (Bat& b : bats)
                {
                    respawnBat(b, bats, scene, playerPos);
                    scene.instance(b.instanceIndex).visible = true;
                }
                for (Ghost& g : ghosts)
                {
                    scene.instance(g.instanceIndex).visible = false;
                }
                for (Ghost& g : ghosts)
                {
                    respawnGhost(g, ghosts, scene, playerPos);
                    scene.instance(g.instanceIndex).visible = true;
                }
            }
            gui.render();

            glfwSwapBuffers(window);
        }

        scene.clear();
        gui.shutdown();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

