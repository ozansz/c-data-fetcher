#include <stdio.h>

FILE *f;

int main (int argc, char **argv, char **envp) {
    int ctr, dctr, ia[16] = {0};
    long la[16] = {0};
    char ca[16] = {0};
    
    int dia[16][4];

    if (argc != 2) {
        printf("\n[+] Usage: ./%s <filename>\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "w");
    
    ctr = 4;
    fwrite(&ctr, sizeof(int), 1, f);

    for (ctr = 0; ctr < 16; ctr++) {
        ia[ctr] = ctr;
        la[ctr] = ctr * 1000;
        ca[ctr] = 65 + ctr;

        for (dctr = 0; dctr < 4; dctr++)
            dia[ctr][dctr] = 2 * ctr * (dctr + 1);
    }

    fwrite(ia, sizeof(int), 16, f);
    fwrite(la, sizeof(long), 16, f);
    fwrite(ca, sizeof(char), 16, f);
    fwrite(dia, sizeof(int), 16 * 4, f);

    fclose(f);

    return 0;
}