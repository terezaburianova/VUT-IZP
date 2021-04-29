/**
 * @file proj3.c
 * @author Tereza Burianova, xburia28
 * @date 10 Dec 2019
 * @brief A triangle maze solver.
 *
 * The program solves a triangular maze using the right-hand or left-hand rule.
 * Arguments:
 * * the requested program response (--help for help, --test to determine the validity of the map file,
 *   --rpath to solve the maze using the right-hand rule, --lpath to solve the maze using the left-hand rule)
 * * (in case of --lpath, --rpath) entry row, entry column
 * * name of the file containing the maze map
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

#define triangle_rotation (curr_coordinates[0]+curr_coordinates[1])%2
#define cell_value ((r-1)*map->cols+c)-1
#define cell_amount map->rows * map->cols

enum wall{horizontal, right, left};

int load_map(Map *map, char *file_name);
int program_response(char argument[], int *hand_rule, int arguments);
int arguments_validity(char *convertErr1, char *convertErr2);
void cell_movement(Map *map, int curr_coordinates[], int *direction, int direction_increment, int updown);
int start_border(Map *map, int r, int c);
int pathfinding(Map *map, int curr_coordinates[], int hand_rule);
bool isborder(Map *map, int r, int c, int border);
void map_dtor(Map *map);
//int map_validity(Map *map);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "There are too few arguments!\n");
        return 1;
    }
    Map map;
    int curr_coordinates[2] = {0, 0};
    int hand_rule;
    char *err1, *err2;
    char *file_name;
    // for --rpath or --lpath
    if(program_response(argv[1], &hand_rule, argc) == 0 && argc > 3) {
        curr_coordinates[0] = strtol(argv[2], &err1, 10);
        curr_coordinates[1] = strtol(argv[3], &err2, 10);
        file_name = argv[4];
        if (arguments_validity(err1, err2) == 1)
            return 1;
    // for --help, --test or invalid input
    } else {
        return 0;
    }
    if (load_map(&map, file_name) == 1)
        return 1;
    if(curr_coordinates[0] > map.rows || curr_coordinates[0] < 1 || curr_coordinates[1] > map.cols || curr_coordinates[0] < 1) {
        fprintf(stderr, "The cell is not located in the maze (%d rows, %d columns)!\n", map.rows, map.cols);
        map_dtor(&map);
        return 1;
    }
    if (pathfinding(&map, curr_coordinates, hand_rule) == -1) {
        // the cell could not be accessed
        map_dtor(&map);
        return 1;
    }
    map_dtor(&map);
    return 0;
}

/* Function load_map:
 * Arguments: Map *map (pointer to a structure of type Map), char *file_name (name of a file containing the maze map)
 * Return value: 1 for error, 0 for success
 * Functionality: Loads the information from the file to a Map struct (rows, columns, cell values).
 */
int load_map(Map *map, char *file_name) {
    FILE *map_file;
    map_file = fopen(file_name, "r");
    if(!map_file) {
        fprintf(stderr, "The program was unable to load the file!\n");
        return 1;
    }
    fscanf(map_file, "%d", &(map->rows));
    fscanf(map_file, "%d", &(map->cols));
    map->cells = malloc(cell_amount * sizeof(unsigned char)+1);
    if(map->cells == NULL)
        return 1;
    for(int i = 0; i < cell_amount; i++) {
        fscanf(map_file, "%hhu", &(map->cells[i]));
    }
    fclose(map_file);
    return 0;
}

/* Function program_response:
 * Arguments: char argument[] (argument determining the program response), int *hand_rule (0 for left, 1 for right),
 *            int arguments (number of arguments given by the user)
 * Return value: 1 for error or for terminating the program after its functionality has been completed, 0 for success
 * Functionality: Determines the next behaviour of the program according to the argument given by the user.
 */
