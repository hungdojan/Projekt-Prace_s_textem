#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 10240
#define MAX_STR_LENGTH 100
#define TILL_END __INT32_MAX__
#define DEFAULT_DELIM ' '
#define CONVERT_TO_UPPER ('A' - 'a')
#define CONVERT_TO_LOWER ('a' - 'A')
#define FLOATING_POINT_CHAR '.'

char temp[MAX_INPUT_LENGTH];

/* ---- Osetreni chyb ------ */
enum Error
{
    ERR_FEMPTY = 1,
    ERR_INPUT_TOO_LONG,
    ERR_STR_LENGTH_TOO_LONG,
    ERR_OUT_OF_BOUND,
    ERR_NOTNUM,
    ERR_WRONG_CCOUNT,
    ERR_UNDEFINED
};

int errors(int error_type)
{
    switch (error_type)
    {
    case ERR_FEMPTY:
        fprintf(stderr, "Error: The file is empty.\n");
        return ERR_FEMPTY;

    case ERR_INPUT_TOO_LONG:
        fprintf(stderr, "Error: length of the line is out of range.\n");
        return ERR_INPUT_TOO_LONG;

    case ERR_STR_LENGTH_TOO_LONG:
        fprintf(stderr, "Error: length of the string is out of range.\n");
        return ERR_STR_LENGTH_TOO_LONG;

    case ERR_OUT_OF_BOUND:
        fprintf(stderr, "Error: index out of bound.\n");
        return ERR_OUT_OF_BOUND;

    case ERR_NOTNUM:
        fprintf(stderr, "Error: input is not a number.\n");
        return ERR_NOTNUM;

    case ERR_WRONG_CCOUNT:
        fprintf(stderr, "Error: the number of columns doesn't match the main line.\n");
        return ERR_WRONG_CCOUNT;

    default:
        fprintf(stderr, "Unexpected error!\n");
        return ERR_UNDEFINED;
    }
}

/* ---- Struktury ---------- */

struct file_t
{
    char *list_of_delims;
    char file_delim;
    char out_delim;
    int clm_count;
    int row_count;
};

struct line_t
{
    char value[MAX_INPUT_LENGTH + 1];
    int clm_count;
};

/* ---- Pomocne funkce ----- */
struct file_t file_init();
void load_first_line(struct line_t *line, struct file_t *file);
void load_line(struct line_t *line, struct file_t *file);
void push_line(struct line_t line, struct file_t file);

int indexof(char *str_in, char c, int start_index, int position);
void ignore_lines(int length, char *str_in);
int get_char_count(char *str_in, char c);
void create_newline(struct file_t *file, bool newline);
void insert_str(char *str_in, int start_index,
                char *str_to_insert, char *str_out);
void remove_str(char *str_in, int start_index, int length, char *str_out);
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);

void get_string_in_cell(char *str_in, int clm,
                        struct file_t file, char *str_out);
void get_cell_info(char *str_in, int clm, struct file_t file,
                   int *pstart_index, int *plen_out);

int last_index(char *str);
bool is_lower(char c);
bool is_upper(char c);
bool is_letter(char c);
bool is_digit(char c);
bool is_number(char *str_in);
bool is_number_negative(char *str_in);
int round_number(char *float_number);
bool is_in_range(int number, int lower_bound, int upper_bound);

/* ---- Uprava tabulky ----- */

void irow(struct file_t *file);
void arow(struct file_t file);
void drow(int row);
void drows(int starting_row, int ending_row);
void icol(struct line_t *line, int clm, struct file_t file);
void dcol(struct line_t *line, int clm, struct file_t file);

/* ---- Zpracovavani dat --- */

