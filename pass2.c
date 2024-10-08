#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void display();

int main()
{
    char a[10], ad[10], label[10], opcode[10], operand[10];
    int start, diff, i, address, add, len, actual_len, finaddr, prevaddr, j = 0;
    char mnemonic[15][15] = {"LDA", "STA", "LDCH", "STCH"};
    char code[15][15] = {"33", "44", "53", "57"};

    FILE *fp1, *fp2, *fp3, *fp4;
    fp1 = fopen("output.txt", "w");
    fp2 = fopen("symtab.txt", "r");
    fp3 = fopen("intermediate.txt", "r");
    fp4 = fopen("objcode.txt", "w");

    if (!fp1 || !fp2 || !fp3 || !fp4) {
        perror("Error opening file");
        return 1;
    }

    // Read first line
    fscanf(fp3, "%s\t%s\t%s", label, opcode, operand);
    prevaddr = 0; // Initialize previous address

    // Iterate until END
    while (strcmp(opcode, "END") != 0)
    {
        prevaddr = address; // Store previous address
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
    }
    finaddr = address; // Final address when END is reached
    
    fclose(fp3);
    fp3 = fopen("intermediate.txt", "r");

    // Read again for START
    fscanf(fp3, "\t%s\t%s\t%s", label, opcode, operand);
    if (strcmp(opcode, "START") == 0)
    {
        fprintf(fp1, "\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(fp4, "H^%s^00%s^%02X\n", label, operand, finaddr); // Use %02X for length
        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
        start = address;
        diff = prevaddr - start;
        fprintf(fp4, "T^%02X^%02X", address, diff);
    }

    // Main processing loop
    while (strcmp(opcode, "END") != 0)
    {
        if (strcmp(opcode, "BYTE") == 0)
        {
            fprintf(fp1, "%d\t%s\t%s\t%s\t", address, label, opcode, operand);
            len = strlen(operand);
            actual_len = len - 3; // Skip C' or X'
            fprintf(fp4, "^");
            for (i = 2; i < (actual_len + 2); i++)
            {   
                // Convert each byte to hex
                fprintf(fp4, "%02x", operand[i]); // Change to hex format
            }
            fprintf(fp1, "\n");
        }
        else if (strcmp(opcode, "WORD") == 0)
        {
            fprintf(fp1, "%d\t%s\t%s\t%s\t00000%s\n", address, label, opcode, operand, operand);
            fprintf(fp4, "^%06d", atoi(operand)); // Ensure six digits for WORD
        }
        else if (strcmp(opcode, "RESB") == 0 || strcmp(opcode, "RESW") == 0)
        {
            fprintf(fp1, "%d\t%s\t%s\t%s\n", address, label, opcode, operand);
        }
        else
        {
            while (strcmp(opcode, mnemonic[j]) != 0 && j < 4) // Ensure within bounds
                j++;
            if (j < 4) // Found opcode
            {
                fprintf(fp1, "%d\t%s\t%s\t%s\t%s%d\n", address, label, opcode, operand, code[j], add);
                fprintf(fp4, "^%s%02d", code[j], add); // Add address to object code
            }
            else
            {
                // Handle undefined opcode
                printf("Warning: Opcode %s not found in mnemonic table\n", opcode);
            }
        }

        fscanf(fp3, "%d%s%s%s", &address, label, opcode, operand);
    }

    // Write final line to output and object files
    fprintf(fp1, "%d\t%s\t%s\t%s\n", address, label, opcode, operand);
    fprintf(fp4, "\nE^%02X", start); // Change to %02X to match expected format
    
    fclose(fp4);
    fclose(fp3);
    fclose(fp2);
    fclose(fp1);

    display();

    return 0;
}

void display() {
    char ch;
    FILE *fp1, *fp2, *fp3, *fp4;

    printf("\nIntermediate file is converted into object code");

    printf("\n\nThe contents of Intermediate file:\n\n");
    fp3 = fopen("intermediate.txt", "r");
    if (fp3) {
        while ((ch = fgetc(fp3)) != EOF) {
            putchar(ch);
        }
        fclose(fp3);
    }

    printf("\n\nThe contents of Symbol Table :\n\n");
    fp2 = fopen("symtab.txt", "r");
    if (fp2) {
        while ((ch = fgetc(fp2)) != EOF) {
            putchar(ch);
        }
        fclose(fp2);
    }

    printf("\n\nThe contents of Output file :\n\n");
    fp1 = fopen("output.txt", "r");
    if (fp1) {
        while ((ch = fgetc(fp1)) != EOF) {
            putchar(ch);
        }
        fclose(fp1);
    }

    printf("\n\nThe contents of Object code file :\n\n");
    fp4 = fopen("objcode.txt", "r");
    if (fp4) {
        while ((ch = fgetc(fp4)) != EOF) {
            putchar(ch);
        }
        fclose(fp4);
    }
}

