
/*\	 phiamoeba coma shell (ɔ)	\*/
/*\	 				\*/
/*\	 hm: ./LICENSE			\*/
/*\	 wikipedia.org/wiki/Ithkuil	\*/

	#define SIGMEOW "\nmeow\n"
	#define OCMA '%'
	#define COMA_EL_KEY "bind -s \"M- \" \"%\""
	#define ITA 100
	#define VERSION "coma-1.1(0)-0rop"

//	#define TEST
	#define PRE
	#define WCOLORS
	#define WUTF8

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <signal.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <fcntl.h>
	#include <poll.h>
	#include <dirent.h>
	#include <sys/syscall.h>
	#include <setjmp.h>
	#include <termios.h>
	#include <limits.h>
	#ifdef WCOLORS
		#ifdef __linux__
			#include <bsd/vis.h>
		#endif
	#endif
	#ifdef __OpenBSD__
		#include <readline/readline.h>
	#else
		#include <editline/readline.h>
	#endif
	#ifdef WUTF8
		#include <locale.h>
	#endif
	
#define ACLIST_S(aua) ((size_t)((char*)(&aua+1)-(char*)&aua)/sizeof(*((aua)+0)))

volatile sig_atomic_t coma=0;
volatile static sigjmp_buf jmp;

void
nop(void){}

void
incr(int* ij,int a){ *ij+=a; }

void
versionIs(void){ printf("coma %s\n",VERSION); }

typedef struct{

	char mod;
	char type;
	char action1;
	char action2;
	char* subst;
}op;

#ifdef PRE
char* mkStrD(char* c);char* iterMkStrD(char* c);void mkStrG(char* c);char* mkString(char* c);char* cTypeInflate(char* s,op* ctype);char* whatAffix(char mod);char* mergeAffix(char* ogrizok);char* findSep(char* p,int* seplen);void mkTypeCsegment_t2_1(char* full);void doRepeat(char* cdr,op* ctype);void run05(char* line);
#endif

op* ops=NULL;
int ops_s=0;

static struct termios termiossv;
static int savetio=0;

static char* shell=NULL;
static char* ps1=NULL;
static char* shellfalse=NULL;
static char splitt[512];
static char prefix[16384]="";
static char suffix[16384]="";
static char sprefix[16384]="";
static char ssuffix[16384]="";
static char lastcmd[16384]="";
static char comahistory[4096]="";
static int globb=0;
static int comac=0;
static int stdinstdin=-1;

void
historySave(void){

	if(*comahistory!='\0'){
		write_history(comahistory);
	}
}

//void
//arcList(char** u1,int u2,int u3){
//
//	(void)u1;
//	(void)u2;
//	(void)u3;
//}

void
printOps(void){

	for(int i=0;i< ops_s;){
		op* o=ops+i;
		printf("%d\t:  %c %c %c%c : %s\n",i,o->mod? o->mod
							  : '0',o->type,o->action1?
								       o->action1 : '0',o->action2?
										       o->action2 : '0',o->subst);
		i++;
	}
}

int
isOperator(char c){

	if(c==OCMA){ return 1; }
	else{ return 0; }
}

void
comaerr(int a,char* what){

	switch(a){
		case 1: fprintf(stderr,"\ncoma!strdup $PATH: segfault\n");
			usleep(969099);
			exit(1);
		case 2: fprintf(stderr,"\ncoma!fork: ??: impossible\n");
			usleep(1090000);
			exit(1);
		case 3: fprintf(stderr,"\ncoma!execCoc: ??: impossible\n");
			usleep(1090000);
			exit(1);
		case 4: fprintf(stderr,"coma!what is: ??: %s\n",what);
			exit(1);
		case 5: fprintf(stderr,"coma!rmOp: operator doesnt exist! %d\n",(int*)what);
			return;
		case 6: fprintf(stderr,"coma!rmOp: operator doesnt exist! %s\n",what);
			return;
		case 7: fprintf(stderr,"coma!config: ~/.comarc not found\n");
			usleep(1090000);
			comaerr(8,NULL);
			usleep(969099);
			exit(1);
		case 8: fprintf(stderr,"coma!what is: ??: 0xfffffffffffff000\n");
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

void
rmOp(int j){

	if((j< 0)
	  || (j >=ops_s)){
		comaerr(5,(char*)j);
		return;
	}
	free((*(ops+j)).subst);
	for(int i=j;i< ops_s-1;){
		*(ops+i)=*(ops+i+1);
		i++;
	}
	ops_s--;
	if(ops_s >0){
		ops=realloc(ops,ops_s*sizeof(op));
	}
	else{
		free(ops);
		ops=NULL;
	}
	globb=0;
	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='g'){
			globb=1;
			break;
		}
		i++;
	}
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
	#ifdef WCOLORS
		strunvis(ps1,ps1);
	#endif
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

	char* mksubst=mkStrD(subst);
	addOp(mod,type,action1,action2,mksubst);
	free(mksubst);
}

