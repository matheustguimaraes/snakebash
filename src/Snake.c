/*
Algorithm: Snake Game
Authors: Matheus Guimaraes (@matheusguimr) and Raul Aquino
Discipline: Laboratorio de Programacao (Programming Laboratory), IFCE - 2018.1
Teacher: Shara Shami
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#define SO 0
#include <windows.h>
#include <locale.h>
#include <conio.h>

void clean_terminal() { system("@cls||clear"); }

int teclar_comando() {
    _kbhit()
  }

#endif

#ifdef linux
#define SO 1

#include <termios.h>

static struct termios old, new;

void initTermios() {
    tcgetattr(0, &old);
    new = old;
    new.c_cc[VTIME] = 0;
    new.c_cc[VMIN] = 0;
    new.c_lflag &= ~ICANON;
    new.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &new);
}

void resetTermios(void) {
    old.c_cc[VTIME] = 1;
    old.c_cc[VMIN] = 1;
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    tcsetattr(0, TCSANOW, &old);
}

char getch() {
    char ch;
    initTermios();
    ch = getchar();
    return ch;
}

void clean_terminal() { system("clear"); }

#endif

#define ROW 10
#define COLUMN 12
#define MAX_SIZE 100

static int life;
static int current_size;
static double game_velocity = 0.15;
static char previous_instruction;

typedef struct snake {
    int **coordinates;
} Snake;

typedef struct candy {
    int *coordinates;
} Candy;

int generate_random() { return rand() % 10; }

char read_keyboard(char input){
    char terminal = getch();
    if (terminal != EOF) { input = terminal; }
    return input;
}

void clear_memory_space(Snake *snake, Candy *candy) {
    free(snake->coordinates);
    free(candy->coordinates);
    if (SO) { resetTermios(); }
}

void set_time(double time) {
    clock_t begin = clock();
    while (1) {
        double time_spent = (double) (clock() - begin) / CLOCKS_PER_SEC;
        if (time_spent >= time) { break; }
    }
}

void set_snake(Snake *snake) {
    int aux = 0;
    current_size = 3;
    snake->coordinates = (int **) malloc(current_size * sizeof(int **));
    for (int i = 0; i < current_size; ++i) { snake->coordinates[i] = (int *) malloc(2 * sizeof(int *)); }
    for (int k = current_size; k > 0; --k) {
        snake->coordinates[aux][0] = 0;
        snake->coordinates[aux][1] = k;
        aux++;
    }
}

void increase_snake(Snake *snake, Candy *candy) {
    if (snake->coordinates[0][0] == candy->coordinates[0] &&
        snake->coordinates[0][1] == candy->coordinates[1]) {
        for (int i = 0; i < 2; ++i) { candy->coordinates[i] = generate_random(); }
        current_size = current_size + 1;
        snake->coordinates = (int **) realloc(snake->coordinates, current_size * sizeof(int **));
        for (int i = current_size - 1; i < current_size; i++) {
            snake->coordinates[i] = (int *) realloc(snake->coordinates[i], 2 * sizeof(int *));
            snake->coordinates[i][0] = ROW;
            snake->coordinates[i][1] = 0;
        }
    } else if (current_size == MAX_SIZE) {
        clean_terminal();
        puts("YOU WIN!");
        clear_memory_space(snake, candy);
        exit(1);
    }
}

void guiding_snake(Snake *snake) {
    for (int l = current_size - 1; l > 0; l--) {
        snake->coordinates[l][0] = snake->coordinates[l - 1][0];
        snake->coordinates[l][1] = snake->coordinates[l - 1][1];
    }
}

void limit_snake(Snake *snake, Candy *candy) {
    for (int i = 1; i < current_size; ++i) {
        if ((snake->coordinates[0][0] < 0 || snake->coordinates[0][0] > (ROW - 1) || snake->coordinates[0][1] < 1 ||
             snake->coordinates[0][1] > (COLUMN - 2)) || (snake->coordinates[0][0] == snake->coordinates[i][0] &&
                                                          snake->coordinates[0][1] == snake->coordinates[i][1])) {
            clean_terminal();
            puts("GAME OVER!");
            clear_memory_space(snake, candy);
            exit(1);
        }
    }
}

void print_snake(Snake *snake, char matrix[ROW][COLUMN]) {
    for (int k = 0; k < current_size; ++k) {
        matrix[snake->coordinates[k][0]][snake->coordinates[k][1]] = '*';
    }
}

void set_candy(Candy *candy) {
    life = 1;
    candy->coordinates = (int *) malloc(2 * sizeof(int));
    for (int i = 0; i < 2; ++i) { candy->coordinates[i] = 20; }
}

void print_candy(char matrix[ROW][COLUMN], Candy *candy, Snake *snake) {
    life += 1;
    for (int i = 1; i < current_size; i++) {
        while ((candy->coordinates[0] < 0 || candy->coordinates[0] > (ROW - 1) ||
                candy->coordinates[1] < 1 || candy->coordinates[1] > (COLUMN - 2)) ||
               (candy->coordinates[0] == snake->coordinates[i][0] &&
                candy->coordinates[1] == snake->coordinates[i][1])) {
            for (int j = 0; j < 2; j++) { candy->coordinates[j] = generate_random(); }
        }
    }
    if (life % 20 == 0) {
        for (int i = 0; i < 2; i++) { candy->coordinates[i] = generate_random(); }
    }
    matrix[candy->coordinates[0]][candy->coordinates[1]] = '#';
}

void print_matrix(char matrix[ROW][COLUMN]) {
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COLUMN; ++j) { printf("%c ", matrix[i][j]); }
        printf(" \n");
    }
}

void set_matrix(char matrix[ROW][COLUMN]) {
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COLUMN; ++j) {
            matrix[i][j] = ' ';
            matrix[i][0] = '|';
            matrix[i][11] = '|';
        }
    }
}

void generate_game(Snake *snake, Candy *candy, char matrix[ROW][COLUMN]) {
    set_matrix(matrix);
    print_candy(matrix, candy, snake);
    limit_snake(snake, candy);
    increase_snake(snake, candy);
    print_snake(snake, matrix);
    clean_terminal();
    print_matrix(matrix);
}

char organize_moves(char input) {
    if (previous_instruction == 'w' && input == 's') {
        input = 'w';
    } else if (previous_instruction == 's' && input == 'w') {
        input = 's';
    } else if (previous_instruction == 'a' && input == 'd') {
        input = 'a';
    } else if (previous_instruction == 'd' && input == 'a') {
        input = 'd';
    }
    return input;
}

void save_game(Snake *snake, Candy *candy) {
    FILE *txt_document = fopen("saved_game.txt", "w+");
    for(int i = 0; i < current_size; i++) {
        for(int j = 0; j < 2; j++) {
            fprintf(txt_document, "%d ", snake->coordinates[i][j]);
            }
        fprintf(txt_document, "\n");
    }
    fclose(txt_document);
}

void read_game(Snake *snake, Candy *candy) {
    FILE *txtDocument = fopen("saved_game.txt", "r");
    char txtValues[5000];
    int reference, lines = -1, columns = -1;

    // txtDocument = fopen("/home/matheus/Dropbox/treinamento-pdi/results/10_image.txt", "r");

	  while (!feof(txtDocument)) {
		      fgets(txtValues, 5000, txtDocument);
		      lines++;
	  }

	  rewind(txtDocument);

	  while (!feof(txtDocument)) {
		      fscanf(txtDocument, "%d", &reference);
		      columns++;
	  }

	  columns = columns / lines;

	  rewind(txtDocument);

	    // Mat_<uchar> tiger(lines, columns, 1);

	  for (int x = 0; x < lines; x++) {
		      for (int y = 0; y < columns; y++) {
		          fscanf(txtDocument, "%d", &reference);
		          snake->coordinates[x][y] = reference;
		      }
	  }

    /*
    while(!feof(txt_document)) {
        fscanf(txt_document, " %d", &x);
        // snake->coordinates[i][j] = x;
        reference[temp_position] = x;
        temp_position++;
    }

    int temp = 0;
    for(int i = 0; i < temp_position; i++) {
        for(int j = 0; j < 2; j++) {
            snake->coordinates[i][j] = reference[temp];
            temp++;
        }
    }
    */


    // rewind(txt_document);

    // for(int i = 0; i < temp_position; i++) {
        // for(int j = 0; j < 2; j++) {
            // fscanf(txt_document, "%d\n", &reference);
            // snake->coordinates[i][j] = reference;
            // }
        // fprintf(txt_document, "\n");
    // }
    fclose(txtDocument);
}

