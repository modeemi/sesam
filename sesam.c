#include <unistd.h>
#include <sys/io.h>
#include <stdio.h>
#include <sys/file.h>
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h> 
#include <ctype.h>
#include <sys/signal.h>

//#define PORT 	0x278
//#define PORT 0x378
#define PORT 0x3BC
#define DATA 	0
#define STATUS 	1
#define CONTROL 2

char*
secure(char* a)
{
  static char buffer[64];
  int c;
  if (!a) return "";
  strncpy(buffer, a, 64);
  buffer[sizeof(buffer) - 1] = 0;
  
  for (c = 0; buffer[c]; ++c) {
    if (!isalnum(buffer[c])) {
      switch (buffer[c]) {
      case '_':
      case '-':
      case '/':
      case ' ':
      case '.':
        break;
      default:
        buffer[c] = 0;
      }
    }
  }

  return buffer;
}

int
main(int argc, char** argv)
{
  char timeBuffer[1024];
  time_t now = time(0);
  struct passwd* passwd;
  int uid = getuid();
  char* argument;
  int c;
  FILE* f = fopen("/status/sesam.log", "a");

  if (!f) {
    fprintf(stderr, "failed to open log\n");
    exit(0);
  }

  for (c = 1; c < 32; ++c) {
    signal(c, SIG_IGN);
  }
    
  flock(fileno(f), LOCK_EX);

#if 1
  iopl(3);
  outb(0xff, PORT + DATA);
  sleep(1);
  outb(0x00, PORT + DATA);
  sleep(1);
  outb(0xff, PORT + DATA);
  iopl(0);
#endif

///////////////////////////////////////////////
  setreuid(-1, -1);
///////////////////////////////////////////////

  if (argc >= 2) {
    argument = argv[1];
  } else {
    argument = "";
  }

  strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", 
           localtime(&now));

  passwd = getpwuid(uid);

  fprintf(f, "%s\t%s\t%d\t", timeBuffer, passwd ? passwd->pw_name : "***" , getuid());
  fprintf(f, "%s\t", secure(argument));
  fprintf(f, "%s\t", secure(getenv("SSH_CLIENT")));
  fprintf(f, "%s\n", secure(ttyname(0)));

  flock(fileno(f), LOCK_UN);
  fclose(f);

  return 0;
}
