#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 10241
#define MAX_STR_LENGTH 101
#define TILL_END __INT32_MAX__
#define DEFAULT_DELIM ' '
#define FLOATING_POINT_CHAR '.'

char temp[MAX_INPUT_LENGTH];
/* ---- Osetreni chyb ------ */

/** 
 * Vyctovy typ Error s vypisem moznych chyb
*/
enum Error
{
    ERR_FEMPTY = 1,
    ERR_INPUT_TOO_LONG,
    ERR_STR_LENGTH_TOO_LONG,
    ERR_OUT_OF_BOUND,
    ERR_NOTNUM,
    ERR_WRONG_CCOUNT,
    ERR_WRONG_ARGV,
    ERR_UNDEFINED
};

/* ---- Struktury ---------- */

/** 
 * Tato struktura uklada hlavni informace o tabulkovem souboru.
 * znak oddelovace, pocet radku a sloupcu
*/
typedef struct file_t
{
    char list_of_delims[MAX_STR_LENGTH]; // mnozina moznych znaku oddelovacu
    char file_delim;                     // znak oddelovace, ktery se nachazi v souboru
    char out_delim;                      // znak oddelovace, ktery se ma objevit ve vyslednem souboru
    int clm_count;                       // pocet sloupcu v souboru (podle prvniho radku)
    int row_count;                       // momentalni pocet radku
} File;

/** 
 * Tato struktura uklada informace o nactenem radku.
*/
typedef struct line_t
{
    char value[MAX_INPUT_LENGTH]; // obsah nacteneho radku ze souboru
    int clm_count;                // pocet sloupcu na danem radku
    bool last_line;               // uklada informaci, zda jde o posledni radek souboru
} Line;

/** 
 * Tato struktura uklada informace o prikazech zpracovani radku a selekci dat.
*/
typedef struct select_t
{
    char command[8];                  // prikaz, ktery se ma provest
    char beginw_str[MAX_STR_LENGTH];  // retezec pro prikaz beginswith
    char contain_str[MAX_STR_LENGTH]; // retezec pro prikaz contains
    // retezex uklada nedefinovanou hodnotu pro pozdejsi pouziti v prikazu
    char cmd_str[MAX_STR_LENGTH];
    // prikazy pro selekci sloupci v prikazech beginswith a contains
    int begin_clm, contain_clm;
    // promenne ukladaji hodnoty sloupcu pro pozdejsi manipulaci
    int clm_C, clm_N, clm_M;
    // promenne ukladaji hodnoty radku pro pozdejsi manipulaci
    int row_R, row_N, row_M;
} Select;

/** 
 * Tato struktura uklada informace o prikazech upravy tabulky.
*/
typedef struct edit_t
{
    int arow;                        // pocet radku, ktere se maji pridat na konec souboru
    char queue_rows[MAX_STR_LENGTH]; // fronta vsech prikazu pro upravu radku
    int temp_row;                    // posledni nacteny radek na upravu
    int acol;                        // pocet sloupcu, ktere se maji pridat na konec radku
    char queue_clm[MAX_STR_LENGTH];  // fronta vsech prikazu pro upravu sloupcu
    int temp_column;                 // posledni nacteny sloupce na upravu
} Edit;

/**
 * Funkce vypise chybovou hlasku podle druhu chyby.
*/
int errors_out(int error_type, File file)
{
    switch (error_type)
    {
    case ERR_FEMPTY:
        fprintf(stderr, "Error (line %d): The file is empty.\n", file.row_count);
        break;

    case ERR_INPUT_TOO_LONG:
        fprintf(stderr, "Error (line %d): length of the line is out of range.\n", file.row_count);
        break;

    case ERR_STR_LENGTH_TOO_LONG:
        fprintf(stderr, "Error (line %d): length of the string is out of range.\n", file.row_count);
        break;

    case ERR_OUT_OF_BOUND:
        fprintf(stderr, "Error (line %d): index out of bound.\n", file.row_count);
        break;

    case ERR_NOTNUM:
        fprintf(stderr, "Error (line %d): input is not a number.\n", file.row_count);
        break;

    case ERR_WRONG_CCOUNT:
        fprintf(stderr, "Error (line %d): the number of columns doesn't match the main line.\n", file.row_count);
        break;

    case ERR_WRONG_ARGV:
        fprintf(stderr, "Error: invalid argument syntax.\n");
        break;

    default:
        fprintf(stderr, "Error (line %d): SEGMENTATION FAULT! CORE PROBABLY DUMPED... Or maybe you r just dumb!\n", file.row_count);
        break;
    }
    return 1;
}

