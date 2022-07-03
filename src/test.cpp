/*
 *
 * Main application which will load the plugins dinamically
 *
 */

#include <vector>
#include "plugin_handler.hpp"

#ifdef _WIN32
#define EXT ".dll"
#else
#define EXT ".so"
#endif

Logging::Logger pLogger = Logging::Logger(Logging::LOG_IN_FILE, Logging::LOG_LEVEL_WARNING, std::string("./test_database.log"));

int main()
{
    auto plugins = load_plugins("./", EXT, PTGameDatabase);
    for (auto ph : plugins)
    {
        fprintf(stderr, "Loading plugin...\n");
        auto plugin = ph->load();
        if (plugin == NULL)
        {
            fprintf(stderr, "The plugin is not loaded correctly\n");
            continue;
        }
        fprintf(stderr, "Plugin loaded\n");
        fprintf(stderr, "Auto loaded plugin: %s, version: %s, type: %d\n", ph->getPluginName().c_str(), ph->getPluginVersion().c_str(), ph->getType());
    }
    for (auto ph : plugins)
    {
        // Load the library functions
        fprintf(stderr, "Loading internal dll class\n");
        auto plugin = ph->load();
        if (!ph->isOK())
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            continue;
        }

        fprintf(stderr, "Opening database file\n");
        bool opened = ph->open("", PTGameDatabase);
        if (!opened)
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            continue;
        }

        // Get the game ID
        fprintf(stderr, "Getting the Title by ID\n");
        std::string title_id_test = ph->getGameData("SCES03884", "title");
        if (title_id_test.c_str() == "")
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            continue;
        }
        else
        {
            fprintf(stderr, "%s\n", title_id_test.c_str());
        }

        // Get the game Region
        fprintf(stderr, "Getting the Region by ID\n");
        std::string region_id_test = ph->getGameData("SCES03884", "region");
        if (region_id_test.c_str() == "")
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            continue;
        }
        else
        {
            fprintf(stderr, "%s\n", region_id_test.c_str());
        }

        // Close the file
        fprintf(stderr, "Closing the test file\n");
        bool closing = ph->close();
        if (!closing)
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        }

        // Unload the library
        ph->unload();
    }

    return 0;
}