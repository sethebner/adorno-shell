#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
// here
#define TRUE    1
#define FALSE   0

#define ALLOCATION_FAILURE          1
#define FAILURE                     2
#define SHELL_CWD_BUFFER_SIZE       1024
#define SHELL_HISTORY_SIZE          10
#define SHELL_READLINE_BUFFER_SIZE  1024
#define SHELL_TOKEN_BUFFER_SIZE     64
#define SHELL_TOKEN_DELIMITERS      " \t\r\n\a"
#define SUCCESS                     0

#define array_cnt(x)    ( sizeof( x ) / sizeof( x[0] ) )

typedef struct command
    {
        int (*function_ptr) (char**);
        char* cmd_string;
        char* help_string;
    }  command;

// FORWARD DECLARATIONS

int cmd_back
    (
        char** args
    );

int cmd_back_list
    (
        char** args
    );

int cmd_cd
    (
        char** args
    );

int cmd_cd_list
    (
        char** args
    );

int cmd_clear
    (
        char** args
    );

int cmd_emacs
    (
        char** args
    );

int cmd_exit
    (
        char** args
    );

int cmd_help
    (
        char** args
    );

static const command commands[] = 
    {
        {&cmd_back, "b", "change to the parent directory"},
        {&cmd_back_list, "bl", "change to the parent directory and list directory contents"},
        {&cmd_clear, "c", "clear the screen"},
        {&cmd_cd, "cd", "change directory"},
        {&cmd_cd_list, "cdl", "change directory and list directory contents"},
        {&cmd_emacs, "e", "launch emacs"},
        {&cmd_exit, "exit", "exit shell"},
        {&cmd_help, "help", "display help"},
    };

char* shell_read_line
    (
        void
    )
{
    // Local variables
    char*   line;
    size_t  buffer_size;

    // Initialize local variables
    line        = NULL;
    buffer_size = 0;

    getline( &line, &buffer_size, stdin );

    return line;
} /* shell_read_line() */

char** shell_split_line
    (
        char* line
    )
{
    // Local variables
    int     buffer_size;
    int     position;
    char**  tokens;
    char*   token;

    // Initialize local variables
    buffer_size = SHELL_TOKEN_BUFFER_SIZE;
    position    = 0;
    tokens      = malloc( sizeof( char* ) * buffer_size );

    if( !tokens )
    {
        fprintf( stderr, "Allocation error\n" );
        exit( ALLOCATION_FAILURE );
    }

    token = strtok( line, SHELL_TOKEN_DELIMITERS );
    while( NULL != token )
    {
        tokens[position] = token;
        position++;

        if( position >= buffer_size )
        {
            buffer_size += SHELL_TOKEN_BUFFER_SIZE;
            tokens = realloc( tokens, sizeof( char* ) * buffer_size );

            if( !tokens )
            {
                fprintf( stderr, "Allocation error\n" );
                exit( ALLOCATION_FAILURE );
            }
        }

        token = strtok( NULL, SHELL_TOKEN_DELIMITERS );
    }

    tokens[position] = NULL;
    return tokens;
} /* shell_split_line() */

int shell_launch
    (
        char** args
    )
{
    // Local variables
    pid_t   pid;
    pid_t   wpid;
    int     status;

    pid = fork();
    if( 0 == pid )
    {
        if( -1 == execvp( args[0], args ) )
        {
            perror( "shell" );
        }
        exit( FAILURE );
    }
    else if( pid < 0 )
    {
        perror( "shell" );
    }
    else
    {
        do
        {
            wpid = waitpid( pid, &status, WUNTRACED );
        }   while( !WIFEXITED( status ) && !WIFSIGNALED( status ) );
    }

    return TRUE;
} /* shell_launch() */

int cmd_back
    (
        char** args
    )
{
    args[0] = "cd";
    args[1] = "..";
    return cmd_cd( args );
} /* cmd_back() */

int cmd_back_list
    (
        char** args
    )
{
    cmd_back( args );

    args[0] = "ls";
    args[1] = NULL;
    return shell_launch( args );
} /* cmd_back_list() */

int cmd_cd
    (
        char** args
    )
{
    if( NULL == args[1] )
    {
        fprintf( stderr, "Expected another argument.\n" );
    }
    else
    {
        if( chdir( args[1] ) != 0 )
        {
            perror( "shell" );
        }
    }

    return TRUE;
} /* cmd_cd() */

int cmd_cd_list
    (
        char** args
    )
{
    cmd_cd( args );

    args[0] = "ls";
    args[1] = NULL;
    return shell_launch( args );
} /* cmd_cd_list() */

int cmd_clear
    (
        char** args
    )
{
    args[0] = "clear";
    return shell_launch( args );
} /* cmd_clear() */

int cmd_emacs
    (
        char** args
    )
{
    if( NULL == args[1] )
    {
        fprintf( stderr, "Expected another argument.\n" );
        return TRUE;
    }
    else
    {
        args[0] = "emacs";
        return shell_launch( args );
    }
} /* cmd_emacs() */

int cmd_exit
    (
        char** args
    )
{
    return SUCCESS;
} /* cmd_exit() */

int cmd_help
    (
        char** args
    )
{
    // Local variables
    int i;

    for( i = 0; i < array_cnt( commands ); i++ )
    {
        printf( "\t%s\t%s\n", commands[i].cmd_string, commands[i].help_string );
    }

    return TRUE;
} /* cmd_help() */

int shell_execute
    (
        char** args
    )
{
    // Local variables
    int i;

    if( NULL == args[0] )
    {
        return TRUE;
    }

    for( i = 0; i < array_cnt( commands ); i++ )
    {
        if( 0 == strcmp( args[0], commands[i].cmd_string ) )
        {
            return (*(commands[i].function_ptr))(args);
        }
    }

    return shell_launch( args );
} /* shell_execute() */

void shell_loop
    (
    void
    )
{
    // Local variables
    char*   line;
    char**  args;
    int     status;
    char    cwd[SHELL_CWD_BUFFER_SIZE];
    char*   printable_cwd;

    do
    {
        // Display the prompt
        getcwd( cwd, sizeof( cwd ) );
        printable_cwd = strrchr( cwd, '/' );
        if( '/' == printable_cwd[0] )
        {
            printable_cwd++;
        }
        printf( "adorno::%s> ", printable_cwd );

        line    = shell_read_line();
        args    = shell_split_line( line );
        status  = shell_execute( args );

        free( line );
        free( args );
    }   while( status );

} /* shell_loop() */

int main
    (
    int     argc,
    char**  argv
    )
{
    shell_loop();

    return SUCCESS;
} /* main() */
