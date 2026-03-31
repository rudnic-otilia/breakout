#include "GameScene.h"
#include "objects.h"
#include "transforms2D.h"

#include <iostream>
using namespace m1;

GameScene::GameScene(int blocksUsed, Tema1* parentScene)
{
    blockCount = blocksUsed;
    parent = parentScene;
}

GameScene::~GameScene()
{
}

bool GameScene::CircleAABB(glm::vec2 circlePos, float radius, glm::vec2 boxPos, glm::vec2 boxSize)
{
    float xNearest = std::max(boxPos.x, std::min(circlePos.x, boxPos.x + boxSize.x));
    float yNearest = std::max(boxPos.y, std::min(circlePos.y, boxPos.y + boxSize.y));

    float dx = circlePos.x - xNearest;
    float dy = circlePos.y - yNearest;

    return (dx * dx + dy * dy) < (radius * radius);
}


void GameScene::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    //paleta
    paddleHeight = 20.0f;
    paddleWidth = std::min(100.0f + blockCount * 10.0f, 400.0f);
    paddlePos = glm::vec2((resolution.x - paddleWidth) / 2.0f, 50.0f);

    //bila
    circleRadius = 4.0f;
    circlePos = glm::vec2(paddlePos.x + paddleWidth / 2.0f, paddlePos.y + paddleHeight * 2);
    
    //blocurile colorate
    orange = glm::vec3(0.8f, 0.4f, 0.0f);
    red = glm::vec3(1.0f, 0.0f, 0.0f);
    green = glm::vec3(0.0f, 1.0f, 0.0f);
    blue = glm::vec3(0.0f, 0.0f, 1.0f);
    purple = glm::vec3(0.6f, 0.0f, 0.8f);

    colorList = { orange, red, green, blue, purple };

    blockWidth = 120.0f;
    blockHeight = 40.0f;
    spacing = 8.0f;

    numCols = 10;
    numRows = 5;

    gridStartX = 0.0f;
    gridStartY = resolution.y - 100.0f;

    blocks.clear();

    for (int row = 0; row < numRows; row++) {
        glm::vec3 color = colorList[row % colorList.size()];
        int blockHealth = numRows - row;

        for (int col = 0; col < numCols; col++) {
            float x = gridStartX + col * (blockWidth + spacing);
            float y = gridStartY - row * (blockHeight + spacing);

            Block block;
            block.pos = glm::vec2(x, y);
            block.color = color;
            block.destroyed = false;
            block.health = blockHealth;
            blocks.push_back(block);
        }
    }

    //text pt score si vieti

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

    textRenderer->Load("assets/fonts/Hack-Bold.ttf", 24);
    score = 0;
    lives = 3;

    // fizica din spatele bilei
    circleReleased = false;
    circleSpeed = 300.0f;
    circleVelocity = glm::vec2(0.0f, 0.0f);

    paddleSpeed = 400.0f;

    collisionTime = 0.0f;

    Mesh* block_orange = objects::CreateSquare("block_orange", glm::vec3(0, 0, 0), 1, glm::vec3(0.8f, 0.4f, 0.0f), true);
    Mesh* block_red = objects::CreateSquare("block_red", glm::vec3(0, 0, 0), 1, glm::vec3(1.0f, 0.0f, 0.0f), true);
    Mesh* block_green = objects::CreateSquare("block_green", glm::vec3(0, 0, 0), 1, glm::vec3(0.0f, 1.0f, 0.0f), true);
    Mesh* block_blue = objects::CreateSquare("block_blue", glm::vec3(0, 0, 0), 1, glm::vec3(0.0f, 0.0f, 1.0f), true);
    Mesh* block_purple = objects::CreateSquare("block_purple", glm::vec3(0, 0, 0), 1, glm::vec3(0.6f, 0.0f, 0.8f), true);


    Mesh* paddle = objects::CreateSquare("paddle", glm::vec3(0, 0, 0), 1, glm::vec3(0.5f, 0.5f, 0.5f), true);
    Mesh* circle = objects::CreateCircle("circle", glm::vec3(0, 0, 0), circleRadius, glm::vec3(1, 1, 0), true);
    AddMeshToList(circle);
    AddMeshToList(paddle);

    AddMeshToList(block_orange);
    AddMeshToList(block_red);
    AddMeshToList(block_green);
    AddMeshToList(block_blue);
    AddMeshToList(block_purple);

}