int program_response(char argument[], int *hand_rule, int arguments) {
    if(strcmp(argument, "--lpath") == 0) {
        if(arguments != 5) {
            fprintf(stderr, "5 arguments are needed to run the program! (path, entry row, entry column, map file)\n");
            return 1;
        }
        *hand_rule = 0;
    }
    else if(strcmp(argument, "--rpath") == 0) {
        if (arguments != 5) {
            fprintf(stderr, "5 arguments are needed to run the program! (path, entry row, entry column, map file)\n");
            return 1;
        }
        *hand_rule = 1;
    }
    else if(strcmp(argument, "--help") == 0) {
        fprintf(stdout, "MAZE SOLVING PROGRAM\n"
                        "* ./proj3 --help ** opens help to the program\n"
                        "* ./proj3 --test ** checks the map file for invalid values\n"
                        "* ./proj3 --rpath entry_row entry_column filename.txt ** solves the maze, starting with entered cell, using the right-hand rule\n"
                        "* ./proj3 --lpath entry_row entry_column filename.txt ** solves the maze, starting with entered cell, using the left-hand rule\n");
        return 1;
    } else if(strcmp(argument, "--test") == 0) {
        // TODO - function
        return 1;
    } else {
        fprintf(stderr, "Invalid argument!\n");
        return 1;
    }
    return 0;
}

/* Function arguments_validity:
 * Arguments: char *convertErr1, char *convertErr2 (char pointers used in the strtol function)
 * Return value: 1 for error, 0 for success
 * Functionality: Checks the arguments for unexpected values.
 */
int arguments_validity(char *convertErr1, char *convertErr2) {
    if (*convertErr1 != '\0' || *convertErr2 != '\0') {
        fprintf(stderr, "No valid starting coordinates in the arguments!\n");
        return 1;
    }
    if (errno == ERANGE) {
        fprintf(stderr, "One of the arguments is too long for the used data type (int)!\n");
        return 1;
    }
    return 0;
}

/* Function cell_movement:
 * Arguments: Map *map (pointer to a structure of type Map), int curr_coordinates (array of the current coordinates),
 *            int *direction (current rotation in the cell), int direction_increment (increment determining the clockwise
 *            or counterclockwise rotation in the cell), int updown (horizontal movement according to the rotation of the
 *            triangle, 0 for down and 2 for up)
 * Return value: void
 * Functionality: Determines the rotation in the current cell and executes the corresponding movement.
 */
void cell_movement(Map *map, int curr_coordinates[], int *direction, int direction_increment, int updown) {
    int moves[4][2] = {
            {1, 0}, {0, -1}, {-1, 0}, {0, 1}
    };
    enum directions{d_down, d_left, d_up, d_right};
    while (1) {
        // rotation in the cell
        *direction = (*direction + direction_increment+3) % 3;
        if (isborder(map, curr_coordinates[0], curr_coordinates[1], *direction) == 0) {
            break;
        }
    }
    fprintf(stdout, "%d,", curr_coordinates[0]);
    fprintf(stdout, "%d\n", curr_coordinates[1]);
    // move to the new cell
    switch (*direction) {
        case 0:
            for (int i = 0; i < 2; i++)
            curr_coordinates[i] += moves[updown][i];
            break;
        case 1:
            for (int i = 0; i < 2; i++)
                curr_coordinates[i] += moves[d_right][i];
            break;
        case 2:
            for (int i = 0; i < 2; i++)
                curr_coordinates[i] += moves[d_left][i];
            break;
    }
}

/* Function start_border:
 * Arguments: Map *map (pointer to a structure of type Map), int r (entry row), int c (entry column),
 *            leftright (//TODO)
 * Return value: -1 for error, other integer values as the index of the current border
 * Functionality: Determines the entry border of the first entered triangle.
 */
int start_border(Map *map, int r, int c) {
    // entry from the left side
    if(c == 1) {
        if(isborder(map, r, c, 2) == 0)
            return 2;
        else if((r == 1 || r == map->rows) && isborder(map, r, c, 0) == 0)
            return 0;
        else {
            fprintf(stderr, "The starting cell could not be accessed!\n");
            return -1;
        }
    }
    // entry from the right side
    if(c == map->cols) {
        if(isborder(map, r, c, 1) == 0)
            return 1;
        else if((r == 1 || r == map->rows) && isborder(map, r, c, 0) == 0)
            return 0;
        else {
            fprintf(stderr, "The starting cell could not be accessed!\n");
            return -1;
        }
    }
    // entry from above/below
    if(r == 1 || r == map->rows) {
        if(isborder(map, r, c, 0) == 0)
            return 0;
        else {
            fprintf(stderr, "The starting cell could not be accessed!\n");
            return -1;
        }
    }
    // the cell is not located on the edge
    fprintf(stderr, "The starting cell must be on the edge of the map!\n");
    return -1;
}

