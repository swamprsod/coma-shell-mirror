
/*\	 phiamoeba coma shell (ɔ)	\*/
/*\	 				\*/
/*\	 hm: ./LICENSE			\*/
/*\	 wikipedia.org/wiki/Ithkuil	\*/

	#define SIGMEOW "\nmeow\n"
	#define COMA '%'
	#define LISP
//	#define TEST

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <signal.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <fcntl.h>
	#include <dirent.h>

	
	#ifdef __OpenBSD__
		#include <readline/readline.h>
	#else
		#include <editline/readline.h>
	#endif
	
#define ACLIST_S(aua) ((size_t)((char*)(&aua+1)-(char*)&aua)/sizeof(*((aua)+0)))

volatile sig_atomic_t coma=0;

void
nop(void){}

void
incr(int* ij,int a){ *ij+=a; }

#ifdef LISP
char* 
mkString(char* c);
#endif

typedef struct{

	char mod;
	char type;
	char action1;
	char action2;
	char* subst;
}op;


op* ops=NULL;
int ops_s=0;

static char splitt[512];

static char* shell=NULL;
static char* ps1=NULL;
static char* shellfalse=NULL;
static char prefix[16384]="";
static char suffix[16384]="";
static char sprefix[16384]="";
static char ssuffix[16384]="";
static char lastcmd[16384]="";

int
isOperator(char c){

	if(c==COMA){ return 1; }
	else{ return 0; }
}

void
comaerr(int a){

	switch(a){
		case 1: fprintf(stderr,"\ncoma!strdup $PATH: segfault\n");
			exit(1);
		case 2: fprintf(stderr,"\ncoma!fork: ??: impossible\n");
			exit(1);
	}
}

int
checkSupression(char* pos,char* end){	/*\	'%\'	\*/

	if(pos+1 >=end){ return 0; }
	if((isOperator(*pos)==1)
	  && (*(pos+1)=='\\')){
		return !checkSupression(pos+2,end);
	}
	return 0;
}

op*
isgAction(char c1,char c2,int* len){

	*len=0;
	if(c2!='\0'){
		for(int j=0;j< ops_s;){
			op* o=ops+j;
			if((*o).action1==c1
			  && (*o).action2==c2
			  && (*o).type=='g'){
				*len=2;
				return o;
			}
			j++;
		}
		for(int j=0;j< ops_s;){
			op* o=ops+j;
			if(((*o).action1==c1)
			  && (*o).action2==c2
			  && (*o).type=='d'){
				*len=2;
				return o;
			}
			j++;
		}
	}
	for(int j=0;j< ops_s;){
		op* o=ops+j;
		if((*o).action1==c1
		  && (*o).action2=='\0'
		  && (*o).type=='g'){
			*len=1;
			return o;
		}
		j++;
	}
	for(int j=0;j< ops_s;){
		op* o=ops+j;
		if((*o).action1==c1
		  && (*o).action2=='\0'
		  && (*o).type=='d'){
			*len=1;
			return o;
		}
		j++;
	}
	return NULL;
}

op*
isSingle(char c){

	for(int j=0;j< ops_s;){
		op* o=ops+j;
		if(o->type=='s'
		  && (o->action1==c)
		  && o->action2=='\0'){ return o; }
		j++;
	}
	return NULL;
}

op*
isSeparator(void){

	for(int j=0;j< ops_s;){
		op* o=ops+j;
		if(o->type=='c'){ return o; }
		j++;
	}
	return NULL;
}

int
isLisp(char* c){

	char* end=c+strlen(c);
	char* rn=c;
	while(*rn){
		if((*rn==COMA)
		  && rn+1<end){
			char c1=*(rn+1);
			char c2=(rn+2< end)? *(rn+2):'\0';
			if((c2!='\0')
			  && (checkSupression(rn+3,end)==0)){
				for(int j=0;j< ops_s;){
					op* o=ops+j;
					if((o->type=='g')
					  && o->mod!=0
					  && o->action1==c1
					  && o->action2==c2){ return 1; }
					j++;
				}
			}
			if(checkSupression(rn+2,end)==0){
				for(int i=0;i< ops_s;){
					op* o=ops+i;
					if((o->type=='g')
					  && o->mod!=0
					  && o->action1==c1
					  && o->action2=='\0'){ return 1; }
					i++;
				}
			}
		}
		rn++;
	}
	return 0;
}

