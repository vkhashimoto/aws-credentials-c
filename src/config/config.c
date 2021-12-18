#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "logging/logging.h"

int checkIfConfigFileExists() {
    if (access(getConfigFilePath(), F_OK) == 0) {
        return CONFIG_FILE_FOUND;
    }
    return CONFIG_FILE_NOT_FOUND;
}

char* _getPathWithHome(char* path) {
    char* fullPath;
    asprintf(&fullPath, "%s%s", getenv("HOME"), path);
    return fullPath;
}

char* getConfigFilePath() {
    return _getPathWithHome("/.config/aws-creds/config");
}

int writeConfigFile(char* credentialsFilePath) {
    FILE* configFile = fopen(getConfigFilePath(), "w");
    if (configFile == NULL) {
        DEBUGF("Config file: %s could not be open with error: %s", getConfigFilePath(), strerror(errno));
        printf("Could not write to config file.");
        return -1;
    }
    printf ("Creating config file with path: %s%s", CREDENTIALS_FILE_PATH_PREFIX ,credentialsFilePath);
    fprintf(configFile, "%s%s", CREDENTIALS_FILE_PATH_PREFIX, credentialsFilePath);
    fclose(configFile);
    return 0;
}

char* getDefaultCredentialsFilePath() {
    return _getPathWithHome("/.aws/credentials");
}

char* getCredentialsFilePath() {
    FILE* configFile = fopen(getConfigFilePath(), "r");
    if (configFile == NULL) {
        DEBUGF("Config file: %s could not be open: %s", getConfigFilePath(), strerror(errno));
        exit(-1);
    }
    char* content = NULL;
    char* path = NULL;
    if (fseek(configFile, 0L, SEEK_END) == 0) {
        long bufsize = ftell(configFile);
        content = malloc(sizeof(char) * (bufsize + 1));
        fseek(configFile, 0L, SEEK_SET);
        size_t newLen = fread(content, sizeof(char), bufsize, configFile);
        content[newLen++] = '\0';
    }

    fclose(configFile);
    if (content != NULL) {
        return content + sizeof(CREDENTIALS_FILE_PATH_PREFIX) - 1;
    }
    return content;
}