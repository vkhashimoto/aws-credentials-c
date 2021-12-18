#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "logging/logging.h"

int checkIfConfigFileExists() {
    LOGLF(TRACE, "Executing checkIfConfigFileExists");
    if (access(getConfigFilePath(), F_OK) == 0) {
        return CONFIG_FILE_FOUND;
    }
    return CONFIG_FILE_NOT_FOUND;
}

char* _getPathWithHome(char* path) {
    LOGLF(TRACE, "Executing _getPathWithHome");
    char* fullPath;
    if (asprintf(&fullPath, "%s%s", getenv("HOME"), path) < 0) {
        LOGLF(ERROR, "Failed to get env 'HOME': %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fullPath;
}

char* getConfigFilePath() {
    LOGLF(TRACE, "Executing getConfigFilePath");
    return _getPathWithHome("/.config/aws-creds/config");
}

int writeConfigFile(char* credentialsFilePath) {
    LOGLF(TRACE, "Executing writeConfigFile");
    char* path = getConfigFilePath();
    FILE* configFile = fopen(path, "w");
    if (configFile == NULL) {
        LOGLF(ERROR, "Config file %s could not be open in write mode. Error: %s", path, strerror(errno));
        printf("Failed to open the config file");
        exit(EXIT_FAILURE);
    }

    LOGLF(DEBUG, "Creating config file in %s, with content: %s%s", path, CREDENTIALS_FILE_PATH_PREFIX, credentialsFilePath);
    fprintf(configFile, "%s%s", CREDENTIALS_FILE_PATH_PREFIX, credentialsFilePath);
    LOG("Config file created successfully");
    fclose(configFile);
    return 0;
}

char* getDefaultCredentialsFilePath() {
    LOGL(TRACE, "Executing getDefaultCredentialsFilePath");
    return _getPathWithHome("/.aws/credentials");
}

char* getCredentialsFilePath() {
    LOGL(TRACE, "Executing getCredentialsFilePath");
    char* configFilePath = getConfigFilePath();
    FILE* configFile = fopen(configFilePath, "r");
    if (configFile == NULL) {
        LOGLF(ERROR, "Config file %s could not be open in read mode. Error: %s", configFilePath, strerror(errno));
        printf("Failed to open the config file");
        exit(EXIT_FAILURE);
    }
    char* content = NULL;
    char* path = NULL;
    if (fseek(configFile, 0L, SEEK_END) == 0) {
        LOGL(DEBUG, "Getting content size from config file");
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
    LOGL(WARN, "Error retrieving config file content");
    return content;
}