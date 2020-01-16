#ifndef __MLD__
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAX_STRUCT_NAME_SIZE 128                  
#define MAX_FIELD_NAME_SIZE 128

/* This enumeration is used to identify
* the type of a field in a structure
*/ 
typedef enum{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,    /* pointer to object of any struct type */ 
    FLOAT,
    DOUBLE,
    OBJ_STRUCT, /* any structure */
}data_type_t;

#define GET_FIELD_OFFSET(struct_name, field_name) \
        (unsigned int)(&(((struct_name*)0)->field_name))
    
#define GET_FIELD_SIZE(struct_name, field_name) \
        sizeof(((struct_name*)0)->field_name)

typedef struct _struct_db_rec_t_ struct_db_rec_t;

/* structure to store information of one field of a
* C structure
*/
typedef struct _field_info_{
    char fname[MAX_FIELD_NAME_SIZE];
    data_type_t dtype;
    unsigned int size;
    unsigned int offset;
    /* This field is meaningful if dtype is OBJ_PTR or OBJ_STRUCT*/
    char nested_struct_name[MAX_STRUCT_NAME_SIZE];
}field_info_t;

/* Structure to store the information of an C structure
*  which could have n_fields
*/

struct _struct_db_rec_t_{
    struct_db_rec_t *next;
    char struct_name[MAX_STRUCT_NAME_SIZE];
    unsigned int struct_size;
    unsigned int n_fields;
    /* array of n_fields elements to store info of each field */ 
    field_info_t *fields_arr;        
};

/* Finally the head of linked lists representing
* the structure database
*/

typedef struct _struct_db_{
    struct_db_rec_t *head;
    unsigned int count;
}struct_db_t;

/* Printing functions */
void print_struct_rec(struct_db_rec_t *struct_rec);
void print_struct_db(struct_db_t *struct_db);

/* Function to add a structure record in structure database.
*  Returns 0 on success, -1 on failure.
*/
int add_structure_to_struct_db(struct_db_t *struct_db,
                               struct_db_rec_t *struct_rec);

/* API's for structure registration */
/* MACRO to fill field info in field_info_t struct */
#define FILL_FIELD_INFO(struct_name, field_name, dtype, nested_struct_name)\
        {#field_name, dtype, GET_FIELD_SIZE(struct_name, field_name),\
        GET_FIELD_OFFSET(struct_name, field_name), #nested_struct_name}

/*MACRO to register structure*/
#define REG_STRUCT(struct_db, st_name, fields)\
        do{   \
            struct_db_rec_t *rec = (struct_db_rec_t*)calloc(1, sizeof(struct_db_rec_t));\
            strncpy(rec->struct_name, #st_name, MAX_STRUCT_NAME_SIZE);\
            rec->struct_size = sizeof(st_name);\
            rec->n_fields = sizeof(fields)/sizeof(field_info_t);\
            rec->fields_arr = fields;    \
            if(add_structure_to_struct_db(struct_db, rec)){\
                assert(0);  \
            }     \
        }while(0);

        
/* Object Database structure definitions*/

typedef struct _object_db_rec_ object_db_rec_t;

struct _object_db_rec_{
    object_db_rec_t *next;
    void *ptr;
    unsigned int units;
    struct_db_rec_t *struct_rec;
};

typedef struct _object_db_{
    struct_db_t *struct_db;
    object_db_rec_t *head;
    unsigned int count;
}object_db_t;

/*Dumping Function*/
void print_object_rec(object_db_rec_t *obj_rec, int i);
void print_object_db(object_db_t *object_db);

/* Function to dynamically allocate memory to objects.
* Besides allocating memory, xcalloc() will also perform
* following tasks.
* 1. Create the object record for new allocated objects,
*    and add the object record in object database.
* 2. Link the object record with structure record for 
*    structure "struct_name".
* 3. Return the pointer to allocated memory.    
*/
void* xcalloc(object_db_t *object_db, char* struct_name, int units);

#endif