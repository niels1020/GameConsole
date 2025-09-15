#include "main.h"
#ifdef TETRIS

bool speed = 0;
bool running = true;

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24
#define CELL_SIZE 10
uint board[BOARD_WIDTH][BOARD_HEIGHT];

#define BACKGROUND_COLOR COLOR_GOLD

enum Block // 6 different blocks
{
    /*
        #
        #
        center
        #
    */
    LONG,
    /*
        ##
         c#
    */
    Z,
    /*
         ##
        #c
    */
    REVERSE_Z,
    /*
          #
         #c#
    */
    T,
    /*
        #
        #
        c#
    */
    L,
    /*
          #
          #
         #c
    */
    REVERSE_L,
    BLOCK_COUNT
};

struct Vector2i
{
    int x;
    int y;
};

// coordinates ofset from center
Vector2i fallingBlockPieces[4];
uint8_t fallingBlockPieceCount = 0;
Block fallingBlock = LONG;
// clockwise rotation of tha falling block
uint8_t fallingBlockRotation = 0;
Vector2i fallingBlockPos = {5, 0};

bool InputProccessing = false;
bool physicsProccesing = false;

#define BLOCK_COLOR_COUNT 5
Color BlockCollors[] = {COLOR_BLUE, COLOR_RED, COLOR_BEIGE, COLOR_DARKGRAY, COLOR_GREEN};
Color BlockHighLightCollors[] = {COLOR_AQUA, COLOR_SALMON, COLOR_WHITE, COLOR_LIGHTGRAY, COLOR_ORANGE};
uint BlockColorIndex = 0;

Vector2i operator+(Vector2i a, Vector2i b)
{
    return {a.x + b.x, a.y + b.y};
}

bool operator==(Vector2i a, Vector2i b)
{
    return (a.x == b.x && a.y == b.y);
}

void drawPiece(uint16_t x, uint16_t y, uint colorIndex)
{
    // background
    GraphicsFillRect(x, y, CELL_SIZE, CELL_SIZE, BlockCollors[BlockColorIndex]);
    // highlicht
    GraphicsFillRect(x, y, CELL_SIZE, CELL_SIZE / 5, BlockHighLightCollors[BlockColorIndex]);
    GraphicsFillRect(x + CELL_SIZE - CELL_SIZE / 5, y, CELL_SIZE / 5, CELL_SIZE, BlockHighLightCollors[BlockColorIndex]);
}

void DrawFallingBlock()
{
    for (uint8_t i = 0; i < fallingBlockPieceCount; i++)
    {
        Vector2i rawPiece = fallingBlockPieces[i];
        Vector2i Piece = rawPiece + fallingBlockPos;

        uint16_t drawX = 70 + (uint16_t)Piece.x * CELL_SIZE;
        uint16_t drawY = (uint16_t)Piece.y * CELL_SIZE;

        drawPiece(drawX, drawY, BlockColorIndex);
    }
}

void clearFallingBlock()
{
    for (uint8_t i = 0; i < fallingBlockPieceCount; i++)
    {
        Vector2i Piece = fallingBlockPieces[i] + fallingBlockPos;
        GraphicsFillRect(70 + (uint16_t)Piece.x * CELL_SIZE, (uint16_t)Piece.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, BACKGROUND_COLOR);
    }
}

const Vector2i blockShapes[BLOCK_COUNT][4][4] = {
    // LONG
    {
        {{0, 2}, {0, 1}, {0, 0}, {0, -1}},  // Rotation 0
        {{2, 0}, {1, 0}, {0, 0}, {-1, 0}},  // Rotation 1
        {{0, 1}, {0, 0}, {0, -1}, {0, -2}}, // Rotation 2
        {{1, 0}, {0, 0}, {-1, 0}, {-2, 0}}  // Rotation 3
    },
    // Z
    {
        {{-1, 0}, {0, 0}, {0, -1}, {1, -1}}, // Rotation 0
        {{0, 1}, {0, 0}, {1, 0}, {1, -1}},   // Rotation 1
        {{-1, 1}, {0, 1}, {0, 0}, {1, 0}},   // Rotation 2
        {{-1, 1}, {-1, 0}, {0, 0}, {0, -1}}  // Rotation 3
    },

    // REVERSE_Z
    {
        {{-1, -1}, {0, -1}, {0, 0}, {1, 0}}, // Rotation 0
        {{1, 1}, {1, 0}, {0, 0}, {0, -1}},   // Rotation 1
        {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},   // Rotation 2
        {{0, 1}, {0, 0}, {-1, 0}, {-1, -1}}  // Rotation 3
    },

    // T
    {
        {{-1, 0}, {0, 1}, {0, 0}, {1, 0}},
        {{0, 1}, {1, 0}, {0, 0}, {0, -1}},
        {{-1, 0}, {0, -1}, {0, 0}, {1, 0}},
        {{0, 1}, {-1, 0}, {0, 0}, {0, -1}}},
    // L
    {
        {{0, 1}, {0, 0}, {0, -1}, {1, -1}},
        {{1, 0}, {0, 0}, {-1, 0}, {-1, -1}},
        {{-1, 1}, {0, 1}, {0, 0}, {0, -1}},
        {{1, 1}, {1, 0}, {0, 0}, {-1, 0}}},
    // REVERSE_L
    {
        {{0, 1}, {0, 0}, {0, -1}, {-1, -1}},
        {{1, -1}, {1, 0}, {0, 0}, {-1, 0}},
        {{1, 1}, {0, 1}, {0, 0}, {0, -1}},
        {{1, 0}, {0, 0}, {-1, 0}, {-1, 1}}}};

