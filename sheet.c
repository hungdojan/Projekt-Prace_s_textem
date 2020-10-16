#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 10241
#define EXCEPTION_OUT_OF_BOUND -2
#define EXCEPTION_NOT_A_NUMBER -2
#define TILL_END __INT32_MAX__
#define DEFAULT_DELIM ' '
#define CONVERT_TO_UPPER ('A' - 'a')
#define CONVERT_TO_LOWER ('a' - 'A')
#define FLOATING_POINT_CHAR '.'

char temp[MAX_INPUT_LENGTH]; // slouzi k ulozeni posledniho nacteneho radku

/* ---- Pomocne funkce ----- */

int indexof(char *str_in, char c, int start_index, int position);
void ignore_lines(int length, char *str_in);
int get_char_count(char *str_in, char c);
char set_delim(char *str_in, char *list_of_delims);
void load_line(char *str_out);
void push_line(char *str_in);
void create_newline(char delim, int column_count, bool newline);
void insert_str(char *str_in, int start_index,
                char *str_to_insert, char *str_out);
void remove_str(char *str_in, int start_index, int length, char *str_out);
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);
void get_string_in_cell(char *str_in, int column,
                        char delim, int column_count, char *str_out);
void get_cell_info(char *str_in, int column, char delim,
                   int column_count, int *pstart_index_out, int *plen_out);
int last_index(char *str);
bool is_lower(char c);
bool is_upper(char c);
bool is_letter(char c);
bool is_digit(char c);
bool is_number(char *str_in);
bool is_number_negative(char *str_in);
int round_number(char *float_number);

/* ---- Uprava tabulky ----- */

void irow(char delim, int column_count);
void arow(char delim, int column_count);
void drow(int row);
void drows(int starting_row, int ending_row);
void icol(int column, char delim, int column_count);
void dcol(int column, char delim, int column_count);

/* ---- Zpracovavani dat --- */

void cset(char *str_in, int column, char delim, int column_count,
          char *str_to_insert, char *str_out);
void to_lower(char *str_in, int column, char delim,
              int column_count, char *str_out);
void to_upper(char *str_in, int column, char delim,
              int column_count, char *str_out);
int round_func(char *str_in, int column, char delim,
               int column_count, char *str_out);
int int_func(char *str_in, int column, char delim,
             int column_count, char *str_out);
void copy(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out);
void swap(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out);
void move(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out);

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
    int column_count = 5;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d") && argc >= i + 2)
        {
            strcpy(list_of_delims, argv[i + 1]);
            output_delim = list_of_delims[0];
        }
    }

    /**
     * For testing purposes
    */
    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        // code...
        push_line(temp);
    }

    (void)column_count;
    (void)file_delim;
    (void)output_delim;
    return 0;
}

/** 
 * Funkce hleda indexovou pozici znaku v retezci.
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
 * Funkce jenom prekopiruje radek do dalsiho souboru.
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
 * Funkce spocita, kolik je v retezci znaku c.
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
 * Funkce zjistuje znak oddelovace, ktery se nachazi v tabulkovem souboru. 
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
 * Funkce nacte do retezce line nasledujici radek ze souboru.
 * @param str_out vystupni retezec
*/
void load_line(char *str_out)
{
    fgets(str_out, MAX_INPUT_LENGTH, stdin);
}

/**
 * Funkce vypise retezec do souboru.
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
 * Funkce odstrani cast retezce ze retezce. 
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
    substring(str_in, start_index + length,
              (int)strlen(str_in) - (start_index + length), post_str);

    // sjednoceni retezcu a ulozeni do str_out
    sprintf(str_out, "%s%s", pre_str, post_str);
}

/** 
 * Funkce prepise cast retezce.
 * @param str_in vstupni retezec
 * @param start_index index prvniho znaku
 * @param length delka retezce, ktery se ma odstranit
 * @param str_replacing retezec, ktery bude pridan misto vybraneho retezce
 * @param str_out vystupni retezec
*/
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out)
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
 * Funkce vraci obsah bunky na radku v tabulkovem souboru. 
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void get_string_in_cell(char *str_in, int column, char delim,
                        int column_count, char *str_out)
{
    // kontrola preteceni
    if (column <= 0 || column > column_count)
    {
        printf("Error: column out of bound!\n");
        return;
    }

    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);
    substring(str_in, start_index, len, str_out);
}

