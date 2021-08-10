#include "list2.h" 
typedef struct{
	record_t *current_record; 
}tree_t; 
typedef struct{
	record_t *closest_record; 
	double distance;
	int count; 
	
}closest_t; 

tree_t *make_empty_tree();
record_t *recursive_insert(record_t *current_record, record_t *new_record,int depth); 
tree_t *insert_in_order(tree_t *tree, data_t *information);
double calculate_distance(double* p1, double* p2);
void free_tree(tree_t *tree);
void free_record(record_t *current_record);
void recursive_free_tree(record_t *current_record, int*free_tree); 
void radius_search(record_t *current_record, double key[], list_t *list, int depth, int *count);