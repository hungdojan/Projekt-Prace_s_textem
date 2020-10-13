#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 10241
#define EXCEPTION_OUT_OF_BOUND -2
#define TILL_END __INT32_MAX__
#define DEFAULT_DELIM ' '

char temp[MAX_INPUT_LENGTH]; // slouzi k ulozeni posledniho nacteneho radku

/* ---- Pomocne funkce ----- */

int indexof(char *str_in, char c, int start_index, int position);
void ignore_lines(int length, char *str_in);
int get_char_count(char *str_in, char c);
char set_delim(char *str_in, char *list_of_delims);
void load_line(char *str_out);
void push_line(char *str_in);
void create_newline(char delim, int column_count, bool newline);
void insert_str(char *str_in, int start_index, char *str_to_insert, char *str_out);
void remove_str(char *str_in, int start_index, int length, char *str_out);
void replace_str(char *str_in, int start_index, int length, char *str_replacing, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);
void get_string_in_cell(char *str_in, int column, char delim, int column_count, char *str_out);
int last_index(char *str);

/* ---- Uprava tabulky ----- */

void irow(char delim, int column_count);
void arow(char delim, int column_count);
void drow(int row);
void drows(int starting_row, int ending_row);
void icol(int column, char delim, int column_count);
void dcol(int column, char delim, int column_count);

/* ---- Zpracovavani dat --- */

/* ------------------------- */

int main(int argc, char *argv[])
{
    /** 
     * TODO: znak oddelovace funguje trochu jinak
     * potreba to dodelat 
    */

    char *list_of_delims = ":|,;";
    char file_delim = ' ';
    char output_delim = ':';
    int column_count = 3;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d") && argc >= i + 2)
        {
            strcpy(list_of_delims, argv[i + 1]);
            output_delim = list_of_delims[0];
        }
    }
    
    (void)column_count;
    (void)file_delim;
    (void)output_delim;
    return 0;
}

/** 
 * Tato funkce hleda indexovou pozici znaku v retezci.
 * @param str_in vstupni retezec
 * @param c hledany znak
 * @param start_index pocatecni index, od ktereho ma funkce zacit hledat
 * @param position cislo udava, kolikaty znak v retezci ma hledat
 * @return vraci index hledaneho znaku v retezci (vraci -1 pokud nic nenasel)
*/
int indexof(char *str_in, char c, int start_index, int position)
{
    // kontrola preteceni
    if (start_index >= (int)strlen(str_in))
    {
        printf("Error: index out of bound!\n");
        return EXCEPTION_OUT_OF_BOUND;
    }

    if (position == 0) // specialni pripad position == 0
        return 0;

    int i = start_index;
    int count = 0; // hodnota udava pocet hledanych znaku, ktere jiz potkal

    for (; i < (int)strlen(str_in); i++) // hledani
    {
        if (str_in[i] == c)
        {
            if (++count == position)
                return i;
        }
    }

    return -1; // chybova navratova hodnota
}

/** 
 * Tato funkce jenom prekopiruje radek do dalsiho souboru.
 * @param length pocet radku, ktere ma precist a vypsat
 * @param str_in vstupni retezec 
 * @return vraci posledni radek, ktery se nevytisknul
*/
void ignore_lines(int length, char *str_in)
{
    /**
     * TODO: pridat str_out
    */

    int i;
    for (i = 0; fgets(str_in, MAX_INPUT_LENGTH, stdin) != NULL && i < length; i++)
    {
        fputs(str_in, stdout);
    }
}

/**
 * Spocita, kolik je v retezci znaku c.
 * @param str_in vstupni retezec
 * @param c hledany znak
 * @return pocet sloupcu
*/
int get_char_count(char *str_in, char c)
{
    int i;
    int char_count = 0;
    for (i = 0; i < (int)strlen(str_in); i++)
    {
        if (str_in[i] == c)
            char_count++;
    }
    return char_count;
}

