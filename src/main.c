#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "logging/logging.h"
#include "config/config.h"

void writeToNewFile(char *text, char* credentialsFilePath) {
    FILE *file;
    char* path;
    LOGL(TRACE, "Executing writeToNewFile");
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

char* readExistingCredentialsFile(char *content, char* credentialsFilePath) {
    LOGL(TRACE, "Executing readExistingCredentialsFile");
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

int readCredentialsFromFile(char *flags[], char* credentialsFilePath) {
    LOGL(TRACE, "Executing readCredentialsFromFile");
    char *content = NULL;
    const char delimiter[2] = "\n";
    int found_profile = 0;
    int found_profile_already_written = 0;

    content = readExistingCredentialsFile(content, credentialsFilePath);
    char to_tokenize[strlen(content)];
    memcpy(to_tokenize, content, strlen(content) + 1);
    char* profileTag;
    if(0 > asprintf(&profileTag, "[%s]", flags[profile])) {
        LOGLF(ERROR, "Failed to get env 'HOME': %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* must check with brackets */
    char *token = strtok(to_tokenize, delimiter);
    while (token != NULL) {
        /* Change credentials */
        if (found_profile == 1) {
            
            LOGF("Changing credentials for profile: %s", profileTag);
            writeToNewFile(profileTag, credentialsFilePath);
            free(profileTag);
            writeToNewFile(flags[credential], credentialsFilePath);
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
                writeToNewFile(token, credentialsFilePath);
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
        writeToNewFile(profiletag, credentialsFilePath);
        free(profiletag);
        writeToNewFile(flags[credential], credentialsFilePath);
    }
    free(content);
    return 1;
}

void renameFiles(char *credentialsFilePath) {
    LOGL(TRACE, "Executing renameFiles");
    /* rename return value */
    int ret;

    char* backupFileName;
    if (asprintf(&backupFileName, "%s%s", credentialsFilePath, ".bkp") < 0) {
        LOGLF(ERROR, "Failed to format the backup file name: %s%s", credentialsFilePath, ".bkp");
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "backupFileName got %s", backupFileName);


    char* newFileName;
    if (asprintf(&newFileName, "%s%s", credentialsFilePath, ".new") < 0) {
        LOGLF(ERROR, "Failed to format the new file name: %s%s", credentialsFilePath, ".new");
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    LOGLF(DEBUG, "newFileName got %s", newFileName);

    LOGLF(DEBUG, "Renaming %s to %s", credentialsFilePath, backupFileName);
    ret = rename(credentialsFilePath, backupFileName);
        
    if(ret == 0) {
        LOGLF(DEBUG, "Renamed %s to %s", credentialsFilePath, backupFileName);
    } else {
        LOGLF(ERROR, "Error renaming %s to %s. Error: %s", credentialsFilePath, backupFileName, strerror(errno));
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
    /* new file */
    LOGLF(DEBUG, "Renaming %s to %s", newFileName, credentialsFilePath);
    ret = rename(newFileName, credentialsFilePath);
        
    if(ret == 0) {
        LOGLF(DEBUG, "Renamed %s to %s", newFileName, credentialsFilePath);
    } else {
        LOGLF(ERROR, "Error renaming %s to %s. Error: %s", newFileName, credentialsFilePath, strerror(errno));
        printf("Error while renaming files");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    handleLogFlags(argc, argv);
    char *flags[FLAGS_SIZE];
    LOGLF(TRACE, "Calling handleFlags");
    handleFlags(argc, argv, flags);

    if (checkIfConfigFileExists() == CONFIG_FILE_NOT_FOUND) {
        LOGL(WARN, "Config file not found");
        /* TODO: Ask for the credentials file path */
        writeConfigFile(getDefaultCredentialsFilePath());
    }
    char* credentialsFilePath = getCredentialsFilePath();
    LOGF("Credentials file path: %s", credentialsFilePath);
    if (readCredentialsFromFile(flags, credentialsFilePath) == 1) {
        renameFiles(credentialsFilePath);
    }
    LOG("Credentials changed successfully");
    return 0;
}