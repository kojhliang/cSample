#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "array.h"

/*
 功能描述：
  实现了多参数整数相加和多字符串拼接功能。
  对于add函数，实现了整数相加，并且会对输入数值和相加结果进行数值溢出检查。
  对于concat函数，实现了多个字符串的拼接，最终拼接的字符串长度限定在预定义的最大长度里。
 */

static const struct option long_option[]={
   {"func_name",optional_argument,NULL,'f'},
   {"func_args",optional_argument,NULL,'a'},
   {"help",optional_argument,NULL,'h'},
   {NULL,0,NULL,'0'}
};

//Return array config
static const int MAX_RETURN_ARRAY_SIZE=4096; //MAX_RETURN_ARRAY_SIZE must larger than MAX_FUNC_PARAMS_ARRAY_SIZE
static const int RETURN_ARRAY_K=5;  
//function params config
static const int MAX_FUNC_PARAMS_ARRAY_SIZE=1024; //参数数组所有参数最大总长度大小。例子：如果输入的参数为a,bcdde, 则至少需要5*8+6+2=48个字节;
static const int  FUNC_PARAMS_ARRAY_K=5; 
//print config
static const int MAX_PRINT_SIZE=5120; 

//Error msg
static  char* ERROR_NUMERIC_OVERFLOW="Error:Numeric overflow!";
static  char* ERROR_STRING_OVERFLOW="Error:String overflow!";

/*
 * definitions of Result  --begin
 */
typedef struct Result{
     int code;
     char *msg;
     StringArray* result_array;
}ResultSet;
/*
 * definitions of Result --end
 */

/*
 * User-defined function areas --begin
 */
signed int safeAdd(signed int si_a,signed int si_b, int *error)
 { 
    signed int sum;
    if(((si_b > 0) && (si_a > (INT_MAX - si_b))) ||
        ((si_b < 0) && (si_a < (INT_MIN - si_b))))
    {
        /* Handle error */
        //printf("%s The INT_MAX is %d,the INT_MIN is %d .\n",ERROR_NUMERIC_OVERFLOW,INT_MAX ,INT_MIN);
        *error=CODE_FAILED;
        return 0;
    }
   return sum = si_a + si_b;
 }

void  add(StringArray* func_params_array, ResultSet *result){
   char** func_params=func_params_array->elements;
   result->code=CODE_SUCCESS;
   result->msg=MSG_SUCCESS;

   //Calculate the result
   int sum=0;
   for(int i=0;i<func_params_array->len;i++){
        int tmp=0;
       //check the input number for overflow
       int error=checkInt32OverFlow(func_params[i]);
       if(error==CODE_SUCCESS){
          sscanf(func_params[i],"%d",&tmp);
          sum=safeAdd(sum,tmp,&error);
       }
       if(error==CODE_FAILED){
           printf("%s The INT_MAX is %d,the INT_MIN is %d .\n",ERROR_NUMERIC_OVERFLOW,INT_MAX ,INT_MIN);
           result->code=CODE_FAILED;
           result->msg=MSG_FAILED;
           Array_InsertElement(result->result_array,ERROR_NUMERIC_OVERFLOW);
           return;
       }
   }

   char sum_str[12]; //int类型32位最大值为2147483647,最小值为-2147483648，因此最多11位,加上结束符\0，即12位
   sprintf(sum_str,"%d",sum);
   int error=Array_InsertElement(result->result_array,sum_str);
   if(error==CODE_FAILED){
       result->code=CODE_FAILED;
       result->msg=MSG_FAILED;
   }
   //StringArray* p=result->result_array;
   //printf("p->elements[result->result_array->len]: %s \n",p->elements[p->len]);
}

 void concat(StringArray* func_params_array,struct Result* result){
     char** func_params=func_params_array->elements;
    result->code=0;
    result->msg="Success";
    char *concat_str=(char*)malloc(func_params_array->use_size- sizeof(char*)*func_params_array->element_pointer_len);
    checkMemroyAllocate(concat_str);
    for(int i=0;i<func_params_array->len;i++){
        strcat(concat_str,func_params[i]);
    }
    int error=Array_InsertElement(result->result_array,concat_str);  
    if(error==CODE_FAILED){
       result->code=CODE_FAILED;
       result->msg=MSG_FAILED;
    }
    free(concat_str);
    concat_str=NULL;
//  StringArray* p=result->result_array;
//  printf("p->elements[result->result_array->len]: %s \n",p->elements[p->len]);
}
//User-defined function areas --end


/*
 * User-defined function map  --begin
 */
