#pragma once

#include "components/simple_scene.h"



namespace m1
{
    class GameScene;
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;
        GameScene* gameScene;
        bool inGame;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.
        float currentX;
        float currentY;
        float palette_width;

        // patratele rosii pt paleta
        struct Square {
            glm::vec2 pos;
            float size;
        };

        std::vector<Square> paletteSquares;
        float paletteWidth;

        // patratul gri pt constructie
        struct GrayBox_t {
            glm::vec2 pos; // pozitia
            float size; // dimensiunea
            bool isDragging; // daca e luat cu mouse ul
            bool isVisible;
            glm::vec2 dragOffset; // offset intre click si colt
            bool isOnGrid;
            int gridRow = -1; // randul din grid, -1 daca nu e plasat
            int gridCol = -1; // coloana din grid
        };

        GrayBox_t grayBox;

        bool showGray = true;
        glm::vec2 grayPos = glm::vec2(0);
        float grayBoxSize = 50.0f;

        // gridul editorului

        struct Cell {
            glm::vec2 pos;
            bool occupied;
        };

        std::vector<std::vector<Cell>> gridCells;
        float gridX, gridY;
        float gridWidth, gridHeight;
        float square_size;
        float spacing;
        float totalCellSize;

        float padding;
        float paddingFromContainer;
        float gridMatrixX, gridMatrixY;

        float numCols;
        float numRows;

        // num of used box

        struct NumSquare {
            glm::vec2 pos;
            float size;
        };

        // bara de sus
        std::vector<NumSquare> numSquares;
        glm::vec2 numBarPos;
        float numBarWidth;
        float numBarHeight;
        float numSquareSize;
        float numSquareSpacing;

        // butonul Start
        glm::vec2 startButtonPos;
        float startButtonSize;

        bool IsInsideGrid(float x, float y);
        glm::vec2 GetNearestCellCenter(float x, float y, int& outRow, int& outCol);

        // pt logica din spatele used blocks;
        int totalBlocks;
        int usedBlocks;
        std::vector<GrayBox_t> grayBoxes;
        int activeBoxIndex;

        // right click stergem block

        void RemoveGrayBoxFromGrid(int index);
        bool IsContinuousLine();
        
        bool canStart;

        // verific constagere

        static bool CompareByColumn(const std::pair<int, int>& a, const std::pair<int, int>& b);
    };
}   // namespace m1