/** 
 * Funkce ziskava dve informace o bunce v retezce: jeji pocatecni index 
 * a delka retezce v bunce.
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu
 * @param pstart_index_out vystupni parametr pro pocatecni index
 * @param plen_out vystupni delka retezce
*/
void get_cell_info(char *str_in, int column, char delim,
                   int column_count, int *pstart_index_out, int *plen_out)
{
    // kontrola preteceni
    if (column <= 0 || column > column_count)
    {
        printf("Error: column out of bound!\n");
        return;
    }

    // pocatecni index je prvni index po prvnim oddelovaci
    // vyjimku tvori prvni sloupec, ktery nema pred sebou oddelovac
    *pstart_index_out = indexof(str_in, delim, 0, column - 1);
    if (column != 1)
        (*pstart_index_out)++;

    /** 
     * delka se urcuje jako rozdil indexu zadniho oddelovace
     * a pocatecniho indexu bunky
     * vyjimku tvori posledni sloupec, ktery nema za sebou oddelovac
     * u posledniho sloupce se provadi kontrola na znak '\n'
    */
    if (column < column_count)
        *plen_out = indexof(str_in, delim, 0, column) - *pstart_index_out;
    else if (str_in[last_index(str_in)] == '\n')
        *plen_out = (int)strlen(str_in) - *pstart_index_out - 1;
    else
        *plen_out = (int)strlen(str_in) - *pstart_index_out;
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

/** 
 * Funkce zjistuje, zda je zadany znak male pismeno
 * @param c znak, na ktery se ptame
 * @return true pokud znak je male pismeno
*/
bool is_lower(char c)
{
    return c >= 'a' && c <= 'z';
}

/** 
 * Funkce zjistuje, zda je zadany znak velke pismeno
 * @param c znak, na ktery se ptame
 * @return true pokud znak je velke pismeno
*/
bool is_upper(char c)
{
    return c >= 'A' && c <= 'Z';
}

/** 
 * Funkce zjistuje, zda je zadany znak pismeno
 * @param c znak, na ktery se ptame
 * @return true pokud znak je pismeno
*/
bool is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/** 
 * Funkce zjisti, zda je dany znak cislice
 * @param c znak, na ktery se ptame
 * @return true pokud znak je cislice
*/
bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/** 
 * Funkce zjisti, zda je retezec cislo
 * @param str_in vstupni retezec
 * @return true pokud je retezec cislo
*/
bool is_number(char *str_in)
{
    if (strlen(str_in) == 0)
        return false;
    bool is_negative = str_in[0] == '-';
    bool is_float = false;
    int i;
    for (i = 0; i < (int)strlen(str_in); i++)
    {
        if (!is_digit(str_in[i]))
        {
            switch (str_in[i])
            {
            case '-':
                // znamenko minus muze byt jen na prvnim miste
                if (i != 0)
                    return false;
                else // pro kontrolu dalsich znaku
                    is_negative = true;
                break;
            case FLOATING_POINT_CHAR:
                // desetinna carka nemuze byt na prvnim miste,
                // nebo po znamenku minus, a muze byt v retezci jen jednou
                if (i == 0 || (is_negative && i == 1) || is_float)
                    return false;
                is_float = !is_float;
                break;
            // ostatni znaky nejsou povolene
            default:
                return false;
            }
        }
    }
    return true;
}

/** 
 * Funkce zjisti, jestli muze byt retezec zaporne cislo
 * @param str_in vstupni retezec
 * @return true pokud by retezec mohl byt cislo zaporne
*/
bool is_number_negative(char *str_in)
{
    return str_in[0] == '-';
}

/** 
 * Funkce prevede retezec na cislo a zaokrouhli ho
 * @param number vstupni retezec cisla
*/
int round_number(char *number)
{
    int int_number = atoi(number);
    float float_number = atof(number);
    if (float_number - int_number * 1.0 >= 0.5)
        int_number++;
    return int_number;
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
 * Funkce vlozi sloupec do radku. Funkce momentalne prochazi cely soubor.
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

        // pokud chce uzivatel pridat sloupec za posledni sloupec
        else if (column > column_count)
            insert_str(temp, last_index(temp), str_to_insert, temp);

        else
        {
            /* zacatek nove bunky */
            int starting_index = indexof(temp, delim, 0, column - 1) + 1;
            insert_str(temp, starting_index, str_to_insert, temp);
        }
        push_line(temp);
    }
}

/** 
 * Funkce odstranuje sloupec z radku. 
 * Funkce momentalne projizdi cely soubor.
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
*/
void dcol(int column, char delim, int column_count)
{
    /** TODO: Error: out of bound */
    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        int start_index, len;
        get_cell_info(temp, column, delim, column_count, &start_index, &len);

        // v pripade, ze radek konci '\n' tak se posledni znak posouva o jedno doleva
        if (column == column_count)
            remove_str(temp, start_index - 1, len + 1, temp);
        else
            remove_str(temp, start_index, len + 1, temp);

        push_line(temp);
    }
}

/* ------------------------------------------------------ */

/** 
 * Funkce do bunky column nastavi retezec str_in. 
 * Promenna str_out slouzi jako vystupni retezec pro vytisknuti do souboru.
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_to_insert retezec, ktery bude nastaven v bunce
 * @param str_out vystupni retezec
*/
void cset(char *str_in, int column, char delim,
          int column_count, char *str_to_insert, char *str_out)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);

    // smaze obsah bunky
    remove_str(str_in, start_index, len, str_in);

    // vlozi novy obsah
    insert_str(str_in, start_index, str_to_insert, str_out);
}