void
addOp(char mod,char type,char action1,char action2,char* subst){
	
	ops=realloc(ops,(ops_s+1)*sizeof(op));
	(*(ops+ops_s)).mod=mod;
	(*(ops+ops_s)).type=type;
	(*(ops+ops_s)).action1=action1;
	(*(ops+ops_s)).action2=action2;
	(*(ops+ops_s)).subst=strdup(subst);
	ops_s++;
}

void
parseLine(char* c){

	if(!strchr(c,':')){ return; }
	char* p=c;
	while(*p==' '
	     || *p=='\t'){
		p++;
	}
	char mod=*p++;
	if(mod=='0'){
		mod=0;
	}
	char type=*p++;
	char action1=*p++;
	char action2=*p++;
	if(type=='s'){
		if((action1=='0')==(action2=='0')){ return; }
	}
	p=strchr(p,':');
	if(p==NULL){ return; }
	p++;
	char* e=p;
	while((*e)
	     && *e!='\n'
	     && *e!='\r'){
		e++;
	}
	int n=e-p;
	char* subst=malloc(n+1);
	memcpy(subst,p,n);
	*(subst+n)='\0';
	if(type=='p'){
		free(ps1);
		ps1=strdup(subst);
		free(subst);
		return;
	}
	if(type=='h'){
		free(shell);
		shell=strdup(subst);
		free(subst);
		return;
	}
	if(action1=='0'
	  && action2=='0'){
		action1='\0';
		action2='\0';
	}
	else
	if(action1=='0'){
		action1=action2;
		action2='\0';
	}
	else
	if(action2=='0'){
		action2='\0';
	}

#ifdef LISP
	if(isLisp(subst)){
		addOp(mod,type,action1,action2,subst);
		free(subst);
		return;
	}
	*prefix='\0';
	*suffix='\0';
	*sprefix='\0';
	*ssuffix='\0';
	char* mksubst=mkString(subst);
	if((*prefix!='\0')
	  || (*suffix!='\0')
	  || (*sprefix!='\0')
	  || (*ssuffix!='\0')){
		int mkslen=strlen(mksubst);
		int lenps=strlen(prefix);
		int lenss=strlen(suffix);
		int lenpc=strlen(sprefix);
		int lensc=strlen(ssuffix);
		char* full=malloc(lenpc+lenps+mkslen+lenss+lensc+1);
		if(full){
			memcpy(full,sprefix,lenpc);
			memcpy(full+lenpc,prefix,lenps);
			memcpy(full+lenpc+lenps,mksubst,mkslen);
			memcpy(full+lenpc+lenps+mkslen,suffix,lenss);
			memcpy(full+lenpc+lenps+mkslen+lenss,ssuffix,lensc);
			*(full+lenpc+lenps+mkslen+lenss+lensc)='\0';
			free(mksubst);
			mksubst=full;
		}
	}
	*prefix='\0';
	*suffix='\0';
	*sprefix='\0';
	*ssuffix='\0';
	addOp(mod,type,action1,action2,mksubst);
	free(mksubst);
#else
	addOp(mod,type,action1,action2,subst);
#endif
	free(subst);
}

void
parseConf(char* comarc){

	FILE* f=fopen(comarc,"r");
	if(f==NULL){ return; }

	char fget[2048];
	int shelltrue=0;
	size_t sblen=0;

	while(fgets(fget,sizeof(fget),f)){

		if(strncmp(fget,"!!!shell",8)==0){
			shelltrue=1;
			continue;
		}
		if(strncmp(fget,"!!!",3)==0){
			shelltrue=0;
			continue;
		}
		if(shelltrue){
			size_t ll=strlen(fget);
			shellfalse=realloc(shellfalse,sblen+ll+1);
			memcpy(shellfalse+sblen,fget,ll);
			sblen+=ll;
			*(shellfalse+sblen)='\0';
			continue;
		}

		parseLine(fget);
	}
	fclose(f);
}


void
addEnd(char** end,int* capa,int* pos,char* c){

	int len=strlen(c);
	while(*capa< *pos+len+1){
		*capa*=2;
		nop();
	}
	*end=realloc(*end,*capa);
	memcpy(*end+*pos,c,len);
	*pos+=len;
}

