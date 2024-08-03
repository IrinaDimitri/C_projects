#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>

#define X 25
#define Y 80
#define CLEAN printf("\e[1;1H\e[2J")

#define RESET_COLOR "\033[0m"
#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"
#define BOLD_TEXT "\033[1m"
#define UNDERLINE_TEXT "\033[4m"

void hello_game(); // Приветствие
void historyOfgame(); // История игры
void WannaStart(); // Вопрос о старте игры
void PrintChangeSpeed(); // Вывод опции смены скорости
int WhatMapDoYouWant(); // Запрос выбора карты
void byebyeEndOfGAME(); // Прощание по завершении игры
void brainOFgame(); // Основной игровой цикл
int **memallocate(); // Выделение памяти для карты
int readmap(int **start_map); // Чтение карты из файла
void checkMap(int **start_map, int **map); // Проверка состояния карты
int CheckneighbotsMap(int **map, int i, int j); // Проверка соседей клетки
void printBorder(int width);
int GameRender(int **map); // Отображение карты
void HelpGameRender_Collor(); // Отображение клетки с цветом
void rewriteNewMap(int **start_map, int **map); // Перезапись карты
void memfree(int **map); // Освобождение памяти карты
void *control_thread_func(); // Функция управления в отдельном потоке
void clearMap(int **map); // Функция очищения карты
void addButterfly(int **map); // Функция добавления фигуры на карту
void addLine(int **map);
void addPulsar(int **map);
void addGlider(int **map);
void addRocket(int **map);
void addBigSpaceship(int **map);
void addGliderShip(int **map);
void addGliderGun(int **map);
void addHeart(int **map);
void addElasticBand(int **map);

int speed = 200000; // Дефолтная скорость
int map_choice = 0;
char starter;

int main() {
    hello_game();
    historyOfgame();
    WannaStart();
    scanf(" %c", &starter); 
    if (starter == 'y' || starter == 'Y') {
        map_choice = 0; 
        brainOFgame();
    } else {
        byebyeEndOfGAME();
    }

    return 0;
}

void WannaStart() {
    printf("\n\t\t\t\t𝔻𝕠 𝕪𝕠𝕦 𝕨𝕒𝕟𝕟𝕒 𝕤𝕥𝕒𝕣𝕥\n\t\t\t\t𝕥𝕙𝕖 𝔾𝕒𝕞𝕖 𝕠𝕗 𝕃𝕚𝕗𝕖❔\n\t\t\t\t\t"BOLD_TEXT GREEN_TEXT"𝕪"RESET_COLOR"/"BOLD_TEXT RED_TEXT"𝕟"RESET_COLOR"\n");
}

void checkMap(int **start_map, int **map) {
    int CheckneighbotsMap_resault;
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            CheckneighbotsMap_resault = CheckneighbotsMap(start_map, i, j);
            if (CheckneighbotsMap_resault == 3)
                map[i][j] = 1;
            else if (((CheckneighbotsMap_resault < 2) || (CheckneighbotsMap_resault > 3)))
                map[i][j] = 0;
            else
                map[i][j] = start_map[i][j];
        }
}

int CheckneighbotsMap(int **map, int i, int j) {
    int count = 0;
    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            if (map[(a + X + i) % X][(b + Y + j) % Y] == 1)
                if (a != 0 || b != 0) count++;
        }
    }
    return count;
}

void brainOFgame() {
    int **start_map = NULL, **map = NULL;
    CLEAN;
    start_map = memallocate();
    map = memallocate();
    readmap(start_map);
    for (int a = 0; a < X; a++)
        for (int b = 0; b < Y; b++) map[a][b] = start_map[a][b];

    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_thread_func, start_map);

    while (1) {
        GameRender(map);
        checkMap(start_map, map);
        rewriteNewMap(start_map, map);
    }

    memfree(map);
    memfree(start_map);
}

