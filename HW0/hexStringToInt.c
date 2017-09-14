#include <stdio.h>

void main(){
   char* str = "8BC";
   int intVal = 0;
   int i = 0;
   
   while(1){
   	 // printf("%c", str[i]);
   	 switch(str[i]){
   	 	case 'A':
   	 		intVal += 10; 
   	 		break;
   	 	case 'B':
   	 		intVal += 11;	
   	 		break;
   	 	case 'C':
   	 		intVal += 12;	   	 		
   	 		break;	
   	 	case 'D':
   	 		intVal += 13;    	 		
   	 		break;
   	 	case 'E':
   	 		intVal += 14;	   	 		
   	 		break;
   	 	case 'F':
   	 		intVal += 15;	   	 		
   	 		break;		
   	 	default:
   	 		intVal += (int)str[i] - 48;	
   	 		
   	 }

   	 
   	 i++;
   	 if(str[i] == '\0'){
   	 	break;
   	 }
   	 intVal = intVal << 4;
   }

   printf("Output %d\n", intVal);
   

}