void
strncatGlobal(op* o){

	char* raw=o->subst;
	if(isLisp(raw)){
		char* mkraw=mkString(raw);
		if(mkraw){
			free(mkraw);
		}
		return;
	}
	if(o->mod=='S'){
		strncat(sprefix,raw,sizeof(sprefix)-strlen(sprefix)-1);
	}
	else
	if(o->mod=='E'){
		strncat(ssuffix,raw,sizeof(ssuffix)-strlen(ssuffix)-1);
	}
	else
	if(o->mod=='s'){
		strncat(prefix,raw,sizeof(prefix)-strlen(prefix)-1);
	}
	else
	if(o->mod=='e'){
		strncat(suffix,raw,sizeof(suffix)-strlen(suffix)-1);
	}
}

char*
mkString(char* c){

	int pos=0;
	int len=strlen(c);
	int capa=len*3;
	char* string=malloc(capa);
	char* end=c+len;
	int i=0;

	while(i<len){
		if(isOperator(*(c+i))
		  && (i+1< len)
		  && (*(c+i+1)=='\\')){
			i+=2;
			continue;
		}

		if(isOperator(*(c+i))
		  && (i+1< len)){
			char c1=*(c+i+1);
			char c2=(i+2< len)? *(c+i+2):'\0';
			int mlen=0;
			op* o=isgAction(c1,c2,&mlen);
			if((o!=NULL)
			  && checkSupression(c+i+1+mlen,end)==0){
				if(o->type=='g'){
					strncatGlobal(o);
				}
				else
				if(o->type=='d'){
					addEnd(&string,&capa,&pos,o->subst);
				}
				i+=1+mlen;
				continue;
			}
		}

	
		op* so=isSingle(*(c+i));
		if((so!=NULL)
		  && checkSupression(c+i+1,end)==0){
			addEnd(&string,&capa,&pos,so->subst);
			i+=1;
			continue;
		}
		char tmp[2]={*(c+i),'\0'};
		addEnd(&string,&capa,&pos,tmp);
		i++;
	}
	*(string+pos)='\0';
	return string;
}


void
addIfNotExist_t3(char*** masstr,int* a,char* c){

	for(int i=0;i< *a;){
		if(!strcmp(*((*masstr)+i),c)){ return; }
		i++;
	}
	*masstr=realloc(*masstr,(*a+1)*sizeof(**masstr));
	*((*masstr)+(*a)++)=strdup(c);
}

char*
isTilda(char* dir){

	char* home=getenv("HOME");
	if(*dir!='~'){ return strdup(dir); }
	if(!home){ return strdup(dir); }

	char* c=(malloc(strlen(home)+strlen(dir)+1));
	strcpy(c,home);
	strcat(c,dir+1);
	return c;
}

int
isDir(char* path){

	int a=open(path,O_RDONLY
		  |O_DIRECTORY);
	if(a< 0){ return 0; }
	close(a);
	return 1;
}

int
isBreak(char c){

	if((c==' ')
	  || c=='\t'
	  || c==COMA
	  || c=='\\'){ return 1; }

	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='s'
		  && (*(ops+i)).action1==c
		  && (*(ops+i)).action2=='\0'){ return 1; }
		i++;
	}
	return 0;
}

char*
lastSep(const char* c){

	char* sep=NULL;
	for(char* i=c;*i;){
		if(isBreak(*i)==1){
			sep=i;
		}
		i++;
	}
	return sep;
}

void
splitSlash(const char* pathh,char* dir,char** c,size_t dirs){

	char* slash=strrchr(pathh,'/');
	if(slash!=NULL){
		size_t n=(slash-pathh+1);
		if(n >=dirs)
			n=dirs-1;
		memcpy(dir,pathh,n);
		*(dir+n)='\0';
		*c=slash+1;
	}
	else{
	*(dir+0)='\0';
	*c=pathh;
	}
}

