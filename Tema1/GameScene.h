#pragma once
#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include "Tema1.h"



namespace m1
{
    class Tema1; // forward declaration

    class GameScene : public gfxc::SimpleScene
    {
    public:
        GameScene(int blocksUsed, Tema1* parentScene);
        ~GameScene();

        void Init() override;
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void OnKeyPress(int key, int mods) override;
        
        Tema1* parent;  // pointer spre scena editor

    private:
        float cx, cy;
        glm::mat3 modelMatrix;
        int blockCount;

        // paleta + bila
        float paddleHeight;
        float paddleWidth;
        glm::vec2 paddlePos;

        float circleRadius;
        glm::vec2 circlePos;

        //blocurile
        struct Block
        {
            glm::vec2 pos;
            glm::vec3 color;
            bool destroyed;
            int health;

            bool isBreaking = false;   // daca e in proces de distrugere
            float breakScale = 1.0f;   // cat de mic devine (de la 1.0 la 0)
            float breakTimer = 0.0f;   // cat timp mai are pana dispare
        };

        std::vector<Block> blocks;
        
        glm::vec3 orange;
        glm::vec3 red;
        glm::vec3 green;
        glm::vec3 blue;
        glm::vec3 purple;

        std::vector<glm::vec3> colorList;

        int numRows;
        int numCols;
        float blockWidth;
        float blockHeight;
        float spacing;
        float gridStartX;
        float gridStartY;

        //text pt lives si score
        gfxc::TextRenderer* textRenderer;

        float score;
        int lives;

        // mingea e released
        glm::vec2 circleVelocity;
        bool circleReleased;
        float circleSpeed;

        float paddleSpeed;

        bool CircleAABB(glm::vec2 circlePos, float radius, glm::vec2 boxPos, glm::vec2 boxSize);
        void OnInputUpdate(float deltaTime, int mods);

        glm::vec2 prevCirclePos;
        float collisionTime;
    };
}
