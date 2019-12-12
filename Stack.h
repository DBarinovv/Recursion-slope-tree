typedef int elem_t;

struct stack_t
{
    int saver_left;

    const char *name;

    long int hash;

    int size;
    int capacity;

    elem_t arr[1];
};

//=============================================================================

enum errors {Size_less_0 = 1000,
             Size_more_Max_Size,
             Invalid_array,
             Try_to_change_array_after_destruct,
             Try_to_change_size_after_destruct,
             Push_out_of_range,
             Pop_out_of_range,
             Saver_left_is_broken,
             Saver_right_is_broken,
             Control_sum_changed
             };

//-----------------------------------------------------------------------------

// d - dark
enum colors {D_blue = 1, D_green, D_cyan, D_red, D_purple, D_yellow, D_white, Grey,
               Blue,       Green,   Cyan,   Red,   Purple,   Yellow,   White};

//=============================================================================

const elem_t C_poison_stack = -8;                                                                                                                                                                                                                      const elem_t C_secret = 179;
const int C_start_capacity = 40;

const int      C_error_size     = -10;
const stack_t* C_error_null_ptr = (stack_t *)5;

const int C_saver_left     = 0xDED;
const elem_t C_saver_right = 0xDED;

//-----------------------------------------------------------------------------

#define STACK_CONSTRUCTOR(elem)                                                                                       \
{                                                                                                                     \
    elem = (stack_t *) ((char *)calloc (sizeof (stack_t) + (C_start_capacity) * sizeof (elem_t), sizeof(char)));      \
    Stack_Constructor (elem);                                                                                         \
                                                                                                                      \
    (*elem).name = #elem;                                                                                             \
}

//=============================================================================

bool Stack_Constructor (stack_t* this_);

bool Stack_Destruct    (stack_t* this_);

bool Stack_Realloc     (stack_t* this_);

//-----------------------------------------------------------------------------

bool Stack_Push  (stack_t* this_, elem_t elem);

bool Stack_Pop   (stack_t* this_, elem_t *elem);

bool Stack_Empty (const stack_t* const this_);

elem_t Stack_Top (const stack_t* const this_);

int Stack_Size   (const stack_t* const this_);

bool Stack_Full  (const stack_t* const this_);

//=============================================================================

bool Stack_Constructor (stack_t* this_)
{
    (this_ -> hash) = 0;
    (this_ -> size) = 0;

    (this_ -> capacity)   = C_start_capacity;
    (this_ -> saver_left) = C_saver_left;

    for (int i = 0; i <= (this_ -> capacity) - 1; i++)
    {
        (this_ -> arr) [i] = C_poison_stack;
    }

    (this_ -> arr) [(this_ -> capacity)] = C_saver_right;

    return true;
}
//=============================================================================

bool Stack_Destruct (stack_t* this_)
{
    for (int i = 0; i <= (this_ -> capacity); i++)
    {
        (this_ -> arr) [i] = C_poison_stack;
    }

    this_ = (stack_t *)C_error_null_ptr;
    free (this_);

    (this_ -> size) = C_error_size;

    return true;
}

//=============================================================================

bool Stack_Realloc (stack_t* this_)
{
    (this_ -> capacity) = (this_ -> capacity) * 2;

    this_ = (stack_t *) realloc (this_, sizeof (stack_t) + (this_ -> capacity + 1) * sizeof (elem_t));

    if (this_ == nullptr)
    {
        return false;
    }

    for (int i = (this_ -> size); i < (this_ -> capacity); i++)
    {
        (this_ -> arr) [i] = C_poison_stack;
    }

    (this_ -> arr) [(this_ -> capacity)] = C_saver_right;

    return true;
}

//=============================================================================

bool Stack_Push (stack_t* this_, elem_t elem)
{
    if ((this_ -> size) >= (this_ -> capacity))
    {
        if (!Stack_Realloc (this_))
        {
      //      PR_B(NO MORE MEMORY\n, Red)
      //      Assert_Ok (this_);
            return false;
        }
    }

    (this_ -> arr) [(this_ -> size)++] = elem;

    return true;
}

//=============================================================================

bool Stack_Pop (stack_t* this_, elem_t *elem)
{
    if (Stack_Empty (this_))
    {
        printf ("LOLLUL\n");
    }

    *elem = (this_ -> arr) [--(this_ -> size)];
    (this_ -> arr) [(this_ -> size)] = C_poison_stack;

    return true;
}

//=============================================================================

bool Stack_Empty (const stack_t* const this_)
{
    return ((this_ -> size) <= 0);
}

//=============================================================================

elem_t Stack_Top (const stack_t* const this_)
{
    if (Stack_Empty(this_))
    {
        printf ("\nStack is empty! There is no top element!\n");
        printf ("So, we give you ");
        printf ("!\n");
        return C_poison_stack;
    }

    return (this_ -> arr)[(this_ -> size) - 1];
}

//=============================================================================

int Stack_Size (const stack_t* const this_)
{
    return (this_ -> size);
}

//=============================================================================

bool Stack_Full (const stack_t* const this_)
{
    return ((this_ -> size) == (this_ -> capacity));
}
