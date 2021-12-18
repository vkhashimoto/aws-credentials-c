#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "logging/logging.h"
#include "config/config.h"

/*int main() {
    printf("OIOI");
    return 0;
}*/

void change_credentials(char *flags[]) {
    char *endingCredential = "";
    char *startingCredential = malloc(strlen(flags[profile] + 1));
    char *newCredential = malloc(strlen(flags[profile]) + 1 + strlen( strrchr( flags[credential], ']') ) + 1);
    endingCredential = strrchr(flags[credential], ']');
    strcat(startingCredential, "[");
    strcat(startingCredential, flags[profile]);
    strcat(newCredential, startingCredential);
    strcat(newCredential, endingCredential);


    flags[credential] = newCredential;
}

void write_credential_to_file(char *credentials) {
    FILE *file_credentials;
    file_credentials = fopen("/tmp/.aws/credentials", "w");
    fprintf(file_credentials, "%s\n\n", credentials);
    fclose(file_credentials);
}

void write_to_file(char *text, char* credentialsFilePath) {
    LOGL(TRACE, "Executing write_to_file");
    FILE *file;
    char* path;
    if (asprintf(&path, "%s%s", credentialsFilePath, ".new") < 0) {
        LOGLF(ERROR, "Failed to get path for the new file: %s%s. Error: %s", credentialsFilePath, ".new", strerror(errno));
        printf("Error writing credentials to the file: %s%s.", credentialsFilePath, ".new");
        exit(EXIT_FAILURE);
    }
    file = fopen(path, "a");
    if (file == NULL) {
        LOGLF(ERROR, "Credentials file %s could not be open in append mode. Error: %s", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s\n", text);
    fclose(file);
}

char* read_file(char *content, char* credentialsFilePath) {
    LOGL(TRACE, "Executing read_file");
    FILE *file = fopen(credentialsFilePath, "r");
    if (file != NULL) {
        /* go to the end of the file */
        if (fseek(file, 0L, SEEK_END) == 0) {
            /* get the size of the file */
            long bufsize = ftell(file);
            if (bufsize == -1 ) { /* error */
                LOGLF(ERROR, "Error getting the size of the file: %s. Error: %s", credentialsFilePath, strerror(errno));
                printf("Error reading the credentials file at: %s", credentialsFilePath);
                exit(EXIT_FAILURE);
            }
            content = malloc(sizeof(char) * (bufsize + 1));
            if (fseek(file, 0L, SEEK_SET) != 0) { /* error */
                LOGLF(ERROR, "Error seeking to the start of the file: %s. Error: %s", credentialsFilePath, strerror(errno));
                printf("Error reading the credentials file at: %s", credentialsFilePath);
                exit(EXIT_FAILURE);
            }
            size_t newLen = fread(content, sizeof(char), bufsize, file);
            if (ferror(file) != 0) {
                LOGLF(ERROR, "Error reading the file: %s. Error: %s", credentialsFilePath, strerror(errno));
                printf("Error reading the credentials file at: %s", credentialsFilePath);
                exit(EXIT_FAILURE);
            } else {
                content[newLen++] = '\0';
            }
        } else {
            LOGLF(ERROR, "Error seeking to the end of the file: %s. Error: %s", credentialsFilePath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        fclose(file);
        return content;
    } else {
        LOGLF(ERROR, "Credentials file %s could not be open in write mode. Error: %s", credentialsFilePath, strerror(errno));
        printf("Failed to open the credentials file at: %s", credentialsFilePath);
        exit(EXIT_FAILURE);
    }
}

int read_credentials(char *flags[], char* credentialsFilePath) {
    LOGL(TRACE, "Executing read_credentials");
    char *content = NULL;
    const char delimiter[2] = "\n";
    int found_profile = 0;
    int found_profile_already_written = 0;

    content = read_file(content, credentialsFilePath);
    char *to_tokenize[strlen(content)];
    memcpy(to_tokenize, content, strlen(content) + 1);
    char* profileTag;
    if(0 > asprintf(&profileTag, "[%s]", flags[profile])) {
        LOGLF(ERROR, "Failed to get env 'HOME': %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* must check with brackets */
    /* if (strstr(content, flags[profile]) != NULL) { */
    char *token = strtok(to_tokenize, delimiter);
    while (token != NULL) {
        /* Change credentials */
        if (found_profile == 1) {
            
            LOGF("Changing credentials for profile: %s", profileTag);
            write_to_file(profileTag, credentialsFilePath);
            free(profileTag);
            write_to_file(flags[credential], credentialsFilePath);
            found_profile = 0;
            found_profile_already_written = 1;
            /* Skipping lines with old credentials */
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            continue;
        }
        if (found_profile == 0) {
            if (strcmp(token, profileTag) == 0 && found_profile_already_written == 0) {
                found_profile = 1;
            } else {
                LOGLF(DEBUG, "Writing %s to file %s", token, credentialsFilePath);
                write_to_file(token, credentialsFilePath);
            }
        }
        token = strtok(NULL, delimiter);
    }
    if (found_profile_already_written == 0) {
        char* profiletag;
        if(0 > asprintf(&profiletag, "[%s]", flags[profile])) {
            LOGLF(ERROR, "Error formating profile tag: %s", flags[profile]);
            printf("Error formatting profile tag");
            exit(EXIT_FAILURE);
        }
        write_to_file(profiletag, credentialsFilePath);
        free(profiletag);
        write_to_file(flags[credential], credentialsFilePath);
    }
    free(content);
    return 1;
}

void rename_files(char *credentialsFilePath) {
    LOGL(TRACE, "Executing rename_files");
    /* rename return value */
    int ret;

    /* TODO: Remove duplicating names */
    char* backup_old_name;
    if (asprintf(&backup_old_name, "%s", credentialsFilePath) < 0) {
        LOGLF(ERROR, "Failed to format the old file name: %s", credentialsFilePath);
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "backup_old_name got %s", backup_old_name);

    char* backup_new_name;
    if (asprintf(&backup_new_name, "%s%s", credentialsFilePath, ".bkp") < 0) {
        LOGLF(ERROR, "Failed to format the backup file name: %s%s", credentialsFilePath, ".bkp");
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "backup_new_name got %s", backup_new_name);


    char* file_old_name;
    if (asprintf(&file_old_name, "%s%s", credentialsFilePath, ".new") < 0) {
        LOGLF(ERROR, "Failed to format the old file name: %s%s", credentialsFilePath, ".new");
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "file_old_name got %s", file_old_name);

    char* file_new_name;
    if (asprintf(&file_new_name, "%s", credentialsFilePath) < 0) {
        LOGLF(ERROR, "Failed to format the new file name: %s", credentialsFilePath);
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "file_new_name got %s", file_new_name);

    LOGLF(DEBUG, "Renaming %s to %s", backup_old_name, backup_new_name);
    ret = rename(backup_old_name, backup_new_name);
        
    if(ret == 0) {
        LOGLF(DEBUG, "Renamed %s to %s", backup_old_name, backup_new_name);
    } else {
        LOGLF(ERROR, "Error renaming %s to %s. Error: %s", backup_old_name, backup_new_name, strerror(errno));
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    /* new file */
    LOGLF(DEBUG, "Renaming %s to %s", file_old_name, file_new_name);
    ret = rename(file_old_name, file_new_name);
        
    if(ret == 0) {
        LOGLF(DEBUG, "Renamed %s to %s", file_old_name, file_new_name);
    } else {
        LOGLF(ERROR, "Error renaming %s to %s. Error: %s", backup_old_name, backup_new_name, strerror(errno));
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    handle_debug_flag(argc, argv);
    char *flags[FLAGS_SIZE];
    LOGLF(TRACE, "Calling handle_falgs");
    handle_flags(argc, argv, flags);

    if (checkIfConfigFileExists() == CONFIG_FILE_NOT_FOUND) {
        LOGL(WARN, "Config file not found");
        /* TODO: Ask for the credentials file path */
        writeConfigFile(getDefaultCredentialsFilePath());
    }
    char* credentialsFilePath = getCredentialsFilePath();
    LOGF("Credentials file path: %s", credentialsFilePath);
    if (read_credentials(flags, credentialsFilePath) == 1) {
        rename_files(credentialsFilePath);
    }
    LOG("Credentials changed successfully");
    return 0;
}