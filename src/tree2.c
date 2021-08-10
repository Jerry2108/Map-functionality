#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tree2.h"
#define DIMENSION 2

tree_t *make_empty_tree(){
	tree_t *tree = (tree_t*)malloc(sizeof(tree_t)); 
	assert(tree != NULL); 
	/*initialize a tree to be empty*/ 
	tree->current_record = NULL; 
	return tree; 
}

record_t* recursive_insert(record_t *current_record, record_t *new_record,int depth){
	if (!current_record){
		return new_record; 
	}
	/*identify which coordinate need to compared , compared coordinate(cc)*/ 
	int cc = depth % DIMENSIONS; 
	/*identify the left coordianate (lc) */
    int lc = 0; 
	if (!cc){
		lc = 1; 
	}
	/*x aligned so compare with x*/ 
	if ((current_record->data->point)[cc] <= (new_record->data->point)[cc]
	&& (current_record->data->point)[lc] != (new_record->data->point)[lc]){
		current_record->right = recursive_insert(current_record->right, new_record, depth + 1);
	}
	/*check if there has been a list available. If there is no list created, create one*/ 
	else if ((current_record->data->point)[0] == (new_record->data->point)[0] &&
	(current_record->data->point)[1] == (new_record->data->point)[1]){
		/*printf("identical\n");*/
		if (!(current_record->next)){
			current_record->next = new_record; 
			current_record->similarLastOne = new_record; 
		}
		else{
			current_record->similarLastOne->next = new_record; 
			current_record->similarLastOne = new_record; 
			
		}
		/*printf("the last one: %s\n", current_record->similarLastOne->data->trading);*/
	}
	else {
		current_record->left =  recursive_insert(current_record->left, new_record, depth + 1);
    }
	return current_record; 
}
tree_t *insert_in_order(tree_t *tree, data_t *information){
	/*make a new record*/ 
	record_t *new_record = (record_t*)malloc(sizeof(record_t)); 
	assert(tree != NULL && new_record != NULL && information != NULL); 
	new_record->data = information; 
	new_record->left = new_record->right = new_record->next = NULL; 
	/*and insert it into the tree*/ 
    tree->current_record = recursive_insert(tree->current_record, new_record, 0); 
	return tree; 
}


double calculate_distance(double* p1, double* p2){
	double distanceX = pow(p1[0] - p2[0], 2);
	double distanceY = pow(p1[1] - p2[1], 2);
	double distance = sqrt(distanceX + distanceY); 
	return distance; 
}

void recursive_free_tree(record_t *current_record, int* count_free){
	if (!current_record){
		return; 
	}
	*count_free++; 
	/*free children first*/
	recursive_free_tree(current_record->left, count_free); 
	recursive_free_tree(current_record->right, count_free);  
	/*finally, free its parent*/ 
	free_record(current_record); 
}
void free_tree(tree_t *tree){
	assert(tree != NULL); 
	int count = 0; 
	recursive_free_tree(tree->current_record, &count);
	printf("count %d\n", count); 
	free(tree);
}
/*current record is a start of a linked list*/ 
void free_record(record_t *current_record){
    record_t *prev_record; 
    /*free memory assigned to each record consecutively*/ 
	int count = 0; 
    while(current_record){
        prev_record = current_record; 
        current_record = current_record->next;
		free(prev_record->data->census_year);
        free(prev_record->data->block_id);
		free(prev_record->data->property_id);
		free(prev_record->data->base_id);
		free(prev_record->data->area);
		free(prev_record->data->trading);
		free(prev_record->data->industry_code);
		free(prev_record->data->industry_des);
		free(prev_record->data->x);
		free(prev_record->data->y);
		free(prev_record->data->loc);
		free(prev_record->data); 
		free(prev_record);
		count++;
   }
}


		
	
void
radius_search(record_t *current_record, double key[], 
				list_t *list, int depth, int *count) {
	if (!current_record){
		return; 
	}
	double point[DIMENSION]; 
	point[0] = key[0]; 
	point[1] = key[1]; 
	double targetFromKey = 0, targetFromRecord = 0; 
	double distance = calculate_distance(point, current_record->data->point);
	*count += 1; 
	if (distance < key[2]) {
		list = insert_at_foot(list, current_record);
		radius_search(current_record->left, key, list, 1+depth, count);
		radius_search(current_record->right, key, list, 1+depth, count);
	} 
	else {
		if (depth %2 == 0) {
			targetFromKey = key[0];
			targetFromRecord = current_record->data->point[0];
		} 
		else {
			targetFromKey = key[1];
			targetFromRecord = current_record->data->point[1]; 
		}
		if (targetFromKey < targetFromRecord) {
			radius_search(current_record->left, key, list, 1+depth, count);
			if (fabs(targetFromKey - targetFromRecord) < key[2]){
				radius_search(current_record->right, key, list, 1+depth, count);
			}
			
		} 
		else {
			radius_search(current_record->right, key, list, 1+depth, count);
			if (fabs(targetFromKey - targetFromRecord) < key[2]){
				radius_search(current_record->left, key, list, 1+depth, count);
			}
		}
	}

}