/* Function pathfinding:
 * Arguments: Map *map (pointer to a structure of type Map), int curr_coordinates (array of the current coordinates),
 * int hand_rule (0 for left, 1 for right)
 * Return value: 1 for error, 0 for success
 * Functionality: Calls the function cell_movement until the solution of the maze is found.
 */
int pathfinding(Map *map, int curr_coordinates[], int hand_rule) {
    int direction = start_border(map, curr_coordinates[0], curr_coordinates[1]);
    if (direction == -1)
        return 1;
    int crossed, direction_increment, triangle_increment;
    enum rules{left_hand, right_hand};
    enum increment{increment = 1, decrement = -1};
    if(hand_rule == left_hand) {
        triangle_increment = decrement;
    } else {
        triangle_increment = increment;
    }
    // while the cells are still inside the maze
    while(curr_coordinates[0] > 0 && curr_coordinates[1] > 0 && curr_coordinates[0] <= map->rows && curr_coordinates[1] <= map->cols) {
        direction_increment = triangle_increment;
        // normal triangle
        if(triangle_rotation == 1)
            cell_movement(map, curr_coordinates, &direction, direction_increment, 0);
        // upside down triangle
        else {
            direction_increment *= -1;
            cell_movement(map, curr_coordinates, &direction, direction_increment, 2);
        }
        crossed = direction;
        // crossed border in the context of the next triangle
        if(crossed == 2)
            direction = 1;
        if(crossed == 1)
            direction = 2;
    }
    return 0;
}

/* Function isborder:
 * Arguments: Map *map (pointer to a structure of type Map), int r, int c (current coordinates),
 *            int border (wanted triangle border index, horizontal = 0, right = 1, left = 2)
 * Return value: 1 (true) for an impassable wall, 0 (false) for free passage
 * Functionality: Checks a border of a current (r, c) triangle for a wall.
 */
bool isborder(Map *map, int r, int c, int border) {
    // value of the needed cell in the map.cells array
    int value = (int)map->cells[cell_value];
    bool borders[3];
    // conversion from decimal to binary numbers
    value %= 8;
    if(value >= 4) {
        borders[horizontal] = 1;
        value -= 4;
    } else
        borders[horizontal] = 0;
    if(value >= 2) {
        borders[right] = 1;
        value -= 2;
    } else
        borders[right] = 0;
    if(value >= 1)
        borders[left] = 1;
    else
        borders[left] = 0;
    return borders[border];
}

/* Function map_dtor:
 * Arguments: Map *map (pointer to a structure of type Map)
 * Return value: void
 * Functionality: Deallocates the space previously allocated for cell values and sets the cells value to NULL.
 */
void map_dtor(Map *map) {
    if (map->cells != NULL) {
        free(map->cells);
        map->cells = NULL;
    }
}

/* does not work
int map_validity(Map *map) {
    int r = 1, c = 1;
    // shared border check
    while((r != map->rows) && (c != map->cols)) {
        if (isborder(map, r, c, left) != isborder(map, r, c-1, right) && c != 1) {
            fprintf(stderr, "Shared borders are invalid!");
            return 1;
        }
        if ((isborder(map, r, c, right) != isborder(map, r, c+1, left)) && c != map->cols) {
            fprintf(stderr, "Shared borders are invalid!");
            return 1;
        }
        // normal triangle
        if((r+c)%2 == 1) {
            if (isborder(map, r, c, horizontal) != isborder(map, r+1, c, horizontal) && r != map->rows) {
                fprintf(stderr, "Shared borders are invalid!");
                return 1;
            }
        }
            // upside down triangle
        else {
            if (isborder(map, r, c, horizontal) != isborder(map, r-1, c, horizontal) && r != 1) {
                fprintf(stderr, "Shared borders are invalid!");
                return 1;
            }
        }
        if(r != map->rows)
            r++;
        else {
            c++;
            r = 1;
        }
    }
    return 0;
}
*/