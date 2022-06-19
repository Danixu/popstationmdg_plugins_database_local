#include "local_sqlite_database.hpp"

int dataInsert(void *data, int argc, char **argv, char **azColName)
{
    std::map<std::string, std::string> *gameData = (std::map<std::string, std::string> *)data;
    for (size_t i = 0; i < argc; i++)
    {
        gameData->insert({std::string(azColName[i]), std::string(argv[i])});
    }

    for (auto it = gameData->begin(); it != gameData->end(); it++)
    {
        fprintf(stderr, "Item key = %s - Item value = %s\n", it->first.c_str(), it->second.c_str());
    }
    return 0;
}

// Reader constructor
LocalSQlite::LocalSQlite()
{
}

// Reader destructor
LocalSQlite::~LocalSQlite()
{
    // Clear the last returned value string
    if (last_returned_value != NULL)
    {
        delete[] last_returned_value;
    }

    // Clear the last error string
    if (last_error != NULL)
    {
        delete[] last_error;
    }

    close();
}

// Open the ISO file
bool LocalSQlite::openDatabase(std::string filename)
{
    returnCode = sqlite3_open_v2(filename.c_str(), &database, SQLITE_OPEN_READONLY, NULL);
    if (returnCode)
    {
        setLastError(std::string("There was an error opening the database file"));
        return false;
    }

    return true;
}

char *LocalSQlite::getGameData(std::string gameID, std::string value)
{
    if (last_id != gameID)
    {
        /* Create SQL statement */
        std::string sql = "SELECT title, region from games WHERE id = '" + gameID + "';";
        char *error = NULL;

        /* Execute SQL statement */
        returnCode = sqlite3_exec(database, sql.c_str(), dataInsert, &gameData, &error);

        if (returnCode != SQLITE_OK)
        {
            if (error != NULL)
            {
                setLastError(std::string("There was an error doing the sql query: ") + std::string(error));
            }
            else
            {
                setLastError(std::string("There was an error doing the sql query"));
            }

            return NULL;
        }
    }

    last_id = gameID;
    auto found = gameData.find(value);
    if (found != gameData.end())
    {
        if (last_returned_value != NULL)
        {
            delete[] last_returned_value;
        }
        size_t value_length = found->second.length() + 1;
        last_returned_value = new char[value_length];
        memset(last_returned_value, 0, value_length);

        strncpy_s(last_returned_value, value_length, found->second.c_str(), found->second.length());
        return last_returned_value;
    }
    else
    {
        return NULL;
    }
}

// Close the ISO file (if was opened)
bool LocalSQlite::close()
{
    if (database != NULL)
    {
        sqlite3_close_v2(database);
        database = NULL;
    }
    return true;
}

// Check if Status is OK
bool LocalSQlite::isOK()
{
    return is_ok;
}

// Get the last error
char *LocalSQlite::getError()
{
    return last_error;
}

// Clear the last error and isOK status
void LocalSQlite::clearError()
{
    last_error = NULL;
    is_ok = true;
}

void LocalSQlite::setLastError(std::string error)
{
    size_t value_length = error.length() + 1;
    // If string is not empty
    if (value_length > 1)
    {
        if (last_error != NULL)
        {
            delete[] last_error;
        }

        last_error = new char[value_length];
        memset(last_error, 0, value_length);

        strncpy_s(last_error, value_length, error.c_str(), error.length());
    }
}

// Set the last error text and isOK to false
void LocalSQlite::setLastError(char *error)
{
    if (error != NULL)
    {
        if (last_error != NULL)
        {
            delete[] last_error;
        }

        last_error = error;
        is_ok = false;
    }
}

extern "C"
{
    //
    // Creates a new plugin object in memory and return its address
    //
    void SHARED_EXPORT *load()
    {
        return (void *)new LocalSQlite();
    }

    //
    // Delete the plugin object. Must be called before unload the library or memory leaks will occur
    //
    void SHARED_EXPORT unload(void *ptr)
    {
        delete (LocalSQlite *)ptr;
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
    const char SHARED_EXPORT *getPluginName()
    {
        return "Local SQLite Database";
    }

    //
    // Return the plugin version
    //
    const char SHARED_EXPORT *getPluginVersion()
    {
        return "0.0.1";
    }

    bool SHARED_EXPORT openDatabase(void *handler, const char *filename)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->openDatabase(filename);
    }

    bool SHARED_EXPORT close(void *handler)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->close();
    }

    // ISO Images doesn't have any information about title
    char SHARED_EXPORT *getGameData(void *handler, const char *gameID, const char *value)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->getGameData(gameID, value);
    }

    bool SHARED_EXPORT isOK(void *handler)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->isOK();
    }

    char SHARED_EXPORT *getError(void *handler)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->getError();
    }

    void SHARED_EXPORT clearError(void *handler)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        object->clearError();
    }
}