#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define DEF_BUFFER_SIZE 101
#define DEFAULT_APP_MAIN_DIR "C:\\poly_utility_app"
#define DEFAULT_DATA_FILE_NAME "poly_utility_app_data.daf"
#define DEFAULT_DATA_FILE_PATH "C:\\Program Files\\poly_utility_app_data.daf"
#define DEFAULT_CMD_INFO_FILE_NAME "poly_utility_cmd_info.txt"
#define DEFAULT_CMD_INFO_FILE_PATH "C:\\Program Files\\poly_utility_cmd_info.txt"
#define MAX_USERS 9
#define MAX_CHARS_PER_LINE 241
#define EXIT_CODE -1
#define BOOT_CODE 1
#define POST_ENTRY_CODE 2
#define DEF_TASK_LIST_SIZE 100
#define encryption_key 100

//Note: .daf files will be the primary extension used for user data and other files storing persistent application data

enum task_status_enum {NOT_SUBMITTED, SUBMITTED, OVERDUE, UPCOMING, FINISHED};
typedef enum task_status_enum task_status_enum;

enum is_group_task_enum {INDIVIDUAL, GROUP};
typedef enum is_group_task_enum is_group_task_enum;

enum task_type_enum {ASSIGNMENT, EXAM};
typedef enum task_type_enum task_type_enum;

struct Task
{
    char* task_name;

    task_type_enum task_type;

    task_status_enum task_status;

    is_group_task_enum is_group_task;
};
typedef struct Task Task;

struct User
{
    char* user_name;

    char* user_passwd;

    Task* task_list[DEF_TASK_LIST_SIZE];

    int task_count;
};
typedef struct User User;

//Global variables
User* current_app_user = NULL;
//char test_username[] = "test1";
//char test_passwd[] = "password";
User* users_list[MAX_USERS] = {0};
int total_users_count = 0;
User guest_user = {0};
int app_message = 0;
int password_mask_on = 1;

//Functions
void encrypt_daf_text(char* char_ptr)
{
    int char_index = 0;

    //printf("Starting encryption process...\n");

    while (char_ptr[char_index] != '\0')
    {
        //printf("Current decrypted char: %c\n", char_ptr[char_index]);

        char_ptr[char_index] = char_ptr[char_index] + encryption_key;

        //printf("Current encrypted char: %c\n", char_ptr[char_index]);

        char_index++;
    }
}

void decrypt_daf_text(char* char_ptr)
{
    int char_index = 0;

    //printf("Starting decryption process...\n");

    while (char_ptr[char_index] != '\0')
    {
        //printf("Current encrypted char: %c\n", char_ptr[char_index]);

        char_ptr[char_index] = char_ptr[char_index] - encryption_key;

        //printf("Current decrypted char: %c\n", char_ptr[char_index]);

        char_index++;
    }
}

int find_task_in_user_list_by_name(User* user_ptr, char* task_name)
{
    for (int i = 0; i < user_ptr->task_count; i++)
    {
        if (strcmp(user_ptr->task_list[i]->task_name, task_name) == 0)
        {
            //return the index in the user's task list at which the task with the same name was found at
            return i;
        }
    }

    //return an error code to indicate the task was not found in the user's task list
    return -1;
}

void add_task_to_user_list(User* user_ptr, Task* task_to_add)
{
    user_ptr->task_list[user_ptr->task_count] = task_to_add;

    user_ptr->task_count++;
}

int remove_task_from_user_list(User* user_ptr, int task_index)
{
    if (task_index >= user_ptr->task_count || task_index < 0)
    {
        //in future updates, an error message describing this error could also be written to the error logging file

        //return an easily identifiable error code
        return -1;
    }

    free(user_ptr->task_list[task_index]);

    for (int i = task_index; i < user_ptr->task_count; i++)
    {
        user_ptr->task_list[i] = user_ptr->task_list[i + 1];
    }

    user_ptr->task_count -= 1;

    //return a default value of 0 to indicate no error and success
    return 0;
}

