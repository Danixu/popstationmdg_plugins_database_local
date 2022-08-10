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

#include "plugins/export.h"
#include "plugins/plugin_assistant.h"
#include "sqlite/sqlite3.h"

#include "nlohmann_json/json.hpp"
using ordered_json = nlohmann::ordered_json;

#ifndef _PLUGIN_HPP_H_
#define _PLUGIN_HPP_H_

namespace PopstationmdgPlugin
{
    class LocalSQLite
    {
    public:
        // Constructor and destructor
        LocalSQLite(void *logger = nullptr);
        ~LocalSQLite();

        // Common
        bool close();
        bool isOK();
        bool getError(char *error, unsigned long long buffersize);
        void clearError();
        bool setSettings(char *settingsData, unsigned long &settingsSize, unsigned int mode);

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
}

#endif // _PLUGIN_HPP_H_