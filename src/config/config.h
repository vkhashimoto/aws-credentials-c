#ifndef CONFIG_H
#define CONFIG_H
#define CONFIG_FILE_FOUND (1)
#define CONFIG_FILE_NOT_FOUND (2)

static const char CREDENTIALS_FILE_PATH_PREFIX[] = "path=";

char* _getPathWithHome(char* path);

char* getCredentialsFilePath();

int checkIfConfigFileExists();

int writeConfigFile();

char* getConfigFilePath();

char* getDefaultCredentialsFilePath();

#endif