void print_tasks_in_list(Task** task_list_start_ptr, int task_list_size)
{
    if (task_list_size == 0)
    {
        printf("No assignments and exams found.\n");

        return;
    }

    printf("\nCurrent assignments and exams:\n");

    for (int i = 0; i < task_list_size; i++)
    {
        if (task_list_start_ptr[i]->task_type == ASSIGNMENT)
        {
            printf("%d. Assignment\n", i + 1);
        }
        else
        {
            printf("%d. Exam\n", i + 1);
        }

        printf("Name: %s\n", task_list_start_ptr[i]->task_name);

        if (task_list_start_ptr[i]->is_group_task == INDIVIDUAL)
        {
            printf("Type: %s\n", "Individual");
        }
        else
        {
            printf("Type: %s\n", "Group");
        }

        if (task_list_start_ptr[i]->task_status == NOT_SUBMITTED)
        {
            printf("Status: %s\n", "Not Submitted");
        }
        else if (task_list_start_ptr[i]->task_status == SUBMITTED)
        {
            printf("Status: %s\n", "Submitted");
        }
        else if (task_list_start_ptr[i]->task_status == OVERDUE)
        {
            printf("Status: %s\n", "Overdue");
        }
        else if (task_list_start_ptr[i]->task_status == UPCOMING)
        {
            printf("Status: %s\n", "Upcoming");
        }
        else if (task_list_start_ptr[i]->task_status == FINISHED)
        {
            printf("Status: %s\n", "Finished");
        }

        printf("\n");
    }
}

void print_tasks_for_user(User* user_ptr)
{
    print_tasks_in_list(user_ptr->task_list, user_ptr->task_count);
}

void remove_user_in_list(User** user_list_start_ptr, int user_index)
{
    free(user_list_start_ptr[user_index]);

    for (int i = user_index; i < total_users_count; i++)
    {
        user_list_start_ptr[i] = user_list_start_ptr[i + 1];
    }

    total_users_count -= 1;
}

int iterate_through_chars(char* start_char_ptr)
{
    int char_index = 0;

    while (start_char_ptr[char_index] != '\0')
    {
        //printf("Current char (char value): %c\n", start_char_ptr[char_index]);

        //printf("Current char (decimal value): %d\n", start_char_ptr[char_index]);

        char_index++;
    }

    int string_len_with_null = char_index + 1;

    //printf("String length: %d\n", string_len_with_null);

    return string_len_with_null;
    //returns an integer containing the length of the string including the sentinel null termination character
}

void initialize_char_ptr_content(char* char_start_ptr)
{
    int char_index = 0;

    while (char_start_ptr[char_index] != '\0')
    {
        //printf("Current char (char value): %c\n", char_start_ptr[char_index]);

        //printf("Current char (decimal value): %d\n", char_start_ptr[char_index]);

        char_start_ptr[char_index] = 0;

        char_index++;
    }
}

char* get_user_tasks_data_name(User* user_ptr)
{
    char* user_tasks_file_name = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

    initialize_char_ptr_content(user_tasks_file_name);

    strcat(user_tasks_file_name, user_ptr->user_name);

    strcat(user_tasks_file_name, "_tasks.daf");

    //printf("current user: %s\n", user_ptr->user_name);

    //printf("current user file name: %s\n", user_tasks_file_name);

    //e.g. if user's name == arcane, then file name == arcane_tasks.daf

    return user_tasks_file_name;
}

void load_user_tasks_data(User* user_ptr)
{
    char* current_user_task_file_name = get_user_tasks_data_name(user_ptr);

    FILE* user_tasks_file_ptr = fopen(current_user_task_file_name, "r+");

    if (user_tasks_file_ptr == NULL)
    {
        printf("Error: Unable to read from/create tasks data file for user '%s'.\n", user_ptr->user_name);

        return;
    }

    user_ptr->task_count = 0;

    char temp_task_name[DEF_BUFFER_SIZE] = {0};

    int temp_task_type = 0;

    int temp_task_status = 0;

    int temp_is_group_task = 0;

    while (fscanf(user_tasks_file_ptr, "%s %d %d %d\n", temp_task_name, &temp_task_type, &temp_task_status, &temp_is_group_task) != EOF)
    {
        Task* new_task_ptr = (Task*)malloc(sizeof(Task));

        new_task_ptr->task_name = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

        strcpy(new_task_ptr->task_name, temp_task_name);

        new_task_ptr->task_type = temp_task_type;

        new_task_ptr->task_status = temp_task_status;

        new_task_ptr->is_group_task = temp_is_group_task;

        user_ptr->task_list[user_ptr->task_count] = new_task_ptr;

        user_ptr->task_count++;
    }
}