void initiate_game(Snake *snake, Candy *candy) {
    char choice;
    printf("DO YOU WANT TO INITIATE A NEW GAME, OR RESTORE A SAVED ONE?");
    printf("IF INITIATE: TYPE 'N', ELSE IF RESTORE 'R'");
    scanf("%c", &choice);
    clean_terminal();
    if(choice == 'n') {
        set_snake(snake);
        set_candy(candy);
    } else if (choice == 'r') {
        read_game(snake, candy);
    }
}

void insert_commands() {
    char input = 'd';
    Candy candy;
    Snake snake;
    initiate_game(&snake, &candy);
    do {
        char matrix[ROW + 1][COLUMN];
        generate_game(&snake, &candy, matrix);
        previous_instruction = input;
        set_time(game_velocity);
        input = read_keyboard(input);
        input = organize_moves(input);
        switch (input) {
            case 'w':
                guiding_snake(&snake);
                snake.coordinates[0][0]--;
                generate_game(&snake, &candy, matrix);
                break;
            case 's':
                guiding_snake(&snake);
                snake.coordinates[0][0]++;
                generate_game(&snake, &candy, matrix);
                break;
            case 'a':
                guiding_snake(&snake);
                snake.coordinates[0][1]--;
                generate_game(&snake, &candy, matrix);
                break;
            case 'd':
                guiding_snake(&snake);
                snake.coordinates[0][1]++;
                generate_game(&snake, &candy, matrix);
                break;
            case 'q':
                clean_terminal();
                break;
            case 'r':
                generate_game(&snake, &candy, matrix);
                save_game(&snake, &candy);
                printf("SAVING THE GAME\n");
                sleep(1);
                input = previous_instruction;
                break;
            default:
                clean_terminal();
                print_matrix(matrix);
                printf("THE ALLOWED KEYS ARE: W, S, A, D\n");
                printf("FOR LEAVE THE GAME, TYPE: Q\n");
                sleep(1);
                input = previous_instruction;
                break;
        }
    } while (input != 'q');
    clear_memory_space(&snake, &candy);
}

int main() {
    insert_commands();
    printf("THE GAME WAS FINISHED\n");
    return 0;
}
