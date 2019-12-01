#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================

const int C_max_len = 20;

const char *C_string = "";

//=============================================================================

struct node_t
{
    char *data;
    char *type;   // int, str, op
    node_t *left;
    node_t *right;
};

//=============================================================================

node_t *Create_Node (char *data, char *type);

//-----------------------------------------------------------------------------

node_t* Get_G ();

node_t* Get_E ();

node_t* Get_T ();

node_t* Get_P ();

node_t *Get_Id ();

node_t* Get_N ();

//-----------------------------------------------------------------------------

void Sin_Error (const char *name_of_func);

//-----------------------------------------------------------------------------

void PNG_Dump (node_t *node);

void Print_PNG_Labels (node_t *node, FILE *fout);

void Print_PNG        (node_t *node, FILE *fout);

//=============================================================================

void Simplify_Tree (node_t *node);

void Unit (node_t *node);

//=============================================================================

int main ()
{
    node_t* node = Get_G ();

    Simplify_Tree (node);

    PNG_Dump (node);

    printf ("END!\n");

    return 0;
}

//=============================================================================

node_t* Create_Node (char *data, char *type)
{
    node_t *node = (node_t *) calloc (1, sizeof (node_t) + 2);
    (node -> data) = (char *) calloc (C_max_len, sizeof (char));
    (node -> type) = (char *) calloc (10, sizeof (char));

    strcpy (node -> data, data);
    strcpy (node -> type, type);
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

        if (op == '+') new_res = Create_Node ("+", "op");
        else           new_res = Create_Node ("-", "op");

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
        if   (op == '*') new_res = Create_Node ("*", "op");
        else             new_res = Create_Node ("/", "op");

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

    return Create_Node (helper, "str");
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

    char *helper = (char *) calloc (C_max_len, sizeof (char));
    itoa (val, helper, 10);

    return Create_Node (helper, "int");
}

//-----------------------------------------------------------------------------

void Sin_Error (const char *name_of_func)
{
    printf ("SINT ERROR IN %s!\n", name_of_func);
}

//=============================================================================

void PNG_Dump (node_t *node)
{
    FILE *fout = fopen ("DOT", "w");

    fprintf (fout, "digraph\n");
    fprintf (fout, "{\n");
    fprintf (fout, "\"%p\" [label=\"%s\"];\n", node->data, node->data);

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
        fprintf (fout, "\"%p\" [label=\"%s\"];\n", (node -> left) -> data, (node -> left) -> data);
        Print_PNG_Labels (node -> left, fout);
    }

    if (node -> right)
    {
        fprintf (fout, "\"%p\" [label=\"%s\"];\n", (node -> right) -> data, (node -> right) -> data);
        Print_PNG_Labels (node -> right, fout);
    }
}

//-----------------------------------------------------------------------------

void Print_PNG (node_t *node, FILE *fout)
{
    if (node -> left)
    {
        fprintf (fout, "\"%p\"->\"%p\";\n", node -> data, (node -> left) -> data);
        Print_PNG (node -> left, fout);
    }

    if (node -> right)
    {
        fprintf (fout, "\"%p\"->\"%p\";\n", node -> data, (node -> right) -> data);
        Print_PNG (node -> right, fout);
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

    Unit (node);
}

//-----------------------------------------------------------------------------

void Unit (node_t *node)
{
    if (strcmp ((node->left)->type, "int") == 0 && strcmp ((node->right)->type, "int") == 0)
    {
        if (strcmp (node->type, "op") == 0)
        {
            char *helper = (char *) calloc (C_max_len, sizeof (char));

            if (strcmp (node->data, "+") == 0)
            {
                node->data = itoa (atoi ((node->left)->data) + atoi ((node->right)->data), helper, 10);
            }
            else if (strcmp (node->data, "-") == 0)
            {
                node->data = itoa (atoi ((node->left)->data) - atoi ((node->right)->data), helper, 10);
            }
            else if (strcmp (node->data, "*") == 0)
            {
                node->data = itoa (atoi ((node->left)->data) * atoi ((node->right)->data), helper, 10);
            }
            else if (strcmp (node->data, "/") == 0)
            {
                node->data = itoa (atoi ((node->left)->data) / atoi ((node->right)->data), helper, 10);
            }
            node->type = "int";

            free (node->left);
            node->left = nullptr;
            free (node->right);
            node->right = nullptr;
        }
    }
}