void save_user_tasks_data(User* user_ptr)
{
    FILE* user_tasks_file_ptr = fopen(get_user_tasks_data_name(user_ptr), "w");

    if (user_tasks_file_ptr == NULL)
    {
        printf("Error: Unable to write to/create tasks data file for user '%s'.\n", user_ptr->user_name);

        return;
    }

    for (int i = 0; i < user_ptr->task_count; i++)
    {
        fprintf(user_tasks_file_ptr, "%s %d %d %d\n", user_ptr->task_list[i]->task_name, user_ptr->task_list[i]->task_type, user_ptr->task_list[i]->task_status, user_ptr->task_list[i]->is_group_task);
    }
}

void load_data()
{
    //test_user.user_name = test_username;

    //printf("test user's username: %s\n", test_user.user_name);

    //test_user.user_passwd = test_passwd;

    //printf("test user's password: %s\n", test_user.user_passwd);

    FILE* data_file_ptr = fopen(DEFAULT_DATA_FILE_NAME, "r");

    if (data_file_ptr == NULL)
    {
        //printf("App data file not found, creating one...\n");

        data_file_ptr = fopen(DEFAULT_DATA_FILE_NAME, "r+");
    }
    else
    {
        //printf("App data file found successfully.\n");

        int users_list_i = 0;

        char temp_user_name[DEF_BUFFER_SIZE] = {0};

        char temp_user_passwd[DEF_BUFFER_SIZE] = {0};

        while (fscanf(data_file_ptr, "%s %s", temp_user_name, temp_user_passwd) != EOF)
        {
            /*
            decrypt the encrypted user names and passwords of each user only when the app starts up (so as to prevent
            data from being decrypted multiple times)
            */

            if (app_message == BOOT_CODE)
            {
                decrypt_daf_text(temp_user_name);

                decrypt_daf_text(temp_user_passwd);
            }

            if (users_list_i > MAX_USERS - 1)
            {
                break;
            }

            //printf("New user found.\n");

            //printf("Retrieved user name: %s\n", temp_user_name);

            //printf("Retrieved password: %s\n", temp_user_passwd);

            User* new_user = (User*)malloc(sizeof(User));

            new_user->task_count = 0;

            new_user->user_name = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

            new_user->user_passwd = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

            strcpy(new_user->user_name, temp_user_name);

            strcpy(new_user->user_passwd, temp_user_passwd);

            //printf("User's name: %s\n", new_user.user_name);

            //printf("User's password: %s\n\n", new_user.user_passwd);

            load_user_tasks_data(new_user);

            users_list[users_list_i] = new_user;

            total_users_count++;

            users_list_i++;
        }

        fclose(data_file_ptr);
    }
}

void save_data()
{
    FILE* app_data_file_ptr = fopen(DEFAULT_DATA_FILE_NAME, "w");

    for (int i = 0; i < total_users_count; i++)
    {
        //printf("decrypted user name: %s\n", users_list[i].user_name);

        //printf("decrypted user password: %s\n", users_list[i].user_passwd);

        /*
        encrypts the user's username and password before writing them into the app data file only when app is about to
        terminate (to prevent encryption from occurring multiple times)
        */
        save_user_tasks_data(users_list[i]);

        if (app_message == EXIT_CODE)
        {
            encrypt_daf_text(users_list[i]->user_name);

            encrypt_daf_text(users_list[i]->user_passwd);
        }

        //printf("encrypted user name: %s\n", users_list[i].user_name);

        //printf("encrypted user password: %s\n", users_list[i].user_passwd);

        //printf("Encrypted text: %s\n", encrypted_text);

        fprintf(app_data_file_ptr, "%s %s\n", users_list[i]->user_name, users_list[i]->user_passwd);
    }

    fclose(app_data_file_ptr);
}

