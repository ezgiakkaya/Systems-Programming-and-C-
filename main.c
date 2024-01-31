//EZGI AKKAYA 79676

#include <stdio.h>
#include <stdlib.h>

#define EMPTY '-'
#define HUMAN 'H'
#define ZOMBIE 'Z'

// Function Prototypes
char** read_map_from_txt(const char* filename, int *width, int *height);
void print_grid(char** grid, int width, int height);
void simulate(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead);
void cleanup_grid(char** grid, int height);
void move_entities(char** grid, char** temp_grid, int width, int height, int step);
void move_zombies(char** grid, char** temp_grid, int width, int height, int step);
void zombie_human_rules(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead);
void copy_grid(char** source, char** destination, int width, int height);
int are_humans_left(char** grid, int width, int height);



//this function aims to read the given map as txt file
//malloc is used to dynamically allocate the needed memory which depends on widht and height

char** read_map_from_txt(const char* filename, int *width, int *height) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file\n");

        return NULL;
    }

    fscanf(file, "%d %d", width, height);
    

    char **grid = (char **)malloc(*height * sizeof(char *));
    for (int i = 0; i < *height; ++i) {
        grid[i] = (char *)malloc((*width + 1) * sizeof(char));
        fscanf(file, "%s", grid[i]);
    }

    fclose(file);
    return grid;
}

//using a for loop printing the grid!
void print_grid(char** grid, int width, int height) {
   for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}


// we need to use a temporary grid

char** create_temp_grid(int width, int height) {
    //again we need to use dynamic allocation
    // allocate memory for an array of pointers, each representing a row in the grid.
    //  size of each element in this array is the size of a pointer to char,
    // and the total size is the number of rows (height) times the size of a char pointer.
  char **temp_grid = (char **)malloc(height * sizeof(char *));
  //iterate over each row
    for (int i = 0; i < height; i++) {
        temp_grid[i] = (char *)malloc((width + 1) * sizeof(char));
        for (int j = 0; j < width; j++) {
            temp_grid[i][j] = EMPTY;
        }
        //making each row a valid C-style string.
        temp_grid[i][width] = '\0'; 
    }
    return temp_grid;
}




int are_humans_left(char** grid, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == HUMAN) {
                return 1; // Human found
            }
        }
    }
    return 0; // No humans left
}


void move_entities(char** grid, char** temp_grid, int width, int height, int step) {
    // Copy the original grid to the temporary grid
    copy_grid(grid, temp_grid, width, height);

    // FOR HUMANS
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == HUMAN) {
                int new_i = i, new_j = j;
                if (step % 2 == 1) { // Even steps - move down
                    new_i++;
                } else { // Odd steps - move right
                    new_j++;
                }

                //BOUNDARY CHECK
                if (new_i < height && new_j < width && grid[new_i][new_j] == EMPTY) {
                    temp_grid[new_i][new_j] = HUMAN;
                    temp_grid[i][j] = EMPTY;
                }
            }
        }
    }

    move_zombies(grid, temp_grid, width, height, step);
}
void move_zombies(char** grid, char** temp_grid, int width, int height, int step) {
    // zombies move right fown left up
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == ZOMBIE) {
                int direction_of_movement = step % 4; // Determines the direction of movement
                int new_i = i + dy[direction_of_movement];
                int new_j = j + dx[direction_of_movement];

//check the conditions!
                if (new_i >= 0 && new_i < height && new_j >= 0 && new_j < width && temp_grid[new_i][new_j] == EMPTY) {
                    temp_grid[new_i][new_j] = ZOMBIE;
                    temp_grid[i][j] = EMPTY;
                }
            }
        }
    }
   
}




void zombie_human_rules(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead) {
    //up right down left
    int dx[] = {0, 1, 0, -1}; 
    int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == ZOMBIE) {
                int human_count = 0;

                // we need to check the adjacent cells for humans.
                for (int k = 0; k < 4; k++) {
                    int adj_x = j + dx[k];
                    int adj_y = i + dy[k];

//checking the boundaries and be sure the adj are human
                    if (adj_x >= 0 && adj_x < width && adj_y >= 0 && adj_y < height && grid[adj_y][adj_x] == HUMAN) {
                        human_count++;
                    }
                }
//zombies die with at least 2 human interaction
                if (human_count >= 2) {
                    grid[i][j] = EMPTY; // Zombie is killed
                    (*zombies_dead)++;
                }
            }
        }
    }

    // checking for humans turning into zombies and humans reaching the exit
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] == HUMAN) {
                int zombie_count = 0;

                // checking adjacent cells for zombies
                for (int k = 0; k < 4; k++) {
                    int adj_x = j + dx[k];
                    int adj_y = i + dy[k];

                    if (adj_x >= 0 && adj_x < width && adj_y >= 0 && adj_y < height && grid[adj_y][adj_x] == ZOMBIE) {
                        zombie_count++;
                    }
                }

                if (zombie_count >= 1) {
                    grid[i][j] = ZOMBIE; // --> case when human is infected by zombies
                    (*humans_infected)++;
                } else if (i == height - 1 && j == width - 1) {
                    grid[i][j] = EMPTY; // human SUCCESFULLY exit!
                    (*humans_survived)++;
                }
            }
        }
    }
}


// duplicates the state of the source grid into the destination grid.
 // which is useful for updating the grid state while keeping the changes atomic and isolated
// until the entire grid has been done
void copy_grid(char** source, char** destination, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            destination[i][j] = source[i][j];
        }
    }
}

void cleanup_grid(char** grid, int height) {
    //we need to clean the allocated memory
     for (int i = 0; i < height; i++) {
        //doign for each row
        free(grid[i]);
    }
    // after freeing all the individual rows, free the memory allocated for the array of pointers.
    // this array was holding the pointers to each row. not freeing this memory would also lead
    // to a memory leak. 
    
    //as in the lecture notes, it is important to first free each row and then free the array
    free(grid);

}

//
void simulate(char** grid, int width, int height, int *humans_survived, int *humans_infected, int *zombies_dead) {
    // creating a temporary grid that mirrors the original grid. 

    // we apply changes during each step of the simulation without affecting the original grid until all
    // changes for a single step are completed.

    char** temp_grid = create_temp_grid(width, height);
    int step = 0;

    // the simulation goes as long as there are humans left on the grid. 
    // simulation ends when no humans are left.
    while (are_humans_left(grid, width, height)) {

        move_entities(grid, temp_grid, width, height, step);
        zombie_human_rules(temp_grid, width, height, humans_survived, humans_infected, zombies_dead);
        // copy the updated temporary grid back to the original grid.
        // changesare reflected  (movements and interactions) 
        copy_grid(temp_grid, grid, width, height);
        print_grid(grid, width, height);
        printf("\n");
        step++;
    }

    cleanup_grid(temp_grid, height);
}



int main(int argc, char **argv) {
   
    int width, height;
    char *filename = argv[1]; 

    char **grid = read_map_from_txt(filename, &width, &height);

    if (grid == NULL) {
        return 1;
    }

    printf("Initial Grid:\n");
    print_grid(grid, width, height);
    
    int humans_infected = 0;
    int humans_survived = 0;
    int zombies_dead = 0;
    
   simulate(grid, width, height, &humans_survived, &humans_infected, &zombies_dead);
    
    printf("Humans survived: %d\nHumans infected: %d\nZombies died: %d\n", humans_survived, humans_infected, zombies_dead);
    printf("Final Grid:\n");
    print_grid(grid, width, height);
    
    cleanup_grid(grid, height);

    return 0;
}

