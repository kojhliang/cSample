#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*
 * definitions  and implement of Array  --begin
 */
static  char* ERROR_INSERT_OVERFLOW="Error:Insert element error,because the number of elements exceeds the maximum ";
static  char* ERROR_DELETE_OVERFLOW="Error:Delete element error,because the array is already empty!";

typedef struct Array {
    int len; //当前数组的元素个数
    int element_pointer_len; //当前数组分配的存储元素指针的长度
    char **elements; //存储数组具体内容，包括元素指针和元素本身的字符串
    int max_size; //数组的最多允许占用的空间（字节）
    int use_size; //数组的现在已经使用空间（字节）
    int k;      //数组元素指针每次调整的步宽，初始化数组时，默认分配k个元素指针
}StringArray;

int Array_InsertElement(StringArray* array,char* str){     
     int reallocFlag=(array->len>0) && (((array->len+1)%array->k)==0);
     if((reallocFlag==1 && strlen(str) + 1 + array->use_size + array->k*sizeof(char*) > array->max_size)||
        (reallocFlag==0 && strlen(str) + 1 + array->use_size > array->max_size)){
             printf("Error:Insert element error,space reaches maximum(%d) limit!\n",array->max_size);
             return CODE_FAILED;
         }

     //In this case, the element pointer space needs to be reassigned
     if(reallocFlag==1){
        array->elements=(char**)realloc(array->elements,sizeof(char**) * (array->len + array->k));
        checkMemroyAllocate(array->elements);
        array->element_pointer_len=array->element_pointer_len + array->k;
        array->use_size=array->use_size + array->k*sizeof(char*);
     }

     array->elements[array->len]=(char*)malloc(strlen(str)+1);
     checkMemroyAllocate(array->elements[array->len]);
     strcpy(array->elements[array->len],str);
     array->len=array->len+1;
     array->use_size=array->use_size + strlen(str)+1;
     return CODE_SUCCESS;
}

int Array_DeleteElement(StringArray* array){
    if(array->len-1==0){
        printf("%s \n",ERROR_DELETE_OVERFLOW);
        return CODE_FAILED;
    }

    int reallocFlag=(array->len>0) && ((array->len-1)%array->k==0) 
                     && (array->element_pointer_len - (array->len-1)==(2*array->k));
     //In this case, the element pointer space needs to be reassigned
     if(reallocFlag==1){
      array->elements=(char**)realloc(array->elements,sizeof(char**) * (array->len - array->k));
      checkMemroyAllocate(array->elements);
      array->element_pointer_len=array->element_pointer_len - array->k;      
      array->use_size=array->use_size - array->k*sizeof(char*);
     } 

     array->use_size=array->use_size - strlen(array->elements[array->len-1]) - 1;              
     free(array->elements[array->len-1]);
     array->elements[array->len-1]=NULL;
     array->len=array->len-1;    
     return CODE_SUCCESS;
}

int Array_IsEmpty(StringArray* array){
    if(array->len==0){
        return CODE_SUCCESS;
    }else{
        return CODE_FAILED;
    }
}

// element num * element size 
StringArray* initArray(StringArray *array,int max_size,int k){
    array->len=0;
    
    array->k=k;
    array->use_size=sizeof(char**) * k;
    array->max_size=max_size;
    array->elements=(char**)malloc(sizeof(char**) * k);
    checkMemroyAllocate(array->elements);
    array->element_pointer_len=k;
    // for(int i=0;i<max_element;i++){
    //      array->elements[i]=(char*)malloc(sizeof(char*) * element_size);
    //      checkMemroyAllocate(array->elements[i]);
    // }
    return array;
}

void deleteArray(StringArray* array){
     for(int i=0;i<array->len;i++){
         free(array->elements[i]);
         array->elements[i]=NULL;
     }
     free(array->elements);
     array->elements=NULL;
}

/*
 * definitions  and implement of Array  --end
 */