void cset(struct line_t *line, int clm, struct file_t file, char *str_to_insert);
void to_lower(struct line_t *line, int clm, struct file_t file);
void to_upper(struct line_t *line, int clm, struct file_t file);
int round_func(struct line_t *line, int clm, struct file_t file);
int int_func(struct line_t *line, int clm, struct file_t file);
void copy(struct line_t *line, int col_N, int col_M, struct file_t file);
void swap(struct line_t *line, int col_N, int col_M, struct file_t file);
void move(struct line_t *line, int col_N, int col_M, struct file_t file);
void csum(struct line_t *line, int col_C, int col_N, int col_M, struct file_t file);
void cavr(struct line_t *line, int col_C, int col_N, int col_M, struct file_t file);

/* ---- Selekce radku ------ */

bool begins_with(char *str_in, int clm, struct file_t file, char *str_to_cmp);
bool contains(char *str_in, int clm, struct file_t file, char *str_to_cmp);

/* ------------------------- */

int main(int argc, char *argv[])
{
    /** 
     * TODO: znak oddelovace funguje trochu jinak
     * potreba to dodelat 
    */

    struct file_t file = file_init();
    struct line_t line = {"", 0};
    bool arow = false;

    /* TODO: argumenty*/
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d") && argc >= i + 2)
        {
            strcpy(file.list_of_delims, argv[i + 1]);
            file.out_delim = file.list_of_delims[0];
        }
    }

    file.list_of_delims = ":|,;";
    file.file_delim = ':';
    file.out_delim = '|';

    load_first_line(&line, &file);
    if (line.value[0] == '\0')
        return errors(ERR_FEMPTY);

    /**
     * For testing purposes
    */
    do
    {
        // TODO: sth
        push_line(line, file);
        load_line(&line, &file);
    } while (line.value[0] != '\0');

    if (arow)
    {
        fputs("\n", stdout);
        create_newline(&file, false);
    }

    return 0;
}

/* --------------------------------------------------- */

struct file_t file_init()
{
    struct file_t file = {.list_of_delims = ""};
    file.file_delim = ' ';
    file.out_delim = ' ';
    file.clm_count = 0;
    file.row_count = 0;
    return file;
}

void load_first_line(struct line_t *line, struct file_t *file)
{
    // nacte radek
    // zkontroluje zda je prazdny
    if (fgets(line->value, MAX_INPUT_LENGTH, stdin) == NULL)
        return;

    // zkontroluje jaky oddelovac se nachazi v souboru
    // pokud se program dostal az do tehle faze
    // muzeme usoudit, ze v tabulkovem souboru se nachazi alespon 1 radek a sloupec
    int delim_count = 0;
    file->clm_count = 1;
    file->row_count = 1;
    for (int i = 0; file->list_of_delims[i] != '\0'; i++)
    {
        delim_count = get_char_count(line->value, file->list_of_delims[i]);
        if (delim_count != 0)
        {
            file->file_delim = file->list_of_delims[i];
            file->clm_count = ++delim_count;
            line->clm_count = file->clm_count;
            break;
        }
    }
}

/**
 * Funkce nacte do retezce line nasledujici radek ze souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param file strukture file_t obsahujici informace o programu a souboru 
*/
void load_line(struct line_t *line, struct file_t *file)
{
    // resetting value of line.value to empty string
    if (fgets(line->value, MAX_INPUT_LENGTH, stdin) == NULL)
    {
        strcpy(line->value, "");
        return;
    }
    line->clm_count = get_char_count(line->value, file->file_delim) + 1;
    file->row_count++;
    if (line->clm_count != file->clm_count)
    {
        // TODO: ERR_WRONG_CCOUNT
    }
    /**
     * TODO: check if the string ends with \0 if the length is 
     */
}

/**
 * Funkce vypise retezec do souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param file strukture file_t obsahujici informace o programu a souboru 
*/
void push_line(struct line_t line, struct file_t file)
{
    // prepisuje vsechny oddelovace na vystupni oddelovac
    int index;
    while ((index = indexof(line.value, file.file_delim, 0, 1)) != -1)
        line.value[index] = file.out_delim;

    fputs(line.value, stdout);
    (void)file;
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
        //return EXCEPTION_OUT_OF_BOUND;
    }

    if (position == 0) // specialni pripad position == 0
        return 0;

    int i = start_index;
    int count = 0; // hodnota udava pocet hledanych znaku, ktere jiz potkal

    for (; str_in[i] != '\0'; i++) // hledani
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
    for (i = 0; str_in[i] != '\0'; i++)
    {
        if (str_in[i] == c)
            char_count++;
    }
    return char_count;
}

