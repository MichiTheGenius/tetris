#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <unistd.h>
#include <map>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20 // pixels
#define PFIELD_WIDTH 12
#define PFIELD_HEIGHT 19
#define X_OFFSET 50
#define Y_OFFSET 50
#define STANDARD_TICK 1.0f
#define FAST_TICK 0.05f

std::string tetrominos[7];
char pfield[PFIELD_WIDTH * PFIELD_HEIGHT];

std::map<int, sf::Color> tetromino_colors{
    {0, sf::Color::Cyan},
    {1, sf::Color(255, 165, 0)}, // orange
    {2, sf::Color::Blue},
    {3, sf::Color::Magenta},
    {4, sf::Color::Yellow},
    {5, sf::Color::Red},
    {6, sf::Color::Green},
    {7, sf::Color::White}, // line clear
};

void print_field()
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

int rotate(int x, int y, int rotation)
{
    switch (rotation)
    {
    case 0:
        return x + y * 4;
        break;

    case 1: // 90 degrees
        return 12 + y - (x * 4);
        break;

    case 2: // 180 degrees
        return 15 - x - y * 4;
        break;

    case 3: // 270 degrees
        return 3 + x * 4 - y;
        break;

    default:
        return x + y * 4;
    }
}

int convert_2D_to_1D(int x, int y, int width)
{
    return x + y * width;
}

bool does_tetromino_fit(int current_tetromino, int rot, int pos_x, int pos_y)
{
    for (int tetro_y = 0; tetro_y < 4; tetro_y++)
    {
        for (int tetro_x = 0; tetro_x < 4; tetro_x++)
        {
            int current_tetro_pos = rotate(tetro_x, tetro_y, rot);
            if (pfield[convert_2D_to_1D(pos_x + tetro_x, pos_y + tetro_y, PFIELD_WIDTH)] != '.' && tetrominos[current_tetromino][current_tetro_pos] == 'X')
            {
                return false;
            }
        }
    }

    return true;
}

int get_line_count(int starting_line)
{
    int line_count = 0;
    int pieces_per_line;
    for (int y = starting_line; y > starting_line - 4; y--)
    {
        pieces_per_line = 0;
        for (int x = 1; x < PFIELD_WIDTH - 1; x++)
        {
            if (pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '#' && pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '.')
            {
                pieces_per_line++;
            }
        }
        if (pieces_per_line == PFIELD_WIDTH - 2) // subtract 2 because of the walls on each side
        {
            line_count++;
        }
    }
    return line_count;
}

// 0 for empty, 1 for full
int find_first_full_line()
{
    int spaces;
    for (int y = PFIELD_HEIGHT - 2; y > 1; y--)
    {
        spaces = 0;
        for (int x = 1; x < PFIELD_WIDTH - 1; x++)
        {
            if (pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '#' && pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '.')
            {
                spaces++;
            }
        }
        if (spaces == PFIELD_WIDTH - 2) // subtract 2 because of the walls on each side
        {
            return y;
        }
    }
    return -1;
}

void remove_lines(int starting_line, int count)
{
    for (int y = starting_line; y > starting_line - count; y--)
    {
        for (int x = 1; x < PFIELD_WIDTH - 1; x++)
        {
            if (pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '#' && pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] != '.')
            {
                pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] = '.';
            }
        }
    }
    std::cout << "=== field after removing ===" << std::endl;
    print_field();
    std::cout << "\n\n";

    // move all lines down
    for (int y = starting_line; y > count; y--)
    {
        for (int x = 1; x < PFIELD_WIDTH - 1; x++)
        {
            pfield[convert_2D_to_1D(x, y, PFIELD_WIDTH)] = pfield[convert_2D_to_1D(x, y - count, PFIELD_WIDTH)];
        }
    }
    std::cout << "=== field after moving down ===" << std::endl;
    print_field();
    std::cout << "\n\n";
}

