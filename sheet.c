#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 10240
#define EXCEPTION_OUT_OF_BOUND -2
#define TILL_END __INT32_MAX__

char temp[MAX_INPUT_LENGTH]; // slouzi k ulozeni posledniho nacteneho radku

/* ---- Pomocne funkce ----- */

int indexof(char *line, char c, int start_index, int position);
void ignore_lines(int length, char *line);
int get_column_count(char *line, char delim);
void load_line(char *line);
void push_line(char *line);
void create_newline(char delim, int column_count, bool newline);
void insert_str(char *str_in, int start_index, char *str_insert, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);
int last_index(char *str);

/* ---- Uprava tabulky ----- */

void irow(char delim, int column_count);
void arow(char delim, int column_count);
void drow(int row);
void drows(int starting_row, int ending_row);
void icol(int column, char delim, int column_count);

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
    (void)column_count;
    (void)delim;
    // arow(delim, column_count);
    // drows(5, 7);
    // char str1[MAX_INPUT_LENGTH] = "Hello world!";
    // char *ins_char = ",";
    // insert_str(str1, ",", 5);
    // printf("%s\n", str1);
    icol(4, delim, column_count);
    return 0;
}

/** 
 * Tato funkce prohledava retezec a 
 * hleda index znaku
 * @param line retezec jednoho radku
 * @param c hledany znak
 * @param start_index pocatecni index, od ktereho ma funkce zacit hledat
 * @param position cislo udava, kolikaty znak v retezci ma hledat
 * @return vraci index hledaneho znaku v retezci
*/
int indexof(char *line, char c, int start_index, int position)
{
    if (start_index >= (int)strlen(line))
    {
        printf("Error: index out of bound!\n");
        return EXCEPTION_OUT_OF_BOUND;
    }

    int i = start_index;
    int count = 0;
    for (; i < (int)strlen(line); i++)
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
    int i;
    int delim_count;
    for (i = 0; i < (int)strlen(line); i++)
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
 * Funkce vytvori novy radek a vypise do stdout souboru
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu
 * @param newline true, pokud je potreba dat znak '\n' na konci radku
*/
void create_newline(char delim, int column_count, bool newline)
{
    int array_length = newline ? column_count : column_count - 1;
    char loc_str[array_length];
    loc_str[array_length] = '\0';
    for (int i = 0; i < column_count - 1; i++)
    {
        loc_str[i] = delim;
    }
    if (newline)
        loc_str[array_length - 1] = '\n';
    push_line(loc_str);
}

/**
 * Funkce vytvari cast retezce
 * @param str_in puvodni retezec
 * @param start_index pocatecni umisteni (odkud se zacina)
 * @param len delka noveho retezce
 * @param str_out ukazatel na vysledny retezec
*/
void substring(char *str_in, int start_index, int len, char *str_out)
{
    // pokud dojde k preteceni -> vraci prazdny retezec
    if (start_index + len > (int)strlen(str_in))
    {
        strcpy(str_out, "\0");
        return;
    }

    char loc_str[len + 1]; /* lokalni retezec */

    // tvorba retezce samotneho
    for (int i = 0; i < len; i++)
    {
        loc_str[i] = str_in[start_index + i];
    }
    loc_str[len] = '\0';

    // prirazeni k vyslednemu retezci
    strcpy(str_out, loc_str);
}

/**
 * Funkce slouzi k vlozeni retezce do retezce
 * @param str_in puvodni retezec
 * @param start_index index, na kterem ma funkce vlozit retezec
 * @param str_insert retezec, ktery se ma vlozit
 * @param str_out vysledny retezec
*/
void insert_str(char *str_in, int start_index, char *str_insert, char *str_out)
{
    // funkce nepocita '\n' za vysledny znak
    if (str_in[last_index(str_in)] == '\n' && start_index > 0)
        start_index--;

    // pokud je start_index == len -> pridat dozadu (append)
    // pokud je start_index > len -> chyba preteceni -> vraci prazdny retezec
    if (start_index > (int)strlen(str_in))
    {
        strcpy(str_out, "\0");
        return;
    }
    char loc_str[MAX_INPUT_LENGTH]; /* lokalni retezec */
    loc_str[0] = '\0';

    // cast pred vkladany retezec
    if (start_index > 0)
    {
        int pre_str_len = start_index + 1;
        char pre_str[pre_str_len];
        substring(str_in, 0, start_index, pre_str);
        strcat(loc_str, pre_str);
    }

    strcat(loc_str, str_insert);

    // cast po vlozenem retezci
    if (start_index <= (int)strlen(str_in))
    {
        int post_str_len = (int)strlen(str_in) - start_index + 1;
        char post_str[post_str_len];
        substring(str_in, start_index, (int)strlen(str_in) - start_index, post_str);
        strcat(loc_str, post_str);
    }

    // prirazeni do vysledneho retezce
    strcpy(str_out, loc_str);
}

/** 
 * Funkce vraci index posledniho znaku
 * @param str retezec
 * @return index posledniho znaku
*/
int last_index(char *str)
{
    return (int)strlen(str) - 1;
}

/* ---------------------------------- */

/**
 * Funkce prida novy radek pred radek row. 
 * @param row radek, pred ktery se ma vlozit novy radek (row > 0)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu
*/
void irow(char delim, int column_count)
{
    create_newline(delim, column_count, true);
    push_line(temp);
}

/** 
 * Funkce smaze radek v souboru. \n 
 * Funkce prochazi cely soubor
 * @param row radek, ktery ma byt smazan (row > 0)
*/
void drow(int row)
{
    ignore_lines(row - 1, temp);
    ignore_lines(TILL_END, temp);
}

/** 
 * Funkce smaze radky od starting_row do ending_row vcetne
 * @param starting_row pocatek intervalu (prvni radek, ktery se smaze)
 * @param ending_row konec intervalu (posledni radek, ktery se smaze)
*/
void drows(int starting_row, int ending_row)
{
    ignore_lines(starting_row - 1, temp);
    for (int i = 0; i <= ending_row - starting_row; i++)
    {
        ignore_lines(0, temp);
    }
    push_line(temp);
    ignore_lines(TILL_END, temp);
}

/** 
 * Vlozi sloupec do radku
 * @param column cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
*/
void icol(int column, char delim, int column_count)
{
    char str_insert[2];         /* retezec oddelovace */
    str_insert[0] = delim;
    str_insert[1] = '\0';

    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        if (column == 1)        // pokud chce uzivatel pridat novy sloupec pred prvni
            insert_str(temp, 0, str_insert, temp);
        else if (column == column_count + 1)    // pokud chce uzivatel pridat sloupec za posledni sloupec
            insert_str(temp, last_index(temp) + 1, str_insert, temp);
        else
        {
            int starting_index = indexof(temp, delim, 0, column - 1) + 1;   /* zacatek nove bunky */
            insert_str(temp, starting_index, str_insert, temp);
        }
        push_line(temp);
    }
}