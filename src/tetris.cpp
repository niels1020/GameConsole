/*
file for tetris game
*/
#include "main.h"
#ifdef TETRIS
// TODO: make less global for less memory in use when not being played
// TODO: more docs
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

// TODO: expand and move falling block to this
// struct for combining block data
struct BlockData
{
    Vector2i pieces[4];
    uint8_t pieceAmount;
};

// operators for Vector2i
Vector2i operator+(Vector2i a, Vector2i b)
{
    return {a.x + b.x, a.y + b.y};
}
bool operator==(Vector2i a, Vector2i b)
{
    return (a.x == b.x && a.y == b.y);
}

// returns true if a specific cell in the board is empty
bool isCellEmpty(Vector2i pos)
{
    return board[pos.x][pos.y] >= BLOCK_COLOR_COUNT;
}

// Duplicate use isCellEmpty
// checks if there is a piece at the specific location in the board
bool isPiece(uint x, uint y)
{
    return !isCellEmpty({(int)x, (int)y});
}

// checks if a position is out of bounds
bool IsPosOutOfBounds(Vector2i pos)
{
    if (pos.x < 0 && pos.y < 0)
    {
        return true;
    }
    if (pos.y > BOARD_HEIGHT-1 && pos.x > BOARD_WIDTH)
    {
        return true;
    }
    // no else needed because of early return
    // this reached means in bounds or there is a bug
    return false;
}

// checks if in a given scenario for falling piece is not in another piece or out of bounds
bool canExist(Vector2i block[4], uint8_t pieceCount, Vector2i position)
{
    // early exit if it cannot exist
    for (int i = 0; i < pieceCount; i++)
    {
        // get piece
        Vector2i rawPiece = block[i];
        Vector2i piece = rawPiece + position;

        // check out of bounds
        if (IsPosOutOfBounds(piece))
        {
            printf("cannot exist: %d, %d\n", piece.x, piece.y);
            return false;
        }

        // chek for other pieces in the sam pos
        if (!isCellEmpty(piece))
        {
            printf("cannot exist: %d, %d\n", piece.x, piece.y);
            return false;
        }
    }

    // only reacheble if checks have been good
    return true;
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

// gets the pieces of a block use pointers for setting needed data
BlockData GetBlockPieces(Block type, uint8_t rotation)
{
    BlockData data;

    if (type >= BLOCK_COUNT || rotation < 0 || rotation > 3)
    {
        data.pieceAmount = 0;
    }

    for (int i = 0; i < 4; ++i)
    {
        data.pieces[i] = blockShapes[type][rotation][i];
        data.pieceAmount = 4;
    }

    return data;
}

void redrawBoard()
{
    printf("redrawing board\n");
    st7789_fill(BACKGROUND_COLOR);
    GraphicsDrawRect(69, 0, 102, 239, COLOR_GRAY);
    for (uint16_t x = 0; x < BOARD_WIDTH; x++)
    {
        uint *column = board[x];
        for (uint16_t y = 0; y < BOARD_HEIGHT; y++)
        {
            if (isPiece(x, y))
            {

                uint16_t drawX = 70 + x * CELL_SIZE;
                uint16_t drawY = y * CELL_SIZE;
                drawPiece(drawX, drawY, column[y]);
            }
        }
    }
    printf("board redrawn\n");
}

void GenerateBlock()
{
    int randomIndex = rand() % BLOCK_COUNT;
    fallingBlock = static_cast<Block>(randomIndex);

    BlockColorIndex = rand() % BLOCK_COLOR_COUNT;
    fallingBlockRotation = rand() % 4;
    fallingBlockPos = {5, 0};
    BlockData data = GetBlockPieces(fallingBlock, fallingBlockRotation);
    fallingBlockPieceCount = data.pieceAmount;
    // manually copy over because c++
    for (int i = 0; i < fallingBlockPieceCount; ++i)
    {
        fallingBlockPieces[i] = data.pieces[i];
    }
}

void CheckClearRow()
{
    bool redraw_needed = false;
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool has_empty = false;
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (!isPiece(x, y))
            {
                has_empty = true;
                break;
            }
        }

        // clear board
        if (!has_empty)
        {
            redraw_needed = true;
            printf("clearing: %d\n", y);
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                board[x][y] = BLOCK_COLOR_COUNT;
            }

            for (int y2 = y - 1; y2 > 0; y2--)
            {
                for (int x2 = 0; x2 < BOARD_WIDTH; x2++)
                {
                    if (isPiece(x2, y2))
                    {
                        board[x2][y2 + 1] = board[x2][y2];
                        board[x2][y2] = BLOCK_COLOR_COUNT;
                    }
                }
            }
        }
    }
    if (redraw_needed)
    {
        redrawBoard();
    }
}

void software_reset()
{
    watchdog_enable(1, 1);
    while (1)
        ;
}

// TODO: replace with can Exist
void checkGameOver()
{
    Vector2i comp = {5, 2};
    if (fallingBlockPos == comp)
    {
        printf("game over\n");
        software_reset();
    }
}

// TODO: replace with can Exist check
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

void fillBoard()
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
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
        Vector2i newPos = {fallingBlockPos.x, fallingBlockPos.y + 1};
        if (canExist(fallingBlockPieces, fallingBlockPieceCount, newPos))
        {
            clearFallingBlock();
            fallingBlockPos = newPos;
            DrawFallingBlock();
        }
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
            uint8_t rotation = fallingBlockRotation == 0 ? 3 : fallingBlockRotation - 1;
            BlockData data = GetBlockPieces(fallingBlock, rotation);
            if (canExist(data.pieces, data.pieceAmount, fallingBlockPos))
            {
                clearFallingBlock();
                fallingBlockRotation = rotation;
                fallingBlockPieceCount = data.pieceAmount;
                // manually copy over because c++
                for (int i = 0; i < fallingBlockPieceCount; ++i)
                {
                    fallingBlockPieces[i] = data.pieces[i];
                }
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
        else if (button == Button2)
        {
            InputProccessing = true;
            Vector2i newPos = {fallingBlockPos.x - 1, fallingBlockPos.y};
            if (canExist(fallingBlockPieces, fallingBlockPieceCount, newPos))
            {
                clearFallingBlock();
                fallingBlockPos = newPos;
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
        else if (button == Button3)
        {
            InputProccessing = true;
            Vector2i newPos = {fallingBlockPos.x, fallingBlockPos.y + 1};
            if (canExist(fallingBlockPieces, fallingBlockPieceCount, newPos))
            {
                clearFallingBlock();
                fallingBlockPos = newPos;
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
        else if (button == Button4)
        {
            InputProccessing = true;
            Vector2i newPos = {fallingBlockPos.x + 1, fallingBlockPos.y};
            if (canExist(fallingBlockPieces, fallingBlockPieceCount, newPos))
            {
                clearFallingBlock();
                fallingBlockPos = newPos;
                DrawFallingBlock();
            }
            InputProccessing = false;
        }
    }
}
#endif