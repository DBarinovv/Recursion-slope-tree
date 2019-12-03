#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Enum.h"

//=============================================================================

const int C_max_len = 20;

const int C_max_cnt_of_names = 10;

char* *G_names = (char **) calloc (C_max_cnt_of_names, sizeof (char *));

const char *C_functions[] = {"sin", "cos"};

const char *C_string = "sin(5)+sin(5*7)-cos(sin(2+3))";

//=============================================================================

struct node_t
{
    int data;
    int type;      // E_int, E_str, E_op
    node_t *left;
    node_t *right;
};

//=============================================================================

node_t *Create_Node (const int data, const int type);

//-----------------------------------------------------------------------------

node_t* Get_G ();

node_t* Get_E ();

node_t* Get_T ();

node_t* Get_P ();

node_t* Get_Id ();

node_t* Get_N ();

//-----------------------------------------------------------------------------

void Sin_Error (const char *name_of_func);

//-----------------------------------------------------------------------------

node_t* Case_Functions (const char *str);

//bool What_Func (const char *str);

//=============================================================================

void PNG_Dump (node_t *node);

void Print_PNG_Labels (node_t *node, FILE *fout);

void Print_PNG        (node_t *node, FILE *fout);

void Print_Node_Data_In_Right_Way (node_t *node, FILE *fout);

//=============================================================================

void Simplify_Tree (node_t *node);

void Unit (node_t *node);

void Unit_For_Func (node_t *node, const int data);

//=============================================================================

int main ()
{
    node_t* node = Get_G ();

//    printf ("1)NODE->DATA = [%d], NODE->TYPE = [%d]\n", node->data, node->type);
//    printf ("2)NODE->DATA = [%d], NODE->TYPE = [%d]\n", (node->left)->data, (node->left)->type);
//    printf ("3)NODE->DATA = [%d], NODE->TYPE = [%d]\n", (node->right)->data, (node->right)->type);

//    Simplify_Tree (node);

    PNG_Dump (node);

    printf ("END!\n");

    return 0;
}

//=============================================================================

node_t* Create_Node (const int data, const int type)
{
    node_t *node = (node_t *) calloc (1, sizeof (node_t) + 2);

    node -> data = data;
    node -> type = type;
    node -> left  = nullptr;
    node -> right = nullptr;

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
        Sin_Error ("Get_G");
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
        else Sin_Error ("Get_P");
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

node_t *Get_Id ()
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

    strcpy (G_names[cnt++], helper);

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

    return Create_Node (val, E_int);
}

//-----------------------------------------------------------------------------

void Sin_Error (const char *name_of_func)
{
    printf ("SINT ERROR IN %s!\n", name_of_func);
}

//=============================================================================

