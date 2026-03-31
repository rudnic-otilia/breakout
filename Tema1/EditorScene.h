#pragma once
#include "components/simple_scene.h"
#include "utils/glm_utils.h"
#include <vector>

class EditorScene {
public:
    EditorScene();
    ~EditorScene();

    void Init(gfxc::SimpleScene* scene);   // primeste contextul grafic (mesh/shaders)
    void Update(float deltaTimeSeconds, gfxc::SimpleScene* scene);
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods);

    bool CanStartGame() const { return canStart; }

private:
    bool IsInsideGrid(float x, float y);
    glm::vec2 GetNearestCellCenter(float x, float y, int& outRow, int& outCol);
    void RemoveGrayBoxFromGrid(int index);
    bool IsContinuousLine();

    struct Cell {
        glm::vec2 pos;
        bool occupied;
    };

    struct GrayBox_t {
        glm::vec2 pos;
        float size;
        bool isDragging;
        bool isVisible;
        glm::vec2 dragOffset;
        bool isOnGrid;
        int gridRow, gridCol;
    };

    struct Square {
        glm::vec2 pos;
        float size;
    };

    struct NumSquare {
        glm::vec2 pos;
        float size;
    };

    std::vector<std::vector<Cell>> gridCells;
    std::vector<Square> paletteSquares;
    std::vector<NumSquare> numSquares;
    std::vector<GrayBox_t> grayBoxes;

    int activeBoxIndex;
    int totalBlocks, usedBlocks;
    bool canStart;

    // coordonate si dimensiuni
    float gridX, gridY, gridWidth, gridHeight;
    float square_size, spacing, totalCellSize;
    float palette_width, numBarWidth, numBarHeight, numSquareSize, numSquareSpacing;
    glm::vec2 numBarPos, startButtonPos;
    float startButtonSize;
};
