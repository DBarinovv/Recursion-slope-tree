//=============================================================================
//                                INCLUDES                                    ;
//=============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Enum.h"

//=============================================================================
//                               STRUCTURES                                   ;
//=============================================================================

struct func_t
{
    const char *name;
    const int num;
};

//-----------------------------------------------------------------------------

struct node_t
{
    int data;
    int type;      // E_int, E_str, E_op
    node_t* left;
    node_t* right;
};

//=============================================================================
//                          GLOBAL/CONST VARIABLES                            ;
//=============================================================================

const int C_max_len = 20;

const int C_max_cnt_of_names = 10;

const int C_accuracy = pow (10, 3);

char* *G_names = (char **) calloc (C_max_cnt_of_names, sizeof (char *));

const func_t C_functions[] = {
                            {"sin", E_sin},
                            {"cos", E_cos},
                            {"pow", E_pow},
                            {"dif", E_dif},
                            {"log", E_log},
                            {"exp", E_exp}
                             };

const char *C_string = "x*(2-1)";

//=============================================================================
//                              RECURSION SLOPE                               ;
//=============================================================================

node_t* Create_Node (const int data, const int type);

//-----------------------------------------------------------------------------

node_t* Get_G ();

node_t* Get_E ();

node_t* Get_T ();

node_t* Get_P ();

node_t* Get_Id ();

node_t* Get_N ();

node_t* Get_P_With_Comma ();   // for functions with 2 arguments

//-----------------------------------------------------------------------------

void Syntax_Error (const char *name_of_func);

//-----------------------------------------------------------------------------

node_t* Case_Functions (const char *str);

//=============================================================================
//                             PNG DUMP FOR TREE                              ;
//=============================================================================

void PNG_Dump (node_t* node);

void Print_PNG_Labels (node_t* node, FILE *fout);

void Print_PNG        (node_t* node, FILE *fout);

void Print_Node_Data_In_Right_Way (node_t* node, FILE *fout);

//=============================================================================
//                              SIMPLIFY TREE                                 ;
//=============================================================================

node_t* Simplify_Tree (node_t* node);

node_t* Unit (node_t* node);

void Unit_For_Oper_With_Two_Arg (node_t* node);

void Unit_For_One_And_Zero (node_t* node);

void Unit_For_Oper_With_One_Arg (node_t* node);

node_t* Case_Differentiation (node_t* node);

node_t* Unit_Differentiation (node_t* node);

node_t* Unit_Copy (node_t* node_res, node_t* node_cpy);

//=============================================================================

int main ()
{
    for (int i = 0; i < C_max_cnt_of_names; i++) G_names[i] = "";

    node_t* node = Get_G ();

    node = Simplify_Tree (node);
    node = Simplify_Tree (node);
    node = Simplify_Tree (node);

    PNG_Dump (node);

    printf ("END!\n");

    return 0;
}

//=============================================================================

node_t* Create_Node (const int data, const int type)
{
    node_t* node = (node_t *) calloc (1, sizeof (node_t) + 2);

    node -> data = data;
    node -> type = type;
    node -> left  = nullptr;
    node -> right = nullptr;

    if (node == nullptr)  Syntax_Error ("Create_Node nullptr");

    return node;
}

//=============================================================================

