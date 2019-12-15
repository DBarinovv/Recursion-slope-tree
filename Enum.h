// Enums.h

enum types
{
    E_int  = 1,
    E_str  = 2,
    E_op   = 3,
    E_line = 4,
    E_key  = 5,
    E_key_op = 6,
    E_label  = 7,    // $
    E_call   = 8,
    E_func_label = 9,
};

enum operations
{
    E_default = 0,

    E_plus  = 10,
    E_minus = 11,
    E_mult  = 12,
    E_div   = 13,

    E_equal = 20,

    E_sin = 100,
    E_cos = 101,
    E_pow = 102,
    E_dif = 103,
    E_log = 104,
    E_exp = 105,
};

enum keywords
{
    E_if = 150,
    E_while = 151,
    E_func = 152,
};

enum keywords_opers
{
    E_ja  = 200,   // >
    E_jb  = 201,   // <
    E_jae = 203,   // >=
    E_jbe = 204,   // <=
    E_jne = 205,   // !=
    E_je  = 206,   // ==

    E_jmp  = 207,
    E_ret  = 208,
};
