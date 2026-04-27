#include "app_state.h"
#include "file_loader.h"

#include "imgui/rl_imgui.h"
#include "imgui_internal.h"

void DestroyFileInfo(file_info* info)
{
    for (unsigned int i = 0; i < info->children.count; i++)
        DestroyFileInfo(&info->children.data[i]);

    if (info->children.count > 0)
        delete info->children.data;
}

void BuildFileInfo(file_info* info)
{
    FilePathList file_list = LoadDirectoryFiles(info->path);
    
    if (file_list.count > 0)
    {
        info->children.count = file_list.count;
        info->children.data = new file_info[info->children.count];

        for (unsigned int i = 0; i < info->children.count; i++)
        {
            info->children.data[i] = { 0 };
            info->children.data[i].name = GetFileName(file_list.paths[i]);

            info->children.data[i].is_directory = !IsPathFile(file_list.paths[i]);
            info->children.data[i].path = file_list.paths[i];

            if (info->children.data[i].is_directory)
                BuildFileInfo(&info->children.data[i]);
        }

        RL_FREE(file_list.paths);
    }
}

void AddTab(app_state* state, file_type type, file_info* info)
{
    tab_info tab = {};
    tab.type = type;
    tab.file = info;
    tab.opened = true;
    
    file* f = DispatchLoadFile(info->path);
    bool succ = false;

    if (f != nullptr)
    {
        switch (type)
        {
            case file_type::ARCHIVE: {
                tab.data = new file_archive();
                succ = f->BuildArchive((file_archive*)tab.data);
                break;
            }
            default: {
                break;
            }
        }

        f->Unload();
        delete f;
    }

    if (tab.data != nullptr && !succ)
    {
        delete tab.data;
        tab.data = nullptr;
    }

    state->opened_tabs.push_back(tab);
}

void RemoveTab(app_state* state)
{
    
}

void CloseProject(app_state* state)
{
    if (state->project_opened)
    {
        DestroyFileInfo(&state->project);
    }

    for (size_t i = 0; i < state->opened_tabs.size(); i++)
    {
        if (state->opened_tabs[i].data != nullptr)
        {
            //delete state->opened_tabs[i].data;
        }
    }

    memset(state, 0, sizeof(app_state));
}

void UpdateContent_Archive(app_state* state, file_archive* archive)
{
    for (size_t i = 0; i < archive->GetFileCount(); i++)
    {
        file_archive::file_archive_info* info = archive->GetFileInfoByIdx(i);
        ImGui::Text(info->fileName.c_str());
    }
}

void UpdateTabView(app_state* state, tab_info* info)
{
    if (ImGui::BeginTabItem(info->file->name, &info->opened))
    {
        ImGui::BeginChild((const char*)(info));
        {
            if (info->data != nullptr)
            {
                switch (info->type)
                {
                    case file_type::ARCHIVE:
                        UpdateContent_Archive(state, (file_archive*)info->data);
                        break;
                    default:
                        break;
                }

            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed to load file");
            }
        }
        ImGui::EndChild();

        ImGui::EndTabItem();
    }

    if (!info->opened)
    {
        //RemoveTab(state);
    }
}

void UpdateContextMenu(app_state* state, file_info* info)
{
    int option_count = 0;

    if (IsFileArchive(info->path))
    {
        option_count++;
        if (ImGui::MenuItem("Open as Archive")) AddTab(state, file_type::ARCHIVE, info);
    }

    if (IsFileModel(info->path))
    {
        option_count++;
        if (ImGui::MenuItem("Open as Model")) AddTab(state, file_type::MODEL, info);
    }

    if (IsFileImage(info->path))
    {
        option_count++;
        if (ImGui::MenuItem("Open as Image")) AddTab(state, file_type::IMAGE, info);
    }

    if (IsFileImageAnimated(info->path))
    {
        option_count++;
        if (ImGui::MenuItem("Open as Animated Image")) AddTab(state, file_type::IMAGE_ANIM, info);
    }

    if (IsFileFont(info->path))
    {
        option_count++;
        if (ImGui::MenuItem("Open as Font")) AddTab(state, file_type::FONT, info);
    }

    if (option_count <= 0)
    {
        ImGui::MenuItem("No option", nullptr, false, false);
    }
}

