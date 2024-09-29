#include <unistd.h>
#include <sys/io.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_misc.h>

//#define PORT 	0x278
//#define PORT 0x378
#define PORT 0x3BC
#define DATA 	0
#define STATUS 	1
#define CONTROL 2
#define USERNAMESIZE 1024

static int test_conv(int num_msg, 
		     const struct pam_message **msgm,
		     struct pam_response **response, 
		     void *appdata_ptr)
{
  printf("%d tuli\n", num_msg);
  return 0;
}

static struct pam_conv conv = { 
  misc_conv,
  NULL
};

static int cleanup_func(pam_handle_t *pamh, void *data, int error_status)
{
  printf("Cleaning up!\n");
  return PAM_SUCCESS;
}

int
openDoor(struct passwd passwd)
{
  int status;
  printf("\nKerberos steps aside..\n");
  if (fork() == 0) {
    initgroups(passwd.pw_name, passwd.pw_gid);
    setgid(passwd.pw_gid);
    setreuid(passwd.pw_uid, passwd.pw_uid);
    if (system("/opt/local/bin/sesam") == 0) {
      return 1;
    } else {
      return 0;
    }
  }
  wait(&status);
  return status;
}

int
authenticate(struct passwd* passwd)
{
  pam_handle_t *pamh;
  char	*name = ( char *) malloc( USERNAMESIZE + 1 );
  char	*p = NULL; 
  char	*s = NULL;
  int retcode = 0;

  if (!name) {
    perror( "Heaven collapses!");
    exit( -1 );
  }

  printf("\nKerberos yells: Who shall enter! \n[hp 1/1] say ");
  name = fgets( name, USERNAMESIZE, stdin );
  if (!name) {
    printf("\n");
    return 0;
  }
	
  *(name + strlen(name) - 1) = 0;

  pam_start("ovi", name, &conv, &pamh);

#if 1
  pam_set_item(pamh, PAM_AUTHTOK, p);
#endif

  retcode = (pam_authenticate(pamh, 0) == PAM_SUCCESS);

  //retcode = (retcode == PAM_SUCCESS) ? pam_authenticate(pamh, 0) == PAM_SUCCESS : retcode;

  if (retcode) {
    char* user;
    retcode = pam_get_item(pamh, PAM_USER, (const void **) &user);
    if (retcode == PAM_SUCCESS) {
      struct passwd* pwd;
      setpwent();
      pwd = getpwnam(user);
      if (pwd && pwd->pw_uid) {
        *passwd = *pwd;
        retcode = 1;
      } else {
        retcode = 0;
      }
    } else {
      retcode = 0;
    }
  } else {
    retcode = 0;
  }

  pam_end(pamh, PAM_SUCCESS);

  return retcode;
}

int
main()
{
  fflush(stdout);
  while (1) {
    struct passwd passwd;
    printf("\e[H\e[J");
    printf("You see here a portal. There is a nice little dog by the gateway.\n\n"); 
    if (authenticate(&passwd)) {
      if (openDoor(passwd)) {
        printf("You open the door with your key.\n");
        sleep(1);
      } else {
        printf("You must possess the key to enter!\n");
        sleep(2);
      }
    } else {
      printf("Kerberos bites you! Kerberos bites you! You die.\n");
      sleep(2);
    }
  }
  return 0;
}



