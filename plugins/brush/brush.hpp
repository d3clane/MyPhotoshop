#ifndef SPRAY_PLUGIN_H
#define SPRAY_PLUGIN_H

extern "C"
{

bool onLoadPlugin();
void onUnloadPlugin();

}

#endif // SPRAY_PLUGIN_H