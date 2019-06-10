#include <stdio.h>

int main (int argc, char **argv, char **envp) {
    char *name = "OZAN";
    unsigned int x = 100;
    int i, j, n = 5, m = 3;
    double arr[5][3];

    FILE *f = fopen("data4", "w");

    for (i = 0; i < 5; i++)
        for (j = 0; j < 3; j++)
            arr[i][j] = (double)i / (double)(j+10);

    for (i = 0; i < 4; i++)
        fwrite(name+i, sizeof(char), 1, f);

    fwrite(&m, sizeof(int), 1, f);
    fwrite(&n, sizeof(int), 1, f);
    fwrite(arr, sizeof(double), 5*3, f);
    fwrite(&x, sizeof(unsigned int), 1, f);

    fclose(f);

    return 0;    
}