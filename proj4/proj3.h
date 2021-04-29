/**
 * \file proj3.h
 * \author Tereza Burianova, xburia28
 * \date 16 Dec 2019
 * \brief Header file for a triangle maze solver.
 */

#ifndef PROJ3_H
#define PROJ3_H
/**
 * \brief Defines a structure for the maze map.
 *
 * Loads the map file, including size and cell values, to use in the maze solving algorithm.
 */
typedef struct {
    int rows;    /**< Expected number of rows. */
    int cols;   /**< Expected number of columns. */
    unsigned char *cells;   /**< Array of cell values determining the borders. */
} Map;

/**
 * \brief Determines the permeability of borders.
 *
 * Determines the values (thus the permeability) according to last 3 bits of the cell value.
 */
enum borders { BLEFT=0x1, BRIGHT=0x2, BTOP=0x4, BBOTTOM=0x4 };

/**
 * \brief Deallocates the previously allocated memory.
 * \param map A structure containing the allocated memory.
 * \return void
 * \pre The memory must be previously successfully allocated.
 * \post The previously allocated memory is deallocated and the array value is set to NULL.
 */
void free_map(Map *map);

/**
 * \brief Opens the file containing a map and loads the values in the Map structure.
 * \param map A structure to load the values in.
 * \param filename The name of the file to load the values from.
 * \return 0 for success, other int values for errors
 * \pre The file must exist, the memory in the structure must be successfully allocated.
 * \post The values from the file are loaded in the Map structure and the file is closed.
 */
int load_map(const char *filename, Map *map);

/**
 * \brief Finds out if the requested border is passable or not.
 * \param map A structure containing the map values.
 * \param r Current row.
 * \param c Current column.
 * \param border Index of the requested border.
 * \return 0 for a passable border, 1 for an impassable border.
 * \pre The Map structure must contain valid values (unsigned int), the current cell and requested border must exist.
 * \post The patency of the border is determined and returned.
 */
bool isborder(Map *map, int r, int c, int border);

/**
 * \brief Finds out if the current triangle is normal or upside down by determining if the bottom border exists.
 * \param r Current row.
 * \param c Current column.
 * \return 1 (true) for normal triangle (bottom border present), 0 (false) for upside down triangle.
 * \pre The cell must exist and have valid value (unsigned int).
 * \post The rotation of the triangle is determined.
 */
bool hasbottom(int r, int c);

/**
 * \brief Determines the first border to potentially cross.
 * \param map A structure containing the map values.
 * \param r Enter row.
 * \param c Enter column.
 * \param leftright Current rule (left-hand rule or right-hand rule).
 * \return Index of the first border to potentially cross.
 * \pre The cell must exist and the outer border must be passable.
 * \post The border to start with is determined.
 */
int start_border(Map *map, int r, int c, int leftright);

/**
 * \brief Checks the Map structure for potential errors (wrong size, wrong values type).
 * \param map A structure containing the map values.
 * \return 0 for success, other integer values for errors.
 * \pre The structure must be valid (successfully allocated with loaded values).
 * \post The map structure is either checked and ready to use, or an error is returned.
 */
int check_map(Map *map);

/**
 * \brief Opens the file containing a map and loads the values in the Map structure,
 * then checks the Map structure for potential errors (wrong size, wrong values type).
 * \param map A structure to load the values in.
 * \param filename The name of the file to load the values from.
 * \return 0 for success, other int values for errors
 * \pre The file must exist, it must contain valid values (unsigned integers),
 * the memory in the structure must be successfully allocated.
 * \post The values from the file are loaded in the Map structure and the file is closed,
 * then the map structure is either checked and ready to use, or an error is returned.
 */
int load_and_check_map(const char *filename, Map *map);

/**
 * \brief Check the location of the cell and determines if the maze is solved (the next cell would be outside of the maze).
 * \param map A structure containing the map values.
 * \param r Current row.
 * \param c Current column.
 * \return 0 (false) for cells inside the maze, 1 (true) for the cell outside the maze.
 * \pre The maze solving algorithm must work in order to reach the exit.
 * \post The solution (exit) is found.
 */
bool is_out(Map *map, int r, int c);

/**
 * \brief Prints out the coordinates of the passed cell.
 * \param map A structure containing the map values.
 * \param r Current row.
 * \param c Current column.
 * \param leftright Current rule (left-hand rule or right-hand rule).
 * \return void
 * \pre The maze solving algorithm must work in order to pass the cells and the cells must be valid.
 * \post The current coordinates of the passed cells are printed out (stdout).
 */
void print_path(Map *map, int r, int c, int leftright);

#endif