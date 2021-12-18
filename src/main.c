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

void write_to_file(char *text, char* credentialsFilePath) {
    FILE *file;
    char* path;
    asprintf(&path, "%s%s", credentialsFilePath, ".new");
    DEBUGF("write_to_file path: %s", credentialsFilePath);
    file = fopen(path, "a");
    if (file == NULL) {
        DEBUGF("file: %s could not be open with error: %s", credentialsFilePath, strerror(errno));
        exit(-1);
    }
    fprintf(file, "%s\n", text);
    fclose(file);
}

char* read_file(char *content, char* credentialsFilePath) {
    FILE *file = fopen(credentialsFilePath, "r");
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

int read_credentials(char *flags[], char* credentialsFilePath) {
    char *content = NULL;
    const char delimiter[2] = "\n";
    int found_profile = 0;
    int found_profile_already_written = 0;

    content = read_file(content, credentialsFilePath);
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
            DEBUGF("Changing credentials for profile %s",  profiletag);
            write_to_file(profiletag, credentialsFilePath);
            free(profiletag);
            write_to_file(flags[credential], credentialsFilePath);
            found_profile = 0;
            found_profile_already_written = 1;
            /* skip wirting the 3 lines with old credentials for this profile */
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            token = strtok(NULL, delimiter);
            continue;
        }
        if (found_profile == 0) {
            if (strcmp(token, flags[profile]) == 0 && found_profile_already_written == 0) {
                found_profile = 1;
            } else {
                DEBUGF("WRITING %s TO FILE\n", token);
                write_to_file(token, credentialsFilePath);
            }
        }
        token = strtok(NULL, delimiter);
    }
    if (found_profile_already_written == 0) {
        char* profiletag;
        if(0 > asprintf(&profiletag, "[%s]", flags[profile])) {
            printf("ERROR FORMATING PROFILE TAG");
        }
        write_to_file(profiletag, credentialsFilePath);
        free(profiletag);
        write_to_file(flags[credential], credentialsFilePath);
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

void rename_files(char *credentialsFilePath) {
    /* backup */
    int ret;

    /* char backup_old_name[] = "/tmp/.aws/credentials";*/
    char* backup_old_name;
    asprintf(&backup_old_name, "%s", credentialsFilePath);

    /* char backup_new_name[] = "/tmp/.aws/credentials.bkp"; */
    char* backup_new_name;
    asprintf(&backup_new_name, "%s%s", credentialsFilePath, ".bkp");

    /* char file_old_name[] = "/tmp/.aws/credentials.new"; */
    char* file_old_name;
    asprintf(&file_old_name, "%s%s", credentialsFilePath, ".new");

    /* char file_new_name[] = "/tmp/.aws/credentials"; */
    char* file_new_name;
    asprintf(&file_new_name, "%s", credentialsFilePath);

    ret = rename(backup_old_name, backup_new_name);
        
    if(ret == 0) {
        DEBUGF("Renamed %s to %s", backup_old_name, backup_new_name);
    } else {
        DEBUGF("Error renaming %s to %s", backup_old_name, backup_new_name);
    }
    /* new file */
    ret = rename(file_old_name, file_new_name);
        
    if(ret == 0) {
        DEBUGF("Renamed %s to %s", file_old_name, file_new_name);
    } else {
        DEBUGF("Error renaming %s to %s", file_old_name, file_new_name);
    }
}

int main(int argc, char *argv[]) {
    handle_debug_flag(argc, argv);
    char *flags[FLAGS_SIZE];
    

    DEBUG("Calling handle_flags.");
    handle_flags(argc, argv, flags);
    if (checkIfConfigFileExists() == CONFIG_FILE_NOT_FOUND) {
        DEBUG("Config file not found, creating one!");
        /* TODO: ask the aws credentials path (if none, uses default) */
        if (writeConfigFile(getDefaultCredentialsFilePath()) == -1) {
            return -1;
        }
    }
    char* credentialsFilePath = getCredentialsFilePath();
    DEBUGF("CREDENTIALS FILE PATH: %s", credentialsFilePath);
    if (read_credentials(flags, credentialsFilePath) == 1) {
        rename_files(credentialsFilePath);
    }
    return 0;
    return 0;
}