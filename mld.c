#include "mld.h"


char *DTYPE[] = { "UINT8", "UINT32", "INT32", "CHAR", "OBJ_PTR","FLOAT", "DOUBLE","OBJ_STRUCT"};
/* Printing functions */
void print_struct_rec(struct_db_rec_t *struct_rec){

    if(struct_rec == NULL)
        return;

    printf("Structure Name = %s\t|",struct_rec->struct_name);
    printf("\tStructure Size = %d\t|",struct_rec->struct_size);
    printf("\tNo. of Fields = %d\n",struct_rec->n_fields);
    printf("Fields = \n");
    for(int i = 0 ; i < struct_rec->n_fields ; i++){
        printf("\t\tfname = %s\t|",struct_rec->fields_arr[i].fname);
        printf("\tdtype = %s\t|",DTYPE[struct_rec->fields_arr[i].dtype]);
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
                                    struct_db_rec_t *struct_rec,
                                    mld_boolean_t is_root){

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
        obj->is_root = is_root;

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


// static void delete_object_record_from_object_db(object_db_t *object_db, 
//                                     object_db_rec_t *object_rec){

//     assert(object_rec);

//     object_db_rec_t *head = object_db->head;
//     if(head == object_rec){
//         object_db->head = object_rec->next;
//         free(object_rec);
//         return;
//     }
    
//     object_db_rec_t *prev = head;
//     head = head->next;

//     while(head){
//         if(head != object_rec){
//             prev = head;
//             head = head->next;
//             continue;
//         }

//         prev->next = head->next;
//         head->next = NULL;
//         free(head);
//         return;
//     }
// }


// void xfree(object_db_t *object_db, void *ptr){

//     if(!ptr) return;
//     object_db_rec_t *object_rec = 
//         object_db_look_up(object_db, ptr);
        
//     assert(object_rec);
//     assert(object_rec->ptr);
//     free(object_rec->ptr);
//     object_rec->ptr = NULL;
//     /*Delete object record from object db*/
//     delete_object_record_from_object_db(object_db, object_rec);
// }

void* xcalloc(object_db_t *object_db,
              char *struct_name, int units){
     
    /* First check if structure struct_name exists in
    * structure database or not
    */

    struct_db_rec_t *st = struct_db_look_up(object_db->struct_db,
                                            struct_name);
    assert(st);
    void *obj = calloc(units, sizeof(struct_name));
    add_object_to_object_db(object_db, obj, units, st, MLD_FALSE); /* set objects as non-root by default*/
    return obj;
}




void print_obj_rec(object_db_rec_t *obj_rec, int i){
    
    if(!obj_rec)
        return;
    printf("%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s | is_root = %s\n", 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name, obj_rec->is_root ? "TRUE":"FALSE"); 
    
}


void print_obj_db(object_db_t *object_db){
    object_db_rec_t *curr = object_db->head;
    unsigned int i = 0;
    printf("\nPrinting OBJECT DATABASE\n");
    for(; curr; curr = curr->next){
        print_obj_rec(curr, i++);
    }
}


static struct_db_rec_t * struct_db_look_up(struct_db_t *struct_db,
                                          char *struct_name){
    struct_db_rec_t* curr = struct_db->head;
    if(curr == NULL)
        return NULL;
    while(curr){
        if(strncmp(curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}



static object_db_rec_t *object_db_look_up(object_db_t *object_db,
                                          void *ptr){
    object_db_rec_t *curr = object_db->head;
    if(curr == NULL)
        return NULL;
    while(curr){
        if(curr->ptr == ptr)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

/* API's to register root object */

/* The global object of application which is not registered 
* created using xcalloc should be registered with object
* database using below API
*/
void mld_register_global_object_as_root(object_db_t *object_db,
                                        void *objptr,
                                        char *struct_name,
                                        unsigned int units){
    /* check whether struct_name has been registered with struct db or not*/
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);

    assert(struct_rec);

    /* create a new object rec and add it to object database */
    add_object_to_object_db(object_db, objptr, units, struct_rec, MLD_TRUE);
    
}


void mld_set_dynamic_object_as_root(object_db_t *object_db,
                                   void *obj_ptr){

    object_db_rec_t *obj_rec = object_db_look_up(object_db ,obj_ptr);
    assert(obj_rec);
    obj_rec -> is_root = MLD_TRUE;
}

/* Marks all objects in object database as unvisited*/
static void init_mld_algorithm(object_db_t *object_db){

     object_db_rec_t *obj_rec = object_db->head;
     while(obj_rec){
         obj_rec->is_visited = MLD_FALSE;
         obj_rec = obj_rec->next;
     }
}

/* Get the next root object */
static object_db_rec_t * get_next_root_object(object_db_t *object_db, 
                     object_db_rec_t *starting_from_here){

    object_db_rec_t *first = starting_from_here ? starting_from_here->next : object_db->head;
    while(first){
        if(first->is_root)
            return first;
        first = first->next;
    }
    return NULL;
}

/* recursive function to explore objects 
*/
static void mld_explore_objects_recursively(object_db_t *object_db, 
                                object_db_rec_t *parent_obj_rec){

    unsigned int i , n_fields;
    char *parent_obj_ptr = NULL,
         *child_obj_offset = NULL;
    void *child_object_address = NULL;
    field_info_t *field_info = NULL;

    object_db_rec_t *child_object_rec = NULL;
    struct_db_rec_t *parent_struct_rec = parent_obj_rec->struct_rec;

    /*Parent object must have already visited*/
    assert(parent_obj_rec->is_visited);

    for( i = 0; i < parent_obj_rec->units; i++){

        parent_obj_ptr = (char *)(parent_obj_rec->ptr) + (i * parent_struct_rec->struct_size);

        for(n_fields = 0; n_fields < parent_struct_rec->n_fields; n_fields++){

            field_info = &parent_struct_rec->fields_arr[n_fields];

            /*We are only concerned with fields which are pointer to
             * other objects*/
            switch(field_info->dtype){
                case UINT8:
                case UINT32:
                case INT32:
                case CHAR:
                case FLOAT:
                case DOUBLE:
                case OBJ_STRUCT:
                    break;
                case OBJ_PTR:
                default:
                    ;

                child_obj_offset = parent_obj_ptr + field_info->offset;
                memcpy(&child_object_address, child_obj_offset, sizeof(void *));
                if(!child_object_address) continue;

                child_object_rec = object_db_look_up(object_db, child_object_address);

                assert(child_object_rec);
                if(!child_object_rec->is_visited){
                    child_object_rec->is_visited = MLD_TRUE;
                    mld_explore_objects_recursively(object_db, child_object_rec);
                }
                else{
                    continue; 
                }
            }
        }
    }
}
 

void run_mld_algorithm(object_db_t *object_db){

    init_mld_algorithm(object_db);
    object_db_rec_t *root_obj = get_next_root_object(object_db, NULL);

    while(root_obj){
        if(root_obj->is_visited){
            root_obj = get_next_root_object(object_db, root_obj);
            continue;
        }
        root_obj->is_visited = MLD_TRUE;
        mld_explore_objects_recursively(object_db, root_obj);
        root_obj = get_next_root_object(object_db, root_obj);
    } 
}
   

static void mld_dump_object_rec_detail(object_db_rec_t *obj_rec){

    int n_fields = obj_rec->struct_rec->n_fields;
    field_info_t *field = NULL;

    int units = obj_rec->units, obj_index = 0,
        field_index = 0;

    for(; obj_index < units; obj_index++){
        char *current_object_ptr = (char *)(obj_rec->ptr) + \
                        (obj_index * obj_rec->struct_rec->struct_size);

        for(field_index = 0; field_index < n_fields; field_index++){
            
            field = &obj_rec->struct_rec->fields_arr[field_index];

            switch(field->dtype){
                case UINT8:
                case INT32:
                case UINT32:
                    printf("%s[%d]->%s = %d\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(int *)(current_object_ptr + field->offset));
                    break;
                case CHAR:
                    printf("%s[%d]->%s = %s\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, (char *)(current_object_ptr + field->offset));
                    break;
                case FLOAT:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(float *)(current_object_ptr + field->offset));
                    break;
                case DOUBLE:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(double *)(current_object_ptr + field->offset));
                    break;
                case OBJ_PTR:
                    printf("%s[%d]->%s = %p\n", obj_rec->struct_rec->struct_name, obj_index, field->fname,  (void *)*(int *)(current_object_ptr + field->offset));
                    break;
                case OBJ_STRUCT:
                    /*Later*/
                    break;
                default:
                    break;
            }
        }
    }
}


void report_leaked_objects(object_db_t *object_db){

    int i = 0;
    object_db_rec_t *head;

    printf("Dumping Leaked Objects\n");

    for(head = object_db->head; head; head = head->next){
        if(!head->is_visited){
            print_obj_rec(head, i++);
            mld_dump_object_rec_detail(head);
            printf("\n\n");
        }
    }
}

/* This function will register primitive datatypes with struct_db 
* as a structure with 0 fields  
*/
// void mld_init_primitive_datatypes_support(struct_db_t* struct_db){

//     REG_STRUCT(struct_db, int, 0);
//     REG_STRUCT(struct_db, float, 0);
//     REG_STRUCT(struct_db, double, 0);
// }