int
comaLambda(const char* lambda){

	char* lam=strdup(lambda);
	if(lam==NULL){ return -1; }
	parseLine(lam);
	free(lam);

	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='g'){
			globb=1;
			break;
		}
		i++;
	}
	fprintf(stdout,"coma!lambda %s\nop index: %d\n",lambda,ops_s-1);
	return 0;
}

void
parseConf(char* comarc){

	FILE* f=fopen(comarc,"r");
	if(f==NULL){ return; }

	char fget[2048];
	int shelltrue=0;
	size_t sblen=0;

	while(fgets(fget,sizeof(fget),f)){
		if(strcmp(fget,"!shell<\n")==0){
			shelltrue=1;
			continue;
		}
		if(strcmp(fget,"!end shell\n")==0){
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

char*
whatAffix(char mod){

	if(mod=='S'){ return sprefix; }
	if(mod=='E'){ return ssuffix; }
	if(mod=='s'){ return prefix; }
	if(mod=='e'){ return suffix; }
	return NULL;
}

void
strncatGlobal(op* o){

	char* raw=strdup(o->subst);
	if(raw==NULL){ return; }
	if(strchr(raw,OCMA)){
		mkStrG(raw);
	}
	char* inflated=cTypeInflate(raw,isSeparator());
	free(raw);
	if(inflated==NULL){ return; }
	char* affix=whatAffix(o->mod);
	if(affix!=NULL){
		size_t lenaf=strlen(affix);
		size_t len=strlen(inflated);
		if(lenaf+len+1<= 16384){
			memcpy(affix+lenaf,inflated,len+1);
		}
	}
	free(inflated);
}

char*
cTypeInflate(char* c,op* ctype){

	if(ctype==NULL){ return strdup(c); }

	size_t lenc=strlen(ctype->subst);
	size_t len=strlen(c);
	char* out=malloc(len*(lenc >1? lenc : 1)+1);
	if(out==NULL){ return strdup(c); }

	char* w=out;
	for(char* mv=c;*mv;){
		if(isOperator(*mv)
		  && *(mv+1)=='\\'){
			mv+=2;
			continue;
		}
		if(*mv==ctype->action1
		  && checkSupression(mv+1,c+len)==0){
			memcpy(w,ctype->subst,lenc);
			w+=lenc;
			mv++;
			continue;
		}
		*w++=*mv++;
	}
	*w='\0';
	return out;
}

char*
mkStrD(char* c){

	char* cag=strdup(c);
	if(cag==NULL){ return NULL; }
	int i=0;
	while(i< ITA){
		char* next=iterMkStrD(cag);
		if(next==NULL){
			free(cag);
			return NULL;
		}
		if(strcmp(next,cag)==0){
			free(next);
			break;
		}
		free(cag);
		cag=next;
		i++;
	}
	for(char* p=cag;*p;){
		if(*p=='\x01'){
			*p=OCMA;
		}
		p++;
	}
	return cag;
}

char*
iterMkStrD(char* c){

	int pos=0;
	int len=strlen(c);
	int capa=len*3+16;
	char* string=malloc(capa);
	char* end=c+len;
	int i=0;

	while(i< len){
		if(isOperator(*(c+i))
		  && (i+1< len)
		  && *(c+i+1)=='\\'){
			i+=2;
			continue;
		}
		if(isOperator(*(c+i))
		  && (i+1< len)){
			char c1=*(c+i+1);
			char c2=(i+2< len)? *(c+i+2):'\0';
			int whatlen=0;
			op* o=isgAction(c1,c2,&whatlen);
			if(o!=NULL){
				if(checkSupression(c+i+1+whatlen,end)){
					char tmp[4];
					memcpy(tmp,c+i,1+whatlen);
					*(tmp+1+whatlen)='\0';
					*tmp='\x01';
					addEnd(&string,&capa,&pos,tmp);
					i+=1+whatlen;
					continue;
				}
				if((whatlen==1)
				  && (c1==OCMA)
				  && (i+2< len)
				  && (*(c+i+2)=='\\')){
					addEnd(&string,&capa,&pos,"\x01");
					i+=3;
					continue;
				}
				if(o->type=='g'){
					char tmp[4];
					memcpy(tmp,c+i,1+whatlen);
					*(tmp+1+whatlen)='\0';
					addEnd(&string,&capa,&pos,tmp);
					i+=1+whatlen;
					continue;
				}
				if(o->type=='d'){
					addEnd(&string,&capa,&pos,o->subst);
					i+=1+whatlen;
					continue;
				}
			}
		}
		op* sici=isSingle(*(c+i));
		if((sici!=NULL)
		  && checkSupression(c+i+1,end)==0){
			addEnd(&string,&capa,&pos,sici->subst);
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
mkStrG(char* c){

	char* end=c+strlen(c);
	char* mv=c;
	char* w=c;

	while(*mv){
		if(isOperator(*mv)
		  && (mv+1< end)){
			char c1=*(mv+1);
			char c2=(mv+2< end)? *(mv+2):'\0';
			int whatlen=0;
			op* o=isgAction(c1,c2,&whatlen);
			if((o!=NULL)
			  && o->type=='g'
			  && checkSupression(mv+1+whatlen,end)==0){
				strncatGlobal(o);
				mv+=1+whatlen;
				continue;
			}
		}
		*w++=*mv++;
	}
	*w='\0';
}

char*
mkString(char* c){

	char* mkD=mkStrD(c);
	if(mkD!=NULL
	  && globb){
		mkStrG(mkD);
	}
	return mkD;
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
		  | O_DIRECTORY);
	if(a< 0){ return 0; }
	close(a);
	return 1;
}

int
isBreak(char c){

	if((c==' ')
	  || c=='\t'
	  || c==OCMA
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
	*dir='\0';
	*c=(char*)pathh;
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
		char* path=getenv("PATH")? strdup(getenv("PATH")):(comaerr(1,NULL),NULL);
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
	*(splitt+a++)=OCMA;
	*(splitt+a++)='\\';
	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='s'
		  && (*(ops+i)).action1
		  && (*(ops+i)).action2=='\0'){
			*(splitt+a++)=(*(ops+i)).action1;
		}
		i++;
	}
	*(splitt+a)='\0';
}

int
isSingleOrPrefix(char* c){

	if(strchr(c,'~')){ return 1; }
	if(strchr(c,OCMA)){ return 1; }

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
	char** korm;
	int iscmd=1;
	for(int i=start-1;i >=0;){
		if((*(rl_line_buffer+i)!=' ')
		  && (*(rl_line_buffer+i)!='\t')){
			iscmd=0;
			break;
		}
		i--;
	}
	if(isSingleOrPrefix((char*)c)){
		iscmd=0;
	}
	if(strchr(c,'/')
	  || (*c=='.')){
		iscmd=0;
	}
	korm=rl_completion_matches(c,iscmd? commandCreate:fileCreate);
	if(korm==NULL){ return NULL; }
	int n=0;
	while(*(korm+n)){
		n++;
	}
	if(n==1){ return korm; }
	char* p=strdup(*korm);
	for(int i=1;i< n;){
		int j=0;
		while(*(p+j)
		     && (*(*(korm+i)+j))
		     && (*(p+j)==*(*(korm+i)+j))){
			j++;
		}
		*(p+j)='\0';
		i++;
	}
	for(int i=0;i< n;){
		free(*(korm+i));
		i++;
	}
	free(korm);
	if(strlen(p)<= strlen(c)){
		free(p);
		rl_attempted_completion_over=1;
		return NULL;
	}
	korm=malloc(2*sizeof(char*));
	*korm=p;
	*(korm+1)=NULL;
	return korm;
}

int
insertOCMA(int count,int key){

	(void)count;
	(void)key;
	rl_insert_text("%");
	return 0;
}

int
execCoc(char* full){

	pid_t pid=fork();
	if(pid< 0){
		return -1;
	}
	if(pid==0){
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		execl(shell,shell,"-c",full,NULL);
		_exit(1);
	}
	int status;
	waitpid(pid,&status,0);
	return status;
}

void
execCommand(char* c){

	char full[16384];
	char* pre=shellfalse? shellfalse : "";

	if(comac){
		snprintf(full,sizeof(full),"%s%s",pre,c);
#ifdef TEST
	fprintf(stderr,"\n2:: %s\n",c);
#endif
	if(execCoc(full)< 0){
			comaerr(2,NULL);
		}
		return;
	}

	int fd[2];
	if(pipe(fd)!=0){
		comaerr(3,NULL);
		return;
	}

	pid_t pid=fork();
	if(pid< 0){
		close(*fd);
		close(*(fd+1));
		comaerr(2,NULL);
		return;
	}
	if(pid==0){
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		close(*fd);
		if(*(fd+1)!=9){
			dup2(*(fd+1),9);
			if(stdinstdin >=0){
				dup2(stdinstdin,0);
				close(stdinstdin);
			}
			close(*(fd+1));
		}
#ifdef TEST
	fprintf(stderr,"\n2:: %s\n",c);
#endif
	snprintf(full,sizeof(full),"%s%s\npwd >&9\nexec 9>&-",pre,c);
		execl(shell,shell,"-c",full,NULL);
		_exit(1);
	}
	if(stdinstdin>=0){
		close(stdinstdin);
		stdinstdin=-1;
	}

	close(*(fd+1));

	int status;
	waitpid(pid,&status,0);
	char patha[PATH_MAX+2];
	ssize_t readsum=0;
	struct pollfd pfd={
		*fd,
		POLLIN,
		0
	};
	if(poll(&pfd,1,100) >0){
		readsum=read(*fd,patha,sizeof(patha)-1);
		if(readsum< 0){
			readsum=0;
		}
	}
	*(patha+readsum)='\0';
	close(*fd);

	while((readsum >0)
	     && (*(patha+readsum-1)=='\n'
	     || *(patha+readsum-1)=='\r'
	     || *(patha+readsum-1)==' ')){
		*(patha+--readsum)='\0';
	}
	if((readsum >0)
	  && (*patha=='/')
	  && (isDir(patha))
	  && (chdir(patha)==0)){
		char* old=getenv("PWD");
		if((old!=NULL)
		  && strcmp(old,patha)!=0){
			setenv("OLDPWD",old,1);
		}
		setenv("PWD",patha,1);
	}
}

char*
mergeAffix(char* ogrizok){

	int a=0;
	int lensp=strlen(sprefix);
	int lenp=strlen(prefix);
	int lentmp=strlen(ogrizok);
	int lens=strlen(suffix);
	int lenss=strlen(ssuffix);

	char* full=malloc(lensp+lenp+lentmp+lens+lenss+1);
	if(full==NULL){ return NULL; }
	memcpy(full+a,sprefix,lensp);
	a+=lensp;
	memcpy(full+a,prefix,lenp);
	a+=lenp;
	memcpy(full+a,ogrizok,lentmp);
	a+=lentmp;
	memcpy(full+a,suffix,lens);
	a+=lens;
	memcpy(full+a,ssuffix,lenss);
	a+=lenss;
	*(full+a)='\0';

	*prefix='\0';
	*suffix='\0';
	*sprefix='\0';
	*ssuffix='\0';
	return full;
}

char*
findSep(char* p,int* seplen){

	*seplen=0;
	for(char* q=p;*q;){
		if((*q=='|')
		  && (*(q+1)=='|')){
			*seplen=2;
			return q;
		}
		if((*q=='&')
		  && *(q+1)=='&'){
			*seplen=2;
			return q;
		}
		if((*q=='|')
		  || *q==';'
		  || *q=='&'){
			*seplen=1;
			return q;
		}
		q++;
	}
	return NULL;
}

int
isExit(char* p){

	while((*p==' ')
	     || (*p=='\t')){
		p++;
	}
	if(strncmp(p,"exit",4)!=0){ return 0; }
	p+=4;
	while((*p==' ')
	     || (*p=='\t')){
		p++;
	}
	return *p=='\0';
}

char*
isComa(char* p){

	while((*p==' ')
	     || (*p=='\t')){
		p++;
	}
	if(strncmp(p,"coma",4)!=0){ return NULL; }
	p+=4;
	if((*p!='\0')
	  && (*p!=' ')
	  && (*p!='\t')){ return NULL; }
	return p;
}

void
comaRun(char* p){

	while((*p)
	     && (*p==' '
	     || *p=='\t')){
		p++;
	}
	while(*p){
		while((*p==' ')
		     || (*p=='\t')){
			p++;
		}
		if(strncmp(p,"-a",2)==0
		  && ((*(p+2)=='\0')
		  || (*(p+2)==' ')
		  || (*(p+2)=='\t'))){
			printOps();
			p+=2;
			continue;
		}
		if(strncmp(p,"-v",2)==0
		  && ((*(p+2)=='\0')
		  || (*(p+2)==' ')
		  || (*(p+2)=='\t'))){
			versionIs();
			p+=2;
			continue;
		}
		if(strncmp(p,"-f=",3)==0){
			char* end=p+3;
			while(*end){
				if(*end==','){
					char* q=end+1;
					while((*q==' ')
					     || (*q=='\t')){
						q++;
					}
					if((*q=='-')
					  || (*q=='\0')){
						break;
					}
				}
				end++;
			}
			char saved=*end;
			*end='\0';
			comaLambda(p+3);
			*end=saved;
			if(saved=='\0'){ break; }
			p=end+1;
			continue;
		}
		if(strncmp(p,"-d=",3)==0){
			char* end=p;
			while((*end)
			     && (*end!=' ')
			     && *end!='\t'){
				end++;
			}
			char saved=*end;
			*end='\0';
			if((*(p+3) >='0')
			  && (*(p+3)<= '9')){
				rmOp(atoi(p+3));
			}
			else{
				char p3p3[5]={*(p+3),'.','.','.','\0'};
				comaerr(6,p3p3);
			}
			*end=saved;
			p=end;
			continue;
		}
		break;
	}
}

int
stdoutWhat(char* p){

	while((*p==' ')
	     || (*p=='\t')){
		p++;
	}
	if(*p!='-'){ return 0; }
	p++;
	if((*p=='a')
	  || (*p=='v')){
		p++;
		return *p=='\0';
	}
	return 0;
}

void
mkTypeCsegment_t2_1(char* full){

	char* p=full;
	int olen=0;
	int ocapa=strlen(full)*3+16;
	char* out=malloc(ocapa);
	if(out==NULL){ return; }
	*out='\0';

	while(*p){
		int seplen=0;
		char* sep=findSep(p,&seplen);
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
		if(isExit(p)){
			if(olen >0){
				*(out+olen)='\0';
				snprintf(lastcmd,sizeof(lastcmd),"%s",out);
				execCommand(out);
			}
			free(out);
			exit(0);
		}
		char* cp=isComa(p);
		if(cp!=NULL){
			if(olen >0){
				*(out+olen)='\0';
				snprintf(lastcmd,sizeof(lastcmd),"%s",out);
				execCommand(out);
				olen=0;
				*out='\0';
			}
			snprintf(lastcmd,sizeof(lastcmd),"%s",p);
			if((sep!=NULL)
			  && (s1=='|')
			  && (seplen==1)
			  && stdoutWhat(cp)){
				int pfd[2];
				if(pipe(pfd)==0){
					int saved=dup(1);
					dup2(*(pfd+1),1);
					close(*(pfd+1));
					comaRun(cp);
					fflush(stdout);
					dup2(saved,1);
					close(saved);
					stdinstdin=*pfd;
				}
				else{
					comaRun(cp);
				}
			}
			else{
				comaRun(cp);
			}
			if(sep==NULL){
				break;
			}
			*sep=s1;
			if(seplen==2){
				*(sep+1)=s2;
			}
			p=sep+seplen;
			continue;
		}
		addEnd(&out,&ocapa,&olen,p);
		if(sep==NULL){
			break;
		}
		*sep=s1;
		if(seplen==2){
			*(sep+1)=s2;
		}
		char tmp[3]={s1,'\0','\0'};
		if(seplen==2){
			*(tmp+1)=s2;
		}
		addEnd(&out,&ocapa,&olen,tmp);
		p=sep+seplen;
	}
	*(out+olen)='\0';
	if(olen >0){
		snprintf(lastcmd,sizeof(lastcmd),"%s",out);
		execCommand(out);
	}
	if(stdinstdin >=0){
		close(stdinstdin);
		stdinstdin=-1;
	}
	free(out);
}

void
run05(char* line){

	if((ops_s >0)
	  && ((*ops).type=='d')
	  && ((*ops).action1==OCMA)
	  && ((*ops).action2=='\0')){
		free((*ops).subst);
		(*ops).subst=strdup(lastcmd);
	}
	op* ctype=isSeparator();
	char* inflated=mkStrD(line);
	if(inflated==NULL){ return; }

	if(globb){
		mkStrG(inflated);
	}
	char* full=mergeAffix(inflated);
	free(inflated);
	if(full==NULL){ return; }

	for(int i=0;i< ops_s;){
		if((*(ops+i)).type=='c'){
			char* t=cTypeInflate(full,ops+i);
			free(full);
			full=t;
		}
		i++;
	}

	mkTypeCsegment_t2_1(full);
	free(full);
}

void
sig(int a){

	(void)a;
	write(1,SIGMEOW,sizeof(SIGMEOW)-1);
	if(coma){
		siglongjmp(jmp,1);
	}
}

size_t
main(int argc,char** argv){

	addOp(0,'d',OCMA,'\0',"");
	setenv("SHELL","coma",1);
#ifdef WUTF8
	setlocale(LC_ALL,"");
#endif
	signal(SIGINT,sig);
	signal(SIGQUIT,sig);

	char cfgpath[4096];
	char* home=getenv("HOME");
	if(!home){
		comaerr(7,NULL);
	}
	snprintf(cfgpath,sizeof(cfgpath),"%s/.comarc",home);
	if(access(cfgpath,F_OK)!=0){
		comaerr(7,NULL);
	}
	parseConf(cfgpath);
	for(int i=1;i< argc;){		
		char* a=*(argv+i);
		if(strncmp(a,"-f=",3)==0){
			comaLambda(a+3);
			i++;
			continue;
		}
		if(strncmp(a,"-c=",3)==0){
			comac=1;
			run05(a+3);
			return 0;
		}
		if(strcmp(a,"-v")==0){
			versionIs();
			return 0;
		}
		if(strcmp(a,"-a")==0){
			printOps();
			return 0;
		}
		if(strncmp(a,"-d=",3)==0){
			if((*(a+3)< '0')
			  || (*(a+3) >'9')){
				comaerr(4,a);
			}
			rmOp(atoi(a+3));
			i++;
			continue;
		}
		comaerr(4,a);
	}
	for(int i=0;i< ops_s;i++){
		if((*(ops+i)).type=='g'){
			globb=1;
			break;
		}
	}
	if(!ps1){
		ps1=strdup("/bin/sh < coma < ");
	}

	if(!shell){
		shell=strdup("/bin/sh");
	}

	buildBreaks();
	rl_catch_signals=0;
	rl_initialize();
	rl_parse_and_bind(COMA_EL_KEY);
	if(tcgetattr(STDIN_FILENO,&termiossv)==0){
		savetio=1;
	}
	rl_attempted_completion_function=complete;
	rl_completion_query_items=16384;
	rl_completion_append_character='\0';
	rl_basic_word_break_characters=splitt;

	if(home!=NULL){
		snprintf(comahistory,sizeof(comahistory),"%s/.comahistory.txt",home);
		read_history(comahistory);
		atexit(historySave);
	}
	char* line;
	for(;;){
		if(sigsetjmp(jmp,1)){
			coma=0;
			if(savetio){
				tcsetattr(STDIN_FILENO,TCSANOW,&termiossv);
			}
			rl_cleanup_after_signal();
			rl_reset_after_signal();
			continue;
		}
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
			add_history(line);
			run05(line);
		}
		else{
			write(1,"\n",1);
		}
		free(line);
	}
	return 0;
}
