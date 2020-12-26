typedef struct Type {
    char name[32];
    enum { OBJECT, ARRAY, STRING, NUMBER, BOOL, VNULL } category;
    union {
        struct Array *array;
        struct FieldList *object;
        struct String *string;
        struct Number *number;
        struct Bool *bool;
        struct Vnull *vnull;
    };
} Type;

typedef struct String {
    int len;
    char *first_char;
} String;

typedef struct Number {
    double v;
} Number;

typedef struct Bool {
    enum { true, false } v;
} Bool;

typedef struct Vnull {
} Vnull;

typedef struct Array {
    struct Type *base;
    int size;
} Array;

typedef struct FieldList {
    char name[32];
    struct Type *type;
    struct FieldList *next;
} FieldList;
