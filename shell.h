typedef struct
    {
        int (*function_ptr) (char**);
        char* cmd_string;
        char* help_string;
    }   command_type;

char* shell_read_line
    (
        void
    );

char** shell_split_line
    (
        char* line
    );

int shell_launch
    (
        char** args
    );

int shell_execute
    (
        char** args
    );

void shell_loop
    (
    void
    );

void shell_intro
    (
        void
    );

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

int cmd_list
    (
        char** args
    );

int cmd_sublime_text_2
    (
        char** args
    );

int cmd_vi
    (
        char** args
    );