int readmap(int **start_map) {
    start_map[1][1] = 2;
    char start_map1[X][Y];
    FILE *file = NULL;

    if (map_choice > 0 && map_choice <= 7) {
        char filename[10];
        snprintf(filename, sizeof(filename), "map%d.txt", map_choice);
        file = fopen(filename, "r");

        if (file == NULL) {
            printf("Unable to open file.\n");
            return -1;
        }

        for (int i = 0; i < X; i++)
            for (int j = 0; j < Y; j++) fscanf(file, "%c ", &start_map1[i][j]);

        fclose(file);
    } else {
        for (int a = 0; a < X; a++)
            for (int b = 0; b < Y; b++) start_map1[a][b] = ' ';
        start_map1[8][4] = '*';
        start_map1[8][5] = '*';
        start_map1[8][6] = '*';
        start_map1[7][6] = '*';
        start_map1[6][5] = '*';
    }

    for (int a = 0; a < X; a++)
        for (int b = 0; b < Y; b++) {
            if (start_map1[a][b] == '*' || start_map1[a][b] == '1')
                start_map[a][b] = 1;
            else
                start_map[a][b] = 0;
        }
    return 0;
}

int **memallocate() {
    int **map = (int **)malloc(X * sizeof(int *));
    for (int i = 0; i < X; i++) map[i] = (int *)malloc(Y * sizeof(int));
    return map;
}

void printBorder(int width) {
    for (int i = 0; i < width; i++) {
        printf(BOLD_TEXT BLUE_TEXT "-"RESET_COLOR);
    }
    printf("\n");
}

int GameRender(int **map) {
    int user_speed = (200000 - speed) / 50000;

    printf("\nSpeed: "BOLD_TEXT RED_TEXT "%d"RESET_COLOR"\t\t\t\tto change speed: press" BOLD_TEXT GREEN_TEXT " + " RESET_COLOR"or" BOLD_TEXT GREEN_TEXT" -\n" RESET_COLOR, user_speed);
    printf("Map: " BOLD_TEXT RED_TEXT "%s\t\t\t\t\t" RESET_COLOR"to change map choose from" BOLD_TEXT GREEN_TEXT " 0 " RESET_COLOR"to" BOLD_TEXT GREEN_TEXT" 9\n" RESET_COLOR,
           map_choice == 0 ? "0" : 
           map_choice == 1 ? "1" : 
           map_choice == 2 ? "2" : 
           map_choice == 3 ? "3" : 
           map_choice == 4 ? "4" : 
           map_choice == 5 ? "5" : 
           map_choice == 6 ? "6" : 
           "7");
    printBorder(Y + 2);
    for (int a = 0; a < X; a++) {
        printf(BOLD_TEXT BLUE_TEXT"|"RESET_COLOR); // Левый край
        for (int b = 0; b < Y; b++) {
            if (map[a][b] == 1)
                HelpGameRender_Collor();
            else
                printf(" ");
        }
        printf(BOLD_TEXT BLUE_TEXT"|\n"RESET_COLOR); // Правый край
    }
    printBorder(Y + 2);
    printf("Add shape:\n\t" BOLD_TEXT GREEN_TEXT "b" RESET_COLOR " - butterfly\t"BOLD_TEXT GREEN_TEXT"l" RESET_COLOR" - line\t"BOLD_TEXT GREEN_TEXT"g" RESET_COLOR" - glider\n");
    printf(BOLD_TEXT GREEN_TEXT"\tr"RESET_COLOR" - rocket\t"BOLD_TEXT GREEN_TEXT"s"RESET_COLOR" - spaceship\t"BOLD_TEXT GREEN_TEXT"G"RESET_COLOR" - GliderShip\n");
    printf(BOLD_TEXT GREEN_TEXT"\tf"RESET_COLOR" - fun glider\t"BOLD_TEXT GREEN_TEXT"h"RESET_COLOR" - heart\t"BOLD_TEXT GREEN_TEXT"e"RESET_COLOR" - ElasticBand\n");
    printf("Clean map: "BOLD_TEXT BLUE_TEXT"c"RESET_COLOR"\tReset to default: "BOLD_TEXT BLUE_TEXT"d"RESET_COLOR"\tExit game: "BOLD_TEXT BLUE_TEXT"q"RESET_COLOR"\tBlow up everything: "BOLD_TEXT RED_TEXT"p\n"RESET_COLOR);

    usleep(speed);
    CLEAN;
    return 0;
}

void HelpGameRender_Collor() {
    int randNum = rand() % 10;
    char *colors[] = {"\033[0;31m", "\033[0;32m", "\033[0;33m", "\033[0;34m", "\033[0;35m",
                      "\033[0;36m", "\033[0;37m", "\033[1;31m", "\033[1;32m", "\033[1;33m"};
    printf("%s", colors[randNum]);
    printf("o");
    printf("\033[0m");
}

void memfree(int **map) {
    for (int i = 0; i < X; i++) free(map[i]);
    free(map);
}

