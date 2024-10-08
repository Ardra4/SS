#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void passOne(char label[10], char opcode[10], char operand[10], char code[10], char mnemonic[3]);
void display();

int main() {
    // for reading from input
    char label[10], opcode[10], operand[10];
    // for reading from optab
    char code[10], mnemonic[3];
    // call the function
    passOne(label, opcode, operand, code, mnemonic);

    return 0;
}

void passOne(char label[10], char opcode[10], char operand[10], char code[10], char mnemonic[3]) {
    int locctr, start, length;

    FILE *fp1, *fp2, *fp3, *fp4, *fp5; // file pointers

    // read mode
    if ((fp1 = fopen("input.txt", "r")) == NULL) {
        perror("Error opening input.txt");
        exit(1);
    }
    if ((fp2 = fopen("optab.txt", "r")) == NULL) {
        perror("Error opening optab.txt");
        exit(1);
    }
    // write mode
    if ((fp3 = fopen("symtab.txt", "w")) == NULL) {
        perror("Error opening symtab.txt");
        exit(1);
    }
    if ((fp4 = fopen("intermediate.txt", "w")) == NULL) {
        perror("Error opening intermediate.txt");
        exit(1);
    }
    if ((fp5 = fopen("length.txt", "w")) == NULL) {
        perror("Error opening length.txt");
        exit(1);
    }

    fscanf(fp1, "%s\t%s\t%s", label, opcode, operand); // read first line

    if (strcmp(opcode, "START") == 0) {
        start = atoi(operand); 
        locctr = start;
        fprintf(fp4, "\t%s\t%s\t%s\n", label, opcode, operand); // write to output file (additional tab space as start will not have any locctr)
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand); // read next line
    } else {
        locctr = 0;
    }

    // iterate till end
    while (strcmp(opcode, "END") != 0) {
        // 1. transfer address and read line to output file
        fprintf(fp4, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);

        // 2. make symtab file with values not starting with **
        if (strcmp(label, "**") != 0) {
            fprintf(fp3, "%s\t%d\n", label, locctr);
        }

        // 3. read from optab (code and mnemonic value)
        rewind(fp2); // Ensure that we start reading from the beginning of the file each time
        while (fscanf(fp2, "%s\t%s", code, mnemonic) != EOF) {
            if (strcmp(opcode, code) == 0) { // if opcode in input matches the one in optab, increment locctr by 3
                locctr += 3;
                break;
            }
        }

        // 4. Searching opcode for WORD, RESW, BYTE, RESB keywords and updating locctr
        if (strcmp(opcode, "WORD") == 0) {
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += (3 * (atoi(operand))); // convert operand to integer and multiply with 3
        } else if (strcmp(opcode, "BYTE") == 0) {
            // The actual byte size of BYTE should be calculated properly; assuming 1 byte for simplicity
            ++locctr;
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        }

        // read next line
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);
    }
    // 6. transfer last line to file
    fprintf(fp4, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);

    // 7. Close all files
    fclose(fp4);
    fclose(fp3);
    fclose(fp2);
    fclose(fp1);

    // 8. display outputs
    display();

    // 9. calculate length of program
    length = locctr - start;
    fprintf(fp5, "%d", length);
    fclose(fp5);
    printf("\nThe length of the code : %d\n", length);
}

void display() {
    char str;
    FILE *fp1, *fp2, *fp3;

    
    printf("\nThe contents of Input Table :\n\n");
    if ((fp1 = fopen("input.txt", "r")) == NULL) {
        perror("Error opening input.txt");
        return;
    }
    while ((str = fgetc(fp1)) != EOF) {
        printf("%c", str);
    }
    fclose(fp1);

   
    printf("\n\nThe contents of Output Table :\n\n");
    if ((fp2 = fopen("intermediate.txt", "r")) == NULL) {
        perror("Error opening intermediate.txt");
        return;
    }
    while ((str = fgetc(fp2)) != EOF) {
        printf("%c", str);
    }
    fclose(fp2);

    // 3. Symtable
    printf("\n\nThe contents of Symbol Table :\n\n");
    if ((fp3 = fopen("symtab.txt", "r")) == NULL) {
        perror("Error opening symtab.txt");
        return;
    }
    while ((str = fgetc(fp3)) != EOF) {
        printf("%c", str);
    }
    fclose(fp3);
}

