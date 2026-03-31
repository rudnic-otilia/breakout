#include "Tema1.h"
#include "transforms2D.h"
#include "objects.h"
#include "GameScene.h"
#include "core/world.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;



Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


bool Tema1::IsInsideGrid(float x, float y) {
    return (x >= gridX && x <= gridX + gridWidth &&
        y >= gridY && y <= gridY + gridHeight);
}

glm::vec2 Tema1::GetNearestCellCenter(float x, float y, int& outRow, int& outCol) {
    glm::vec2 bestCenter;
    float minDist = 1e9;

    for (int i = 0; i < gridCells.size(); i++) {
        for (int j = 0; j < gridCells[i].size(); j++) {
            glm::vec2 center = gridCells[i][j].pos + glm::vec2(square_size / 2.0f);
            float dist = glm::distance(center, glm::vec2(x, y));
            if (dist < minDist) {
                minDist = dist;
                bestCenter = center;
                outRow = i;
                outCol = j;
            }
        }
    }
    return bestCenter;
}

void Tema1::RemoveGrayBoxFromGrid(int index)
{
    // salvam dimensiunea inainte de erase
    float oldSize = grayBoxes[index].size;

    // 1. Calculam centrul patratului
    float centerX = grayBoxes[index].pos.x + oldSize / 2.0f;
    float centerY = grayBoxes[index].pos.y + oldSize / 2.0f;

    // 2. Verificam daca se afla intr-o celula
    int row = -1, col = -1;
    glm::vec2 dummy = GetNearestCellCenter(centerX, centerY, row, col);

    if (row != -1 && col != -1) {
        gridCells[row][col].occupied = false;
    }

    // 3. stergem patratul gri din vector
    grayBoxes.erase(grayBoxes.begin() + index);

    // 4. Daca aveam blocuri folosite, dam unul inapoi
    if (usedBlocks > 0) {
        usedBlocks--;

        // refacem vizibil un patrat verde
        if (usedBlocks < numSquares.size())
            numSquares[usedBlocks].size = numSquareSize;

        // 5. cream un nou patrat gri in paleta
        if (usedBlocks < totalBlocks) {
            GrayBox_t newGrayBox;
            newGrayBox.size = oldSize;
            newGrayBox.isDragging = false;
            newGrayBox.isVisible = true;
            newGrayBox.isOnGrid = false;

            newGrayBox.pos = glm::vec2(
                paletteSquares.back().pos.x + (paletteSquares.back().size - oldSize) / 2.0f,
                paletteSquares.back().pos.y + (paletteSquares.back().size - oldSize) / 2.0f
            );

            grayBoxes.push_back(newGrayBox);
        }
    }

}

bool Tema1::CompareByColumn(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
    // intoarce true daca a e inaintea lui b
    // (adica daca coloana lui a < coloana lui b)
    return a.second < b.second;
}


bool Tema1::IsContinuousLine() {
    std::vector<std::pair<int, int>> placedCells;

    // 1️ Adunam toate blocurile plasate in grila
    for (int i = 0; i < grayBoxes.size(); i++) {
        GrayBox_t box = grayBoxes[i];
        if (box.isOnGrid) {
            placedCells.push_back(std::make_pair(box.gridRow, box.gridCol));
        }
    }

    // Daca sunt mai putin de 4, nu poate fi linie valida
    if (placedCells.size() < 4)
        return false;

    // 2️ Verificam daca sunt toate pe acelasi rand
    int firstRow = placedCells[0].first;
    for (int i = 0; i < placedCells.size(); i++) {
        if (placedCells[i].first != firstRow) {
            return false; // alt rand => nu e linie
        }
    }

    // 3️ Sortam blocurile dupa coloana (cu functia noastra)
    std::sort(placedCells.begin(), placedCells.end(), Tema1::CompareByColumn);

    // 4️ Verificam daca sunt consecutive
    for (int i = 1; i < placedCells.size(); i++) {
        int currentCol = placedCells[i].second;
        int previousCol = placedCells[i - 1].second;

       if (currentCol != previousCol + 1) {
            return false; // exista o „gaura”
        }
    }
    // 5️ Toate conditiile sunt bune -> e linie continua
    return true;
}