char*
commandCreate(const char* c,int sos){

	static char** list=NULL;
	static int sum=0;
	static int amo=0;
	if(!sos){
		if(list!=NULL){
			for(int i=0;i< sum;){
				free(*(list+i));
				i++;
			}
			free(list);
		}
		list=NULL;
		sum=0;
		amo=0;
		size_t len=strlen(c);
		char* path=getenv("PATH")? strdup(getenv("PATH")):(comaerr(1),NULL);
		if(path!=NULL){
			char* dir=strtok(path,":");
			while(dir!=NULL){
				DIR* oopendir=opendir(dir);
				if(oopendir!=NULL){
					struct dirent* dname;
					while((dname=readdir(oopendir))){
						if(*(*dname).d_name=='.'){
							continue;
						}
						if(strncmp(dname->d_name,c,len)){
							continue;
						}
						addIfNotExist_t3(&list,&sum,(*dname).d_name);
					}
					closedir(oopendir);
				}
				dir=strtok(NULL,":");
			}
			free(path);
		}
	}
	if(amo< sum){ return strdup(*(list+amo++)); }

	return NULL;
}

char*
fileCreate(const char* c,int sos){

	static DIR* o=NULL;
	static char first[16384];
	static char partdir[8192];
	static char fulldir[8192];
	static char* partname;
	static size_t len;

	if(!sos){
		char* last=lastSep(c);
		const char* cdr=c;
		if(last){
			size_t n=last-c+1;
			if(n >=sizeof(first))
				n=sizeof(first)-1;
			memcpy(first,c,n);
			*(first+n)='\0';
			cdr=last+1;
		}
		else{
			*first='\0';
		}
		splitSlash(cdr,partdir,&partname,sizeof(partdir));
		len=strlen(partname);

		char* open1=*partdir? partdir : ".";
		char* tild=isTilda(open1);
		if(tild){
			snprintf(fulldir,sizeof(fulldir),"%s",tild);
			free(tild);
			o=opendir(fulldir);
		}
		else{
			*fulldir='\0';
			o=NULL;
		}
		if(!o){ return NULL; }
	}

	struct dirent* dname;
	while((dname=readdir(o))){
		char* name=(*dname).d_name;
		if((*name=='.')
		  && (*partname!='.')){
			continue;
		}
		if(strncmp(name,partname,len)!=0){
			continue;
		}
		char full[16384];
		snprintf(full,sizeof(full),"%s%s",fulldir,name);

		char mb[32768];
		if(!isDir(full)){
			snprintf(mb,sizeof(mb),"%s%s%s",first,partdir,name);
		}
		else
			snprintf(mb,sizeof(mb),"%s%s%s/",first,partdir,name);
		return strdup(mb);
	}
	closedir(o);
	o=NULL;
	return NULL;
}

void
buildBreaks(void){

	int a=0;
	*(splitt+a++)=' ';
	*(splitt+a++)='\t';
	*(splitt+a++)='\n';
	*(splitt+a++)=COMA;
	*(splitt+a++)='\\';
	for(int i=0;i< ops_s;){
		*(splitt+a++)=(*(ops+i)).action1;
		i++;
	}
	*(splitt+a)='\0';
}

int
isSingleOrPrefix(char* c){

	if(strchr(c,'~')){ return 1; }
	if(strchr(c,COMA)){ return 1; }

	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='s'
		  && strchr(c,(*(ops+i)).action1)){ return 1; }
		i++;
	}
	return 0;
}

char**
complete(const char* c,int start,int end){

	(void)end;

	if(isSingleOrPrefix((char*)c)!=0)
	return rl_completion_matches(c,fileCreate);
	for(int i=start-1;i >=0;){
		char lbu=*(rl_line_buffer+i);
		if((lbu!=' ')
		  && (lbu!='\t'))
			return rl_completion_matches(c,fileCreate);
		i--;
	}
	return rl_completion_matches(c,commandCreate);
}

void
execCommand(char* c){

	pid_t pid=fork();
	if(pid< 0){
		comaerr(2);
		return;
	}
	if(pid==0){
		char full[16384];
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		snprintf(full,sizeof(full),"%s%s",shellfalse? shellfalse:"",c);
		execl(shell,shell,"-c",full,NULL);
		_exit(0);
	}
	int status;
	waitpid(pid,&status,0);
}

