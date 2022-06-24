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
    bool close();                                              // The plugin must flush, write, cleanup... when this method is called
    bool isOK();                                               // Method to check if the plugin have any error
    bool getError(char *error, unsigned long long buffersize); // Get the last error string. Will be empty if there was no errors.
    void clearError();                                         // Clear the las error.

    // Database
    bool openDatabase(std::string filename);                                                                    // Open the source/destination file
    bool getGameData(const char *gameID, const char *value, char *return_value, unsigned long long buffersize); // Return the total disks in this input/output file

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

    bool SHARED_EXPORT close(void *handler);                                                                                                 // The plugin must flush, write, cleanup... when this method is called
    bool SHARED_EXPORT isOK(void *handler);                                                                                                  // Method to check if the plugin have any error
    bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize);                                                  // Get the last error string. Will be empty if there was no errors.
    void SHARED_EXPORT clearError(void *handler);                                                                                            // Clear the las error.
    bool SHARED_EXPORT openDatabase(void *handler, const char *filename);                                                                    // Open the source file
    bool SHARED_EXPORT getGameData(void *handler, const char *gameID, const char *value, char *return_value, unsigned long long buffersize); // Get the Game Title from the ID
}

#endif // _PLUGIN_HPP_H_