/** 
 * Funkce prepise vsechna velka pismena v bunce na mala
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void to_lower(char *str_in, int column, char delim, int column_count, char *str_out)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);

    // projede retezec a prevede velka pismena na mala
    for (int i = start_index; i <= len + start_index; i++)
    {
        if (is_upper(str_in[i]))
        {
            str_in[i] += CONVERT_TO_LOWER;
        }
    }

    // prevod na str_out
    strcpy(str_out, str_in);
}

/** 
 * Funkce prepise vsechna mala pismena v bunce na velka
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void to_upper(char *str_in, int column, char delim,
              int column_count, char *str_out)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);

    // projede retezec a prevede mala pismena na velka
    for (int i = start_index; i <= len + start_index; i++)
    {
        if (is_lower(str_in[i]))
        {
            str_in[i] += CONVERT_TO_UPPER;
        }
    }

    // prevod na str_out
    strcpy(str_out, str_in);
}

/** 
 * Funkce z bunky ziska retezec, ktery prevede 
 * na cislo a zaokrouhli ho.
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int round_func(char *str_in, int column, char delim,
               int column_count, char *str_out)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, column, delim, column_count, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_number(loc_str))
        return EXCEPTION_NOT_A_NUMBER;

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", round_number(loc_str));
    cset(str_in, column, delim, column_count, loc_str, str_out);
    return 0;
}

/** 
 * Funkce prevede desetinne cislo na cele cislo a ulozi do bunky.
 * @param str_in vstupni retezec
 * @param column poradi sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int int_func(char *str_in, int column, char delim,
             int column_count, char *str_out)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(str_in, column, delim, column_count, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, column, delim, column_count, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_number(loc_str))
        return EXCEPTION_NOT_A_NUMBER;

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", atoi(loc_str));
    cset(str_in, column, delim, column_count, loc_str, str_out);
    return 0;
}

/** 
 * Funkce prepise obsah bunky ve column_M hodnotami ze column_N.
 * @param str_in vstupni retezec
 * @param column_N sloupec, ze ktereho se kopiruje
 * @param column_M sloupec, do ktereho se kopiruje
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void copy(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out)
{
    char temp_str[MAX_INPUT_LENGTH];
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(str_in, column_N, delim, column_count, &start_index, &len);

    // nacteni hodnoty ve sloupci N
    get_string_in_cell(str_in, column_N, delim, column_count, temp_str);

    // nastavime novou hodnotu do bunky M
    cset(str_in, column_M, delim, column_count, temp_str, str_out);
}

/** 
 * Funkce vymeni obsahy buneky z column_M a column_N.
 * @param str_in vstupni retezec
 * @param column_N prvni sloupec
 * @param column_M druhy sloupec
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void swap(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out)
{
    char temp_str_M[MAX_INPUT_LENGTH];
    char temp_str_N[MAX_INPUT_LENGTH];
    int start_M, len_M, start_N, len_N;

    // ulozeni prvniho indexu a delky retezce bunky ve sloupci M
    // nacteni hodnoty ve sloupci M do temp_str_M
    get_cell_info(str_in, column_M, delim, column_count, &start_M, &len_M);
    get_string_in_cell(str_in, column_M, delim, column_count, temp_str_M);

    // ulozeni prvniho indexu a delky retezce bunky ve sloupci N
    // nacteni hodnoty ve sloupci N do temp_str_N
    get_cell_info(str_in, column_N, delim, column_count, &start_N, &len_N);
    get_string_in_cell(str_in, column_N, delim, column_count, temp_str_N);

    // nastavime novou hodnotu do bunky M a N
    cset(str_in, column_M, delim, column_count, temp_str_N, str_out);
    cset(str_in, column_N, delim, column_count, temp_str_M, str_out);
}

/** 
 * Funkce přesune obsah bunky v column_N před column_M.
 * @param str_in vstupni retezec
 * @param column_N sloupec, ktery se presouva
 * @param column_M sloupec, pred ktery se ma presunout obsah bunky column_N
 * @param delim znak oddelovace
 * @param column_count pocet sloupcu na radku
 * @param str_out vystupni retezec
*/
void move(char *str_in, int column_N, int column_M,
          char delim, int column_count, char *str_out)
{
    /** 
     * pokud je column_N nalevo od column_M
     * column_N se prohazuje s pravou bunkou
     * dokud nebude hned vedle column_M
    */
    while (column_N < column_M - 1)
    {
        swap(str_in, column_N, column_N + 1, delim, column_count, str_in);
        column_N++;
    }

    /** 
     * pokud je column_N napravo od column_M
     * tak se column_N se prohazuje s levou bunkou
     * dokud nebude nalevo od column_M (nebo-li pred column_M)
    */
    while (column_N > column_M)
    {
        swap(str_in, column_N, column_N - 1, delim, column_count, str_in);
        column_N--;
    }

    // zkopiruje obsah retezce do str_out
    strcpy(str_out, str_in);
}