char*
afterRepeat(char* c){

	char* p=c;
	while((*p==' ')
	     || *p=='\t'){
		p++;
	}
	if(*p==COMA
	  && *(p+1)==COMA){
		char* end=c+strlen(c);
		if(checkSupression(p+1,end)==0
		  && checkSupression(p+2,end)==0){ return p+2; }
	}
	return NULL;
}

void
run1(char* c){

	char* cdr=afterRepeat(c);
	if(cdr){
		if(*lastcmd!='\0'){
			*prefix='\0';
			*suffix='\0';
			char* cdrstr=mkString(cdr);
			if(cdrstr){
				int lenp=strlen(prefix);
				int lenlast=strlen(lastcmd);
				int lencdr=strlen(cdrstr);
				int lens=strlen(suffix);
				char* full=malloc(lenp+lenlast+lencdr+lens+1);
				if(full){
					memcpy(full,prefix,lenp);
					memcpy(full+lenp,lastcmd,lenlast);
					memcpy(full+lenp+lenlast,cdrstr,lencdr);
					memcpy(full+lenp+lenlast+lencdr,suffix,lens);
					*(full+lenp+lenlast+lencdr+lens)='\0';
					execCommand(full);
					free(full);
				}
				free(cdrstr);
			}
			*prefix='\0';
			*suffix='\0';
		}
		return;
	}

	*prefix='\0';
	*suffix='\0';
	*sprefix='\0';
	*ssuffix='\0';
	char* result=mkString(c);
	if(result){
		if((*prefix!=0)
		  || (*suffix!=0)){
			int lenp=strlen(prefix);
			int lens=strlen(suffix);
			int lencdr=strlen(result);
			char* full=malloc(lenp+lencdr+lens+1);
			if(full){
				memcpy(full,prefix,lenp);
				memcpy(full+lenp,result,lencdr);
				memcpy(full+lenp+lencdr,suffix,lens);
				*(full+lenp+lencdr+lens)='\0';
				free(result);
				result=full;
			}
			*prefix='\0';
			*suffix='\0';
		}
		if((*sprefix!=0)
		  || (*ssuffix!=0)){
			int lenp=strlen(sprefix);
			int lens=strlen(ssuffix);
			int lencdr=strlen(result);
			char* full=malloc(lenp+lencdr+lens+1);
			if(full){
				memcpy(full,sprefix,lenp);
				memcpy(full+lenp,result,lencdr);
				memcpy(full+lenp+lencdr,ssuffix,lens);
				*(full+lenp+lencdr+lens)='\0';
				free(result);
				result=full;
			}
			*sprefix='\0';
			*ssuffix='\0';
		}
		snprintf(lastcmd,sizeof(lastcmd),"%s",result);
		execCommand(result);
		free(result);
	}
}

