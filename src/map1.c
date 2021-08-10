#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tree.c"
#define RECORD_LENGTH 512
#define FIELD_LENGTH 128
#define NUM_FIELDS 11
#define DIMENSION 2
char** separate_fields(char *line);
data_t *fill_in_information(char** fields);
tree_t *create_a_tree(char* file_name1);
void getRidOfNewLine(char* string); 
void produce_outputFile(char* string1, char* string2, record_t*current, char* file_name2);

char** separate_fields(char* line){
    int i = 0, has_commas = 0, j = 0, c = 0;
    char tmp[FIELD_LENGTH + 1]; 
	char** fields = (char**)malloc(sizeof(char*)*(NUM_FIELDS));
	assert(fields != NULL && line != NULL); 
	
    while(j < NUM_FIELDS){
        /*the current field doesn't have commas as its character*/
        if (line[i] == ',' && !has_commas){
            /*if there is no information to read in this field anymore, close the
			temporary array and copy it to the official array*/ 
            if (c != 0){
                tmp[c] = '\0';
                c = 0;
                fields[j]= (char*)malloc(sizeof(char)*(strlen(tmp)+ 1));
                assert(fields[j] != NULL); 
                strcpy(fields[j++], tmp); 	
            }
            /*check if the next field has commas or not*/
            if (line[i+1] == '"'){
                has_commas = 1;
                /*if it has commas, skip a delimiter comma and a quotation mark following
				the comma*/
                i += 2;
            }
            else{
                i+= 1;
            }
        }
        /*there are 3 cases when the current character is a quotation mark*/ 
		else if (line[i] == '"'){
			/*the 1st case: the current field has double quotes and the field
			 has ended,*/ 
			if (line[i+1] == '"' && line[i+2] == '"') {
				tmp[c++] = line[i];
				has_commas = 0; 
				/*skip double quotes following to reach the comma separating this field
				and next field*/
				i += 3; 
			}
			/*the 2nd case: the current field has double quotes and there is still
			information in this field to read*/ 
			else if (line[i+1] == '"' && line[i+2] != '"'){
				tmp[c++] = line[i]; 
				i += 2;
			}
			/*the 3rd case: there are no double quotes in this field 
			and the current quotation marks ends the field so we need to stop reading*/
	        else{
				/*close the temporary array and copy it to the official array*/ 
				tmp[c] = '\0';
				c = 0; 
				has_commas = 0; 
				fields[j]= (char*)malloc(sizeof(char)*(strlen(tmp)+ 1));
                assert(fields[j] != NULL); 
                strcpy(fields[j++], tmp); 
				i++; 
			}
		}
        /*otherwise, keep getting character for the sample array*/ 
        else{
            tmp[c++] = line[i];
            i++;
        }
    }
	return fields;
}

/*data is get as a whole package */ 
data_t *fill_in_information(char** fields){
	data_t *information = (data_t*)malloc(sizeof(data_t)); 
	assert(information != NULL && fields != NULL); 
	
    information->census_year = fields[0];
    information->block_id = fields[1];
    information->property_id = fields[2];
    information->base_id = fields[3];
    information->area = fields[4];
    information->trading= fields[5];
    information->industry_code = fields[6];
    information->industry_des = fields[7];
    information->x = fields[8];
    information->y = fields[9];
    information->loc = fields[10];
	/*char *c; */ 
	(information->point)[0] = atof(information->x); 
	/*strtod(information->x, &c);*/
	(information->point)[1] = atof(information->y);
    /*strtod(information->y, &c); */ 
    return information;
}

/*A linked list is created based on data from the file*/ 
tree_t *create_a_tree(char* file_name1){
    FILE* input_file = fopen(file_name1, "r");
    assert(input_file != NULL); 
    char **fields;
    tree_t *records = make_empty_tree();
    char *line = NULL;  
    size_t c = 0;
	
    /*get a header from the file*/
    getline(&line, &c, input_file); 
    /*read data line by line*/ 
    while (getline(&line, &c, input_file) != -1){
        /*each field is separated by commas as delimiters*/
        fields = separate_fields(line);
		assert(fields != NULL); 
        /*information of one record is displayed as a struct*/ 
        data_t *information = fill_in_information(fields);
        /*insert new record into the kd_tree*/ 
        records = insert_in_order(records, information); 
		free(fields);
	
    }
	free(line); 

    /*close the input file*/
    fclose(input_file);
    return records;
}

/*the linked list is considered as a dictionary of records*/
/*produce the output file based on keys searched */ 
void produce_outputFile(char* string1, char* string2, record_t* current, char* file_name2){
    FILE *output_file = fopen(file_name2, "a"); 
    assert(output_file != NULL && current != NULL);
	record_t* tmp = current; 
    if (!current){
		fprintf(output_file, "%s %s -->NOTFOUND\n", string1, string2); 
		return; 
	}
	/*compares the trading name of each record consecutively with the key provided*/
    while(tmp != NULL){
		tmp = current; 
		/*copy a current record into the output file if they have a key required*/ 
		fprintf(output_file, "%s %s -->", string1, string2); 
		fprintf(output_file, "Census year: %s || Block ID: %s || Property ID: %s ",
		current->data->census_year,current->data->block_id, current->data->property_id);
		fprintf(output_file, "|| Base property ID: %s || CLUE small area: %s ",
		current->data->base_id, current->data->area);
		fprintf(output_file, "|| Industry (ANZSIC4) code: %s || Industry (ANZSIC4) description: %s ",
		current->data->industry_code, current->data->industry_des);
		fprintf(output_file, "|| x coordinate: %s || y coordinate: %s || Location: %s || \n",
		current->data->x, current->data->y, current->data->loc);
		/*move to the next record*/ 
		tmp = tmp->next; 
    }
	/*close the output file*/ 
	fclose(output_file); 
}



int main(int argc, char** argv){
    char *name1 = argv[1]; 
	char *name2 = argv[2]; 
	char *key = NULL; 
	size_t c = 0;
	double point[DIMENSION]; 
    tree_t *records =  create_a_tree(name1);
	closest_t* closest = (closest_t*)malloc(sizeof(closest_t)); 
	closest->count = 0; 
	closest->closest_record = NULL; 
	closest->distance = 0; 
	while (getline(&key, &c, stdin) != -1){
		char* x = strtok(key, " ");
		char* tmpy = strtok(NULL, ""); 
		char* y = (char*)malloc(sizeof(char)*strlen(tmpy)+1); 
		assert(y != NULL);
		strcpy(y, tmpy); 
		getRidOfNewLine(y);
		point[0] = atof(x); 
		point[1] = atof(y); 
		find_the_closest_point(records->current_record,closest, point, 0); 
		printf("%s %s -->%d\n", x, y, closest->count); 
		produce_outputFile(x,y,closest->closest_record, name2);
		free(y);
    }
	free(closest); 
	free(key);
	free_tree(records); 
	
}

void getRidOfNewLine(char* string){
	char* newString = (char*)malloc(sizeof(char) * strlen(string) + 1);
	assert(newString != NULL); 
	strcpy(newString, string); 
	int i=0;
	
	while (i<strlen(string)){
		newString[i] = string[i];
		if ((newString[i] == '\n')||(newString[i] == '\r')||(newString[i] == ' ')){
			newString[i] = '\0';
			break;
		}
		i++;
	}
	strcpy(string, newString); 
	free(newString);
}



