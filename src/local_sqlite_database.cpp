#include "local_sqlite_database.hpp"

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
    // Logger
    if (logger != nullptr)
    {
        log = (Logging::Logger *)logger;
        log->debug("Initializing the Local SQLite database plugin.", __METHOD_NAME__);
    }
}

// Reader destructor
LocalSQLite::~LocalSQLite()
{
    if (log != nullptr)
        log->debug("Deleting object.", __METHOD_NAME__);
    // Clear the last error string
    if (last_error != NULL)
    {
        if (log != nullptr)
            log->debug("Clearing last error", __METHOD_NAME__);
        delete[] last_error;
    }

    if (log != nullptr)
        log->debug("Closing the database", __METHOD_NAME__);
    close();

    if (log != nullptr)
        log->debug("Done", __METHOD_NAME__);
}

// Open the database file
bool LocalSQLite::open(std::string filename, unsigned int mode, unsigned int threads)
{
    if (mode & PTGameDatabase)
    {
        if (log != nullptr)
            log->debug("Opening the database file", __METHOD_NAME__);
        returnCode = sqlite3_open_v2("./local_sqlite_database.db", &database, SQLITE_OPEN_READONLY, NULL);
        if (returnCode)
        {
            if (log != nullptr)
                log->error("There was an error opening the database file", __METHOD_NAME__);
            setLastError(std::string("There was an error opening the database file"));
            return false;
        }

        if (log != nullptr)
            log->debug("Opened correctly", __METHOD_NAME__);
        return true;
    }

    if (log != nullptr)
        log->error("The open mode is not correct.", __METHOD_NAME__);
    setLastError(std::string("The open mode is not correct."));
    return false;
}

bool LocalSQLite::getGameData(const char *gameID, const char *value, char *return_value, unsigned long long buffersize)
{
    if (log != nullptr)
        log->debug(std::string("Getting the game data for the ID ").append(gameID), __METHOD_NAME__);
    std::string game_id = gameID;

    if (last_id != game_id)
    {
        if (log != nullptr)
            log->debug("There is no stored data. Getting it from database.", __METHOD_NAME__);
        /* Create SQL statement */
        std::string sql = "SELECT * from games WHERE serial = '" + game_id + "';";
        char *error = nullptr;

        /* Execute SQL statement */
        returnCode = sqlite3_exec(database, sql.c_str(), dataInsert, &gameData, &error);

        if (returnCode != SQLITE_OK)
        {
            if (error != nullptr)
            {
                if (log != nullptr)
                    log->error(std::string("There was an error doing the sql query: ").append(error), __METHOD_NAME__);
                setLastError(std::string("There was an error doing the sql query: ").append(error));
            }
            else
            {
                if (log != nullptr)
                    log->error("There was an error doing the sql query", __METHOD_NAME__);
                setLastError(std::string("There was an error doing the sql query"));
            }

            return false;
        }

        if (gameData.size() == 0)
        {
            if (log != nullptr)
                log->debug(std::string("There is no data about the gameID ").append(gameID), __METHOD_NAME__);

            return false;
        }
    }

    if (log != nullptr)
        log->debug(std::string("GameData found. Looking for the requested field: ").append(value), __METHOD_NAME__);

    last_id = gameID;
    auto found = gameData.find(value);
    if (found != gameData.end())
    {
        if (log != nullptr)
            log->debug("Field found... returning the data.", __METHOD_NAME__);

        size_t value_length = found->second.length() + 1;
        if (value_length > buffersize)
        {
            if (log != nullptr)
                log->error("Output buffer is too small.", __METHOD_NAME__);
            return false;
        }
        if (log != nullptr)
            log->debug(std::string("Copying the value to the output buffer: ").append(found->second), __METHOD_NAME__);
        strncpy_s(return_value, buffersize, found->second.c_str(), found->second.length());

        return true;
    }
    else
    {
        if (log != nullptr)
            log->debug("Field not found...", __METHOD_NAME__);
        return false;
    }
}

// Close the DB file (if was opened)
bool LocalSQLite::close()
{
    if (log != nullptr)
        log->debug("Closing the database...", __METHOD_NAME__);

    if (database != NULL)
    {
        if (log != nullptr)
            log->debug("Database is opened, so will be closed.", __METHOD_NAME__);
        sqlite3_close_v2(database);
        database = NULL;
    }
    if (log != nullptr)
        log->debug("Database closed...", __METHOD_NAME__);
    return true;
}

// Check if Status is OK
bool LocalSQLite::isOK()
{
    if (log != nullptr)
        log->debug("Returning the plugin status (isOK)", __METHOD_NAME__);
    return is_ok;
}

// Get the last error
// Get the last error
bool LocalSQLite::getError(char *error, unsigned long long buffersize)
{
    if (log != nullptr)
        log->debug("Returning the last error", __METHOD_NAME__);
    if (last_error != NULL)
    {
        size_t error_size = strlen(last_error);
        if (error_size > buffersize)
        {
            if (log != nullptr)
                log->debug("The output buffer size is too small", __METHOD_NAME__);
            setLastError(std::string("The output buffer size is too small"));
            return false;
        }

        if (log != nullptr)
            log->debug("Copying the error to the output buffer", __METHOD_NAME__);
        strncpy_s(error, buffersize, last_error, error_size);

        if (log != nullptr)
            log->debug("Done", __METHOD_NAME__);

        return true;
    }
    else
    {
        if (log != nullptr)
            log->debug("There was no error", __METHOD_NAME__);
        return true;
    }
}

// Clear the last error and isOK status
void LocalSQLite::clearError()
{
    if (log != nullptr)
        log->debug("Clearing the last error buffer.", __METHOD_NAME__);
    if (last_error != nullptr)
    {
        if (log != nullptr)
            log->debug("Deleting the char array.", __METHOD_NAME__);
        delete[] last_error;
        last_error = nullptr;
    }

    is_ok = true;
    if (log != nullptr)
        log->debug("Last error buffer cleared.", __METHOD_NAME__);
}

// Set the last error text and isOK to false
void LocalSQLite::setLastError(std::string error)
{
    if (log != nullptr)
        log->debug("Setting last error value.", __METHOD_NAME__);
    size_t value_length = error.length() + 1;
    // If string is not empty
    if (value_length > 1)
    {
        if (log != nullptr)
            log->debug("Checking if an old message exists to clear it.", __METHOD_NAME__);
        if (last_error != nullptr)
        {
            if (log != nullptr)
                log->debug("Clearing the old message.", __METHOD_NAME__);
            delete[] last_error;
        }

        if (log != nullptr)
            log->debug("Reserving the new buffer space for the new message.", __METHOD_NAME__);
        last_error = new char[value_length];
        if (log != nullptr)
            log->debug("Zeroing the new buffer space.", __METHOD_NAME__);
        memset(last_error, 0, value_length);

        if (log != nullptr)
            log->debug("Copying the error message to the new buffer.", __METHOD_NAME__);
        strncpy_s(last_error, value_length, error.c_str(), error.length());
        is_ok = false;
    }
    if (log != nullptr)
        log->debug("Done.", __METHOD_NAME__);
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