void
run05(char* line){

	op* chain=isSeparator();
	if(chain==NULL){
		run1(line);
		return;
	}
	*sprefix='\0';
	*ssuffix='\0';
	char* end=line+strlen(line);
	char* p=line;
	int olen=0;
	int ocapa=strlen(line)*3+1;
	char* out=malloc(ocapa);
	if(!out){ return; }

	while(*p){
		char* sep=NULL;
		int seplen=0;
		int truec=0;

		for(char* q=p;*q;){
			if(isOperator(*q)
			  && *(q+1)=='\\'){
				q++;
				continue;
			}
			if(*q==chain->action1
			  && checkSupression(q+1,end)==0){
				sep=q;
				seplen=1;
				truec=1;
				break;
			}
			if(*q=='|'
			  && *(q+1)=='|'){
				sep=q;
				seplen=2;
				break;
			}
			if(*q=='&'
			  && *(q+1)=='&'){
				sep=q;
				seplen=2;
				break;
			}
			if(*q=='|'
			  || *q==';'
			  || *q=='&'){
				sep=q;
				seplen=1;
				break;
			}
			q++;
		}

		char s1=0;
		char s2=0;
		if(sep){
			s1=*sep;
			if(seplen==2){
				s2=*(sep+1);
				*(sep+1)='\0';
			}
			*sep='\0';
		}
		char* cdr=afterRepeat(p);
		if(cdr){
			if(*lastcmd!='\0'){
				*prefix='\0';
				*suffix='\0';
				char* cdrstr=mkString(cdr);
				if(*prefix!=0){
					addEnd(&out,&ocapa,&olen,prefix);
				}
				addEnd(&out,&ocapa,&olen,lastcmd);
				if(cdrstr){
					addEnd(&out,&ocapa,&olen,cdrstr);
					free(cdrstr);
				}
				if(*suffix!=0){
					addEnd(&out,&ocapa,&olen,suffix);
				}
				*prefix='\0';
				*suffix='\0';
			}
		}
		else{
			*prefix='\0';
			*suffix='\0';
			char* r=mkString(p);

			if(*prefix!=0){
				addEnd(&out,&ocapa,&olen,prefix);
			}
			if(r!=NULL){
				addEnd(&out,&ocapa,&olen,r);
				free(r);
			}
			if(*suffix!=0){
				addEnd(&out,&ocapa,&olen,suffix);
			}
			*prefix='\0';
			*suffix='\0';
		}

		if(!sep){
			break;
		}
		*sep=s1;
		if(seplen==2){
			*(sep+1)=s2;
		}

		if(truec){
			addEnd(&out,&ocapa,&olen,chain->subst);
		}
		else{
			char tmp[3]={*sep,'\0','\0'};
			if(seplen==2){
				*(tmp+1)=*(sep+1);
			}
			addEnd(&out,&ocapa,&olen,tmp);
		}

		p=sep+seplen;
	}

	if(*sprefix!=0
	  || *ssuffix!=0){
		int lenp=strlen(sprefix);
		int lens=strlen(ssuffix);
		while(ocapa< olen+lenp+lens+1){
			ocapa*=2;
		}
		out=realloc(out,ocapa);
		memmove(out+lenp,out,olen+1);
		memcpy(out,sprefix,lenp);
		memcpy(out+lenp+olen,ssuffix,lens);
		olen+=lenp+lens;
		*(out+olen)='\0';
		*sprefix='\0';
		*ssuffix='\0';
	}

	*(out+olen)='\0';
#ifdef TEST
	printf("\n0:: %s :::: out",out);
	printf("\n1:: %s :::: lastcmd\n\n",lastcmd);
#endif
	if(olen >0){
		snprintf(lastcmd,sizeof(lastcmd),"%s",out);
	}
	execCommand(out);
	free(out);
}

void
sig(int a){

	(void)a;
	if(coma){
		write(1,SIGMEOW,6);
		rl_free_line_state();
		rl_cleanup_after_signal();
		rl_reset_after_signal();
		write(1,ps1,strlen(ps1));
	}
}

float
main(int argc,char** argv){

signal(SIGINT,sig);
signal(SIGQUIT,sig);

char* home=getenv("HOME");
char* cfg=getenv("COMA_CONFIG");
char cfgpath[4096];

if(!cfg
  && home){
	snprintf(cfgpath,sizeof(cfgpath),"%s/.comarc",home);
	cfg=cfgpath;
}
if(cfg){
	parseConf(cfg);
}
if(argc >=2
  && strcmp(*(argv+1),"--init")==0){
	if(shellfalse!=NULL){
		fputs(shellfalse,stdout);
	}
	return 0;
}

if(!ps1){
	char* e=getenv("COMA_PS1");
	ps1=strdup(e? e : "; coma< ");
}

if(!shell){
	char* e=getenv("COMA_SH");
	shell=strdup((e && *e)? e : "/bin/sh");
}


if(argc>=3
  && strcmp(*(argv+1),"-c")==0){
	run05(*(argv+2));
	return 0;
}

buildBreaks();
rl_catch_signals=0;
rl_initialize();
rl_attempted_completion_function=complete;
rl_completion_append_character='\0';
rl_completer_word_break_characters=splitt;

char histpath[4096]="";
if(home!=NULL){
	snprintf(histpath,sizeof(histpath),"%s/tmptest.txt",home);
	read_history(histpath);
}
char* line;
for(;;){
	coma=1;
	line=readline(ps1);
	coma=0;
	if(!line){
		if(feof(stdin)){
			break;
		}
		continue;
	}
	if(*line!='\0'){
		if(strcmp(line,"exit")==0){
			free(line);
			break;
		}
		add_history(line);
		run05(line);
	}
	free(line);
}

if(home!=NULL){
	write_history(histpath);
}
return 0.5;
}
