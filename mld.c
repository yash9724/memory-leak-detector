#include "mld.h"


char *dtype[] = { "UINT8", "UINT32", "INT32", "CHAR", "OBJ_PTR","FLOAT", "DOUBLE","OBJ_STRUCT"};
/* Printing functions */
void print_struct_rec(struct_db_rec_t *struct_rec){
    printf("Structure Name = %s\t|",struct_rec->struct_name);
    printf("\tStructure Size = %d\t|",struct_rec->struct_size);
    printf("\tNo. of Fields = %d\n",struct_rec->n_fields);
    printf("Fields = \n");
    for(int i = 0 ; i < struct_rec->n_fields ; i++){
        printf("\t\tfname = %s\t|",struct_rec->fields_arr[i].fname);
        printf("\tdtype = %s\t|",dtype[struct_rec->fields_arr[i].dtype]);
        printf("\tsize = %u\t|",struct_rec->fields_arr[i].size);
        printf("\toffset = %u\t|",struct_rec->fields_arr[i].offset);
        printf("\tnested_struct = %s\t|",struct_rec->fields_arr[i].nested_struct_name);
        printf("\n");
    }
}

void print_struct_db(struct_db_t *struct_db){
    struct_db_rec_t *curr = struct_db->head;
    if(curr == NULL){
        printf("Structure Database is empty!\n");
        return;
    }

    while(curr){
        print_struct_rec(curr);
        curr = curr->next;
    }
}

/* Function to add a structure record in structure database.
*  Returns 0 on success, -1 on failure.
*/
int add_structure_to_struct_db(struct_db_t *struct_db,
                struct_db_rec_t *struct_rec){

                struct_db_rec_t *head = struct_db->head;
                if(!head){
                    struct_db->head = struct_rec;
                    struct_rec->next = NULL;
                    struct_db->count++;
                    return 0;
                }

                struct_rec->next = head;
                struct_db->head = struct_rec;
                struct_db->count++;
                return 0;
            }

static void add_object_to_object_db(object_db_t *object_db, 
                                    void *ptr, int units,
                                    struct_db_rec_t *struct_rec){

        /* check whether object already exists.
        *  If yes, then don't add it twice.
        */
        object_db_rec_t* obj = object_db_look_up(object_db,
                                                 ptr);
        assert(!obj);

        obj = calloc(1, sizeof(object_db_rec_t));
        obj->next = NULL;
        obj->ptr = ptr;
        obj->units = units;
        obj->struct_rec = struct_rec;

        object_db_rec_t* curr = object_db->head;
        
        if(!curr){
            object_db->head = obj;
            object_db->count++;
            return;
        }

        obj->next = object_db->head;
        object_db->head = obj;
        object_db->count++;
    
    }


void* xcalloc(object_db_t *object_db,
              char *struct_name, int units){
     
    /* First check if structure struct_name exists in
    * structure database or not
    */

    struct_db_rec_t *st = struct_db_look_up(object_db->struct_db,
                                            struct_name);
    assert(st);
    void *obj = calloc(units, sizeof(struct_name));
    add_object_to_object_db(object_db, obj, units, st);
    return obj;
}

void print_obj_rec(object_rec_db_t *obj_rec, int i){
    

}

void print_object_db(object_db_t *object_db){

}


static struct_db_rec_t * struct_db_look_up(struct_db_t *struct_db,
                                          char *struct_name){

        struct_db_rec_t* curr = struct_db->head;
        while(curr){
            if(strncmp(curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0)
                return curr;
        }
        return NULL;
    }



static object_db_rec_t *object_db_look_up(object_db_t *object_db,
                                          void *ptr){

        object_db_rec_t *curr = object_db->db;
        while(curr){
            if(curr->ptr == ptr)
                return curr;
        }
        return NULL;
    }

                               