void init_main_menu()
{
    printf("Username: ");

    char* temp_name_buffer = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

    scanf("%s", temp_name_buffer);

    printf("Password: ");

    char* temp_passwd_buffer = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

    int passwd_buffer_i = 0;

    while ((temp_passwd_buffer[passwd_buffer_i] = getch()) != '\r')
    {
        if (password_mask_on == 1)
        {
            printf("*");
        }
        else
        {
            printf("%c", temp_passwd_buffer[passwd_buffer_i]);
        }

        passwd_buffer_i++;
    }

    temp_passwd_buffer[passwd_buffer_i] = '\0';

    printf("\n");

    //printf("Entered password: %s\n", temp_passwd_buffer);

    //printf("temp_name_buffer current address: %p\n", temp_name_buffer);

    //printf("temp_passwd_buffer current address: %p\n", temp_passwd_buffer);

    //printf("Result of username check: %d\n", strcmp(test_user.user_name, temp_name_buffer));

    //printf("temp_name_buffer iteration check:\n");

    //iterate_through_chars(temp_name_buffer);

    //printf("test_user.user_name iteration check:\n");

    //iterate_through_chars(test_user.user_name);

    //Iterate through users in users list
    for (int i = 0; i < total_users_count; i++)
    {
        User* current_user = users_list[i];

        if (current_user->user_name == NULL && current_user->user_passwd == NULL)
        {
            continue;
        }

        //printf("\nNew user found\n");

        //printf("User's name: %s\n", current_user.user_name);

        //printf("User's password: %s\n\n", current_user.user_passwd);
    }

    for (int i = 0; i <= total_users_count; i++)
    {
        if (i == total_users_count)
        {
            printf("Incorrect username and/or password entered. Login failed.\n");

            return;
        }

        User* current_user = users_list[i];

        if (current_user->user_name == NULL && current_user->user_passwd == NULL)
        {
            continue;
        }

        if (strcmp(current_user->user_name, temp_name_buffer) == 0 && strcmp(current_user->user_passwd, temp_passwd_buffer) == 0)
        {
            //printf("Unlocked.\n");

            current_app_user = current_user;

            //Unlock();

            break;
        }
    }

    free(temp_name_buffer);

    free(temp_passwd_buffer);
}

void register_account()
{
    printf("Enter a new username: ");

    char temp_name_buffer[DEF_BUFFER_SIZE] = {0};

    scanf("%s", temp_name_buffer);

    printf("Enter a new password: ");

    char temp_passwd_buffer[DEF_BUFFER_SIZE] = {0};

    scanf("%s", temp_passwd_buffer);

    if (strcmp(temp_name_buffer, "") != 0 && strcmp(temp_passwd_buffer, "") != 0)
    {
        if (total_users_count == MAX_USERS - 1)
        {
            printf("Unable to register the new account. You have reached the maximum number of users.\n");

            return;
        }

        User* created_user = (User*)malloc(sizeof(User));

        created_user->user_name = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

        created_user->user_passwd = (char*)malloc(sizeof(char) * DEF_BUFFER_SIZE);

        strcpy(created_user->user_name, temp_name_buffer);

        strcpy(created_user->user_passwd, temp_passwd_buffer);

        total_users_count++;

        users_list[total_users_count - 1] = created_user;

        save_data();

        printf("Account created successfully.\n");
    }
    else
    {
        printf("Please enter a valid username/password, the username and password fields cannot be blank.\n");

        return;
    }
}

void delete_account()
{
    printf("Enter the username of the account that you would like to delete: ");

    char temp_name_buffer[DEF_BUFFER_SIZE] = {0};

    scanf("%s", temp_name_buffer);

    printf("Enter the password of the account that you would like to delete: ");

    char temp_passwd_buffer[DEF_BUFFER_SIZE] = {0};

    scanf("%s", temp_passwd_buffer);

    for (int i = 0; i < total_users_count; i++)
    {
        if (strcmp(users_list[i]->user_name, temp_name_buffer) == 0 && strcmp(users_list[i]->user_passwd, temp_passwd_buffer) == 0)
        {
            FILE* app_data_file_ptr = fopen(DEFAULT_DATA_FILE_NAME, "r+");

            if (app_data_file_ptr == NULL)
            {
                printf("Error: App data file not found.\n");

                return;
            }

            remove_user_in_list(users_list, i);

            save_data();

            printf("'%s' account deleted successfully.\n", temp_name_buffer);

            return;
        }
    }

    printf("'%s' account not found.\n", temp_name_buffer);
}