void UpdateHierachy(app_state* state, file_info* info)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (state->selected_file == info)
        flags |= ImGuiTreeNodeFlags_Selected;

    if (info->children.count == 0)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool expanded =	ImGui::TreeNodeBehavior(ImGui::GetID(info), flags, info->name, nullptr);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        state->selected_file = info;

    if (!info->is_directory && ImGui::BeginPopupContextItem())
    {
        UpdateContextMenu(state, info);
        ImGui::EndPopup();
    }

    if (expanded)
    {
        for (unsigned int i = 0; i < info->children.count; i++)
        {
            UpdateHierachy(state, &info->children.data[i]);
        }

        ImGui::TreePop();
    }
}

void UpdateAboutDialog(app_state* state)
{
    ImGui::SetNextWindowSize(ImVec2(320, 96));
    
    ImGui::Begin("About YW-Viewer", nullptr);

    ImGui::Text("YW-Viewer: Version Dev");
    ImGui::Text("2026 Liam/DEV");
    
    if (ImGui::Button("OK"))
    {
        state->about_dialog_shown = false;
    }

    ImGui::End();
}

void UpdateOpenDialog(app_state* state)
{
    ImGui::SetNextWindowSize(ImVec2(320, 96));

    ImGui::Begin("Open a directory...", nullptr, ImGuiWindowFlags_NoDocking);

    if (ImGui::InputText("Path", state->project_path, sizeof(state->project_path) / sizeof(char)))
    {
        state->open_dialog_error = false;
    }

    if (state->open_dialog_error)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid directory");
    }

    if (ImGui::Button("Cancel"))
    {
        memset(state->project_path, 0, sizeof(state->project_path) / sizeof(char));
        state->open_dialog_error = false;
        state->open_dialog_shown = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("OK"))
    {
        if (!DirectoryExists(state->project_path) || strcmp(state->project_path, GetPrevDirectoryPath(state->project_path)) == 0)
        {
            state->open_dialog_error = true;
        }
        else
        {
            state->project.name = "root";
            state->project.is_directory = true;
            state->project.path = state->project_path;
            BuildFileInfo(&state->project);

            state->project_opened = true;
            state->open_dialog_shown = false;
        }
    }

    ImGui::End();
}

void InitAppState(app_state* state)
{
    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
}

void DestroyAppState(app_state* state)
{
    if (state->project_opened)
    {
        DestroyFileInfo(&state->project);
    }

    rlImGuiShutdown();
}

bool UpdateAppState(app_state* state)
{
    rlImGuiBegin();
    
    // Base
    {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);
        ImGui::SetNextWindowViewport(vp->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());

        ImGui::Begin("Content", nullptr,
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        );
        
        ImGui::PopStyleVar(3);
            
        ImGui::Dummy(ImVec2(0, ImGui::GetFrameHeight() - 4.0f));

        ImGuiID dockspaceId = ImGui::GetID("Dock");
        ImGui::DockSpace(dockspaceId, ImVec2(), ImGuiDockNodeFlags_PassthruCentralNode);

        if (!state->app_init)
        {
            ImGuiID nextId = dockspaceId;

            ImGui::DockBuilderDockWindow("Hierachy", ImGui::DockBuilderSplitNode(
                nextId, ImGuiDir_Left, 0.3f, nullptr, &nextId
            ));

            ImGui::DockBuilderDockWindow("Main", nextId);

            ImGui::DockBuilderFinish(dockspaceId);

            state->app_init = true;
        }

        ImGui::End();
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) state->open_dialog_shown = true;
            if (ImGui::MenuItem("Close", nullptr, nullptr, state->project_opened)) state->project_close_now = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4")) state->app_exit = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("GitHub")) state->open_github_page = true;

            ImGui::Separator();

            if (ImGui::MenuItem("About")) state->about_dialog_shown = true;

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        if (state->open_dialog_shown)
        {
            UpdateOpenDialog(state);
        }

        if (state->project_close_now)
        {
            CloseProject(state);
        }

        if (state->open_github_page)
        {
            OpenURL("https://github.com/liro-ze/ywviewer");
            state->open_github_page = false;
        }

        if (state->about_dialog_shown)
        {
            UpdateAboutDialog(state);
        }
    }

    ImGui::Begin("Hierachy", nullptr, ImGuiWindowFlags_NoCollapse);
    {
        //ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        if (state->project_opened)
        {
            UpdateHierachy(state, &state->project);
        }
    }
    ImGui::End();

    ImGui::Begin("Main");
    {
        ImGui::BeginTabBar("TabView", ImGuiTabBarFlags_AutoSelectNewTabs);
        {
            for (size_t i = 0; i < state->opened_tabs.size(); i++)
                UpdateTabView(state, &state->opened_tabs[i]);
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
    
    rlImGuiEnd();

    return (!state->app_exit);
}