node_t* Case_Functions (const char *str)
{
    for (int i = 0; i < sizeof (C_functions) / sizeof (C_functions[0]); i++)
    {
        if (strcmp (C_functions[i], str) == 0)
        {
            node_t* node = nullptr;

            if (str[0] == 's')
            {
                node = Create_Node (E_sin, E_op);
            }
            else
            {
                node = Create_Node (E_cos, E_op);
            }

            node->left = Get_P ();

//            node_t *node = Create_Node (0, E_op);
//            if (str[0] == 's') node->data = E_sin;
//            else node->data = E_cos;
//
//            node->left = Get_P ();

            return node;
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

//bool What_Func (const char *str)
//{
//    const char *functions[] = {"sin", "cos"};
//
//    for (int i = 0; i < sizeof (functions) / sizeof (functions[0]); i++)
//    {
//        if (strcmp (functions[i], str) == 0) return true;
//    }
//}

//=============================================================================

void PNG_Dump (node_t *node)
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

void Print_PNG_Labels (node_t *node, FILE *fout)
{
    if (node -> left)
    {
//        fprintf (fout, "\"%p\" [label=\"%d\"];\n", (node -> left) -> data, (node -> left) -> data);
        fprintf (fout, "\"%p\" [label=\"", node->left);
        Print_Node_Data_In_Right_Way (node->left, fout);
        fprintf (fout, "\"]\n");

        Print_PNG_Labels (node -> left, fout);
    }

    if (node -> right)
    {
//        fprintf (fout, "\"%p\" [label=\"%d\"];\n", (node -> right) -> data, (node -> right) -> data);
        fprintf (fout, "\"%p\" [label=\"", node->right);
        Print_Node_Data_In_Right_Way (node->right, fout);
        fprintf (fout, "\"]\n");

        Print_PNG_Labels (node -> right, fout);
    }
}

//-----------------------------------------------------------------------------

void Print_PNG (node_t *node, FILE *fout)
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

void Print_Node_Data_In_Right_Way (node_t *node, FILE *fout)
{
    if (node->type == E_int)
    {
        fprintf (fout, "%d", node->data);
    }
    else if (node->type == E_str)
    {
        fprintf (fout, "%s", G_names[node->data]);
        printf ("%s\n", G_names[node->data]);
    }
    else if (node->type == E_op)
    {
        switch (node->data)
        {
            case (E_plus):
            {
                fprintf (fout, "+");
                return ;
            }
            case (E_minus):
            {
                fprintf (fout, "-");
                return ;
            }
            case (E_mult):
            {
                fprintf (fout, "*");
                return ;
            }
            case (E_div):
            {
                fprintf (fout, "/");
                return ;
            }
            case (E_sin):
            {
                fprintf (fout, "sin");
                return ;
            }
            case (E_cos):
            {
                fprintf (fout, "cos");
                return ;
            }
        }
    }
}

//=============================================================================

void Simplify_Tree (node_t *node)
{
    if (node->left && ((node->left)->left || (node->left)->right))
    {
        Simplify_Tree (node->left);
    }

    if (node->right && ((node->right)->left || (node->right)->right))
    {
        Simplify_Tree (node->right);
    }

//    Unit (node);
}

//-----------------------------------------------------------------------------

//////void Unit (node_t *node)
//////{
//////    if (strcmp ((node->left)->type, "int") == 0 && strcmp ((node->right)->type, "int") == 0)
//////    {
//////        if (strcmp (node->type, "op") == 0)
//////        {
//////            char *helper = (char *) calloc (C_max_len, sizeof (char));
//////
//////            if (strcmp (node->data, "+") == 0)
//////            {
//////                node->data = itoa (atoi ((node->left)->data) + atoi ((node->right)->data), helper, 10);
//////            }
//////            else if (strcmp (node->data, "-") == 0)
//////            {
//////                node->data = itoa (atoi ((node->left)->data) - atoi ((node->right)->data), helper, 10);
//////            }
//////            else if (strcmp (node->data, "*") == 0)
//////            {
//////                node->data = itoa (atoi ((node->left)->data) * atoi ((node->right)->data), helper, 10);
//////            }
//////            else if (strcmp (node->data, "/") == 0)
//////            {
//////                node->data = itoa (atoi ((node->left)->data) / atoi ((node->right)->data), helper, 10);
//////            }
//////            node->type = "int";
//////
//////            free (node->left);
//////            node->left = nullptr;
//////            free (node->right);
//////            node->right = nullptr;
//////        }
//        else if (strcmp (node->type, "func") == 0)
//        {
//            Unit_For_Func (node, node->data);
//        }
//////    }
//    else
//    {
//        if (strcmp ((node->left)->type, "func") == 0)
//        {
//            char *helper = (char *) calloc (C_max_len, sizeof (char));
//
//            int pos = 0;
//            for (int i = 0; 'a' <= node->data[i] && node->data[i] <= 'z'; i++)
//            {
//                helper[pos++] = node->data[i];
//            }
//
//        }
//
//        if (strcmp ((node->right)->type, "func") == 0)
//        {
//        }
//    }
////////}

//-----------------------------------------------------------------------------

//void Unit_For_Func (node_t *node, const char *data)
//{
//    char *helper = (char *) calloc (C_max_len, sizeof (char));
//
//    int pos = 0;
//    for (int i = 0; 'a' <= data[i] && data[i] <= 'z'; i++)
//    {
//        helper[pos++] = data[i];
//    }
//
//    for (int i = 0; i < sizeof (C_functions) / sizeof (C_functions[0]); i++)
//    {
//        if (strcmp (C_functions[i], helper) == 0)
//        {
//            return;
//        }
//    }
//}