/** 
 * Zjistuje znak oddelovace, ktery se nachazi v tabulkovem souboru. 
 * Vraci DEFAULT_DEMIL pokud nic nenasel.
 * @param str_in vstupni retezec
 * @param list_of_delims retezec moznych oddelovacich znaku (priorita podle poradi)
 * @return oddelovaci znak, ktery se v retezci objevuje alespon jednou
*/
char set_delim(char *str_in, char *list_of_delims)
{
    int i;
    for (i = 0; i < (int)strlen(list_of_delims); i++)
    {
        int char_count = get_char_count(str_in, list_of_delims[i]);
        if (char_count > 0)
            return list_of_delims[i];
    }
    return DEFAULT_DELIM;
}

/**
 * Nacte do retezce line nasledujici radek ze souboru.
 * @param str_out vystupni retezec
*/
void load_line(char *str_out)
{
    fgets(str_out, MAX_INPUT_LENGTH, stdin);
}

/**
 * Vypise retezec do souboru.
 * @param str_in vstupni retezec 
*/
void push_line(char *str_in)
{
    fputs(str_in, stdout);
}

/** 
 * Funkce vytvori novy radek a vypise do stdout souboru.
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu
 * @param newline true, pokud je potreba dat znak '\n' na konci radku
*/
void create_newline(char delim, int column_count, bool newline)
{
    /** 
     * TODO: mozne budouci pridani str_out
    */
    // delka pole se urcuje podle toho, zda-li ma byt na konci radku znak '\n'
    int array_length = newline ? column_count : column_count - 1;
    char loc_str[array_length];
    loc_str[array_length] = '\0'; // posledni znak je ukoncovaci znak '\0'

    // tvorba noveho radku
    for (int i = 0; i < column_count - 1; i++)
    {
        loc_str[i] = delim;
    }
    if (newline)
        loc_str[array_length - 1] = '\n';
    push_line(loc_str);
}

/** 
 * Odstrani cast retezce ze retezce. 
 * Rozdeluje retezec na 3 casti: predpona, cast k odstraneni a priponu.
 * @param str_in vstupni retezec
 * @param start_index index prvniho znaku
 * @param length delka retezce, ktery se ma odstranit
 * @param str_out vystupni retezec 
*/
void remove_str(char *str_in, int start_index, int length, char *str_out)
{
    int pre_str_len = start_index + 1;
    int post_str_len = (int)strlen(str_in) - (start_index + length) + 1;

    char pre_str[pre_str_len];   // predpona retezce, ktery se ma odstranit
    char post_str[post_str_len]; // pripona retezce, ktery se ma odstranit

    substring(str_in, 0, start_index, pre_str);
    substring(str_in, start_index + length, (int)strlen(str_in) - (start_index + length), post_str);

    sprintf(str_out, "%s%s", pre_str, post_str); // sjednoceni retezcu a ulozeni do str_out
}

/** 
 * Prepise cast retezce.
 * @param str_in vstupni retezec
 * @param start_index index prvniho znaku
 * @param length delka retezce, ktery se ma odstranit
 * @param str_replacing retezec, ktery bude pridan misto vybraneho retezce
 * @param str_out vystupni retezec
*/
void replace_str(char *str_in, int start_index, int length, char *str_replacing, char *str_out)
{
    remove_str(str_in, start_index, length, str_in);
    insert_str(str_in, start_index, str_replacing, str_out);
}

