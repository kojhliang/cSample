
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const int CODE_SUCCESS=0;
const int CODE_FAILED=-1;
char* MSG_SUCCESS="Success";
char* MSG_FAILED="Failed";
char* ERROR_MEMORY_ALLOCATE="Error:Memory allocation error!";

void checkMemroyAllocate(void* p){
    if(p==NULL){
      printf("%s\n",ERROR_MEMORY_ALLOCATE);
      exit(-1);
    }
}

// return CODE_FAILED means  overflow,return CODE_SUCCESS means  no overflow
int checkInt32OverFlow(char* num){
    char int_max[31];
    char int_min[31];
     snprintf(int_max, 31, "%d", INT_MAX);
     snprintf(int_min, 31, "%d", INT_MIN);
     if (num[0] == '-') {
            if (strlen(num) < strlen(int_min)) {
                return CODE_SUCCESS;
            }   
            for (int i = 1; num[i]; ++i) {
                if (num[i] < int_min[i]) {
                    return CODE_SUCCESS;
                } else if (num[i] > int_min[i]) {
                    return CODE_FAILED;
                }   
            }   
            return CODE_SUCCESS;
        } else {
            if (strlen(num) < strlen(int_max)) {
                return CODE_SUCCESS;
            }   
            for (int i = 0; num[i]; ++i) {
                if (num[i] < int_max[i]) {
                    return CODE_SUCCESS;
                } else if (num[i] > int_max[i]) {
                    return CODE_FAILED;
                }   
            }   
            return CODE_SUCCESS;
        }   
}