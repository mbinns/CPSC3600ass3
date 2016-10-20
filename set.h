#ifndef SET_H
#define SET_H
struct node {
	char * data;
	struct node * next;
};

struct set {
	struct node * first;
};

// The set_init routine initializes the given set structure.
void set_init(struct set * st);
// The set_add routine adds the given data if it is not already present.
void set_add(struct set * st, const char * data);
// The set_data routine gets the data at the given node.
const char * set_data(struct node * node);
// The set_first routine gets the first item in the list for iteration.
struct node * set_first(struct set * st);
// The set_next routine gets the next item after the given.
struct node * set_next(struct node * node);
// The set_free routine frees all of the data in the structure.
void set_free(struct set * st);
#endif
