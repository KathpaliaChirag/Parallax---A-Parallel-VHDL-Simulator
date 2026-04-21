#ifndef UTILS_H
#define UTILS_H

// CHIRAG : 08-03-26 (01:05)
// so i actually started with building a dynamic array type using a marcos idea however i was a little confused how to do that
// basically here we make a struct ....the marcos is supposed to be in a single line so we use \ to hint next line is same 
// then there is use of T which gets replaced everywhere ...its done by preprocessor before compilation
// its like passing a value you pass int and it becomes int *data
// lastly there is ## which like sticks and makes DynArray_int if t was int
#define DYNARRAY_TYPE(T) \
typedef struct { \
    T *data; \
    int size; \
    int capacity; \
} DynArray_##T;

// next up i am making a sorta function but as a marco which will initialise it and i plan to start it from null so size 0 cap 0 
// and whenever cap == size we do some capacity increase and use a realloc to sorta double size
// marco does same thing as inline ....where it simply replaces before compilation
#define DYNARRAY_INIT(arr) \
{ \
    arr.data = NULL; \
    arr.size = 0; \
    arr.capacity = 0; \
}
// CHIRAG : 08-03-26 (14:38) :
// okay so now i need next is insertion and size realloc.... thats next goal
#define DYNARRAY_GROW(arr) \
{ \
    if(arr.capacity == 0 ) \
    {\
        arr.capacity =1;\
        arr.data = realloc(arr.data, arr.capacity * sizeof(*arr.data)); \
    }\
    else \
    { \
        arr.capacity*=2; \
        arr.data = realloc(arr.data, arr.capacity * sizeof(*arr.data)); \
    }\
}
#define DYNARRAY_INSERT(arr, val) \
{ \
    if(arr.size == arr.capacity) \
    { \
    DYNARRAY_GROW(arr); \
    } \
    arr.data[arr.size] = val; \
    arr.size++; \
}

#endif