node_t* Get_G ()
{
    node_t* res = Get_E ();

    if (*C_string == '\0')
        return res;
    else
    {
        Syntax_Error ("Get_G");
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

node_t* Get_E ()
{
    node_t* node1 = Get_T ();
    node_t* new_res = nullptr;

    while (*C_string == '+' || *C_string == '-')
    {
        char op = *C_string;
        C_string++;

        node_t* node2 = Get_T ();

        if (op == '+') new_res = Create_Node (E_plus,  E_op);
        else           new_res = Create_Node (E_minus, E_op);

        new_res->left  = node1;
        new_res->right = node2;

        node1 = new_res;
    }

    return node1;
}

//-----------------------------------------------------------------------------

node_t* Get_T ()
{
    node_t* node1 = Get_P ();

    while (*C_string == '*' || *C_string == '/')
    {
        char op = *C_string;
        C_string++;

        node_t* node2 = Get_P ();
        node_t* new_res = nullptr;

        if   (op == '*') new_res = Create_Node (E_mult, E_op);
        else             new_res = Create_Node (E_div , E_op);

        new_res->left  = node1;
        new_res->right = node2;

        node1 = new_res;
    }

    return node1;
}

//-----------------------------------------------------------------------------

node_t* Get_P ()
{
    if (*C_string == '(')
    {
        C_string++;
        node_t* helper = Get_E ();

        if (*C_string == ')')
        {
            C_string++;
            return helper;
        }
        else Syntax_Error ("Get_P");
    }
    else if ('a' <= *C_string && *C_string <= 'z')
    {
        return Get_Id ();
    }
    else
    {
        return Get_N ();
    }
}

//-----------------------------------------------------------------------------

node_t* Get_Id ()
{
    char *helper = (char *) calloc (C_max_len, sizeof (char));

    int pos = 0;
    helper[pos++] = *C_string;
    C_string++;

    while ('a' <= *C_string && *C_string <= 'z')
    {
        helper[pos++] = *C_string;
        C_string++;
    }

    node_t* node = Case_Functions (helper);
    if (node != nullptr) return node;

    static int cnt = 0;

    for (int i = 0; i < C_max_cnt_of_names; i++)
    {
        if (strcmp (G_names[i], helper) == 0)
        {
            return Create_Node (i, E_str);
        }
    }

    G_names[cnt++] = helper;

    return Create_Node (cnt - 1, E_str);
}

//-----------------------------------------------------------------------------

node_t* Get_N ()
{
    int val = 0;

    val = val * 10 + (*C_string - '0');
    C_string++;

    while ('0' <= *C_string && *C_string <= '9')
    {
        val = val * 10 + (*C_string - '0');
        C_string++;
    }

    val *= C_accuracy;

    return Create_Node (val, E_int);
}

//-----------------------------------------------------------------------------

node_t* Get_P_With_Comma ()
{
    if (*C_string == '(')
    {
        C_string++;
        node_t* node1 = Get_E ();

        if (*C_string == ',')
        {
            C_string++;
            node_t* node2 = Get_E ();

            if (*C_string != ')')
            {
                Syntax_Error ("Get_P_With_Comma have no ')'");
                return false;
            }

            C_string++;

            node_t* node = Create_Node (E_default, E_op);
            node->left  = node1;
            node->right = node2;

            return node;
        }
        else Syntax_Error ("Get_P_With_Comma have no ','");
    }
    else Syntax_Error ("Get_P_With_Comma have no '('");

    return nullptr;
}

//-----------------------------------------------------------------------------

void Syntax_Error (const char *name_of_func)
{
    printf ("SYNTAX ERROR IN %s!\n", name_of_func);
}

//=============================================================================

node_t* Case_Functions (const char *str)
{
    for (int i = 0; i < sizeof (C_functions) / sizeof (C_functions[0]); i++)
    {
        if (strcmp (C_functions[i].name, str) == 0)
        {
            node_t* node = Create_Node (C_functions[i].num, E_op);

            switch (C_functions[i].num)
            {
                case (E_default):
                {
                    printf ("OSHIBKA V INITIALIZATION, ERROR!!!\n");
                }
                case (E_sin):
                {
                    node->left = Get_P ();
                    break;
                }
                case (E_cos):
                {
                    node->left = Get_P ();
                    break;
                }
                case (E_pow):
                {
                    node = Get_P_With_Comma ();
                    node->data = E_pow;

                    break;
                }
                case (E_dif):
                {
                    node->left = Get_P ();
                    break;
                }
                case (E_log):
                {
                    node->left = Get_P ();
                    break;
                }
                case (E_exp):
                {
                    node->left = Get_P ();
                    break;
                }
                default:
                    printf ("NET TAKOY FUNC (Case_Functions), ERROR!!!\n");
            }

            return node;
        }
    }

    return nullptr;
}

//=============================================================================

void PNG_Dump (node_t* node)
{
    FILE *fout = fopen ("DOT", "w");

    fprintf (fout, "digraph\n");
    fprintf (fout, "{\n");
    fprintf (fout, "\"%p\" [label=\"", node);
    Print_Node_Data_In_Right_Way (node, fout);
    fprintf (fout, "\"]\n");

    Print_PNG_Labels (node, fout);
    Print_PNG        (node, fout);

    fprintf (fout, "}");

    fclose (fout);
}

//-----------------------------------------------------------------------------

void Print_PNG_Labels (node_t* node, FILE *fout)
{
    if (node -> left)
    {
        fprintf (fout, "\"%p\" [label=\"", node->left);
        Print_Node_Data_In_Right_Way (node->left, fout);
        fprintf (fout, "\"]\n");

        Print_PNG_Labels (node -> left, fout);
    }

    if (node -> right)
    {
        fprintf (fout, "\"%p\" [label=\"", node->right);
        Print_Node_Data_In_Right_Way (node->right, fout);
        fprintf (fout, "\"]\n");

        Print_PNG_Labels (node -> right, fout);
    }
}

//-----------------------------------------------------------------------------

void Print_PNG (node_t* node, FILE *fout)
{
    if (node -> left)
    {
        fprintf (fout, "\"%p\"->\"%p\";\n", node, node->left);
        Print_PNG (node -> left, fout);
    }

    if (node -> right)
    {
        fprintf (fout, "\"%p\"->\"%p\";\n", node, node->right);
        Print_PNG (node -> right, fout);
    }
}

//-----------------------------------------------------------------------------

void Print_Node_Data_In_Right_Way (node_t* node, FILE *fout)
{
    if (node->type == E_int)
    {
        fprintf (fout, "%lf", (double) (node->data) / C_accuracy);
    }
    else if (node->type == E_str)
    {
        fprintf (fout, "%s", G_names[node->data]);
    }
    else if (node->type == E_op)
    {
        switch (node->data)
        {
            case (E_plus):
            {
                fprintf (fout, "+");
                return;
            }
            case (E_minus):
            {
                fprintf (fout, "-");
                return;
            }
            case (E_mult):
            {
                fprintf (fout, "*");
                return;
            }
            case (E_div):
            {
                fprintf (fout, "/");
                return;
            }
            case (E_sin):
            {
                fprintf (fout, "sin");
                return;
            }
            case (E_cos):
            {
                fprintf (fout, "cos");
                return;
            }
            case (E_pow):
            {
                fprintf (fout, "pow");
                return;
            }
            case (E_dif):
            {
                fprintf (fout, "dif");
                return;
            }
            case (E_log):
            {
                fprintf (fout, "log");
                return;
            }
            case (E_exp):
            {
                fprintf (fout, "exp");
                return;
            }
            default:
                printf ("NET TAKOGO OPERATORA, ERROR!!!\n");
        }
    }
}

//=============================================================================

node_t* Simplify_Tree (node_t* node)
{
    if (node->left && ((node->left)->left || (node->left)->right))
    {
        node->left = Simplify_Tree (node->left);
    }

    if (node->right && ((node->right)->left || (node->right)->right))
    {
        node->right = Simplify_Tree (node->right);
    }

    node = Unit (node);

    return node;
}

//-----------------------------------------------------------------------------

node_t* Unit (node_t* node)
{
    if (node->right)                // functions with 2 arguments
    {
        if ((node->left)->type == E_int && (node->right)->type == E_int)
        {
            Unit_For_Oper_With_Two_Arg (node);
        }
        else if (node->type == E_op)
        {
            Unit_For_One_And_Zero (node);
        }
    }
    else if (node->type == E_op)    // functions with 1 argument
    {
        Unit_For_Oper_With_One_Arg (node);
    }

    return node;
}

//-----------------------------------------------------------------------------

void Unit_For_Oper_With_Two_Arg (node_t* node)
{
    if (node->type == E_op)
    {
        switch (node->data)
        {
            case (E_plus):
            {
                node->data = (node->left)->data + (node->right)->data;
                break;
            }
            case (E_minus):
            {
                node->data = (node->left)->data - (node->right)->data;
                break;
            }
            case (E_mult):
            {
                node->data = (node->left)->data * (node->right)->data / C_accuracy;
                break;
            }
            case (E_div):
            {
                node->data = (node->left)->data / (node->right)->data * C_accuracy;
                break;
            }
            case (E_pow):
            {
                node->data = (int) floor (C_accuracy * pow ((node->left)->data / C_accuracy, (node->right)->data / C_accuracy));
                break;
            }
            default:
                printf ("POKA CHTO NE PRIDUMAL, ERROR!!!\n");
        }

        node->type = E_int;

        free (node->left);
        node->left = nullptr;
        free (node->right);
        node->right = nullptr;
    }
}

//-----------------------------------------------------------------------------

void Unit_For_One_And_Zero (node_t* node)
{
    if (node->data == E_mult)            // case  *0 || 0* || 1* || *1
    {
        if (((node->left)->data == 0 && (node->left)->type  == E_int) ||                   // 0*
           ((node->right)->data == 0 && (node->right)->type == E_int))                     // *0
        {
            node->data = 0;
            node->type = E_int;

            free (node->left);
            node->left = nullptr;
            free (node->right);
            node->right = nullptr;
        }
        else if (((node->left)->data == 1 * C_accuracy && (node->left)->type == E_int))    // 1*
        {
            free (node->left);

            node->data  = (node->right)->data;
            node->type  = (node->right)->type;
            node->right = nullptr;
            node->left  = nullptr;
        }
        else if (((node->right)->data == 1 * C_accuracy && (node->right)->type == E_int))  // *1
        {
            free (node->right);

            node->data  = (node->left)->data;
            node->type  = (node->left)->type;
            node->right = nullptr;
            node->left  = nullptr;
        }
    }
    else if (node->data == E_plus)       // case  +0 || 0+
    {
        if (((node->left)->data == 0 && (node->left)->type == E_int))
        {
            free (node->left);
            node->left = nullptr;

            node->data = node->right->data;
            node->type = node->right->type;

            free (node->right);
            node->right = nullptr;
        }
        else if (((node->right)->data == 0 && (node->right)->type == E_int))
        {
            free (node->right);
            node->right = nullptr;

            node->data = node->left->data;
            node->type = node->left->type;

            free (node->left);
            node->left = nullptr;
        }
    }
    else if (node->data == E_minus)      // case  -0
    {
        if (((node->right)->data == 0 && (node->right)->type == E_int))
        {
            free (node->right);
            node->right = nullptr;

            node->data = node->left->data;
            node->type = node->left->type;

            free (node->left);
            node->left = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------

void Unit_For_Oper_With_One_Arg (node_t* node)
{
    if ((node->left)->type == E_int)
    {
        switch (node->data)
        {
            case (E_sin):
            {
                node->data = (int) floor (C_accuracy * sin ((node->left)->data / C_accuracy));
                break;
            }
            case (E_cos):
            {
                node->data = (int) floor (C_accuracy * cos ((node->left)->data / C_accuracy));
                break;
            }
            case (E_dif):
            {
                node = Case_Differentiation (node);
                break;
            }
            case (E_log):
            {
                node->data = (int) floor (C_accuracy * log ((node->left)->data / C_accuracy));
                break;
            }
            case (E_exp):
            {
                node->data = (int) floor (C_accuracy * exp ((node->left)->data / C_accuracy));
                break;
            }
            default:
                printf ("NET TAKOY FUNC (Unit), ERROR!!!\n");
        }

        node->type = E_int;

        free (node->left);
        node->left = nullptr;
    }
    else
    {
        if (node->data == E_dif)
        {
            node = Case_Differentiation (node);
        }
    }
}

//-----------------------------------------------------------------------------

node_t* Case_Differentiation (node_t* node)
{
    node_t* res = Unit_Differentiation (node->left);

    return res;
}

//-----------------------------------------------------------------------------

#define CN(data, type)\
    Create_Node (data, type)

#define UD(node)\
    Unit_Differentiation (node)

#define NL\
    node->left

#define NR\
    node->right

#define CPY(where, from)                 \
    where = CN(from->data, from->type);  \
    where = Unit_Copy (where, from);


node_t* Unit_Differentiation (node_t* node)
{
    if (node->type == E_int)
    {
        return CN(0, E_int);
    }

    if (node->type == E_str)
    {
        return CN(1 * C_accuracy, E_int);
    }

    if (node->type == E_op)
    {
        switch (node->data)
        {
            case (E_default):
            {
                printf ("OSHIBKA PRI INITIALIZATION, ERROR!!!\n");
                break;
            }
            case (E_plus):
            {
                node_t* res = CN(E_plus, E_op);
                res->left  = UD(NL);
                res->right = UD(NR);

                return res;
                break;
            }
            case (E_minus):
            {
                node_t* res = CN(E_minus, E_op);
                res->left  = UD(NL);
                res->right = UD(NR);

                return res;
                break;
            }
            case (E_mult):
            {
                node_t* res = CN(E_plus, E_op);

                res->left = CN(E_mult, E_op);
                (res->left)->left  = UD(NL);
                CPY((res->left)->right, NR)

                res->right = CN(E_mult, E_op);
                CPY((res->right)->left, NL)
                (res->right)->right = UD(NR);

                return res;
                break;
            }
            case (E_div):
            {
                node_t* res = CN(E_div, E_op);

                res->right = CN(E_pow, E_op);
                CPY((res->right)->left, NR)
                (res->right)->right = CN(2 * C_accuracy, E_int);


                (res->left) = CN(E_minus, E_op);

                (res->left)->left = CN(E_mult, E_op);     //}
                ((res->left)->left)->left  = UD(NL);      //|
                CPY(((res->left)->left)->right, NR)       //|
                                                          //|  mult, when res = res->left
                (res->left)->right = CN(E_mult, E_op);    //|
                CPY(((res->left)->right)->left, NL)       //|
                ((res->left)->right)->right = UD(NR);     //}

                return res;
                break;
            }
            case (E_sin):
            {
                node_t* res = CN(E_mult, E_op);

                res->left = CN(E_cos, E_op);
                CPY((res->left)->left, NL)

                res->right = UD(NL);

                return res;
                break;
            }
            case (E_cos):
            {
                node_t* res = CN(E_mult, E_op);

                res->left = CN(E_mult, E_op);
                (res->left)->left = CN(E_sin, E_op);
                CPY(((res->left)->left)->left, NL)

                (res->left)->right = CN(-1 * C_accuracy, E_int);

                res->right = UD(NL);

                return res;
                break;
            }
            case (E_pow):
            {
//                node_t* res = CN(E_dif, E_op);                   }
//                                                                 |
//                res->left         = CN(E_exp,  E_op);            |
//                (res->left)->left = CN(E_mult, E_op);            |
//                                                                 |
//                ((res->left)->left)->left = CN(E_log, E_op);     |  with exp
//                CPY((((res->left)->left)->left)->left, NL)       |
//                                                                 |
//                CPY(((res->left)->left)->right, NR)              |
//                                                                 |
//                return res;                                      }

                node_t* res = CN(E_mult, E_op);

                res->right = CN(E_pow, E_op);
                CPY((res->right)->left, NL)
                CPY((res->right)->right, NR)

                res->left = CN(E_dif, E_op);
                (res->left)->left = CN(E_mult, E_op);

                ((res->left)->left)->left = CN(E_log, E_op);
                CPY((((res->left)->left)->left)->left, NL)

                CPY(((res->left)->left)->right, NR)

                return res;
                break;
            }
            case (E_dif):
            {
                return Case_Differentiation (node);
                break;
            }
            case (E_log):
            {
                node_t* res = CN(E_mult, E_op);

                res->left = CN(E_div, E_op);
                (res->left)->left  = CN(1 * C_accuracy, E_int);
                CPY((res->left)->right, NL)

                res->right = CN(E_log, E_op);
                CPY((res->right)->left, NL)

                return res;
                break;
            }
            case (E_exp):
            {
                node_t* res = CN(E_mult, E_op);

                res->left = UD(NL);
                res->right = CN(E_exp, E_op);
                CPY((res->right)->left, NL);

                return res;
                break;
            }
        }
    }
}

#undef CN
#undef UF
#undef NL
#undef NR
#undef CPY

//-----------------------------------------------------------------------------

node_t* Unit_Copy (node_t* node_res, node_t* node_cpy)
{
    if (node_cpy->left)
    {
        node_res->left = Create_Node ((node_cpy->left)->data, (node_cpy->left)->type);
        Unit_Copy (node_res->left, node_cpy->left);
    }

    if (node_cpy->right)
    {
        node_res->right = Create_Node ((node_cpy->right)->data, (node_cpy->right)->type);
        Unit_Copy (node_res->right, node_cpy->right);
    }

    return node_res;
}