/*
void add_assignment_for_user(User* user_ptr, Task* task_ptr)
{
    user_ptr->task_list[user_ptr->task_count] = *(task_ptr);

    user_ptr->task_count++;
}

int get_user_assignment_index_by_name(User* user_ptr, char* task_name)
{
    for (int i = 0; i < user_ptr->task_count; i++)
    {
        if (strcmp(user_ptr->task_list[i].task_name, task_name) == 0)
        {
            //return the index of the task that has been found with the same name
            return i;
        }
    }

    //return error code if no task with the same name was found
    return -1;
}

void remove_assignment_for_user(User* user_ptr, int task_index)
{
    free(user_ptr->task_list[task_index].task_name);

    for (int i = task_index; i < user_ptr->task_count; i++)
    {
        user_ptr->task_list[i] = user_ptr->task_list[i + 1];
    }
}
*/

void create_app_dir()
{
    /*
    if (PathFileExists(DEFAULT_APP_MAIN_DIR) != TRUE)
    {
        printf("Main app directory not found, creating one now...\n");

        _mkdir(DEFAULT_APP_MAIN_DIR);

        printf("Main app directory created successfully.\n");
    }
    */
}

int check_user_files_exist(User* user_ptr)
{
    //checks if the user's own directory and internal files (e.g. tasks file) exist, otherwise, it creates them automatically


    return 0;
}