/** 
 * Funkce vytvori novy radek a vypise do stdout souboru.
 * @param file strukture file_t obsahujici informace o programu a souboru 
 * @param newline true, pokud je potreba dat znak '\n' na konci radku
*/
void create_newline(struct file_t *file, bool newline)
{
    /** 
     * TODO: mozne budouci pridani str_out
    */
    // delka pole se urcuje podle toho, zda-li ma byt na konci radku znak '\n'
    int array_length = newline ? file->clm_count : file->clm_count - 1;
    char loc_str[array_length];
    loc_str[array_length] = '\0'; // posledni znak je ukoncovaci znak '\0'

    // tvorba noveho radku
    for (int i = 0; i < file->clm_count - 1; i++)
    {
        loc_str[i] = file->out_delim;
    }

    if (newline)
        loc_str[array_length - 1] = '\n';

    file->row_count++;
    fputs(loc_str, stdout);
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
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @param str_out vystupni retezec
*/
void get_string_in_cell(char *str_in, int clm, struct file_t file, char *str_out)
{
    // kontrola preteceni
    if (clm <= 0 || clm > file.clm_count)
    {
        printf("Error: column out of bound!\n");
        return;
    }

    int start_index, len;
    get_cell_info(str_in, clm, file, &start_index, &len);
    substring(str_in, start_index, len, str_out);
}

/** 
 * Funkce ziskava dve informace o bunce v retezce: jeji pocatecni index 
 * a delka retezce v bunce.
 * @param str_in vstupni retezec
 * @param c sloupce v radku (pocita se od 1)
 * @param file struktura file_t
 * @param pstart_index vystupni parametr pro pocatecni index
 * @param plen_out vystupni delka retezce
*/
void get_cell_info(char *str_in, int clm, struct file_t file,
                   int *pstart_index, int *plen_out)
{
    // kontrola preteceni
    if (clm <= 0 || clm > file.clm_count)
    {
        printf("Error: column out of bound!\n");
        return;
    }

    // pocatecni index je prvni index po prvnim oddelovaci
    // vyjimku tvori prvni sloupec, ktery nema pred sebou oddelovac
    *pstart_index = indexof(str_in, file.file_delim, 0, clm - 1);
    if (clm != 1)
        (*pstart_index)++;

    /** 
     * delka se urcuje jako rozdil indexu zadniho oddelovace
     * a pocatecniho indexu bunky
     * vyjimku tvori posledni sloupec, ktery nema za sebou oddelovac
     * u posledniho sloupce se provadi kontrola na znak '\n'
    */
    if (clm < file.clm_count)
        *plen_out = indexof(str_in, file.file_delim, 0, clm) - *pstart_index;
    else if (str_in[last_index(str_in)] == '\n')
        *plen_out = (int)strlen(str_in) - *pstart_index - 1;
    else
        *plen_out = (int)strlen(str_in) - *pstart_index;
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
    return is_upper(c) || is_lower(c);
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
    for (i = 0; str_in[i] != '\0'; i++)
    {
        // kontroluje vyjimky, kdyz zadany znak neni cislice
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
    double float_number = atof(number);

    // pokud je rozdil desetinneho cisla a jeho cele casti
    // vetsi nez 0.5, cislo se zaokrouhli nahoru
    if (float_number - int_number * 1.0 >= 0.5)
        int_number++;
    return int_number;
}

/** 
 * Funkce kontroluje, zda cislo number lezi v intervalu
 * @param number cislo, na ktere se ptame
 * @param lower_bound nejnizsi cislo intervalu
 * @param upper_bound nejvyssi cislo intervalu
*/
bool is_in_range(int number, int lower_bound, int upper_bound)
{
    return number >= lower_bound && number <= upper_bound;
}

/* ---------------------------------- */

/**
 * Funkce prida novy radek. Je mozne pouzit jen create_newline() 
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void irow(struct file_t *file)
{
    create_newline(file, true);
    /*
    pripadne v mainu
    while (file.row_count >= 3 && file.row_count <= 5)
        irow(&file);
    */
}

/** 
 * TODO: UPRAVIT A DODELAT
 * Funkce smaze radek v souboru.  
 * Funkce momentalne prochazi cely soubor.
 * @param row radek, ktery ma byt smazan (row > 0)
*/
void drow(int row)
{
    // TODO: preskoci jeden radek
    ignore_lines(row - 1, temp);
    ignore_lines(TILL_END, temp);
}

/** 
 * TODO: UPRAVIT A DODELAT
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
    // push_line();
    ignore_lines(TILL_END, temp);
}

/** 
 * Funkce vlozi sloupec do radku. Funkce momentalne prochazi cely soubor.
 * @param clm cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void icol(struct line_t *line, int clm, struct file_t file)
{
    char str_to_insert[2]; /* retezec oddelovace */
    str_to_insert[0] = file.file_delim;
    str_to_insert[1] = '\0';

    if (clm == 1) // pokud chce uzivatel pridat novy sloupec pred prvni
        insert_str(line->value, 0, str_to_insert, line->value);

    // pokud chce uzivatel pridat sloupec za posledni sloupec
    else if (clm > file.clm_count)
    {
        int start_index, len;
        get_cell_info(line->value, file.clm_count, file, &start_index, &len);
        insert_str(line->value, start_index + len, str_to_insert, line->value);
    }
    else
    {
        // zacatek nove bunky
        int starting_index = indexof(line->value, file.file_delim, 0, clm - 1) + 1;
        insert_str(line->value, starting_index, str_to_insert, line->value);
    }

    /** TODO: Error: out of bound */
    /*
    while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    {
        if (clm == 1) // pokud chce uzivatel pridat novy sloupec pred prvni
            insert_str(temp, 0, str_to_insert, temp);

        // pokud chce uzivatel pridat sloupec za posledni sloupec
        else if (clm > file.clm_count)
            insert_str(temp, last_index(temp), str_to_insert, temp);

        else
        {
            // zacatek nove bunky
            int starting_index = indexof(temp, file.file_delim, 0, clm - 1) + 1;
            insert_str(temp, starting_index, str_to_insert, temp);
        }
        // push_line(temp);
    }
    */
}

