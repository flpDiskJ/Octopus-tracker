#include "moduleformat.h"

ModuleFormat::ModuleFormat(Tracker *t_pointer)
{
    t = t_pointer;
}

ModuleFormat::~ModuleFormat()
{

}

bool ModuleFormat::save_module(string path)
{
    return false;
}

bool ModuleFormat::load_module(string path)
{
    return false;
}
