#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 10240
#define EXCEPTION_OUT_OF_BOUND -2
#define TILL_END __INT32_MAX__

char temp[MAX_INPUT_LENGTH]; // slouzi k ulozeni posledniho nacteneho radku

/* ---- Pomocne funkce ----- */

int indexof(char *line, char c, int starting_index, int position);
void ignore_lines(int length, char *line);
int get_column_count(char *line, char delim);
void load_line(char *line);
void push_line(char *line);
void get_newline(char delim, int column_count, bool newline);

/* ---- Uprava tabulky ----- */

void irow(int row, char delim, int column_count);
void arow(char delim, int column_count);
void drow(int row);

/* ---- Zpracovavani dat --- */

/* ------------------------- */

int main(int argc, char *argv[])
{
    char delim = ':';
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d") && argc >= i + 2)
            delim = argv[i + 1][0];
    }
    int column_count = 3;
    // (void)column_count;
    (void)delim;
    //ignore_lines(3);
    // irow(3, delim, column_count);
    // drow(3);
    arow(delim, column_count);
    return 0;
}

/** 
 * Tato funkce prohledava retezec a 
 * hleda index znaku
 * @param line retezec jednoho radku
 * @param c hledany znak
 * @param starting_index pocatecni index, od ktereho ma funkce zacit hledat
 * @param position cislo udava, kolikaty znak v retezci ma hledat
 * @return vraci index hledaneho znaku v retezci
*/
int indexof(char *line, char c, int starting_index, int position)
{
    if ((long unsigned int)starting_index >= strlen(line))
    {
        printf("Error: index out of bound!\n");
        return EXCEPTION_OUT_OF_BOUND;
    }

    long unsigned int i = starting_index;
    int count = 0;
    for (; i < strlen(line); i++)
    {
        if (line[i] == c)
        {
            if (++count == position)
                return i;
        }
    }
    return -1;
}

/** 
 * Tato funkce jenom prekopiruje radek do dalsiho souboru
 * @param length pocet radku, ktere ma precist a vypsat
 * @return vraci posledni radek, ktery se nevytisknul
*/
void ignore_lines(int length, char *line)
{
    int i;
    for (i = 0; fgets(line, MAX_INPUT_LENGTH, stdin) != NULL && i < length; i++)
    {
        fputs(line, stdout);
    }
}

/**
 * Spocita, kolik je v retezci oddelovacu
 * a vraci pocet sloupcu
 * @param line retezec radku
 * @param delim znak oddelovace
 * @return pocet sloupcu
*/
int get_column_count(char *line, char delim)
{
    long unsigned int i;
    int delim_count;
    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == delim)
            delim_count++;
    }
    return delim_count + 1;
}

/**
 * Nacte do retezce line nasledujici radek ze souboru
 * @param line retezec
*/
void load_line(char *line)
{
    fgets(line, MAX_INPUT_LENGTH, stdin);
}

/**
 * Vypise retezec do souboru
 * @param line retezec 
*/
void push_line(char *line)
{
    fputs(line, stdout);
}

/** 
 * Funkce vytvori novy radek a 
*/
void get_newline(char delim, int column_count, bool newline)
{
    int array_length = newline ? column_count : column_count - 1;
    char str[array_length];
    str[array_length] = '\0';
    for (int i = 0; i < column_count - 1; i++)
    {
        str[i] = delim;
    }
    if (newline)
        str[array_length - 1] = '\n';
    push_line(str);
}

/* ---------------------------------- */

void irow(int row, char delim, int column_count)
{
    ignore_lines(row - 1, temp);

    get_newline(delim, column_count, true);

    push_line(temp);
    ignore_lines(TILL_END, temp);
}

void arow(char delim, int column_count)
{
    ignore_lines(TILL_END, temp);
    push_line("\n\0");
    get_newline(delim, column_count, false);
}
void drow(int row)
{
    ignore_lines(row - 1, temp);
    ignore_lines(TILL_END, temp);
}