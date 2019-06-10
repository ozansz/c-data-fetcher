#include <stdio.h>

int main (int argc, char **argv, char **envp) {
    int n = 3, ctr;
    long la[3][2];
    float q = 3.14159265359;
    double qq[2] = {3.14159265359, 3.14159265359 * 3.14159265359};

    FILE *f = fopen("data3", "w");

    for (ctr = 0; ctr < 3; ctr++) {
        la[ctr][0] = ctr + 1;
        la[ctr][1] = ctr * 2;
    }

    fwrite(&n, sizeof(int), 1, f);
    fwrite(la, sizeof(long), 3*2, f);
    fwrite(&q, sizeof(float), 1, f);
    fwrite(qq, sizeof(double), 2, f);

    fclose(f);

    return 0;
}