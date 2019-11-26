#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================

const int C_max_len = 20;

const char *s = "(2+x)*4/(9-4)";

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

void *Add_Node (node_t *node);

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

int main ()
{
    node_t* node = Get_G ();
    printf ("FIRST\n");
    printf ("NODE = %p\n", node);
    PNG_Dump (node);
    printf ("SECOND\n");

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

//-----------------------------------------------------------------------------

void *Add_Node (node_t *node)
{
}

//=============================================================================

node_t* Get_G ()
{
    node_t* res = Get_E ();

    if (*s == '\0')
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

//    printf ("NODE1->data = %s\n", node1->data);

    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;

        node_t* node2 = Get_T ();

        if (op == '+') new_res = Create_Node ("+", "op");
        else           new_res = Create_Node ("-", "op");

        new_res->left  = node1;
        new_res->right = node2;

        node1 = new_res;
    }

//    printf ("RES = %p\n", res);

    return node1;
}

//-----------------------------------------------------------------------------

node_t* Get_T ()
{
    node_t* node1 = Get_P ();

//    printf ("NODE1->data = %s\n", node1->data);

    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;

        node_t* node2 = Get_P ();

        node_t* new_res = nullptr;
        if   (op == '*') new_res = Create_Node ("*", "op");
        else             new_res = Create_Node ("/", "op");

//        printf ("NEW_RES->data = %s\n", new_res->data);

        new_res->left  = node1;
        new_res->right = node2;

        node1 = new_res;
    }

//    printf ("RES = %p\n", res);

    return node1;
}

//-----------------------------------------------------------------------------

node_t* Get_P ()
{
    if (*s == '(')
    {
        s++;
        node_t* helper = Get_E ();

        if (*s == ')')
        {
            s++;
            return helper;
        }
        else Sin_Error ("Get_P");
    }
    else if ('a' <= *s && *s <= 'z')
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
    helper[pos++] = *s;
    s++;

    while ('a' <= *s && *s <= 'z')
    {
        helper[pos++] = *s;
        s++;
    }

    return Create_Node (helper, "str");
}

//-----------------------------------------------------------------------------

node_t* Get_N ()
{
    int val = 0;

    val = val * 10 + (*s - '0');
    s++;

    while ('0' <= *s && *s <= '9')
    {
        val = val * 10 + (*s - '0');
        s++;
    }

//    node_t *node = Create_Node (itoa (val), "int");
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