int main()
{
    srand(getpid());

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Tetris");
    window.setFramerateLimit(60);

    // SETUP TETROMINOS =============================
    int pos_x = 4;
    int pos_y = 0;
    int rot = 0;
    int current_tetromino = rand() % 7;
    bool rotation_key_pressed = false;
    bool left_pressed = false;
    bool right_pressed = false;
    bool falling_tick = false;
    float falling_tick_time = 1.0f;

    tetrominos[0].append(".X..");
    tetrominos[0].append(".X..");
    tetrominos[0].append(".X..");
    tetrominos[0].append(".X..");

    tetrominos[1].append(".X..");
    tetrominos[1].append(".X..");
    tetrominos[1].append(".XX.");
    tetrominos[1].append("....");

    tetrominos[2].append("..X.");
    tetrominos[2].append("..X.");
    tetrominos[2].append(".XX.");
    tetrominos[2].append("....");

    tetrominos[3].append("....");
    tetrominos[3].append("XXX.");
    tetrominos[3].append(".X..");
    tetrominos[3].append("....");

    tetrominos[4].append("....");
    tetrominos[4].append(".XX.");
    tetrominos[4].append(".XX.");
    tetrominos[4].append("....");

    tetrominos[5].append("....");
    tetrominos[5].append(".XX.");
    tetrominos[5].append("..XX");
    tetrominos[5].append("....");

    tetrominos[6].append("....");
    tetrominos[6].append(".XX.");
    tetrominos[6].append("XX..");
    tetrominos[6].append("....");

    // SETUP FIELD =============================
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

    // SETUP CLOCK ==================================
    sf::Clock clock;

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
        // TIMING ==================================
        sf::Time elapsed_time = clock.getElapsedTime();
        if (elapsed_time.asSeconds() >= falling_tick_time)
        {
            falling_tick = true;
            clock.restart();
        }

        // USER INPUT ==============================

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && rotation_key_pressed)
        {
            rotation_key_pressed = false;
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && left_pressed)
        {
            left_pressed = false;
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && right_pressed)
        {
            right_pressed = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            falling_tick_time = FAST_TICK;
        }
        else
        {
            falling_tick_time = STANDARD_TICK;
        }

        // GAME LOGIC ==============================

        // ROTATION ==============
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !rotation_key_pressed)
        {
            rotation_key_pressed = true;
            if (does_tetromino_fit(current_tetromino, !rot, pos_x, pos_y) && (current_tetromino == 0 || current_tetromino == 5 || current_tetromino == 6)) // I, Z and S tetrominos only rotate 90 deg and back
            {
                rot = !rot;
            }
            else if (does_tetromino_fit(current_tetromino, rot + 1, pos_x, pos_y) && (current_tetromino == 1 || current_tetromino == 2 || current_tetromino == 3)) // T, L, J tetrominos have full rotation; O tetromino does not rotate
            {
                // correct the rotation of the T tetromino, so that it pivots around one fixed point
                if (current_tetromino == 3)
                {
                    if (rot == 0)
                    {
                        pos_x--;
                    }
                    else if (rot == 1)
                    {
                        pos_y--;
                    }
                    else if (rot == 2)
                    {
                        pos_x++;
                    }
                    else if (rot == 3)
                    {
                        pos_y++;
                    }
                }
                rot++;
                if (rot > 3)
                {
                    rot = 0;
                }
            }
        }

        // LEFT RIGHT MOVE =======
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !left_pressed && does_tetromino_fit(current_tetromino, rot, pos_x - 1, pos_y))
        {
            left_pressed = true;
            pos_x--;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !right_pressed && does_tetromino_fit(current_tetromino, rot, pos_x + 1, pos_y))
        {
            right_pressed = true;
            pos_x++;
        }

        // FALLING ===============
        if (falling_tick)
        {
            falling_tick = false;
            pos_y++;
            if (!does_tetromino_fit(current_tetromino, rot, pos_x, pos_y))
            {
                pos_y--;
                // place tetromino on board
                for (int tetro_y = 0; tetro_y < 4; tetro_y++)
                {
                    for (int tetro_x = 0; tetro_x < 4; tetro_x++)
                    {
                        int current_tetro_pos = rotate(tetro_x, tetro_y, rot);
                        if (tetrominos[current_tetromino][current_tetro_pos] == 'X')
                        {
                            pfield[convert_2D_to_1D(pos_x + tetro_x, pos_y + tetro_y, PFIELD_WIDTH)] = "ABCDEFG"[current_tetromino];
                        }
                    }
                }

                // check if line is created
                int first_full_line = find_first_full_line();
                std::cout << "first_full_line: " << first_full_line << std::endl;
                std::cout << "=== field before removing ===" << std::endl;
                print_field();
                std::cout << "\n\n";
                if (first_full_line != -1)
                {
                    int line_count = get_line_count(first_full_line);
                    std::cout << "line_count: " << line_count << std::endl;
                    remove_lines(first_full_line, line_count);
                }

                // remove lines if necesssary

                // move all pieces on board down

                // TODO increase score if necessary

                // create new random teromino
                pos_x = 4;
                pos_y = 0;
                current_tetromino = rand() % 7;
            }
        }

        // DRAW SCREEN =============================

        window.clear(sf::Color::Black);

        // DRAW TETROMINOS =======
        for (int tetro_y = 0; tetro_y < 4; tetro_y++)
        {
            for (int tetro_x = 0; tetro_x < 4; tetro_x++)
            {
                int current_tetro_pos = rotate(tetro_x, tetro_y, rot);
                if (tetrominos[current_tetromino][current_tetro_pos] == 'X')
                {
                    sf::RectangleShape tetro_rect(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                    tetro_rect.setFillColor(tetromino_colors[current_tetromino]);
                    tetro_rect.setPosition(X_OFFSET + (pos_x + tetro_x) * GRID_SIZE, Y_OFFSET + (pos_y + tetro_y) * GRID_SIZE);
                    window.draw(tetro_rect);
                }
            }
        }

        for (int y = 0; y < PFIELD_HEIGHT; y++)
        {
            for (int x = 0; x < PFIELD_WIDTH; x++)
            {
                if (pfield[y * PFIELD_WIDTH + x] == '#')
                {
                    sf::RectangleShape border_rect(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                    border_rect.setFillColor(sf::Color::White);
                    border_rect.setPosition(X_OFFSET + x * GRID_SIZE, Y_OFFSET + y * GRID_SIZE);
                    window.draw(border_rect);
                }
                else if (pfield[y * PFIELD_WIDTH + x] >= 'A')
                {
                    sf::RectangleShape piece_rect(sf::Vector2f(GRID_SIZE, GRID_SIZE));
                    int tetro_color_index = pfield[y * PFIELD_WIDTH + x] - 65;
                    piece_rect.setFillColor(tetromino_colors[tetro_color_index]);
                    piece_rect.setPosition(X_OFFSET + x * GRID_SIZE, Y_OFFSET + y * GRID_SIZE);
                    window.draw(piece_rect);
                }
            }
        }

        window.display();
        // print_field();
        // std::cout << "\n\n";
    }
    return 0;
}