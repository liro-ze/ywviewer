#pragma once

#include <raylib.h>
#include <vector>

#include "file_type.h"

struct file_info
{
    const char* name;
    const char* path;
    bool is_directory;
    struct {
        unsigned int count;
        struct file_info* data;
    } children;
};

struct tab_info
{
    std::string name;
    file_type type;
    file_info* file;
    bool opened;
    void* data;
};

struct app_state
{
    bool app_exit;
    bool app_init;
    
    char project_path[256];
    bool about_dialog_shown;
    bool open_github_page;
    bool open_dialog_shown;
    bool open_dialog_error;
    bool project_close_now;
    bool project_opened;

    file_info project;
    file_info* selected_file;

    std::vector<tab_info> opened_tabs;
};

void InitAppState(app_state* state);
void DestroyAppState(app_state* state);
bool UpdateAppState(app_state* state);