/* ---- Set-up funkce -------*/

void init_file(File *file);
void init_select(Select *sel);
void init_edit(Edit *edit);
int load_arguments(int argc, char **argv, Select *sel, Edit *edit, File *file);
int load_delim(File *file, int argc, char **argv);
int load_edit_cmd(Edit *edit, int argc, char **argv, int *i, bool *flag);
int load_one_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_two_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_three_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_rest_cmds(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_selection_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_first_line(Line *line, File *file);

/* ---- Pomocne funkce ----- */

int load_line(Line *line, File *file);
void push_line(Line line, File file);

int indexof(char *str_in, char c, int start_index, int position);
void ignore_lines(int length, char *str_in);
int get_char_count(char *str_in, char c);
void create_newline(File *file, bool newline);
void insert_str(char *str_in, int start_index,
                char *str_to_insert, char *str_out);
void remove_str(char *str_in, int start_index, int length, char *str_out);
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);

void get_string_in_cell(char *str_in, int clm,
                        File file, char *str_out);
void get_cell_info(char *str_in, int clm, File file,
                   int *pstart_index, int *plen_out);

int last_index(char *str);
bool is_letter(char c);
bool is_num(char *str_in);
int round_number(char *float_number);
bool in_range(int number, int lower_bound, int upper_bound);

void enqueue(Edit *edit, char *cmd, int value);
void dequeue(char *str);
void first_item(char *str, char *item_out);

/* ---- Uprava tabulky ----- */

void irow(File *file);
void arow(File file);
void drow(int row);
void drows(int starting_row, int ending_row);
void icol(Line *line, int clm, File file);
void dcol(Line *line, int clm, File file);

/* ---- Zpracovavani dat --- */

void do_command(Select sel, Line *line, File file);
void cset(Line *line, int clm, File file, char *str_to_insert);
void to_lower(Line *line, int clm, File file);
void to_upper(Line *line, int clm, File file);
int round_func(Line *line, int clm, File file);
int int_func(Line *line, int clm, File file);
void copy(Line *line, int clm_N, int clm_M, File file);
void swap(Line *line, int clm_N, int clm_M, File file);
void move(Line *line, int clm_N, int clm_M, File file);
void csum(Line *line, int clm_C, int clm_N, int clm_M, File file);
void cavg(Line *line, int clm_C, int clm_N, int clm_M, File file);

/* ---- Selekce radku ------ */

bool begins_with(char *str_in, int clm, File file, char *str_to_cmp);
bool contains(char *str_in, int clm, File file, char *str_to_cmp);
int set_beginswith(Select *sel, char **argv, int *index);
int set_contains(Select *sel, char **argv, int *index);
int set_rows(Select *sel, char **argv, int *index);

/* ------------------------- */

int main(int argc, char *argv[])
{
    File file;
    Line line = {.value = "", .clm_count = 0, .last_line = false};
    Select sel;
    Edit edit;
    init_file(&file);
    init_select(&sel);

    int error = 0;

    // projede argumenty a vraci pripadne chyby
    if ((error = load_arguments(argc, argv, &sel, &edit, &file)))
        return errors_out(error, file);

    // nacte prvni radek souboru a nacte potrebna informace o souboru
    error = load_first_line(&line, &file);
    if (error)
        return errors_out(error, file);

    /**
     * For testing purposes
    */
    do
    {
        // TODO: adding 1 plus 1
        if ((begins_with(line.value, sel.begin_clm, file, sel.beginw_str) ||
             contains(line.value, sel.contain_clm, file, sel.contain_str)) &&
            in_range(file.row_count, sel.row_N, sel.row_M))
            do_command(sel, &line, file);
        push_line(line, file);
        if ((error = load_line(&line, &file)))
            return errors_out(error, file);
    } while (!line.last_line);

    return 0;
}

/* --------------------------------------------------- */

/**
 * Funkce inicializuje datovou strukturu File.
 * @param file ukazatel na vytvorenou strukturu File
*/
void init_file(File *file)
{
    strcpy(file->list_of_delims, "");
    file->file_delim = DEFAULT_DELIM;
    file->out_delim = DEFAULT_DELIM;
    file->clm_count = 0;
    file->row_count = 0;
}