typedef void (*FUNC)(StringArray* func_params_array,struct Result *result );
struct Func_Map{
     char* func_name;
     FUNC fun;
};
static const struct Func_Map func_map[]={
     {"add",add},
     {"concat",concat},
 };
 //User-defined function map --end


void print_result( ResultSet* result){
     char *str=(char *)malloc(MAX_PRINT_SIZE); 
     checkMemroyAllocate(str);
    if(sizeof(result)>0 && sizeof(result->result_array[0])>0){ 
        strcat(str,"["); 
     for(int i=0;i<result->result_array->len;i++){
        int memNum=sizeof(i)+strlen(result->result_array->elements[i])+10;
       //printf("strlen(s): %lu byte, tolal memNum: %d byte\n",strlen((*result).res[0]),memNum);
        char *tmp=(char* )malloc(memNum);  
         checkMemroyAllocate(tmp);
        if(i==result->result_array->len-1){          
           sprintf(tmp,"{\"%d\":\"%s\"}]",i,result->result_array->elements[i]);
           //printf("tmp: %s\n",tmp);
           strcat(str,tmp);
           free(tmp);
           tmp=NULL;
        }else{
           sprintf(tmp,"{\"%d\":\"%s\"},",i,result->result_array->elements[i]);
           strcat(str,tmp);          
        }       
    }
  }
  printf("Result:{\"code:\"\"%d\",\"msg\":\"%s\",\"res\":%s}\n",(*result).code,(*result).msg,str);
  free(str);
  str=NULL;
}

int main(int argc,char *argv[])
{
   char *func_name;
   //Initializes an function params  Array (10(Element Number) * 32(Element size))
   StringArray params={0};
   StringArray *func_params_array=&params;
   initArray(func_params_array,MAX_FUNC_PARAMS_ARRAY_SIZE,FUNC_PARAMS_ARRAY_K); 
   //Initializes an ReturnSet Array (10(Element Number) * 20(Element size))
   ResultSet r={0};
   ResultSet *res=&r;
   StringArray array;
   res->result_array=&array;
   initArray(res->result_array,MAX_RETURN_ARRAY_SIZE,RETURN_ARRAY_K);

    if(argc==1){
      printf("Test is a test app for contract invoke.\n\
            Usage: \n\
                   test  -f 'func_name'  -a 'func_args' \n\
            Note: The number of function parameters should be less than 20.\n\
            Example1:test  -f add  -a 1,2,3 \n\
            Example2:test  -f concat  -a  func,name,ok \n");   
      return 0;
    }
    int opt=0;

    //Parse the function name and parameters
    while((opt=getopt_long(argc,argv,"f:a:h",long_option,NULL))!=-1)
    {
        char*  p;
        char* delim=",";
        switch(opt)
        {
            case 'f':
            func_name=optarg;
            printf("func_name:%s \n",optarg);break;

            case 'a':   
            printf("func_args:%s \n",optarg);  
             p=strtok(optarg,delim);
             while(p!=NULL){               
                 //printf("strlen(p) %lu\n",strlen(p));
                 int error=Array_InsertElement(func_params_array,p);
                 if(error==CODE_FAILED){
                      printf("%s The total length of the function parameter should be less than %d！\n",ERROR_STRING_OVERFLOW,MAX_FUNC_PARAMS_ARRAY_SIZE);
                 }
                 //Func_Param_Num++;
                //  if(Func_Param_Num>20){
                //    printf("Error:Too many function arguments,must be less than or equal to 20!\n");
                //    break;
                //  }
                 p=strtok(NULL,delim);
             }
            break;

            case 'h':
            printf("Test is a test app for contract invoke.\n\
            Usage: \n\
                   test  -f 'func_name'  -a 'func_args' \n\
            Note: The number of function parameters should be less than 20.\n\
            Example1:test  -f add  -a 1,2,3 \n\
            Example2:test  -f concat  -a  func,name,ok \n");                       
        }
    }

    //invoke the right function and pass the function parameters
    if(sizeof(func_map)>0){       
        int flag=0;
        for(int k=0;k<sizeof(func_map)/sizeof(func_map[0]);k++){
              if(strcmp(func_map[k].func_name,func_name)==0){
                   flag=1;
                   printf("Begin execute %s function ... \n",func_name);
                   func_map[k].fun(func_params_array,res);
                   printf("Execute %s function finish. \n",func_name);
              }
        }
        if(flag==0){
          printf("Error:The function %s was not found in User-defined function map!\n",func_name);
        }

        //print result
        printf("Print the result ==>\n");
        print_result(res);

        //free Result
        deleteArray(res->result_array);
        // free(res);
        // res=NULL;

    }else{
      printf("Error:User-defined function map  is empty!\n");
    }
}