void setFallingBlockPieces()
{
    if (fallingBlock >= BLOCK_COUNT || fallingBlockRotation < 0 || fallingBlockRotation > 3)
    {
        fallingBlockPieceCount = 0;
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        fallingBlockPieces[i] = blockShapes[fallingBlock][fallingBlockRotation][i];
    }

    fallingBlockPieceCount = 4;
}

void redrawBoard()
{
    st7789_fill(BACKGROUND_COLOR);
    GraphicsDrawRect(69, 0, 102, 240, COLOR_GRAY);
    for (uint x = 0; x < BOARD_WIDTH; x++)
    {
        uint *column = board[x];
        for (uint y = 0; y < BOARD_HEIGHT; y++)
        {
            if (!column[y] == BLOCK_COLOR_COUNT)
            {

                drawPiece(x,y, column[y]);
            }
        }
    }
}

bool canMove(int xdirection)
{
    bool can_move = true;

    for (uint8_t i = 0; i < fallingBlockPieceCount; i++)
    {
        Vector2i Piece = fallingBlockPieces[i] + fallingBlockPos;
        if (board[xdirection + Piece.x][Piece.y] != BLOCK_COLOR_COUNT)
        {
            can_move = false;
            break;
        }
        if (xdirection + Piece.x < 0 || xdirection + Piece.x > BOARD_WIDTH-1)
        {
            can_move = false;
            break;
        }
    }
    return can_move;
}

void GenerateBlock()
{
    int randomIndex = rand() % BLOCK_COUNT;
    fallingBlock = static_cast<Block>(randomIndex);

    BlockColorIndex = rand() % BLOCK_COLOR_COUNT;
    fallingBlockRotation = rand() % 4;
    fallingBlockPos = {5, 0};
    setFallingBlockPieces();
}

void CheckClearRow()
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool has_empty = false;
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[x][y] == BLOCK_COLOR_COUNT)
            {
                has_empty = true;
            }
        }

        //clear board
        if (!has_empty)
        {
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                board[x][y] = false;
            }

            for (int y2 = y - 1; y2 > 0; y2--)
            {
                for (int x2 = 0; x2 < BOARD_WIDTH; x2++)
                {
                    if (board[x2][y2])
                    {
                        board[x2][y2] = false;
                        board[x2][y2 + 1] = true;
                    }
                }
            }
            redrawBoard();
        }
    }
}

void software_reset()
{
    watchdog_enable(1, 1);
    while (1);
}

void checkGameOver()
{
    Vector2i comp = {5, 1};
    if (fallingBlockPos == comp)
    {
        software_reset();
    }
}

void doColisions()
{
    bool collided = false;
    for (uint8_t i = 0; i < fallingBlockPieceCount; i++)
    {
        Vector2i Piece = fallingBlockPieces[i] + fallingBlockPos;
        if (Piece.y >= BOARD_HEIGHT - 1)
        {
            collided = true;
            continue;
        }
        Vector2i checkPos = {Piece.x, Piece.y + 1};
        if (board[checkPos.x][checkPos.y] != BLOCK_COLOR_COUNT)
        {
            collided = true;
        }
    }
    if (collided)
    {
        for (uint8_t i = 0; i < fallingBlockPieceCount; i++)
        {
            Vector2i Piece = fallingBlockPieces[i] + fallingBlockPos;
            board[Piece.x][Piece.y] = true;
        }
        checkGameOver();
        speed += 1;
        CheckClearRow();
        GenerateBlock();
    }
}

void fillBoard(){
    for (int x = 0; x < BOARD_WIDTH; x++){
        for (int y = 0; y < BOARD_HEIGHT; y++){
            board[x][y] = BLOCK_COLOR_COUNT;
        }
    }
}

void tetrisLaunch()
{
    srand(get_rand_32());

    fillBoard();
    GenerateBlock();
    redrawBoard();
    DrawFallingBlock();
    while (running)
    {
        sleep_ms(std::clamp(500 - 10 * speed, 0, 500));

        while (InputProccessing)
        {
        }

        physicsProccesing = true;
        clearFallingBlock();
        fallingBlockPos.y += 1;
        DrawFallingBlock();
        doColisions();
        physicsProccesing = false;
    }
}

void tetrisInput(uint button, bool state)
{
    while (physicsProccesing)
    {
    }
    if (state == false)
    {
        if (button == Button1)
        {
            InputProccessing = true;
            clearFallingBlock();
            fallingBlockRotation = fallingBlockRotation == 0 ? 3 : fallingBlockRotation - 1;
            setFallingBlockPieces();
            DrawFallingBlock();
            InputProccessing = false;
        }
        else if (button == Button2)
        {
            InputProccessing = true;
            if (canMove(-1))
            {
                clearFallingBlock();
                fallingBlockPos.x -= 1;
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
        else if (button == Button3)
        {
            InputProccessing = true;
            clearFallingBlock();
            fallingBlockPos.y += 1;
            DrawFallingBlock();
            doColisions();
            InputProccessing = false;
        }
        else if (button == Button4)
        {
            InputProccessing = true;
            if (canMove(1))
            {
                clearFallingBlock();
                fallingBlockPos.x += 1;
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
    }
}
#endif