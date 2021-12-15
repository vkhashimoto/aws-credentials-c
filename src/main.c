#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "logging/logging.h"

/*int main() {
    printf("OIOI");
    return 0;
}*/

void change_credentials(char *flags[]) {
    char *endingCredential = "";
    char *startingCredential = malloc(strlen(flags[profile] + 1));
    char *newCredential = malloc(strlen(flags[profile]) + 1 + strlen( strrchr( flags[credential], ']') ) + 1);
    DEBUG("Assigning endingCredential string.");
    endingCredential = strrchr(flags[credential], ']');
    DEBUG("Concatenating startingCredential string.");
    strcat(startingCredential, "[");
    DEBUG("Concatenating startingCredential string.");
    strcat(startingCredential, flags[profile]);
    DEBUG("Concatenating newCredential string.");
    strcat(newCredential, startingCredential);
    DEBUG("Concatenating newCredential string.");
    strcat(newCredential, endingCredential);


    DEBUG("Assigning newCredential to the flags.");
    flags[credential] = newCredential;
}

void write_credential_to_file(char *credentials) {
    FILE *file_credentials;
    DEBUG("Opening file.");
    file_credentials = fopen("/tmp/.aws/credentials", "w");
    DEBUG("Writing to file.");
    fprintf(file_credentials, "%s\n\n", credentials);
    DEBUG("Closing file.");
    fclose(file_credentials);
}

void write_to_file(char *text) {
    FILE *file;
    file = fopen("/tmp/.aws/credentials.new", "a");
    fprintf(file, "%s\n", text);
    fclose(file);
}

char* read_file(char *content) {
    FILE *file = fopen("/tmp/.aws/credentials", "r");
    if (file != NULL) {
        /* go to the end of the file */
        if (fseek(file, 0L, SEEK_END) == 0) {
            /* get the size of the file */
            long bufsize = ftell(file);
            if (bufsize == -1 ) { /* error */
                printf("ERROR BUFSIZE");
            }
            content = malloc(sizeof(char) * (bufsize + 1));
            if (fseek(file, 0L, SEEK_SET) != 0) { /* error */
                printf("ERROR FSEEK");
            }
            size_t newLen = fread(content, sizeof(char), bufsize, file);
            if (ferror(file) != 0) {
                printf("Error reading file");
            } else {
                content[newLen++] = '\0';
            }
        }
        fclose(file);
        return content;
    }
}

int read_credentials(char *flags[]) {
    char *content = NULL;
    const char delimiter[2] = "\n";
    int found_profile = 0;
    int found_profile_already_written = 0;

    content = read_file(content);
    char *to_tokenize[strlen(content)];
    memcpy(to_tokenize, content, strlen(content) + 1);
    /* must check with brackets */
    /* if (strstr(content, flags[profile]) != NULL) { */
    char *token = strtok(to_tokenize, delimiter);
    while (token != NULL) {
        /* Change credentials */
        if (found_profile == 1) {
            char* profiletag;
            if(0 > asprintf(&profiletag, "[%s]", flags[profile])) {
                printf("ERROR FORMATING PROFILE TAG");
            }
            write_to_file(profiletag);
            free(profiletag);
            write_to_file(flags[credential]);
            found_profile = 0;
            found_profile_already_written = 1;
            /* skip wirting the 3 lines with old credentials for this profile */
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            continue;
        }
        if (found_profile == 0) {
            if (strcmp(token, "[profile234]") == 0 && found_profile_already_written == 0) {
                found_profile = 1;
            } else {
                DEBUGF("WRITING %s TO FILE\n", token);
                write_to_file(token);
            }
        }
        token = strtok(NULL, delimiter);
    }
    if (found_profile_already_written == 0) {
            char* profiletag;
        if(0 > asprintf(&profiletag, "[%s]", flags[profile])) {
            printf("ERROR FORMATING PROFILE TAG");
        }
        write_to_file(profiletag);
        free(profiletag);
        write_to_file(flags[credential]);
    }
    /* } else {
        char* profiletag;
        if(0 > asprintf(&profiletag, "[%s]", flags[profile])) {
            printf("ERROR FORMATING PROFILE TAG");
        }
        write_to_file(profiletag);
        free(profiletag);
        write_to_file(flags[credential]);
    }
    printf("\n\n\n"); */
    free(content);
    return 1;
}

void rename_files() {
    /* backup */
    int ret;
    char backup_old_name[] = "/tmp/.aws/credentials";
    char backup_new_name[] = "/tmp/.aws/credentials.bkp";
    char file_old_name[] = "/tmp/.aws/credentials.new";
    char file_new_name[] = "/tmp/.aws/credentials";
    ret = rename(backup_old_name, backup_new_name);
        
    if(ret == 0) {
        printf("File renamed successfully");
    } else {
        printf("Error: unable to rename the file backup");
    }
    /* new file */
    ret = rename(file_old_name, file_new_name);
        
    if(ret == 0) {
        printf("File renamed successfully");
    } else {
        printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
    }
}

int main (int argc, char *argv[]) {
    handle_debug_flag(argc, argv);
    char *flags[FLAGS_SIZE];
    

    DEBUG("Calling handle_flags.");
    handle_flags(argc, argv, flags);
    /* DEBUG("Calling change_credentials.");
    change_credentials(flags);

    DEBUGF("Porifle: %s", flags[profile]);
    DEBUGF("Credentials: %s", flags[credential]);

    DEBUG("Calling write_credentials_to_file.");
    write_credential_to_file(flags[credential]);

    DEBUG("Finishing program."); */
    /* Create a temp credential file and then substitute the file */
    if (read_credentials(flags) == 1) {
        rename_files();
    }
    return 0;
}