#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"

void set_init(struct set * st)
{
    st->first = NULL;
}

struct node * set_first(struct set * st)
{
    return st->first;
}

void set_add(struct set * st, const char * data)
{
    if (st->first == NULL)
    {
        // create and store first element
        st->first = malloc(sizeof(struct node));

        st->first->data = strdup(data);
        st->first->next = NULL;
    }
    else
    {
        // iterate over set to check for duplicates
        struct node * ptr = st->first;

        if (strcmp(ptr->data, data) == 0)
            return;

        while (ptr->next != NULL)
        {
            ptr = ptr->next;

            if (strcmp(ptr->data, data) == 0)
                return;
        }

        // if not returned, create and store the new element
        ptr->next = malloc(sizeof(struct node));

        ptr->next->data = strdup(data);
        ptr->next->next = NULL;
    }
}

const char * set_data(struct node * node)
{
    return node->data;
}

struct node * set_next(struct node * node)
{
    return node->next;
}

void set_free(struct set * st)
{
    // iterate over the whole list and free all of the structures
    while (st->first != NULL)
    {
        struct node * next = st->first->next;
        free(st->first->data);
        free(st->first);
        st->first = next;
    }
}
