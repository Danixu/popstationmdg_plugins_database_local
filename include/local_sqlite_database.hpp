/*

  This header file is the virtual plugin definition which will be used in derivated plugins and main program

*/

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>

#include "export.h"
#include "plugin_assistant.hpp"
#include "sqlite3.h"

#ifndef _PLUGIN_HPP_H_
#define _PLUGIN_HPP_H_

class LocalSQlite
{
public:
    // Constructor and destructor
    LocalSQlite();
    ~LocalSQlite();

    // Common
    bool close();
    bool isOK();
    bool getError(char *error, unsigned long long buffersize);
    void clearError();

    // Database
    bool open(std::string, unsigned int mode, unsigned int threads);
    bool getGameData(const char *gameID, const char *value, char *return_value, unsigned long long buffersize);

protected:
    // SQLite 3 object
    sqlite3 *database = NULL;
    int returnCode;

    // gameDataVariables
    std::map<std::string, std::string> gameData;
    std::string last_id = "";

    // Error management
    void setLastError(std::string error);
    void setLastError(char *error);
    char *last_error = NULL;
    bool is_ok = true;
};

// C functions definition
extern "C"
{
    void SHARED_EXPORT *load();
    void SHARED_EXPORT unload(void *ptr);
    unsigned int SHARED_EXPORT getType();
    bool SHARED_EXPORT getPluginName(char *name, unsigned long long buffersize);
    bool SHARED_EXPORT getPluginVersion(char *version, unsigned long long buffersize);

    bool SHARED_EXPORT close(void *handler);
    bool SHARED_EXPORT isOK(void *handler);
    bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize);
    void SHARED_EXPORT clearError(void *handler);
    bool SHARED_EXPORT open(void *handler, char *filename, unsigned int mode, unsigned int threads);
    bool SHARED_EXPORT getGameData(void *handler, const char *gameID, const char *value, char *return_value, unsigned long long buffersize);
}

#endif // _PLUGIN_HPP_H_