/** 
 * Funkce odstranuje sloupec z radku. 
 * Funkce momentalne projizdi cely soubor.
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void dcol(struct line_t *line, int clm, struct file_t file)
{
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // v pripade, ze radek konci '\n' tak se posledni znak posouva o jedno doleva
    if (clm == file.clm_count)
        remove_str(line->value, start_index - 1, len + 1, line->value);
    else
        remove_str(line->value, start_index, len + 1, line->value);

    /** TODO: Error: out of bound */
    // while (fgets(temp, MAX_INPUT_LENGTH, stdin) != NULL)
    // {
    //     int start_index, len;
    //     get_cell_info(temp, clm, *file, &start_index, &len);

    //     // v pripade, ze radek konci '\n' tak se posledni znak posouva o jedno doleva
    //     if (clm == file->clm_count)
    //         remove_str(temp, start_index - 1, len + 1, temp);
    //     else
    //         remove_str(temp, start_index, len + 1, temp);

    //     // push_line(temp);
    // }
}

/* ------------------------------------------------------ */

/** 
 * Funkce do bunky column nastavi retezec str_in. 
 * Promenna str_out slouzi jako vystupni retezec pro vytisknuti do souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @param str_to_insert retezec, ktery bude nastaven v bunce
*/
void cset(struct line_t *line, int clm, struct file_t file, char *str_to_insert)
{
    if (strlen(str_to_insert) > 100)
    {
        /** TODO: Error */
    }
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // smaze obsah bunky
    remove_str(line->value, start_index, len, line->value);

    // vlozi novy obsah
    insert_str(line->value, start_index, str_to_insert, line->value);
}

