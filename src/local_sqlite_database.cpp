#include "local_sqlite_database.hpp"

int callback(void *data, int argc, char **argv, char **azColName)
{
    std::string *gameTitle = (std::string *)data;
    if (argc > 0 && argv[0])
    {
        size_t length = strlen(argv[0]);

        gameTitle->assign(argv[0]);
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
    // Clear the last error
    if (last_error != NULL)
    {
        delete[] last_error;
        last_error = NULL;
    }

    close();
}

// Open the ISO file
bool LocalSQlite::openDatabase(const char *filename)
{
    returnCode = sqlite3_open_v2(filename, &database, SQLITE_OPEN_READONLY, NULL);
    if (returnCode)
    {
        setLastError(std::string("There was an error opening the database file"));
        return false;
    }

    return true;
}

const char *LocalSQlite::getTitleByID(const char *gameID)
{
    // char *sql;

    /* Create SQL statement */
    std::string sql = "SELECT title from games WHERE id = '" + std::string(gameID) + "';";

    /* Execute SQL statement */
    returnCode = sqlite3_exec(database, sql.c_str(), callback, &gameTitle, &last_error);

    if (returnCode != SQLITE_OK)
    {
        return NULL;
    }
    else
    {
        return gameTitle.c_str();
    }
}

// Close the ISO file (if was opened)
bool LocalSQlite::close()
{
    sqlite3_close(database);
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
    delete[] last_error;
    last_error = NULL;
    is_ok = true;
}

void LocalSQlite::setLastError(std::string error)
{
    if (error.length() > 0)
    {
        char *error_char = new char[error.length() + 1];
        std::memset(error_char, 0, error.length() + 1);
        strncpy_s(error_char, error.length() + 1, error.c_str(), error.length());
        setLastError(error_char);
    }
}

// Set the last error text and isOK to false
void LocalSQlite::setLastError(char *error)
{
    if (last_error != NULL)
    {
        delete[] last_error;
        last_error = NULL;
    }

    last_error = error;
    is_ok = false;
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
        return PTTitleDatabase;
    }

    //
    // Return the plugin name
    //
    const char SHARED_EXPORT *getPluginName()
    {
        return "ISO Image";
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
    const char SHARED_EXPORT *getTitleByID(void *handler, const char *gameID)
    {
        LocalSQlite *object = (LocalSQlite *)handler;

        return object->getTitleByID(gameID);
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