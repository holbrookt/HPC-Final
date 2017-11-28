#include "data.h"

// Reads data files.
// Data should be in the form of:
// SOLUTION   INDEX:Value INDEX:Value
//
// i.e with three feature data set all valued at .5 and a solution of 1:
// 1 1:.5 2:.5 3:.5
void readfile(char* filename, double ** data, double *solutions, int number_of_features) {
    FILE* fp = fopen(filename, "r");
    //char*
    char line[1200];
    int example_number = 0;
    printf("also should fail? %f\n", data[9999][0]);
    while(fgets(line, sizeof(line), fp)){
        //printf("%s", line);
        char ** line_tokens = malloc((number_of_features+1)*2*sizeof(char*));
        int i = 0;

        // Clear out any newline chars from the end of the file
        int term_length = strcspn(line, "\n\r");
        line[term_length] = '\0';

        // Seperate lines into tokens
        line_tokens[i] = strtok(line, " ");
        
        // The first token is the solution
        solutions[example_number] = (double)atoi(line_tokens[i]);
        while (line_tokens[i] != NULL) {
            line_tokens[++i] = strtok(NULL, " ");
        }

        // Align the data. Start at 0 and continue until null because
        // we don't know the length of the line_tokens array because
        // We could have been missing data in the middle
        i = 1;
        while (line_tokens[i] != NULL) {
            //printf("line_token: %s  -- ", line_tokens[i]);
            // Grab the reported index
            int index = atoi(strtok(line_tokens[i], ":"));
            //printf(" got index %d\n", index);
            char * value = strtok(NULL,":");
            double float_value = atof(value);
            // set the target index to the reported value
            //printf("data == %f\n", data[example_number]);
            data[example_number][index-1] = float_value;//atof(strtok(NULL, ":"));
            i++;
        }

        // Allows us to pull an example data point out incase something
        // weird is happening with the file reading
        if (DEBUG == 1 && example_number == EXAMPLE) {
            printf("Data %d:   ", example_number);
            for (i = 0; i<number_of_features; i++) {
                printf("%.3f  ", data[example_number][i]);
            }
            printf("   Solution: %f\n", solutions[example_number]);
        }
        //printf("example: %d\n", example_number);
        example_number++;
        free(line_tokens);
    }
    printf("Closing file\n");
    fclose(fp);
    printf("number_of_entries: %d\n", example_number);
}