void rewriteNewMap(int **start_map, int **map) {
    for (int a = 0; a < X; a++)
        for (int b = 0; b < Y; b++) start_map[a][b] = map[a][b];
}

void historyOfgame() {
    printf(
        "\t𝕋𝕙𝕖 𝔾𝕒𝕞𝕖 𝕠𝕗 𝕃𝕚𝕗𝕖, 𝕒𝕝𝕤𝕠 𝕜𝕟𝕠𝕨𝕟 𝕤𝕚𝕞𝕡𝕝𝕪 𝕒𝕤 𝕃𝕚𝕗𝕖,𝕚𝕤 𝕒 𝕔𝕖𝕝𝕝𝕦𝕝𝕒𝕣 𝕒𝕦𝕥𝕠𝕞𝕒𝕥𝕠𝕟 𝕕𝕖𝕧𝕚𝕤𝕖𝕕\n 𝕓𝕪 𝕥𝕙𝕖 𝔹𝕣𝕚𝕥𝕚𝕤𝕙 "
        "𝕞𝕒𝕥𝕙𝕖𝕞𝕒𝕥𝕚𝕔𝕚𝕒𝕟 𝕁𝕠𝕙𝕟 ℍ𝕠𝕣𝕥𝕠𝕟 ℂ𝕠𝕟𝕨𝕒𝕪 𝕚𝕟 𝟙𝟡𝟟𝟘. 𝕀𝕥 𝕚𝕤 𝕒 𝕫𝕖𝕣𝕠-𝕡𝕝𝕒𝕪𝕖𝕣 𝕘𝕒𝕞𝕖,\n 𝕞𝕖𝕒𝕟𝕚𝕟𝕘 𝕥𝕙𝕒𝕥 𝕚𝕥𝕤 𝕖𝕧𝕠𝕝𝕦𝕥𝕚𝕠𝕟"
        "𝕚𝕤 𝕕𝕖𝕥𝕖𝕣𝕞𝕚𝕟𝕖𝕕 𝕓𝕪 𝕚𝕥𝕤 𝕚𝕟𝕚𝕥𝕚𝕒𝕝 𝕤𝕥𝕒𝕥𝕖, 𝕣𝕖𝕢𝕦𝕚𝕣𝕚𝕟𝕘 𝕟𝕠 𝕗𝕦𝕣𝕥𝕙𝕖𝕣 𝕚𝕟𝕡𝕦𝕥. \n\t𝕆𝕟𝕖 𝕚𝕟𝕥𝕖𝕣𝕒𝕔𝕥𝕤 "
        "𝕨𝕚𝕥𝕙 𝕥𝕙𝕖 𝔾𝕒𝕞𝕖 𝕠𝕗 𝕃𝕚𝕗𝕖 𝕓𝕪 𝕔𝕣𝕖𝕒𝕥𝕚𝕟𝕘 𝕒𝕟 𝕚𝕟𝕚𝕥𝕚𝕒𝕝 𝕔𝕠𝕟𝕗𝕚𝕘𝕦𝕣𝕒𝕥𝕚𝕠𝕟 𝕒𝕟𝕕 𝕟𝕠𝕓𝕤𝕖𝕣𝕧𝕚𝕟𝕘\n 𝕙𝕠𝕨 𝕚𝕥 𝕖𝕧𝕠𝕝𝕧𝕖𝕤."
        "\n\t𝕀𝕥 𝕚𝕤 𝕋𝕦𝕣𝕚𝕟𝕘 𝕔𝕠𝕞𝕡𝕝𝕖𝕥𝕖 𝕒𝕟𝕕 𝕔𝕒𝕟 𝕤𝕚𝕞𝕦𝕝𝕒𝕥𝕖 𝕒 𝕦𝕟𝕚𝕧𝕖𝕣𝕤𝕒𝕝 𝕔𝕠𝕟𝕤𝕥𝕣𝕦𝕔𝕥𝕠𝕣 𝕠𝕣 𝕒𝕟𝕪\n𝕠𝕥𝕙𝕖𝕣 𝕋𝕦𝕣𝕚𝕟𝕘 𝕞𝕒𝕔𝕙𝕚𝕟𝕖.");
}

