#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <unistd.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20 // pixels
#define PFIELD_WIDTH 12
#define PFIELD_HEIGHT 19
#define X_OFFSET 50
#define Y_OFFSET 50

void print_field(char *pfield)
{
    for (int y = 0; y < PFIELD_HEIGHT; y++)
    {
        for (int x = 0; x < PFIELD_WIDTH; x++)
        {
            std::cout << pfield[y * PFIELD_WIDTH + x];
        }
        std::cout << "\n";
    }
}

int main()
{
    srand(getpid());

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Tetris");
    window.setFramerateLimit(60);

    // SETUP FIELD =============================
    char pfield[PFIELD_WIDTH * PFIELD_HEIGHT];
    for (int y = 0; y < PFIELD_HEIGHT; y++)
    {
        for (int x = 0; x < PFIELD_WIDTH; x++)
        {
            if (x == 0 || x == PFIELD_WIDTH - 1 || y == PFIELD_HEIGHT - 1)
            {
                pfield[y * PFIELD_WIDTH + x] = '#';
            }
            else
            {
                pfield[y * PFIELD_WIDTH + x] = '.';
            }
        }
    }

    // SETUP TETROMINOS =============================
    std::string tetrominos[7];

    tetrominos[0].append("X...");
    tetrominos[0].append("X...");
    tetrominos[0].append("X...");
    tetrominos[0].append("X...");

    tetrominos[1].append("X...");
    tetrominos[1].append("X...");
    tetrominos[1].append("XX..");
    tetrominos[1].append("....");

    tetrominos[2].append(".X..");
    tetrominos[2].append(".X..");
    tetrominos[2].append("XX..");
    tetrominos[2].append("....");

    tetrominos[3].append("XXX.");
    tetrominos[3].append(".X..");
    tetrominos[3].append("....");
    tetrominos[3].append("....");

    tetrominos[4].append("XX..");
    tetrominos[4].append("XX..");
    tetrominos[4].append("....");
    tetrominos[4].append("....");

    tetrominos[5].append("XX..");
    tetrominos[5].append(".XX.");
    tetrominos[5].append("....");
    tetrominos[5].append("....");

    tetrominos[6].append(".XX.");
    tetrominos[6].append("XX..");
    tetrominos[6].append("....");
    tetrominos[6].append("....");

    int startX = 2;
    int startY = 0;
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            pfield[(startY + y) * PFIELD_WIDTH + startX + x] = tetrominos[3][y * 4 + x];
        }
    }

    print_field(pfield);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // USER INPUT ==============================

        // GAME LOGIC ==============================

        // DRAW SCREEN =============================

        window.clear(sf::Color::Black);

        for (int y = 0; y < PFIELD_HEIGHT; y++)
        {
            for (int x = 0; x < PFIELD_WIDTH; x++)
            {
                if (pfield[y * PFIELD_WIDTH + x] == '#')
                {
                    sf::RectangleShape border_rect(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                    border_rect.setPosition(X_OFFSET + x * GRID_SIZE, Y_OFFSET + y * GRID_SIZE);
                    window.draw(border_rect);
                }
                else if (pfield[y * PFIELD_WIDTH + x] == 'X')
                {
                    sf::RectangleShape piece_rect(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                    piece_rect.setFillColor(sf::Color::Blue);
                    piece_rect.setPosition(X_OFFSET + x * GRID_SIZE, Y_OFFSET + y * GRID_SIZE);
                    window.draw(piece_rect);
                }
            }
        }

        window.display();
    }
    return 0;
}