/** 
 * Funkce prepise vsechna velka pismena v bunce na mala
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void to_lower(struct line_t *line, int clm, struct file_t file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // projede retezec a prevede velka pismena na mala
    for (int i = start_index; i <= len + start_index; i++)
    {
        if (is_upper(line->value[i]))
        {
            line->value[i] += CONVERT_TO_LOWER;
        }
    }
}

/** 
 * Funkce prepise vsechna mala pismena v bunce na velka
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void to_upper(struct line_t *line, int clm, struct file_t file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // projede retezec a prevede mala pismena na velka
    for (int i = start_index; i <= len + start_index; i++)
    {
        if (is_lower(line->value[i]))
        {
            line->value[i] += CONVERT_TO_UPPER;
        }
    }
}

/** 
 * Funkce z bunky ziska retezec, ktery prevede 
 * na cislo a zaokrouhli ho.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int round_func(struct line_t *line, int clm, struct file_t file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_number(loc_str))
    {
        return errors(ERR_NOTNUM);
    }

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", round_number(loc_str));
    cset(line, clm, file, loc_str);
    return 0;
}

/** 
 * Funkce prevede desetinne cislo na cele cislo a ulozi do bunky.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int int_func(struct line_t *line, int clm, struct file_t file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_number(loc_str))
    {
        return errors(ERR_NOTNUM);
    }

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", atoi(loc_str));
    cset(line, clm, file, loc_str);
    return 0;
}

/** 
 * Funkce prepise obsah bunky ve col_M hodnotou ze col_N 
 * (zkopiruje obsah bunky col_N do col_M).
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param col_N sloupec, ze ktereho se kopiruje
 * @param col_M sloupec, do ktereho se kopiruje
 * @param file struktura file_t obsahujici informace o programu a souboru

*/
void copy(struct line_t *line, int col_N, int col_M,
          struct file_t file)
{
    char temp_str[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci N
    get_string_in_cell(line->value, col_N, file, temp_str);

    // nastavime novou hodnotu do bunky M
    cset(line, col_M, file, temp_str);
}

/** 
 * Funkce vymeni obsahy buneky z col_M a col_N.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param col_N prvni sloupec
 * @param col_M druhy sloupec
 * @param file struktura file_t obsahujici informace o programu a souboru

*/
void swap(struct line_t *line, int col_N, int col_M,
          struct file_t file)
{
    char temp_str_M[MAX_INPUT_LENGTH];
    char temp_str_N[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci M do temp_str_M
    get_string_in_cell(line->value, col_M, file, temp_str_M);

    // nacteni hodnoty ve sloupci N do temp_str_N
    get_string_in_cell(line->value, col_N, file, temp_str_N);

    // nastavime novou hodnotu do bunky M a N
    cset(line, col_M, file, temp_str_N);
    cset(line, col_N, file, temp_str_M);
}

/** 
 * Funkce přesune obsah bunky v col_N před col_M.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param col_N sloupec, ktery se presouva
 * @param col_M sloupec, pred ktery se ma presunout obsah bunky col_N
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void move(struct line_t *line, int col_N, int col_M,
          struct file_t file)
{
    /** 
     * pokud je col_N nalevo od col_M
     * col_N se prohazuje s pravou bunkou
     * dokud nebude hned vedle col_M
    */
    while (col_N < col_M - 1)
    {
        swap(line, col_N, col_N + 1, file);
        col_N++;
    }