/**
 * Funkce inicializuje datovou strukturu Select.
 * @param sel ukazatel na vytvorenou strukturu Select
*/
void init_select(Select *sel)
{
    strcpy(sel->command, "");
    strcpy(sel->beginw_str, "");
    strcpy(sel->contain_str, "");
    strcpy(sel->cmd_str, "");

    sel->clm_C = 0;
    sel->clm_N = 0;
    sel->clm_M = 0;

    sel->row_R = 0;
    sel->row_N = 1;
    sel->row_M = TILL_END;
}

/** 
 * Funkce inicializuje datovou strukturu Edit
 * @param edit ukazatel na vytvorenou strukturu Edit
*/
void init_edit(Edit *edit)
{
    edit->arow = 0;
    edit->acol = 0;
    strcpy(edit->queue_rows, "");
    edit->temp_row = 0;
    strcpy(edit->queue_clm, "");
    edit->temp_column = 0;
}

/** 
 * Funkce pracuje s argumenty od uzivatel a kontoluje je se vsemi povolenymi prikazy. 
 * V pripade chyby vraci zpatky nenulove cislo.
 * Jako prvni kontroluje argumenty delim a nasledne pojizdi cely seznam.
 * V pripade, ze argument se schoduje s povolenym prikazem, nacte jeho parametry. 
 * Pokud se argument neshoduje s zadnym nabizenym prikazem, vraci chybove hlaseni.
 * Prikazy jsou rozdelene do podskupin pro prehlednejsi a lehci operaci.
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param sel ukazatel na datovou strukturu Select
 * @param edit ukazatel na datovou strukturu Edit
 * @param file ukazatel na datovou strukturu File
 * @return vraci 0 pro bezchybne zadani argumentu; kladne cislo pro chybove hlaseni
*/
int load_arguments(int argc, char **argv, Select *sel, Edit *edit, File *file)
{
    int i = 1;
    int error_state = 0; // Navratova hodnota pro pripad necekaneho vstupu

    // promenna ukazuje, zda se v jednom cyklu zmenila nejaka hodnota
    bool flag;
    if (argc < 2) // zadny argument
        return 0;

    if (!strcmp(argv[1], "-d")) // prvni argument je znak oddelovace
    {
        if ((error_state = load_delim(file, argc, argv)))
            return error_state;
        i += 2;
    }

    // prochazeni argumentu a porovnavani s prikazy
    for (; i < argc; i++)
    {
        flag = false;

        if ((error_state = load_edit_cmd(edit, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_selection_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_rest_cmds(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_three_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_two_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_one_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        // Pokud se argument neshoduje s zadnym ze zadanych
        // Jedna se o chybnou syntaxi argumentu
        return ERR_WRONG_ARGV;
    }
    return error_state;
}

/** TESTING
 * Funkce pracuje s argumenty pro nacteni znaku DELIM. 
 * @param file ukazatel na datovou strukturu File
 * @param argc pocet argumentu
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_delim(File *file, int argc, char **argv)
{
    if (argc >= 3)
    {
        if (strlen(argv[2]) > MAX_STR_LENGTH)
            return ERR_STR_LENGTH_TOO_LONG;

        strcpy(file->list_of_delims, argv[2]);
        file->out_delim = file->list_of_delims[0];
    }

    // V pripade, ze uzivatel zadal -d ale neupresnil mnozinu moznych delim
    else
        return ERR_WRONG_ARGV;
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat s jednim parametrem z argumentu.
 * Args: (clm_C).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_one_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 2)
    {
        if (!strcmp(argv[*i], "tolower") ||
            !strcmp(argv[*i], "toupper") ||
            !strcmp(argv[*i], "round") ||
            !strcmp(argv[*i], "int"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat se dvema parametry z argumentu. 
 * Args: (clm_N, clm_M) nebo (clm_C, str).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_two_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 3)
    {
        if (!strcmp(argv[*i], "copy") ||
            !strcmp(argv[*i], "swap") ||
            !strcmp(argv[*i], "move"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]))
                return ERR_NOTNUM;
            sel->clm_N = atoi(argv[++(*i)]);
            if (!is_num(argv[*i + 1]))
                return ERR_NOTNUM;
            sel->clm_M = atoi(argv[++(*i)]);
            *flag = true;
            return 0;
        }

        if (!strcmp(argv[*i], "cset"))
        {
            strcpy(sel->command, argv[*i]);
            sel->clm_C = atoi(argv[++(*i)]);
            strcpy(sel->cmd_str, argv[++(*i)]);
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat se tremi parametry z argumentu. 
 * Args: (clm_C, clm_N, clm_M) nebo (clm_C, row_N, row_M).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_three_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 4)
    {
        if (!strcmp(argv[*i], "csum") || !strcmp(argv[*i], "cavg") ||
            !strcmp(argv[*i], "cmin") || !strcmp(argv[*i], "cmax") ||
            !strcmp(argv[*i], "ccount"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
                !is_num(argv[*i + 3]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            sel->clm_N = atoi(argv[++(*i)]);
            sel->clm_M = atoi(argv[++(*i)]);
            *flag = true;
            return 0;
        }

        else if (!strcmp(argv[*i], "rsum") || !strcmp(argv[*i], "ravg") ||
                 !strcmp(argv[*i], "rmin") || !strcmp(argv[*i], "rmax") ||
                 !strcmp(argv[*i], "rcount"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
                !is_num(argv[*i + 3]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            sel->row_N = atoi(argv[++(*i)]);
            sel->row_M = atoi(argv[++(*i)]);
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce zkontroluje posledni specialni prikazy mezi argumenty. 
 * Prikaz cseq (clm_N, clm_M, int_B); rseq (row_N, row_M, int_B).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_rest_cmds(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (!strcmp(argv[*i], "cseq") && argc >= *i + 4)
    {
        strcpy(sel->command, argv[*i]);
        if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
            !is_num(argv[*i + 3]))
            return ERR_NOTNUM;
        sel->clm_N = atoi(argv[++(*i)]);
        sel->clm_M = atoi(argv[++(*i)]);
        strcpy(sel->cmd_str, argv[++(*i)]);
        *flag = true;
    }

    else if (!strcmp(argv[*i], "rseq") && argc >= *i + 5)
    {
        strcpy(sel->command, argv[*i]);
        // kontrola spravneho typu
        if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
            !is_num(argv[*i + 3]) ||
            !(is_num(argv[*i + 4]) || argv[*i + 4][0] == '-'))
            return ERR_NOTNUM;
        sel->clm_C = atoi(argv[++(*i)]);
        sel->row_N = atoi(argv[++(*i)]);

        // reseni pomlcky
        if (argv[*i + 1][0] == '-')
        {
            sel->row_M = TILL_END;
            (*i)++;
        }
        else
            sel->row_M = atoi(argv[++(*i)]);

        strcpy(sel->cmd_str, argv[++(*i)]);
        *flag = true;
    }
    return 0;
}

/** 
 * Funkce projizdi prikazy pro upravu tabulky mezi argumenty. 
 * Argumenty znacici hodnotu radku/sloupce musi nesmi narusit 
 * proudost zpracovani souboru.
 * @param edit ukazatel na datovou strukturu Edit
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_edit_cmd(Edit *edit, int argc, char **argv, int *i, bool *flag)
{
    if (!strcmp(argv[*i], "arow"))
    {
        edit->arow++;
        *flag = true;
    }
    else if (!strcmp(argv[*i], "acol"))
    {
        edit->acol++;
        *flag = true;
    }

    else if (argc >= *i + 3 && is_num(argv[*i + 1]) && is_num(argv[*i + 2]))
    {
        int from = atoi(argv[*i + 1]);
        int to = atoi(argv[*i + 2]);

        if (!strcmp(argv[*i], "drows") || !strcmp(argv[*i], "dcols"))
        {
            // kontroluje posloupnost argumentu a moznost proudoveho zpracovani
            if (from > to ||
                (from <= edit->temp_row && argv[*i][1] == 'r') ||
                (from <= edit->temp_column && argv[*i][1] == 'c'))
                return ERR_WRONG_ARGV;

            for (; from <= to; from++)
                enqueue(edit, argv[*i], from);

            // nacteni posledniho zapsaneho radku/sloupce
            if (argv[*i][1] == 'r')
                edit->temp_row = from - 1;
            else
                edit->temp_column = from - 1;

            *i += 2;
            *flag = true;
            return 0;
        }
    }

    else if (argc >= *i + 2 && is_num(argv[*i + 1]))
    {
        if (!strcmp(argv[*i], "irow") || !strcmp(argv[*i], "drow") ||
            !strcmp(argv[*i], "icol") || !strcmp(argv[*i], "dcol"))
        {
            //  moznost proudoveho zpracovani
            if ((atoi(argv[*i + 1]) <= edit->temp_row && argv[*i][1] == 'r') ||
                (atoi(argv[*i + 1]) <= edit->temp_column && argv[*i][1] == 'c'))
                return ERR_WRONG_ARGV;

            // nacteni posledniho zapsaneho radku/sloupce
            if (argv[*i][1] == 'r')
                edit->temp_row = atoi(argv[*i + 1]);
            else
                edit->temp_column = atoi(argv[*i + 1]);

            enqueue(edit, argv[*i], atoi(argv[++(*i)]));
            *flag = true;
        }
    }

    return 0;
}

/** 
 * Funkce projede vsechny prikazy pro selekci radku a bunek. 
 * Po shode nacte do struktur.
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_selection_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    int error;
    if (!strcmp(argv[*i], "beginswith") && argc >= *i + 3)
    {
        if ((error = set_beginswith(sel, argv, i)))
            return error;
        *flag = true;
    }
    else if (!strcmp(argv[*i], "contains") && argc >= *i + 3)
    {
        if ((error = set_contains(sel, argv, i)))
            return error;
        *flag = true;
    }

    else if (!strcmp(argv[*i], "rows") && argc >= *i + 3)
    {
        if ((error = set_rows(sel, argv, i)))
            return error;
        *flag = true;
    }
    return 0;
}

/** 
 * Funkce nacte prvni radek souboru a 
 * nastavi default hodnoty pro chod programu.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param file strukture file_t obsahujici informace o programu a souboru
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_first_line(Line *line, File *file)
{
    // nacte radek
    // zkontroluje zda je prazdny
    if (fgets(line->value, MAX_INPUT_LENGTH + 2, stdin) == NULL)
        return ERR_FEMPTY;

    // zkontroluje jaky oddelovac se nachazi v souboru
    // pokud se program dostal az do tehle faze
    // muzeme usoudit, ze v tabulkovem souboru se nachazi alespon 1 radek a sloupec
    int delim_count = 0;
    file->clm_count = 1;
    file->row_count = 1;
    for (int i = 0; file->list_of_delims[i]; i++)
    {
        // pokud se na radku objevuje alespon jeden znak oddelovace
        // program priradi souboru dane hodnoty
        delim_count = get_char_count(line->value, file->list_of_delims[i]);
        if (delim_count != 0)
        {
            file->file_delim = file->list_of_delims[i];
            file->clm_count = ++delim_count;
            line->clm_count = file->clm_count;
            break;
        }
    }
    if (line->value[MAX_STR_LENGTH + 1])
        return ERR_INPUT_TOO_LONG;

    return 0;
}

/**
 * Funkce nacte do retezce line nasledujici radek ze souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param file strukture file_t obsahujici informace o programu a souboru 
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_line(Line *line, File *file)
{
    // char input[MAX_STR_LENGTH];
    // scanf("%s", input);
    // printf("%ld\n", strlen(input));
    if (fgets(line->value, MAX_INPUT_LENGTH + 2, stdin) == NULL)
    {
        line->last_line = true;
        return 0;
    }
    line->clm_count = get_char_count(line->value, file->file_delim) + 1;
    file->row_count++;

    if (line->clm_count != file->clm_count)
        return ERR_WRONG_CCOUNT;
    if (line->value[MAX_STR_LENGTH + 1])
        return ERR_INPUT_TOO_LONG;

    return 0;
}

/**
 * Funkce vypise retezec do souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param file strukture file_t obsahujici informace o programu a souboru 
*/
void push_line(Line line, File file)
{
    // prepisuje vsechny oddelovace na vystupni oddelovac
    int index;
    if (file.file_delim != file.out_delim)
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

    for (; str_in[i]; i++) // hledani
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
    // TODO: mozne budouci vymazani fce
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
    for (i = 0; str_in[i]; i++)
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
void create_newline(File *file, bool newline)
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
void get_string_in_cell(char *str_in, int clm, File file, char *str_out)
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
void get_cell_info(char *str_in, int clm, File file,
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
 * Funkce zjistuje, zda je zadany znak pismeno
 * @param c znak, na ktery se ptame
 * @return true pokud znak je pismeno
*/
bool is_letter(char c)
{
    return isupper(c) || islower(c);
}

/** 
 * Funkce zjisti, zda je retezec cislo
 * @param str_in vstupni retezec
 * @return true pokud je retezec cislo
*/
bool is_num(char *str_in)
{
    if (strlen(str_in) == 0)
        return false;

    bool is_negative = str_in[0] == '-';
    bool is_float = false;
    int i;
    for (i = 0; str_in[i]; i++)
    {
        // kontroluje vyjimky, kdyz zadany znak neni cislice
        if (!isdigit(str_in[i]))
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
bool in_range(int number, int lower_bound, int upper_bound)
{
    return number >= lower_bound && number <= upper_bound;
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Hodnoty, ktere nacte, pridava nakonec retezce.
 * @param edit ukazatel na datovou strukturu Edit
 * @param cmd prikaz ktery se ma nacist do struktury (row/col)
 * @param value hodnota radku nebo sloupce (zalezi na cmd)
*/
void enqueue(Edit *edit, char *cmd, int value)
{
    char loc_str[12];
    sprintf(loc_str, "%d%c ", value, cmd[0]);

    if (cmd[1] == 'c')
        strcat(edit->queue_clm, loc_str);
    else if (cmd[1] == 'r')
        strcat(edit->queue_rows, loc_str);
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Queue funguje na bazi FIFO (first in, first out); odebere prvni 
 * hodnotu z fronty.
 * @param str retezec reprezentujici frontu (oddeluje se znakem mezery)
*/
void dequeue(char *str)
{
    // TODO: prototype, testing function
    int end_index = indexof(str, ' ', 0, 1);
    remove_str(str, 0, end_index + 1, str);
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Dava moznost uzivateli se podivat na prvni hodnotu ve fronte bez jejiho odebrani.
 * @param str retezec reprezentujici frontu (oddeluje se znakem mezery)
 * @param item_out vraci hodnotu na prvnim miste ve fronte
*/
void first_item(char *str, char *item_out)
{
    int end_index = indexof(str, ' ', 0, 1);
    substring(str, 0, end_index, item_out);
}

/* ---------------------------------- */

/**
 * Funkce prida novy radek. Je mozne pouzit jen create_newline() 
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void irow(File *file)
{
    create_newline(file, true);
    /*
    pripadne v mainu
    while (file.row_count >= 3 && file.row_count <= 5)
        irow(&file);
    */
}

/** 
 * Funkce smaze radek v souboru.  
 * Funkce momentalne prochazi cely soubor.
 * @param row radek, ktery ma byt smazan (row > 0)
*/
void drow(int row)
{
    // TODO: prototype function
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
    // TODO: prototype function
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
void icol(Line *line, int clm, File file)
{
    char str_to_insert[2]; /* retezec oddelovace */
    str_to_insert[0] = file.file_delim;
    str_to_insert[1] = '\0';

    if (clm == 1) // pokud chce uzivatel pridat novy sloupec pred prvni
        insert_str(line->value, 0, str_to_insert, line->value);

    else if (in_range(clm, 2, file.clm_count))
    {
        // zacatek nove bunky
        int starting_index = indexof(line->value, file.file_delim, 0, clm - 1) + 1;
        insert_str(line->value, starting_index, str_to_insert, line->value);
    }

    // pokud chce uzivatel pridat sloupec za posledni sloupec
    // funkce acol
    else if (clm == file.clm_count + 1)
    {
        int start_index, len;
        get_cell_info(line->value, file.clm_count, file, &start_index, &len);
        insert_str(line->value, start_index + len, str_to_insert, line->value);
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
void dcol(Line *line, int clm, File file)
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
 * Funkce provede akci podle nactenych parametru a prikazu ve strukturach.
 * @param sel ukazatel na datovou strukturu Select
 * @param edit ukazatel na datovou strukturu Edit
 * @param file ukazatel na datovou strukturu File
*/
void do_command(Select sel, Line *line, File file)
{
    // TODO: prototype function
    if (!sel.command)
        return;

    if (!strcmp(sel.command, "cset"))
        cset(line, sel.clm_C, file, sel.cmd_str);

    if (!strcmp(sel.command, "tolower"))
        to_lower(line, sel.clm_C, file);
    if (!strcmp(sel.command, "toupper"))
        to_upper(line, sel.clm_C, file);
    if (!strcmp(sel.command, "round"))
        round_func(line, sel.clm_C, file);
    if (!strcmp(sel.command, "int"))
        int_func(line, sel.clm_C, file);
    if (!strcmp(sel.command, "copy"))
        copy(line, sel.clm_N, sel.clm_M, file);
    if (!strcmp(sel.command, "swap"))
        swap(line, sel.clm_N, sel.clm_M, file);
    if (!strcmp(sel.command, "move"))
        move(line, sel.clm_N, sel.clm_M, file);
    if (!strcmp(sel.command, "csum"))
        csum(line, sel.clm_C, sel.clm_N, sel.clm_M, file);
    if (!strcmp(sel.command, "cavg"))
        cavg(line, sel.clm_C, sel.clm_N, sel.clm_M, file);
    if (!strcmp(sel.command, "cmin"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "cmax"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "ccount"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "cseq"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "rseq"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "rsum"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "ravg"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "rmin"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "rmax"))
    {
        //TODO:
    }
    if (!strcmp(sel.command, "rcount"))
    {
        //TODO:
    }
}

/** 
 * Funkce do bunky column nastavi retezec str_in. 
 * Promenna str_out slouzi jako vystupni retezec pro vytisknuti do souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
 * @param str_to_insert retezec, ktery bude nastaven v bunce
*/
void cset(Line *line, int clm, File file, char *str_to_insert)
{
    if (strlen(str_to_insert) > 100)
    {
        // TODO: Error
    }
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // smaze obsah bunky a vlozi novy obsah
    replace_str(line->value, start_index, len, str_to_insert, line->value);
}

/** 
 * Funkce prepise vsechna velka pismena v bunce na mala
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void to_lower(Line *line, int clm, File file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // projede retezec a prevede velka pismena na mala
    for (int i = start_index; i <= len + start_index; i++)
    {
        line->value[i] = tolower(line->value[i]);
    }
}

/** 
 * Funkce prepise vsechna mala pismena v bunce na velka
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void to_upper(Line *line, int clm, File file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // projede retezec a prevede mala pismena na velka
    for (int i = start_index; i <= len + start_index; i++)
    {
        line->value[i] = toupper(line->value[i]);
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
int round_func(Line *line, int clm, File file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_num(loc_str))
    {
        return errors_out(ERR_NOTNUM, file);
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
int int_func(Line *line, int clm, File file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->value, clm, file, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_num(loc_str))
    {
        return errors_out(ERR_NOTNUM, file);
    }

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", atoi(loc_str));
    cset(line, clm, file, loc_str);
    return 0;
}

/** 
 * Funkce prepise obsah bunky ve clm_M hodnotou ze clm_N 
 * (zkopiruje obsah bunky clm_N do clm_M).
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm_N sloupec, ze ktereho se kopiruje
 * @param clm_M sloupec, do ktereho se kopiruje
 * @param file struktura file_t obsahujici informace o programu a souboru

*/
void copy(Line *line, int clm_N, int clm_M,
          File file)
{
    char temp_str[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci N
    get_string_in_cell(line->value, clm_N, file, temp_str);

    // nastavime novou hodnotu do bunky M
    cset(line, clm_M, file, temp_str);
}

/** 
 * Funkce vymeni obsahy buneky z clm_M a clm_N.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm_N prvni sloupec
 * @param clm_M druhy sloupec
 * @param file struktura file_t obsahujici informace o programu a souboru

*/
void swap(Line *line, int clm_N, int clm_M,
          File file)
{
    char temp_str_M[MAX_INPUT_LENGTH];
    char temp_str_N[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci M do temp_str_M
    get_string_in_cell(line->value, clm_M, file, temp_str_M);

    // nacteni hodnoty ve sloupci N do temp_str_N
    get_string_in_cell(line->value, clm_N, file, temp_str_N);

    // nastavime novou hodnotu do bunky M a N
    cset(line, clm_M, file, temp_str_N);
    cset(line, clm_N, file, temp_str_M);
}

/** 
 * Funkce přesune obsah bunky v clm_N před clm_M.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm_N sloupec, ktery se presouva
 * @param clm_M sloupec, pred ktery se ma presunout obsah bunky clm_N
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void move(Line *line, int clm_N, int clm_M,
          File file)
{
    /** 
     * pokud je clm_N nalevo od clm_M
     * clm_N se prohazuje s pravou bunkou
     * dokud nebude hned vedle clm_M
    */
    while (clm_N < clm_M - 1)
    {
        swap(line, clm_N, clm_N + 1, file);
        clm_N++;
    }

    /** 
     * pokud je clm_N napravo od clm_M
     * tak se clm_N se prohazuje s levou bunkou
     * dokud nebude nalevo od clm_M (nebo-li pred clm_M)
    */
    while (clm_N > clm_M)
    {
        swap(line, clm_N, clm_N - 1, file);
        clm_N--;
    }
}

/** 
 * Funkce do bunky v column_C ulozi soucet hodnot bunek na 
 * stejnem radku ve sloupcích clm_N až clm_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void csum(Line *line, int clm_C, int clm_N,
          int clm_M, File file)
{
    /** TOOD: Error */
    if (in_range(clm_C, clm_N, clm_M))
    {
        printf("Error: column C is within the interval.\n");
        return;
    }

    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = clm_N; i <= clm_M; i++)
    {
        get_string_in_cell(line->value, i, file, loc_str);
        /** TOOD: Error */
        if (!is_num(loc_str))
        {
            // ERR_NOTNUM
            printf("Error: not a number.\n");
            return;
        }

        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce clm_C
    sprintf(loc_str, "%g", sum);
    cset(line, clm_C, file, loc_str);
}

/** 
 * Funkce do bunky v column_C ulozi aritmeticky prumer hodnot bunek na 
 * stejnem radku ve sloupcích clm_N až clm_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file struktura file_t obsahujici informace o programu a souboru
*/
void cavg(Line *line, int clm_C, int clm_N,
          int clm_M, File file)
{
    /** TOOD: Error */
    if (in_range(clm_C, clm_N, clm_M))
    {
        printf("Error: column C is within the interval.\n");
        return;
    }

    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = clm_N; i <= clm_M; i++)
    {
        get_string_in_cell(line->value, i, file, loc_str);
        /** TOOD: Error */
        if (!is_num(loc_str))
        {
            // ERR_NOTNUM
            printf("Error: not a number.\n");
            return;
        }

        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce clm_C
    sprintf(loc_str, "%g", sum / (clm_M - clm_N + 1));
    cset(line, clm_C, file, loc_str);
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
bool begins_with(char *str_in, int clm, File file, char *str_to_cmp)
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
              File file, char *str_to_cmp)
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
        while (j < len && loc_str[i] && loc_str[i] == str_to_cmp[j])
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

/** 
 * Funkce nastavi parametry prikazu beginswith.
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu beginswith v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_beginswith(Select *sel, char **argv, int *index)
{
    if (!is_num(argv[*index + 1]))
        return ERR_WRONG_ARGV;
    sel->begin_clm = atoi(argv[++(*index)]);
    strcpy(sel->beginw_str, argv[++(*index)]);
    return 0;
}

/** 
 * Funkce nastavi parametry prikazu contains
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu contains v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_contains(Select *sel, char **argv, int *index)
{
    if (!is_num(argv[*index + 1]))
        return ERR_WRONG_ARGV;
    sel->contain_clm = atoi(argv[++(*index)]);
    strcpy(sel->contain_str, argv[++(*index)]);
    return 0;
}

/** 
 * Funkce nastavi parametry prikazu rows
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu rows v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_rows(Select *sel, char **argv, int *index)
{
    if (!is_num(argv[*index + 1]) && argv[*index + 1][0] != '-')
        return ERR_WRONG_ARGV;
    else if (argv[*index + 1][0] == '-')
    {
        sel->row_N = TILL_END;
        (*index)++;
    }
    else
        sel->row_N = atoi(argv[++(*index)]);

    if (!is_num(argv[*index + 1]) && argv[*index + 1][0] != '-')
        return ERR_WRONG_ARGV;
    else if (argv[*index + 1][0] == '-')
    {
        sel->row_M = TILL_END;
        (*index)++;
    }
    else
        sel->row_M = atoi(argv[++(*index)]);
    return 0;
}