void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);


    float squareSide = 1;

    Mesh* paletteBackground = objects::CreateSquare("paletteBackground", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.8f, 0.1f, 0.1f), false);
    Mesh* elementSquare = objects::CreateSquare("elementSquare", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.8f, 0.1f, 0.1f), false);
    Mesh* gridBackground = objects::CreateSquare("gridBackground", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.0f, 0.0f, 0.9f), false);
    Mesh* gridSquare = objects::CreateSquare("gridSquare", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.2f, 0.6f, 1.0f), true);
    Mesh* squareNum = objects::CreateSquare("squareNum", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.0f, 0.6f, 0.0f), true);
    Mesh* squareNumBack = objects::CreateSquare("squareNumBack", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.0f, 0.7f, 0.0f), false);
    Mesh* graySquare = objects::CreateSquare("graySquare", glm::vec3(0, 0, 0), squareSide, glm::vec3(0.5f, 0.5f, 0.5f), true);
    Mesh* startButtonRed = objects::CreateStartButton("startButtonRed", glm::vec3(0, 0, 0), 1, glm::vec3(0.7f, 0.0f, 0.0f), true);
    Mesh* startButtonGreen = objects::CreateStartButton("startButtonGreen", glm::vec3(0, 0, 0), 1, glm::vec3(0.0f, 1.0f, 0.0f), true);

    AddMeshToList(paletteBackground);
    AddMeshToList(elementSquare);
    AddMeshToList(gridBackground);
    AddMeshToList(gridSquare);
    AddMeshToList(squareNum);
    AddMeshToList(squareNumBack);
    AddMeshToList(startButtonRed);
    AddMeshToList(startButtonGreen);
    AddMeshToList(graySquare);

    paletteSquares.clear();

    currentX = 0;
    currentY = 0;
    palette_width = 250.0f;

    //3 patrate pt paleta, initializarea coordonate X si Y 
    for (int i = 0; i < 3; i++) {
        Square sq;
        sq.pos = glm::vec2(currentX, currentY);
        sq.size = palette_width;
        paletteSquares.push_back(sq);

        currentY += palette_width;
    }

    // patratul gri in centrul patratului mare
    grayBox.size = 50.0f;

    grayBox.pos = glm::vec2(
        paletteSquares.back().pos.x + (paletteSquares.back().size - grayBox.size) / 2.0f,
        paletteSquares.back().pos.y + (paletteSquares.back().size - grayBox.size) / 2.0f
    );

    grayBox.isDragging = false;
    grayBox.isVisible = true;
    grayBox.isOnGrid = false;
    grayBoxes.push_back(grayBox);


    activeBoxIndex = -1;

    // editor grid container
    padding = 20.0f;
    gridX = palette_width + padding;
    gridY = padding;
    gridWidth = resolution.x - palette_width - padding - padding;
    gridHeight = resolution.y - 150.0f - padding;

    //editor grid matrice
    gridCells.clear();
    square_size = 40.0f;
    spacing = 10.0f;
    totalCellSize = square_size + spacing;

    paddingFromContainer = 20.0f;
    gridMatrixX = gridX + paddingFromContainer;
    gridMatrixY = gridY + paddingFromContainer;

    numCols = (int)floor((gridWidth - 2 * paddingFromContainer) / totalCellSize);
    numRows = (int)floor((gridHeight - 2 * paddingFromContainer) / totalCellSize);

    for (int i = 0; i < numRows; i++) {
        std::vector<Cell> row;

        for (int j = 0; j < numCols; j++) {
            float x = gridMatrixX + j * totalCellSize;
            float y = gridMatrixY + i * totalCellSize;

            Cell cell;
            cell.pos = glm::vec2(x, y);
            cell.occupied = false;

            row.push_back(cell);
        }

        gridCells.push_back(row);
    }

    // nr of used blocks render

    float padding = 50.0f;

    // pozitia si dimensiunile fundalului barei
    numBarPos = glm::vec2(gridX, gridHeight + padding);
    numBarWidth = gridWidth;
    numBarHeight = 100.0f;

    // patratelele verzi din bara
    numSquareSize = 45.0f;
    numSquareSpacing = 20.0f;
    float totalCellSize = numSquareSize + numSquareSpacing;

    // coordonate initiale pentru primul patrat verde
    float startX = numBarPos.x + 25.0f;  // padding intern
    float startY = numBarPos.y + 25.0f;

    numSquares.clear();
    for (int i = 0; i < 10; i++) {
        NumSquare sq;
        sq.pos = glm::vec2(startX + totalCellSize * i, startY);
        sq.size = numSquareSize;
        numSquares.push_back(sq);
    }

    startButtonSize = numSquareSize;
    float startPadding = 70.0f;

    // aliniat la dreapta (dreapta barei verzi)
    startButtonPos = glm::vec2(gridX + gridWidth - startPadding, startY);

    // logic behind of used blocks
    totalBlocks = 10;
    usedBlocks = 0;

    //button culoare in functie de constrangeri
    canStart = false;

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    gameScene = nullptr;
    inGame = false;
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();
    if (inGame && gameScene != nullptr) {
        gameScene->FrameStart();
        gameScene->Update(deltaTimeSeconds);
        gameScene->FrameEnd();
        return;
    }

    // patratul gri
    for (int i = 0; i < grayBoxes.size(); i++) {
        GrayBox_t& box = grayBoxes[i];
        if (!box.isVisible) continue;

        glm::mat3 matrix = glm::mat3(1);
        matrix *= transforms2D::Translate(box.pos.x, box.pos.y);
        matrix *= transforms2D::Scale(box.size, box.size);
        RenderMesh2D(meshes["graySquare"], shaders["VertexColor"], matrix);
    }

    //palette squares

    for (Square& sq : paletteSquares)
    {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transforms2D::Translate(sq.pos.x, sq.pos.y);
        modelMatrix *= transforms2D::Scale(sq.size, sq.size);
        RenderMesh2D(meshes["elementSquare"], shaders["VertexColor"], modelMatrix);
    }

    // grid side
    glm::mat3 gridMatrix = glm::mat3(1);
    gridMatrix *= transforms2D::Translate(gridX, gridY);
    gridMatrix *= transforms2D::Scale(gridWidth, gridHeight);
    RenderMesh2D(meshes["gridBackground"], shaders["VertexColor"], gridMatrix);

    // fill the grid with squares

    for (std::vector<Cell>& row : gridCells) {
        for (Cell& cell : row) {
            glm::mat3 squareMatrix = glm::mat3(1);
            squareMatrix *= transforms2D::Translate(cell.pos.x, cell.pos.y);
            squareMatrix *= transforms2D::Scale(square_size, square_size);
            RenderMesh2D(meshes["gridSquare"], shaders["VertexColor"], squareMatrix);
        }
    }


    // number of used blocks

    glm::mat3 numBarMatrix = glm::mat3(1);
    numBarMatrix *= transforms2D::Translate(numBarPos.x, numBarPos.y);
    numBarMatrix *= transforms2D::Scale(numBarWidth, numBarHeight);
    RenderMesh2D(meshes["squareNumBack"], shaders["VertexColor"], numBarMatrix);

    for (NumSquare& numSquare : numSquares) {
        glm::mat3 squareNumMatrix = glm::mat3(1);
        squareNumMatrix *= transforms2D::Translate(numSquare.pos.x, numSquare.pos.y);
        squareNumMatrix *= transforms2D::Scale(numSquare.size, numSquare.size);
        RenderMesh2D(meshes["squareNum"], shaders["VertexColor"], squareNumMatrix);
    }


    // butonul start
    glm::mat3 startButtonMatrix = glm::mat3(1);
    startButtonMatrix *= transforms2D::Translate(startButtonPos.x, startButtonPos.y);
    startButtonMatrix *= transforms2D::Scale(startButtonSize, startButtonSize);
    if (canStart) {
        RenderMesh2D(meshes["startButtonGreen"], shaders["VertexColor"], startButtonMatrix);
    }
    else {
        RenderMesh2D(meshes["startButtonRed"], shaders["VertexColor"], startButtonMatrix);
    }

    /////////////////////////////////////////////////////////////////////////////////////
}


