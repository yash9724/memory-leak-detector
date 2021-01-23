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
    // mld_init_primitive_datatypes_support(struct_db);

    /* create structure record for structure emp_t and student_t */
    static field_info_t emp_fields[] = {

                    FILL_FIELD_INFO(emp_t, emp_name, CHAR, 0),
                    FILL_FIELD_INFO(emp_t, emp_id, UINT32, 0),
                    FILL_FIELD_INFO(emp_t, age, UINT32, 0),
                    FILL_FIELD_INFO(emp_t, mgr, OBJ_PTR, emp_t),
                    FILL_FIELD_INFO(emp_t, salary, FLOAT, 0)
                };

    /* register the structure in structure database*/            

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

    
    /*Working with object database*/
    /*Initialize a new Object database */
    printf("\nWorking with object database");
    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    printf("\nobject_db = %p   and    struct_db = %p",object_db,struct_db);
    printf("\nMID");
    object_db -> struct_db = struct_db;
    printf("\nobject database initialized");
    
    /*Step 2 : Create some sample objects, equivalent to standard 
     * calloc(1, sizeof(student_t))
    */
    student_t *yash = xcalloc(object_db, "student_t", 1);
    mld_set_dynamic_object_as_root(object_db, yash);
    student_t *aashu = xcalloc(object_db, "student_t", 1);
    strncpy(aashu->stud_name, "aashu", strlen("aashu"));
    yash->best_colleague = aashu;
    emp_t *neeshu = xcalloc(object_db, "emp_t", 2);
    mld_set_dynamic_object_as_root(object_db, neeshu);
    

    print_obj_db(object_db);

    run_mld_algorithm(object_db);
    printf("Leaked Objects: \n");
    report_leaked_objects(object_db);

    return 0;
}