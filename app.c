#include<stdio.h>
#include<stdlib.h>
#include "mld.h"

typedef struct emp_{
    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_ *mgr;
    float salary;
}emp_t;

typedef struct student_{
    char stud_name[32];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_ *best_colleague;
}student_t;

int main(){
    /* Step 1: Initialize a new structure database */
    struct_db_t *struct_db = calloc(1,sizeof(struct_db_t));
    

    /* create structure record for structure emp_t and student_t */
    static field_info_t emp_fields[] = {

                    FILL_FIELD_INFO(emp_t, emp_name, CHAR, 0),
                    FILL_FIELD_INFO(emp_t, emp_id, UINT32, 0),
                    FILL_FIELD_INFO(emp_t, age, UINT32, 0),
                    FILL_FIELD_INFO(emp_t, mgr, OBJ_PTR, emp_t),
                    FILL_FIELD_INFO(emp_t, salary, FLOAT, 0)
                };

    REG_STRUCT(struct_db, emp_t, emp_fields);

    static field_info_t stud_fields[] = {

                    FILL_FIELD_INFO(student_t, stud_name, CHAR, 0),
                    FILL_FIELD_INFO(student_t, rollno, UINT32, 0),
                    FILL_FIELD_INFO(student_t, age, UINT32, 0),
                    FILL_FIELD_INFO(student_t, aggregate, FLOAT, 0),
                    FILL_FIELD_INFO(student_t, best_colleague, OBJ_PTR, student_t)
                
                };
    
    REG_STRUCT(struct_db, student_t, stud_fields);

    print_struct_db(struct_db);

    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;

    return 0;

}