/**
 * Funkce vytvari cast retezce z puvodniho retezce.
 * @param str_in vstupni retezec
 * @param start_index pocatecni umisteni (odkud se zacina)
 * @param len delka noveho retezce
 * @param str_out vystupni retezec
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
 * Funkce vraci obsah bunky na radku v tabulkovem souboru. 
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void get_string_in_cell(char *str_in, int column, char delim, int column_count, char *str_out)
{
    // kontrola preteceni
    if (column <= 0 || column > column_count)
    {
        printf("Error: column out of bound!\n");
        return;
    }

    int start_cell = 0; // index prvniho znaku v bunce
    // index poslednuho znaku v bunce; ignoruje se znak '\n'
    int end_cell = str_in[last_index(str_in)] == '\n' ? last_index(str_in) - 1 : last_index(str_in);
    int length; // delka retezce v bunce

    if (column == 1) // pokud uzivatel chce obsah prvniho sloupcu
        end_cell = indexof(str_in, delim, 0, column) - 1;

    else if (column == column_count) // pokud uzivatel chce obsah posledniho sloupce
        start_cell = indexof(str_in, delim, 0, column - 1) + 1;

    else
    {
        start_cell = indexof(str_in, delim, 0, column - 1) + 1;
        end_cell = indexof(str_in, delim, 0, column) - 1;
    }

    length = end_cell - start_cell + 1;
    substring(str_in, start_cell, length, str_out);
}

/**
 * Funkce slouzi k vlozeni retezce do retezce. 
 * Nejprve funkce rozdeli retezec na predponu a priponu 
 * a pak jej slozi dohromady.
 * @param str_in vstupni retezec
 * @param start_index index, na kterem ma funkce vlozit retezec
 * @param str_to_insert retezec, ktery se ma vlozit
 * @param str_out vystupni retezec
*/
void insert_str(char *str_in, int start_index, char *str_to_insert, char *str_out)
{
    // funkce nepocita '\n' za vysledny znak
    if (str_in[last_index(str_in)] == '\n' && start_index == last_index(str_in) - 1)
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

    if (start_index > 0) // tvorba predpony retezce
    {
        int pre_str_len = start_index + 1;
        char pre_str[pre_str_len];
        substring(str_in, 0, start_index, pre_str);
        strcat(loc_str, pre_str);
    }

    strcat(loc_str, str_to_insert);

    if (start_index <= (int)strlen(str_in)) // tvorba pripony retezce
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
 * Funkce vraci index posledniho znaku.
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
 * Funkce smaze radek v souboru.  
 * Funkce momentalne prochazi cely soubor.
 * @param row radek, ktery ma byt smazan (row > 0)
*/
void drow(int row)
{
    ignore_lines(row - 1, temp);
    ignore_lines(TILL_END, temp);
}

/** 
 * Funkce smaze radky od starting_row do ending_row vcetne.
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
 * Vlozi sloupec do radku. Funkce momentalne prochazi cely soubor.
 * @param column cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
*/
void icol(int column, char delim, int column_count)
{
    char str_to_insert[2]; /* retezec oddelovace */
    str_to_insert[0] = delim;
    str_to_insert[1] = '\0';

    /** TODO: Error: out of bound */
    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        if (column == 1) // pokud chce uzivatel pridat novy sloupec pred prvni
            insert_str(temp, 0, str_to_insert, temp);
        else if (column > column_count) // pokud chce uzivatel pridat sloupec za posledni sloupec
            insert_str(temp, last_index(temp), str_to_insert, temp);
        else
        {
            int starting_index = indexof(temp, delim, 0, column - 1) + 1; /* zacatek nove bunky */
            insert_str(temp, starting_index, str_to_insert, temp);
        }
        push_line(temp);
    }
}

/** 
 * Odstranuje sloupec z radku. 
 * Funkce momentalne projizdi cely soubor.
 * @param column cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
*/
void dcol(int column, char delim, int column_count)
{
    /** TODO: Error: out of bound */
    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        int start_index = indexof(temp, delim, 0, column - 1);
        int length = indexof(temp, delim, 0, column) - start_index;

        if (column == column_count) // v pripade, ze radek konci '\n' tak se posledni znak posouva o jedno doleva
            length = temp[last_index(temp)] == '\n' ? last_index(temp) - start_index : last_index(temp) - start_index + 1;

        remove_str(temp, start_index, length, temp);
        push_line(temp);
    }
}