void hello_game() {
    CLEAN;
    printf("\033[32m");
    printf(
        "\t\t ▀█▀ █░█ █▀▀   █▀▀ ▄▀█ █▀▄▀█ █▀▀   █▀█ █▀▀   █░░ █ █▀▀ █▀▀\n\t\t ░█░ █▀█ ██▄  "
        " █▄█ █▀█ █░▀░█ ██▄   █▄█ █▀░   █▄▄ █ █▀░ ██▄ "
        "\n");
    printf("\033[0m");
}

void byebyeEndOfGAME() {
    CLEAN;
    printf("\033[32m");
    printf("\n█▄▄ █▄█ █▀▀   █▄▄ █▄█ █▀▀\n█▄█ ░█░ ██▄   █▄█ ░█░ ██▄\n");
    exit(1);
}

void *control_thread_func(int **start_map) {
    struct termios old_tio, new_tio;
    unsigned char c;

    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    while (1) {
        c = getchar();
        if (c == '+') {
            speed = (speed > 50000) ? speed - 50000 : speed;
        } else if (c == '-') {
            speed += 50000;
        } else if (c == 'q' || c == 'Q') {
            tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
            byebyeEndOfGAME();
        } else if (c == 'd' || c == 'D') {
            map_choice = 0;
            readmap(start_map);
        } else if (c >= '0' && c <= '7') {
            map_choice = c - '0';
            readmap(start_map);
        } else if (c == 'c' || c == 'C') {
            clearMap(start_map);
        } else if (c == 'b' || c == 'B') {
            addButterfly(start_map);
        } else if (c == 'l' || c == 'L') {
            addLine(start_map);
        } else if (c == 'p' || c == 'P') {
            addPulsar(start_map);
        } else if (c == 'g') {
            addGlider(start_map);
        } else if (c == 'r' || c == 'R') {
            addRocket(start_map);
        } else if (c == 's' || c == 'S') {
            addBigSpaceship(start_map);
        } else if (c == 'G') {
            addGliderShip(start_map);
        } else if (c == 'f' || c == 'F') {
            addGliderGun(start_map);
        } else if (c == 'h' || c == 'H') {
            addHeart(start_map);
        } else if (c == 'e' || c == 'E') {
            addElasticBand(start_map);
        }
    } 

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    return NULL;
}

void clearMap(int **map) {
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) map[i][j] = 0;
}

void addLine(int **map) {
    int i = rand() % X / 2, j = rand() % Y / 2;
        map[i][j] = 1;
        map[i][j + 1] = 1;
        map[i][j - 1] = 1;
}

void addPulsar(int **map) {
    int i = rand() % X / 2, j = rand() % Y / 2;
        for (i = 0; i < X; i++) {
                for (j = 0; j < Y; j++) {
                    if ((i % 3 == 1 && j % 3 == 1) || (i % 3 == 1 && j % 3 == 2)) {
                        map[i][j] = 1;
                    }
                }
            }
}

void addGlider(int **map) {
    int i = rand() % X / 2, j = rand() % Y / 2;
    map[i][j] = 1;
    map[i + 1][j + 1] = 1;
    map[i + 2][j - 1] = 1;
    map[i + 2][j] = 1;
    map[i + 1][j - 1] = 1;
}

void addRocket(int **map) {
    int i = rand() % (X - 5), j = rand() % (Y - 10); 
    map[i + 0][j + 2] = 1;
    map[i + 1][j + 1] = 1;
    map[i + 1][j + 2] = 1;
    map[i + 1][j + 3] = 1;
    map[i + 2][j + 1] = 1;
    map[i + 2][j + 3] = 1;
    map[i + 3][j + 2] = 1;
    map[i + 4][j + 2] = 1;
}

void addBigSpaceship(int **map) {
    int i = rand() % (X - 10), j = rand() % (Y - 20); 
    map[i + 0][j + 0] = 1;
    map[i + 0][j + 1] = 1;
    map[i + 0][j + 2] = 1;
    map[i + 0][j + 3] = 1;
    map[i + 1][j + 0] = 1;
    map[i + 1][j + 3] = 1;
    map[i + 2][j + 1] = 1;
    map[i + 2][j + 2] = 1;
    map[i + 2][j + 3] = 1;
    map[i + 3][j + 3] = 1;
    map[i + 4][j + 0] = 1;
    map[i + 4][j + 3] = 1;
    map[i + 5][j + 0] = 1;
    map[i + 5][j + 1] = 1;
    map[i + 5][j + 2] = 1;
    map[i + 5][j + 3] = 1;
}