void Tema1::FrameEnd()
{
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event

}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (activeBoxIndex != -1 && grayBoxes[activeBoxIndex].isDragging) {
        int windowHeight = window->GetResolution().y;
        float y = windowHeight - mouseY;

        GrayBox_t& box = grayBoxes[activeBoxIndex];
        box.pos = glm::vec2(
            mouseX - box.dragOffset.x,
            y - box.dragOffset.y
        );
    }


    ////////////////////////////////////////////////////////////////////////////////
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
        int windowHeight = window->GetResolution().y;
        float y = windowHeight - mouseY; // transform coordonatele de la sus-jos la jos-sus

        for (int i = grayBoxes.size() - 1; i >= 0; i--) {
            GrayBox_t& box = grayBoxes[i];
            if (!box.isVisible) continue;

            if (mouseX >= box.pos.x && mouseX <= box.pos.x + box.size &&
                y >= box.pos.y && y <= box.pos.y + box.size)
            {
                // click pe acest patrat incepe dragging
                box.isDragging = true;
                activeBoxIndex = i;
                box.dragOffset = glm::vec2(mouseX - box.pos.x, y - box.pos.y);
                break;
            }
        }

        float buttonX = startButtonPos.x;
        float buttonY = startButtonPos.y;
        float buttonSize = startButtonSize;

        // verificam daca clickul e in interiorul butonului
        bool insideButton = (mouseX >= buttonX && mouseX <= buttonX + buttonSize &&
            y >= buttonY && y <= buttonY + buttonSize);

        if (insideButton && canStart) {

            if (!inGame) {
                gameScene = new GameScene(usedBlocks, this);
                gameScene->Init();
                inGame = true;
            }
        }
    }

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        int windowHeight = window->GetResolution().y;
        float y = windowHeight - mouseY;

        // verificam doar patratele gri
        for (int i = 0; i < grayBoxes.size(); i++) {
            GrayBox_t& box = grayBoxes[i];

            // verificam daca clickul e pe acest patrat
            if (mouseX >= box.pos.x && mouseX <= box.pos.x + box.size &&
                y >= box.pos.y && y <= box.pos.y + box.size)
            {
                // am dat click pe patratul i il stergem
                RemoveGrayBoxFromGrid(i);
                break;
            }
        }

        if (IsContinuousLine()) {
            canStart = true;
        }
        else {
            canStart = false;
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && activeBoxIndex != -1)
    {
        GrayBox_t& box = grayBoxes[activeBoxIndex];
        box.isDragging = false;

        int windowHeight = window->GetResolution().y;

        // centrul patratului gri
        float dropX = box.pos.x + box.size / 2.0f;
        float dropY = box.pos.y + box.size / 2.0f;

        // daca e in zona gridului
        if (IsInsideGrid(dropX, dropY)) {
            int row = -1, column = -1;
            glm::vec2 nearestCenter = GetNearestCellCenter(dropX, dropY, row, column);

            if (row != -1 && column != -1 && !gridCells[row][column].occupied) {

                // eliberarea vechii celule
                if (box.isOnGrid) {
                    int oldRow = box.gridRow;
                    int oldCol = box.gridCol;
                    if (oldRow >= 0 && oldRow < numRows && oldCol >= 0 && oldCol < numCols) {
                        gridCells[oldRow][oldCol].occupied = false;
                    }
                }

                // setare celula noua
                box.pos = nearestCenter - glm::vec2(box.size / 2);
                gridCells[row][column].occupied = true;
                box.gridRow = row;
                box.gridCol = column;

                // daca e din paleta celula
                if (!box.isOnGrid) {
                    box.isOnGrid = true;
                    usedBlocks++;

                    if (usedBlocks <= totalBlocks) {
                        numSquares[usedBlocks - 1].size = 0;
                    }

                    if (usedBlocks < totalBlocks) {
                        GrayBox_t newGrayBox;
                        newGrayBox.size = box.size;
                        newGrayBox.pos = glm::vec2(
                            paletteSquares.back().pos.x + (paletteSquares.back().size - box.size) / 2.0f,
                            paletteSquares.back().pos.y + (paletteSquares.back().size - box.size) / 2.0f
                        );
                        newGrayBox.isDragging = false;
                        newGrayBox.isVisible = true;
                        newGrayBox.isOnGrid = false;
                        grayBoxes.push_back(newGrayBox);
                    }
                    else {
                        for (int i = 0; i < grayBoxes.size(); i++) {
                            if (!grayBoxes[i].isOnGrid) {
                                grayBoxes[i].isVisible = false;
                            }
                        }
                    }
                }
                // verificare daca e continua paleta
                if (IsContinuousLine()) {
                    canStart = true;
                }
                else {
                    canStart = false;
                }
            }
        }
        else {
            // daca nu e in grid, il trimit inapoi in paleta
            box.isOnGrid = false;

            // pozitia centrala a ultimului patrat din paleta
            box.pos = glm::vec2(
                paletteSquares.back().pos.x + (paletteSquares.back().size - box.size) / 2.0f,
                paletteSquares.back().pos.y + (paletteSquares.back().size - box.size) / 2.0f
            );

            box.isVisible = true; // il fac vizibil din nou

        }

        // resetam indexul activ
        activeBoxIndex = -1;
    }


}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
