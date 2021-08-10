#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tree.h"


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
	/*go to the right side of the current record*/ 
	if ((current_record->data->point)[cc] <= (new_record->data->point)[cc]
	&& (current_record->data->point)[lc] != (new_record->data->point)[lc]){
		current_record->right = recursive_insert(current_record->right, new_record, depth + 1);
	}
	/*if the record inserted has exactly the same location with the current record*/ 
	else if ((current_record->data->point)[0] == (new_record->data->point)[0] &&
	(current_record->data->point)[1] == (new_record->data->point)[1]){
		if (!(current_record->next)){
			current_record->next = new_record; 
			current_record->similarLastOne = new_record; 
		}
		else{
			current_record->similarLastOne->next = new_record; 
			current_record->similarLastOne = new_record; 
			
		}
	}
	/*go to the left side of the current record*/ 
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

void recursive_free_tree(record_t *current_record){
	if (!current_record){
		return; 
	}
	/*free children first*/
	recursive_free_tree(current_record->left); 
	recursive_free_tree(current_record->right);  
	/*finally, free its parent*/ 
	free_record(current_record); 
}

void free_tree(tree_t *tree){
	assert(tree != NULL); 
	recursive_free_tree(tree->current_record);
	free(tree);
}

void free_record(record_t *current_record){
    record_t *prev_record; 
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

void find_the_closest_point(record_t *current_record, closest_t* closest,double loc[], int depth){
	if(!current_record){
		return;  
	}
	double distance; 
	double boundary[DIMENSIONS]; 
	
	/*in case this is the first time u search*/
	if (!closest-> closest_record ){
		closest->closest_record = current_record;
		closest->distance = calculate_distance(current_record->data->point, loc);
	}
	else{
		distance = calculate_distance(current_record->data->point, loc); 
		if (distance < closest->distance){
			closest->closest_record = current_record; 
			closest->distance = distance; 
		}
	}
	/*identify which coordinate needs to be compared*/
	/*compare x coordinate*/ 
    if(depth % 2){
        boundary[0] = loc[0]; 
        boundary[1] = current_record->data->point[1]; 
		/*go to the left side first*/ 
		if (boundary[1] > loc[1]){
			find_the_closest_point(current_record->left, closest, loc, depth+1);
			if (calculate_distance(boundary, loc) < closest->distance){
				find_the_closest_point(current_record->right, closest, loc, depth+1); 
			}
		}
		/*go to the right side first*/ 
		else{
			find_the_closest_point(current_record->right, closest, loc, depth + 1); 
			if (calculate_distance(boundary, loc) < closest->distance){
				find_the_closest_point(current_record->left, closest, loc, depth+1); 
			}
		}
    }
	/*compare y coordinate*/ 
    else{
        boundary[0] = current_record->data->point[0]; 
        boundary[1] = loc[1]; 
		/*go to the left side first*/
		if (boundary[0] > loc[0]){
			find_the_closest_point(current_record->left, closest, loc, depth+1);
			if (calculate_distance(boundary, loc) < closest->distance){
				find_the_closest_point(current_record->right, closest, loc, depth+1); 
			}
		}
		/*go to the right side first*/
		else{
			find_the_closest_point(current_record->right, closest, loc, depth + 1);
			 if (calculate_distance(boundary, loc) < closest->distance){
				find_the_closest_point(current_record->left, closest, loc, depth+1); 
			}	
		}
	}
	/*update the number of records we have visited*/ 
	closest->count += 1; 
}