int main()
{
    app_message = BOOT_CODE;

    printf("Starting application...\n");

    //create_app_dir();

    current_app_user = &guest_user;

    load_data();

    printf("Application start up successful.\nUse 'help' to view a list of all commands currently available.\n");

    app_message = POST_ENTRY_CODE;

    while (app_message != EXIT_CODE)
    {
        //printf("Current app user value: %p\n", current_app_user);

        if (current_app_user == &guest_user)
        {
            printf("> ");
        }
        else
        {
            //printf("Execute for non-guest\n");

            char prompt_line[DEF_BUFFER_SIZE] = {0};

            strcat(prompt_line, "@");

            strcat(prompt_line, current_app_user->user_name);

            strcat(prompt_line, "> ");

            printf(prompt_line);
        }

        char command[DEF_BUFFER_SIZE] = {0};

        scanf("%s", command);

        if (strcmp(command, "login") == 0)
        {
            if (current_app_user == &guest_user)
            {
                init_main_menu();
            }
            else
            {
                printf("You are currently logged into an account, you will need to logout first in order to login to another account, please use the 'logout' command to log out of your current account.\n");
            }

            //printf("Exited main menu.\n");
        }
        else if (strcmp(command, "logout") == 0)
        {
            if (current_app_user == &guest_user)
            {
                printf("You are currently not logged into any account.\n");
            }
            else
            {
                printf("Logged out of account '%s' successfully.\n", current_app_user->user_name);

                current_app_user = &guest_user;
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            app_message = EXIT_CODE;

            save_data();
        }
        else if (strcmp(command, "help") == 0)
        {
            FILE* command_info_file_ptr = fopen(DEFAULT_CMD_INFO_FILE_NAME, "r");

            //Error handling for file not being found
            if (command_info_file_ptr == NULL)
            {
                printf("Error: command info file not found.\n");

                continue;
            }

            char command_info_line[MAX_CHARS_PER_LINE];

            while (fgets(command_info_line, sizeof(command_info_line), command_info_file_ptr) != NULL)
            {
                //print out each line of the command info file
                printf(command_info_line);
            }

            fclose(command_info_file_ptr);
        }
        else if (strcmp(command, "register") == 0)
        {
            if (current_app_user == &guest_user)
            {
                register_account();
            }
            else
            {
                printf("You are currently logged into another account. You will need to log out of your account first in order to register/create a new account. You can use the 'logout' command to log out of your account.\n");
            }
        }
        else if (strcmp(command, "delete_account") == 0)
        {
            if (current_app_user == &guest_user)
            {
                delete_account();
            }
            else
            {
                printf("You are currently logged into another account. You will need to log out of your account first in order to delete an existing account. You can use the 'logout' command to log out of your account.\n");
            }
        }
        else if (strcmp(command, "clear") == 0)
        {
            //Use system("cls") to clear the console for Windows OS, for Linux OS, use system("clear") instead
            system("cls");
        }
        else if (strcmp(command, "add_assignment") == 0)
        {
            if (current_app_user == &guest_user)
            {
                printf("You are currently not logged into any account yet. You must be logged into an account to use this feature.\n");

                continue;
            }

            Task* new_user_task_ptr = (Task*)malloc(sizeof(Task));

            new_user_task_ptr->task_type = ASSIGNMENT;

            new_user_task_ptr->task_name = malloc(sizeof(char) * DEF_BUFFER_SIZE);

            printf("Enter the name of the assignment (must not have spaces): ");

            scanf("%s", new_user_task_ptr->task_name);

            printf("Is the task a group task? (y/n) ");

            char temp_char = 0;

            scanf("\n%c", &temp_char);

            if (temp_char == 'y' || temp_char == 'Y')
            {
                new_user_task_ptr->is_group_task = GROUP;
            }
            else if (temp_char == 'n' || temp_char == 'N')
            {
                new_user_task_ptr->is_group_task = INDIVIDUAL;
            }
            else
            {
                printf("Invalid response. Please answer with y or Y for yes, otherwise n or N for no.\n");

                continue;
            }

            new_user_task_ptr->task_status = NOT_SUBMITTED;

            add_task_to_user_list(current_app_user, new_user_task_ptr);

            printf("New assignment '%s' created successfully.\n", new_user_task_ptr->task_name);
        }
        else if (strcmp(command, "remove_assignment") == 0)
        {
            if (current_app_user == &guest_user)
            {
                printf("You are currently not logged into any account yet. You must be logged into an account to use this feature.\n");

                continue;
            }

            printf("Enter the name of the assignment you would like to remove: ");

            char temp_name_buffer[DEF_BUFFER_SIZE] = {0};

            scanf("%s", temp_name_buffer);

            int task_search_result = find_task_in_user_list_by_name(current_app_user, temp_name_buffer);

            if (task_search_result == -1)
            {
                printf("Task not found.\n");

                continue;
            }

            remove_task_from_user_list(current_app_user, task_search_result);

            printf("Assignment '%s' removed successfully.\n", temp_name_buffer);
        }
        else if (strcmp(command, "view_assignments") == 0)
        {
            if (current_app_user == &guest_user)
            {
                printf("You are currently not logged into any account yet. You must be logged into an account to use this feature.\n");

                continue;
            }

            print_tasks_for_user(current_app_user);
        }
        else if (strcmp(command, "recover_password") == 0)
        {
            if (current_app_user == &guest_user)
            {
                printf("You are currently logged into an account. To recover the password of an account, you must be logged out. You can do so using the 'logout' command.\n");

                continue;
            }

            printf("Enter the username of the account you would like to recover the password for: ");

            char temp_name_buffer[DEF_BUFFER_SIZE] = {0};

            scanf("%s", temp_name_buffer);

            printf("Starting password recovery, please wait...\n");

            int account_found = 0;

            for (int i = 0; i < total_users_count; i++)
            {
                if (strcmp(users_list[i]->user_name, temp_name_buffer) == 0)
                {
                    printf("Password for account '%s' found.\n", temp_name_buffer);

                    printf("The password for this account is: %s\n", users_list[i]->user_passwd);

                    account_found = 1;
                }
            }

            if (account_found == 0)
            {
                printf("No such account exists currently. Unable to recover password for '%s' account. Please ensure you have entered the correct username and try again.\n", temp_name_buffer);
            }
        }
        else
        {
            printf("Invalid command entered. Please try again or use 'help' to see a list of valid commands that are currently available.\n");
        }
    }

    return 0;
}