    /** 
     * pokud je col_N napravo od col_M
     * tak se col_N se prohazuje s levou bunkou
     * dokud nebude nalevo od col_M (nebo-li pred col_M)
    */
    while (col_N > col_M)
    {
        swap(line, col_N, col_N - 1, file);
        col_N--;
    }
}

/** 
 * Funkce do bunky v column_C ulozi soucet hodnot bunek na 
 * stejnem radku ve sloupcích col_N až col_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param col_C sloupce v radku (pocita se od 1)
 * @param col_N sloupec; dolni hranice intervalu
 * @param col_M sloupec; horni hranice intervalu
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void csum(struct line_t *line, int col_C, int col_N,
          int col_M, struct file_t file)
{
    /** TOOD: Error */
    if (is_in_range(col_C, col_N, col_M))
    {
        printf("Error: column C is within the interval.\n");
        return;
    }

    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = col_N; i <= col_M; i++)
    {
        get_string_in_cell(line->value, i, file, loc_str);
        /** TOOD: Error */
        if (!is_number(loc_str))
        {
            // ERR_NOTNUM
            printf("Error: not a number.\n");
            return;
        }

        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce col_C
    sprintf(loc_str, "%g", sum);
    cset(line, col_C, file, loc_str);
}

/** 
 * Funkce do bunky v column_C ulozi aritmeticky prumer hodnot bunek na 
 * stejnem radku ve sloupcích col_N až col_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param col_C sloupce v radku (pocita se od 1)
 * @param col_N sloupec; dolni hranice intervalu
 * @param col_M sloupec; horni hranice intervalu
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void cavr(struct line_t *line, int col_C, int col_N,
          int col_M, struct file_t file)
{
    /** TOOD: Error */
    if (is_in_range(col_C, col_N, col_M))
    {
        printf("Error: column C is within the interval.\n");
        return;
    }

    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = col_N; i <= col_M; i++)
    {
        get_string_in_cell(line->value, i, file, loc_str);
        /** TOOD: Error */
        if (!is_number(loc_str))
        {
            // ERR_NOTNUM
            printf("Error: not a number.\n");
            return;
        }

        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce col_C
    sprintf(loc_str, "%g", sum / (col_M - col_N + 1));
    cset(line, col_C, file, loc_str);
}

/* ---------------------------------------------- */

/** 
 * Funkce porovnava, zda retezec v bunce v column 
 * zacina retezcem str_to_cmp
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @param str_to_cmp retezec, na se kterym porovnavame obsah bunky
 * @return true pokud retezec v obsahu bunky v column 
 * zacina retezcem str_to_cmp 
*/
bool begins_with(char *str_in, int clm, struct file_t file, char *str_to_cmp)
{
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, clm, file, loc_str);

    // cyklus projizdi retezec str_in a str_to_cmp zaroven
    for (int i = 0; i < (int)strlen(str_to_cmp); i++)
    {
        // pokud dochazi k neshode, funkce vraci false
        if (loc_str[i] != str_to_cmp[i])
            return false;
    }
    return true;
}

/** 
 * Funkce kontroluje, zda se retezec str_to_cmp 
 * nachazi v retezci v bunce column.
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @param str_to_cmp retezec, na se kterym porovnavame obsah bunky
 * @return true pokud retezec v obsahu bunky v column nachazi 
*/
bool contains(char *str_in, int clm,
              struct file_t file, char *str_to_cmp)
{
    // nacita obsah bunky do promenne
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, clm, file, loc_str);

    int j = 0;
    int len = (int)strlen(str_to_cmp);

    // prohledava retezec
    for (int i = 0; i < (int)strlen(loc_str); i++)
    {
        // pokud je schoda znaku a index j je stale v intervalu str_to_cmp
        // zvetsujeme indexy i a j o 1
        // (vnejsi cyklus ho za normalnich okolnosti neincrementuje)
        // take kontroluje, zda neskoncil retezec loc_str
        while (j < len && loc_str[i] != '\0' && loc_str[i] == str_to_cmp[j])
        {
            i++;
            j++;
        }

        // pokud index j == len, nebo-li cyklus prosel vsechny znaky
        // str_to_cmp a vsechny se shoduji, znamena to, ze se opravdu nachazi
        if (j == len)
            return true;

        // resetuje se promenna j na konci kontroly
        j = 0;
    }
    return false;
}