void GameScene::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void GameScene::Update(float deltaTimeSeconds)
{
    glm::ivec2 res = window->GetResolution();

    collisionTime -= deltaTimeSeconds;
    if (collisionTime < 0)
        collisionTime = 0;

    // Text Score + Lives 
    textRenderer->RenderText("Score: " + std::to_string((int)score),
        20.0f,
        20.0f,
        1.0f,
        glm::vec3(1.0f, 1.0f, 1.0f));

    textRenderer->RenderText("Lives: " + std::to_string(lives),
        window->GetResolution().x - 120.0f,
        20.0f,
        1.0f,
        glm::vec3(1.0f, 1.0f, 1.0f));

    // CERC SI PALETA

    glm::mat3 paddleM = glm::mat3(1);
    paddleM *= transforms2D::Translate(paddlePos.x, paddlePos.y);
    paddleM *= transforms2D::Scale(paddleWidth, paddleHeight);
    RenderMesh2D(meshes["paddle"], shaders["VertexColor"], paddleM);

    
    //BLOCURI COLORATE
    for (Block& block : blocks) {
        if (block.destroyed) continue;
        // animatia de spargere
        if (block.isBreaking) {
            block.breakTimer -= deltaTimeSeconds;
            block.breakScale -= deltaTimeSeconds * 6.0f;  // viteza de micsorare
            if (block.breakScale < 0) block.breakScale = 0;

            if (block.breakTimer <= 0) {
                block.isBreaking = false;
                block.destroyed = true; // dispare complet
                continue;
            }
        }

        glm::mat3 blockMatrix = glm::mat3(1);
        float scale = block.breakScale; // 1.0 cand e normal, <1.0 cand se micsoreaza

        blockMatrix *= transforms2D::Translate(block.pos.x + blockWidth / 2, block.pos.y + blockHeight / 2);
        blockMatrix *= transforms2D::Scale(blockWidth * scale, blockHeight * scale);
        blockMatrix *= transforms2D::Translate(-0.5f, -0.5f);


        int colorIndex = block.health - 1;

        if (colorIndex < 0)
            colorIndex = 0;
        else if (colorIndex > (int)colorList.size() - 1)
            colorIndex = (int)colorList.size() - 1;

        switch (colorIndex) {
            case 0: RenderMesh2D(meshes["block_orange"], shaders["VertexColor"], blockMatrix); break;
            case 1: RenderMesh2D(meshes["block_red"], shaders["VertexColor"], blockMatrix); break;
            case 2: RenderMesh2D(meshes["block_green"], shaders["VertexColor"], blockMatrix); break;
            case 3: RenderMesh2D(meshes["block_blue"], shaders["VertexColor"], blockMatrix); break;
            case 4: RenderMesh2D(meshes["block_purple"], shaders["VertexColor"], blockMatrix); break;
        }
    }

    // logica fizicii miscarii bilei (release)

    if (!circleReleased) {
        // bila sta lipita de paleta
        circlePos.x = paddlePos.x + paddleWidth / 2.0f;
        circlePos.y = paddlePos.y + paddleHeight * 2;
    }
    else {
        // bila se misca
        prevCirclePos = circlePos;
        circlePos += circleVelocity * deltaTimeSeconds;

        // coliziune cu marginile ecranului
        if (circlePos.x - circleRadius <= 0) {
            circlePos.x = circleRadius;
            circleVelocity.x *= -1;
        }

        if (circlePos.x + circleRadius >= res.x) {
            circlePos.x = res.x - circleRadius;
            circleVelocity.x *= -1;
        }

        if (circlePos.y + circleRadius >= res.y) {
            circlePos.y = res.y - circleRadius;
            circleVelocity.y *= -1;
        }

        if (circlePos.y - circleRadius <= 0) {
            // bila iese jos -> pierdem viata
            lives--;
            circleReleased = false;
            circleVelocity = glm::vec2(0);
            // resetam pozitiile
            paddlePos = glm::vec2((res.x - paddleWidth) / 2.0f, 50.0f);
            circlePos = glm::vec2(paddlePos.x + paddleWidth / 2.0f, paddlePos.y + paddleHeight + circleRadius + 1.0f);
        }

        if (lives <= 0 && parent != nullptr) {
            parent->inGame = false;
            return;  // iesim din Update, revenim la Tema1
        }

    }

    //bila se loveste de paleta

    glm::vec2 paddleSize = glm::vec2(paddleWidth, paddleHeight);
    if (CircleAABB(circlePos, circleRadius, paddlePos, paddleSize) && circleVelocity.y < 0) {

        // determinam cat de adanc a intrat in paleta (pe Y)
        float overlapY = (paddlePos.y + paddleHeight) - (circlePos.y - circleRadius);

        // repozitionam exact deasupra
        circlePos.y += overlapY + 1.0f;

        // inversam viteza verticala
        circleVelocity.y *= -1;

        // efect de control pe orizontala (ca in Pong)
        float paddleCenter = paddlePos.x + paddleWidth / 2.0f;
        float hitDistance = (circlePos.x - paddleCenter) / (paddleWidth / 2.0f);

        float speed = glm::length(circleVelocity);
        float maxAngle = glm::radians(60.0f);
        float angle = hitDistance * maxAngle;

        circleVelocity = glm::normalize(glm::vec2(sin(angle), cos(angle))) * speed;
    }

    // coliiziunea cu blocuri
    for (Block& block : blocks) {
        if (block.destroyed || block.isBreaking) continue;

        if (collisionTime > 0)
            continue;

        glm::vec2 blockSize(blockWidth, blockHeight);
        if (!CircleAABB(circlePos, circleRadius, block.pos, blockSize))
            continue;

        // impact detectat
        block.health--;
        if (block.health <= 0) {
            block.isBreaking = true;
            block.breakTimer = 0.2f; // durata animatiei
            score += 10;
        }

        // calculez cat de mult a patruns pe fiecare axa 
        float closestX = std::max(block.pos.x, std::min(circlePos.x, block.pos.x + blockWidth));
        float closestY = std::max(block.pos.y, std::min(circlePos.y, block.pos.y + blockHeight));

        float overlapX = circleRadius - fabs(circlePos.x - closestX);
        float overlapY = circleRadius - fabs(circlePos.y - closestY);

        // decide axa principala de coliziun
        if (overlapX < overlapY) {
            // coliziune pe X
            if (circlePos.x < block.pos.x)
                circlePos.x -= overlapX;
            else
                circlePos.x += overlapX;

            circleVelocity.x *= -1;
        }
        else {
            // coliziune pe Y
            if (circlePos.y < block.pos.y)
                circlePos.y -= overlapY;
            else
                circlePos.y += overlapY;

            circleVelocity.y *= -1;
        }
        collisionTime = 0.01f; // 100ms pauza la coliziune
        break; // doar un bloc per frame
    }

    glm::mat3 circleM = glm::mat3(1);
    circleM *= transforms2D::Translate(circlePos.x, circlePos.y);
    circleM *= transforms2D::Scale(circleRadius, circleRadius);
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], circleM);

}

void GameScene::FrameEnd()
{
}

void GameScene::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && !circleReleased)
    {
        circleReleased = true;
        // directie la 45 in sus-dreapta
        float angle = glm::radians(45.0f);
        circleVelocity = glm::vec2(cos(angle), sin(angle)) * circleSpeed;
    }
}

void GameScene::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        paddlePos.x += paddleSpeed * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_LEFT))
    {
        paddlePos.x -= paddleSpeed * deltaTime;
    }

    //limite ca sa nu iasa din ecran
    glm::ivec2 res = window->GetResolution();
    if (paddlePos.x < 0)
        paddlePos.x = 0;
    if (paddlePos.x + paddleWidth > res.x)
        paddlePos.x = res.x - paddleWidth;

    // daca bila nu e lansata, o miscam odata cu paleta
    if (!circleReleased) {
        circlePos.x = paddlePos.x + paddleWidth / 2.0f;
        circlePos.y = paddlePos.y + paddleHeight + circleRadius + 1.0f;
    }
}



