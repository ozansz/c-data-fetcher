#include <stdio.h>

FILE *f;

int main (int argc, char **argv, char **envp) {
    int i;
    char ch;
    long arr[2];
    unsigned int arr2[2][1];

    if (argc != 2) {
        printf("\n[+] Usage: ./%s <filename>\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "w");

    for (i = 1; i < 3; i++) {
        ch = 65 + i;
        arr[i-1] = 2 * i * i;
        arr2[i-1][0] = 10 - i;

        fwrite(&i, sizeof(int), 1, f);
        fwrite(&ch, sizeof(char), 1, f);
    }

    fwrite(arr, sizeof(long), 2, f);
    fwrite(arr2, sizeof(unsigned int), 2 * 1, f);

    fclose(f);

    return 0;
}