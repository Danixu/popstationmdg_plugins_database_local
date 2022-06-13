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

int main()
{

    auto plugins = load_plugins("./", EXT);
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
            return 1;
        }

        fprintf(stderr, "Opening database file\n");
        bool opened = ph->openDatabase("test.db");
        if (!opened)
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            return 1;
        }

        // Get the game ID
        fprintf(stderr, "Getting the Title by ID\n");
        std::string id_test = ph->getTitleByID("SCES03884");
        if (id_test.c_str() == "")
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
            return 1;
        }
        else
        {
            fprintf(stderr, "%s\n", id_test.c_str());
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