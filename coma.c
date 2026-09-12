#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* single_aclist[]={
	"^",
	",",
}; int single_aclist_s=2;

char* single_transform[]={
	"cat ",
	"&&"
};

char* aclist[]={
	"?",
	"!",
}; int aclist_s=2;

char* transform[]={
	" grep -i ",
	" sed "
};


void incr(int* ij,int a){ *ij+=a; }

char* mkSubstAndMove(char c,char b,int* j){

		
	if(c==*(*(single_aclist+0))){
		incr(j,1);
		return *(single_transform+0);
	}
	if(c==*(*(single_aclist+1))){
		incr(j,1);
		return *(single_transform+1);
	}
	if(b==*(*(aclist+0))){
		incr(j,2);
		return *(transform+0);
	}
	if(b==*(*(aclist+1))){
		incr(j,2);
		return *(transform+1);
	}
	return NULL;
}


int isOperator(char c){

	if(c=='%') return 1;
	else return 0;
}

int isSingleAction(char c){

	for(int i=0; i< single_aclist_s;){
		if(c==*(*(single_aclist+i))) return 1;
		i++;
		}
	return 0;
}

int isAction(char c){

	for(int i=0; i< aclist_s;){
		if(c==*(*(aclist+i))) return 1;
		i++;
		}
	return 0;
}

int isOpAc(char* c){

	if(*(c+1)!='\0')
		if(isOperator(*c)==1 
		   && isAction(*(c+1))==1) return 1;

	return 0;
}

void addEnd(char** end,int* capa,int* pos,char* c){

	int len=strlen(c);
	while(*capa< *pos + len+1) *capa*=2;
	*end=realloc(*end,*capa);
	memcpy(*end+*pos,c,len);
	*pos+=len;
}

int checkSupression(char* pos,char* end){

	if(pos+1>=end) return 0;
	if((*pos=='%')
	  && (*(pos+1)=='\\')){
		return !checkSupression(pos+2,end);
	}
	return 0;
}

char* mkString(char* c){

	int pos=0;
	int len=strlen(c);
	int capa=len*3; 
	int i=0; int* ii=&i;
	char* string=malloc(capa);
	char* end=c+len;

	
	while(i<len){

		if(isOperator(*(c+i))==1
		  && (i+1< len)
		  && (*(c+i+1)=='\\')){
			i+=2;
			continue;
		}

		int torture=isOpAc(c+i)? 2 : 1;

		if(((isSingleAction(*(c+i))
		  || isOpAc(c+i)))
		  && checkSupression((c+i+torture),end)==0){
			addEnd(&string,&capa,&pos,mkSubstAndMove(*(c+i),*(c+i+1),ii));
		}
		else{
			char tmp[2]={*(c+i),'\0'};
			addEnd(&string,&capa,&pos,tmp);
			i++;
		}
	}
	*(string+pos)='\0';
	return string;
}



int
main(){
	
char a[100];
scanf("%s",a);
printf("%s\n",mkString(a));
}





