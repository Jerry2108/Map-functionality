#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED
#define DIMENSIONS 2
typedef struct record record_t; 
typedef struct{
    char *census_year;
    char *block_id;
    char *property_id;
    char *base_id;
    char *area;
    char *trading;
    char *industry_code;
    char *industry_des;
    char *x;
    char *y;
    char *loc;
	double point[DIMENSIONS]; 
}data_t;
struct record{
	data_t *data; 
	record_t *nextPointWithinR; 
	record_t *next; 
	record_t *similarLastOne; 
	record_t *left; 
	record_t *right; 
};
typedef struct {
    record_t *first_record;
    record_t *last_record;
}list_t;

list_t * make_empty_list();
list_t *insert_at_foot(list_t *list, record_t *new_record);
void print_linked_list(list_t* list); 
#endif // LIST_H_INCLUDED
