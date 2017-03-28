void *
clean_realloc(void *ptr, size_t old_size, size_t new_size);

char *
strnadd(char *restrict s0, char *restrict s1, char *restrict s2, size_t limit);

int 
get_offset_in_array(void *pointer_in_array, void *array, size_t item_size);