void addGliderShip(int **map) {
    int i = rand() % (X - 10), j = rand() % (Y - 20); 
    map[i + 0][j + 0] = 1;
    map[i + 0][j + 1] = 1;
    map[i + 0][j + 2] = 1;
    map[i + 1][j + 0] = 1;
    map[i + 1][j + 2] = 1;
    map[i + 2][j + 1] = 1;
    map[i + 2][j + 2] = 1;
    map[i + 4][j + 0] = 1;
    map[i + 4][j + 1] = 1;
    map[i + 4][j + 2] = 1;
    map[i + 5][j + 0] = 1;
    map[i + 5][j + 2] = 1;
    map[i + 6][j + 1] = 1;
    map[i + 6][j + 2] = 1;
}

void addGliderGun(int **map) {
    int i = rand() % (X - 9), j = rand() % (Y - 36); 
    map[i + 0][j + 24] = 1;
    map[i + 0][j + 25] = 1;
    map[i + 1][j + 24] = 1;
    map[i + 2][j + 12] = 1;
    map[i + 2][j + 13] = 1;
    map[i + 2][j + 20] = 1;
    map[i + 2][j + 21] = 1;
    map[i + 3][j + 11] = 1;
    map[i + 3][j + 15] = 1;
    map[i + 3][j + 20] = 1;
    map[i + 3][j + 21] = 1;
    map[i + 4][j + 11] = 1;
    map[i + 4][j + 15] = 1;
    map[i + 5][j + 12] = 1;
    map[i + 5][j + 13] = 1;
    map[i + 5][j + 20] = 1;
    map[i + 5][j + 21] = 1;
    map[i + 6][j + 22] = 1;
    map[i + 6][j + 23] = 1;
    map[i + 6][j + 26] = 1;
    map[i + 6][j + 27] = 1;
    map[i + 7][j + 22] = 1;
    map[i + 7][j + 23] = 1;
    map[i + 7][j + 26] = 1;
    map[i + 7][j + 27] = 1;
}

void addHeart(int **map) {
    int i = rand() % (X - 7), j = rand() % (Y - 7); 
    map[i + 1][j + 2] = 1;
    map[i + 2][j + 1] = 1;
    map[i + 2][j + 2] = 1;
    map[i + 2][j + 3] = 1;
    map[i + 3][j + 1] = 1;
    map[i + 3][j + 3] = 1;
    map[i + 4][j + 2] = 1;
}

void addElasticBand(int **map) {
    int i = rand() % (X - 7), j = rand() % (Y - 7); 
    map[i + 1][j + 1] = 1;
    map[i + 1][j + 2] = 1;
    map[i + 1][j + 3] = 1;
    map[i + 2][j + 1] = 1;
    map[i + 2][j + 3] = 1;
    map[i + 3][j + 1] = 1;
    map[i + 3][j + 2] = 1;
    map[i + 3][j + 3] = 1;
}

void addButterfly(int **map) {
    // Выбираем случайное местоположение для фигуры
    int i = rand() % (X - 12); // Фигура занимает 12 строк
    int j = rand() % (Y - 24); // Фигура занимает 24 столбца

    // Устанавливаем клетки фигуры Бабочка
    map[i + 0][j + 5] = 1;
    map[i + 0][j + 6] = 1;
    map[i + 0][j + 7] = 1;
    map[i + 1][j + 1] = 1;
    map[i + 1][j + 2] = 1;
    map[i + 1][j + 5] = 1;
    map[i + 1][j + 6] = 1;
    map[i + 1][j + 9] = 1;
    map[i + 2][j + 4] = 1;
    map[i + 2][j + 5] = 1;
    map[i + 2][j + 7] = 1;
    map[i + 2][j + 8] = 1;
    map[i + 2][j + 12] = 1;
    map[i + 3][j + 4] = 1;
    map[i + 3][j + 5] = 1;
    map[i + 3][j + 7] = 1;
    map[i + 3][j + 8] = 1;
    map[i + 4][j + 5] = 1;
    map[i + 4][j + 6] = 1;
    map[i + 4][j + 7] = 1;
    map[i + 5][j + 5] = 1;
    map[i + 5][j + 6] = 1;
    map[i + 5][j + 7] = 1;
    map[i + 6][j + 5] = 1;
    map[i + 6][j + 6] = 1;
    map[i + 6][j + 7] = 1;
    map[i + 7][j + 5] = 1;
    map[i + 7][j + 6] = 1;
    map[i + 7][j + 7] = 1;
}
