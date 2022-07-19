#include "local_sqlite_database.hpp"

namespace PopstationmdgPlugin
{
    int dataInsert(void *data, int argc, char **argv, char **azColName)
    {
        std::map<std::string, std::string> *gameData = (std::map<std::string, std::string> *)data;
        for (size_t i = 0; i < argc; i++)
        {
            gameData->insert({std::string(azColName[i]), std::string(argv[i])});
        }

        return 0;
    }

    // Reader constructor
    LocalSQLite::LocalSQLite(void *logger)
    {
    }

    // Reader destructor
    LocalSQLite::~LocalSQLite()
    {
        // Clear the last error string
        if (last_error != NULL)
        {
            delete[] last_error;
        }
        close();
    }

    // Open the database file
    bool LocalSQLite::open(std::string filename, unsigned int mode, unsigned int threads)
    {
        if (mode & PTGameDatabase)
        {
            returnCode = sqlite3_open_v2("./local_sqlite_database.db", &database, SQLITE_OPEN_READONLY, NULL);
            if (returnCode)
            {
                setLastError(std::string("There was an error opening the database file"));
                return false;
            }

            return true;
        }

        setLastError(std::string("The open mode is not correct."));
        return false;
    }

    bool LocalSQLite::getGameData(const char *gameID, const char *value, char *return_value, unsigned long long buffersize)
    {
        std::string game_id = gameID;

        if (last_id != game_id)
        {
            /* Create SQL statement */
            std::string sql = "SELECT * from games WHERE serial = '" + game_id + "';";
            char *error = nullptr;

            /* Execute SQL statement */
            returnCode = sqlite3_exec(database, sql.c_str(), dataInsert, &gameData, &error);

            if (returnCode != SQLITE_OK)
            {
                if (error != nullptr)
                {
                    setLastError(std::string("There was an error doing the sql query: ").append(error));
                }
                else
                {
                    setLastError(std::string("There was an error doing the sql query"));
                }

                return false;
            }

            if (gameData.size() == 0)
            {
                return false;
            }
        }

        last_id = gameID;
        auto found = gameData.find(value);
        if (found != gameData.end())
        {
            size_t value_length = found->second.length() + 1;
            if (value_length > buffersize)
            {
                return false;
            }
            strncpy_s(return_value, buffersize, found->second.c_str(), found->second.length());

            return true;
        }
        else
        {
            return false;
        }
    }

    // Close the DB file (if was opened)
    bool LocalSQLite::close()
    {
        if (database != NULL)
        {
            sqlite3_close_v2(database);
            database = NULL;
        }
        return true;
    }

    // Check if Status is OK
    bool LocalSQLite::isOK()
    {
        return is_ok;
    }

    // Get the last error
    // Get the last error
    bool LocalSQLite::getError(char *error, unsigned long long buffersize)
    {
        if (last_error != NULL)
        {
            size_t error_size = strlen(last_error);
            if (error_size > buffersize)
            {
                setLastError(std::string("The output buffer size is too small"));
                return false;
            }

            strncpy_s(error, buffersize, last_error, error_size);

            return true;
        }
        else
        {
            return true;
        }
    }

    // Clear the last error and isOK status
    void LocalSQLite::clearError()
    {
        if (last_error != nullptr)
        {
            delete[] last_error;
            last_error = nullptr;
        }

        is_ok = true;
    }

    // Set the last error text and isOK to false
    void LocalSQLite::setLastError(std::string error)
    {
        size_t value_length = error.length() + 1;
        // If string is not empty
        if (value_length > 1)
        {
            if (last_error != nullptr)
            {
                delete[] last_error;
            }

            last_error = new char[value_length];
            memset(last_error, 0, value_length);

            strncpy_s(last_error, value_length, error.c_str(), error.length());
            is_ok = false;
        }
    }

    extern "C"
    {
        //
        // Creates a new plugin object in memory and return its address
        //
        void SHARED_EXPORT *load(void *logger)
        {
            return (void *)new LocalSQLite(logger);
        }

        //
        // Delete the plugin object. Must be called before unload the library or memory leaks will occur
        //
        void SHARED_EXPORT unload(void *ptr)
        {
            delete (LocalSQLite *)ptr;
        }

        //
        // Get the type of plugin to allow to filter
        //
        unsigned int SHARED_EXPORT getType()
        {
            return PTGameDatabase;
        }

        //
        // Return the plugin name
        //
        bool SHARED_EXPORT getPluginName(char *name, unsigned long long buffersize)
        {
            // Compatible extensions for the reader/writter. Use pipe "|" between the extension: "*.iso|*.bin"
            const char pn[] = "Local SQLite Database";

            if (sizeof(pn) > buffersize)
            {
                return false;
            }

            strncpy_s(name, buffersize, pn, sizeof(pn));
            return true;
        }

        //
        // Return the plugin version
        //
        bool SHARED_EXPORT getPluginVersion(char *version, unsigned long long buffersize)
        {
            // Compatible extensions for the reader/writter. Use pipe "|" between the extension: "*.iso|*.bin"
            const char pv[] = "0.0.1";

            if (sizeof(pv) > buffersize)
            {
                return false;
            }

            strncpy_s(version, buffersize, pv, sizeof(pv));

            return true;
        }

        bool SHARED_EXPORT open(void *handler, char *filename, unsigned int mode, unsigned int threads)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            return object->open(filename, mode, threads);
        }

        bool SHARED_EXPORT close(void *handler)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            return object->close();
        }

        // ISO Images doesn't have any information about title
        bool SHARED_EXPORT getGameData(void *handler, const char *gameID, const char *value, char *return_value, unsigned long long buffersize)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            return object->getGameData(gameID, value, return_value, buffersize);
        }

        bool SHARED_EXPORT isOK(void *handler)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            return object->isOK();
        }

        bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            return object->getError(error, buffersize);
        }

        void SHARED_EXPORT clearError(void *handler)
        {
            LocalSQLite *object = (LocalSQLite *)handler;

            object->clearError();
        }
    }
}