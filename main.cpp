#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Window.hpp>
#include <iostream>
#include <experimental/filesystem>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <Windows.h>
#include <winbase.h>
#include <shellapi.h>
#include <math.h>
#include <bass.h>
#include <fstream>

float sizeX = 200;
float sizeY = 400;
ImVec2 posFirst = ImVec2(200,0);
bool showStyleEditor = false;
bool alwaysForeGround = false;
bool firstFrame = true;
bool trackEnding = false;
bool repeatTrack = false;
bool autoPlay = false;
bool noDirSet = true;
std::string playListFolder ;
std::string g_filePath;
static char buf[128] = "";
int selectedTrack = -1;
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
#define IM_NEWLINE  "\r\n"

namespace fs = std::experimental::filesystem;
//helper to deliver a path with const *void type
const void* getFile(std::string* file){
    return file->data();
}
void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

   

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Safe Area Padding");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export"))
            {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            
            static float window_scale = 1.0f;
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
                ImGui::SetWindowFontScale(window_scale);
            ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
            ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            if (ImGui::IsItemActive())
            {
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("(R = radius, N = number of segments)");
                ImGui::Spacing();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    ImGui::BeginGroup();

                    ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    /*
                    const ImVec2 p2 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
                    */

                    ImGui::EndGroup();
                    ImGui::SameLine();
                }
                ImGui::EndTooltip();
            }
            ImGui::SameLine();

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
    if (ImGui::Button("Close"))
    {
        showStyleEditor = false;
    }

}











int main() {
    //production only
    //FreeConsole();

    int device = -1;
    int freq = 44100;
    HSTREAM streamHandle;

    BASS_Init(device,freq,0,0,NULL);

    

    //save file
    std::ofstream settingsfile;
    if (!std::experimental::filesystem::exists("playerSettings.ini"))
    {
        settingsfile.open("playerSettings.ini");
        settingsfile << "[SETTINGS BEGIN]" << std::endl;
        settingsfile.close();
    }
        
    








    sf::VideoMode res = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(400,400), "ImGui + SFML = <3",sf::Style::None);
    
    sf::Image icon;
    icon.loadFromFile("logo-color.png");
    window.setIcon(icon.getSize().x,icon.getSize().y,icon.getPixelsPtr());

    
    SetWindowLong(window.getSystemHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(window.getSystemHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    

    sf::Clock deltaClock;
    bool isLeftPressed = false;
    bool isRightPressed = false;
    bool continousPress = false;
    bool playing = false;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    std::vector<std::string> helperList  = std::vector<std::string>();
    std::vector<std::string> autoPlaylist = std::vector<std::string>();
    typedef std::vector<std::pair<std::string,std::string>> stringMap;
    stringMap playList  = std::vector<std::pair<std::string,std::string>>();
   
    float fft[2048] = {0};



    while (window.isOpen()) {
        //DONT RECREATE OBJECTS IN LOOP
        sf::Event event;
        
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(res.width,res.height);
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        while (window.pollEvent(event)) {
            
            ImGui::SFML::ProcessEvent(event);
            
             if (io.WantCaptureMouse)
            {
               
                
               
                
                if (event.type == sf::Event::MouseButtonPressed )
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        
                          isLeftPressed = true;

 
                        
                        
                                                

                    }

                    if(event.mouseButton.button == sf::Mouse::Right)
                    {
                        isRightPressed = true;
                    }
                    
 
                }

                if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        
                        
                          
                        
                        
                        isLeftPressed = false;
                        continousPress = false;
                        
                    }

                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        isRightPressed = false;
                        continousPress = false;
                    }
                }
            }
               
            
           
            
            

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if(isLeftPressed && isRightPressed)
        {

            //sf::Vector2i posWindowScreenSpace = window.mapCoordsToPixel(sf::Vector2f(posWindow.x,posWindow.y));
            //press from previous frame
           
            sf::Vector2i pos = sf::Mouse::getPosition();
            sf::Vector2i windowPos = window.getPosition();
            
    
            sf::Vector2i offset = windowPos - pos;
            
         
        
            window.setPosition(sf::Vector2i( pos.x,pos.y));
        }

        int playListSize = playList.size();
        ImGui::SFML::Update(window, deltaClock.restart());
        
        
        ImGui::SetNextWindowSize(ImVec2(sizeX,sizeY));
        ImGui::SetNextWindowPos(posFirst);
        ImGui::Begin("Settings",0,ImGuiWindowFlags_NoCollapse);
            //set variable to hide 
            if (showStyleEditor) ImGui::ShowStyleEditor();
            ImGui::Checkbox("Player Always On Top",&alwaysForeGround);
            if (alwaysForeGround)
                SetWindowPos(window.getSystemHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            else
                SetWindowPos(window.getSystemHandle(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            if (ImGui::Button("Press to Configure"))
            {
             //toggle

             showStyleEditor = true;

             
             
             
            }
            
            ImGui::Text("Track Folder Path");
            ImGui::InputText("",buf, IM_ARRAYSIZE(buf));

            //only called firstframe
            if (firstFrame && !noDirSet)
            {
                    firstFrame = false;
                    playListFolder = std::string(buf);
                   
                    for (const auto entry : fs::directory_iterator(playListFolder))
                    {
                        helperList.push_back(entry.path().string());
                    }
                    std::vector<std::string> list = std::vector<std::string>();
                    for ( auto entry : helperList)
                    {
                        
                        std::string helper = entry;
                        
                        const char *str = helper.c_str();
                        char* token = strtok((char*)str, "\\");
                        while (token != NULL)
                        {
                            std::string strTok = std::string(token);
                            if(strTok.find("mp3") != std::string::npos)
                            {
                                playList.push_back(std::pair<std::string,std::string>(strTok,entry));
                                autoPlaylist.push_back(strTok);
                            }

                            token = strtok(NULL,"\\");
                            
                            
                        }

                    }

                    
            }
            
            
            
            if (ImGui::Button("Set"))
            {
                if (playListFolder != std::string(buf))
                {
                    
                    //need to check if entry already written
                    playListFolder = std::string(buf);
                   
                    for (const auto entry : fs::directory_iterator(playListFolder))
                    {
                    // std::cout << entry.path() << std::endl;
                        helperList.push_back(entry.path().string());
                    }
                    std::vector<std::string> list = std::vector<std::string>();
                    for ( auto entry : helperList)
                    {
                        //copy
                        std::string helper = entry;
                        //use copy
                        const char *str = helper.c_str();
                        char* token = strtok((char*)str, "\\");
                        //std::cout << entry << std::endl;
                        while (token != NULL)
                        {
                            //std::cout << token << std::endl;
                            std::string strTok = std::string(token);
                            if(strTok.find("mp3") != std::string::npos)
                            {
                                playList.push_back(std::pair<std::string,std::string>(strTok,entry));
                            }

                            token = strtok(NULL,"\\");
                            
                            
                        }

                    }
                    
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear List"))
            {
                helperList.clear();
                playList.clear();
                memset(&buf,0,sizeof(buf));
                playListFolder.clear();
            }
            ImGui::Text("Tracklist");
            bool erased = false;
            if (ImGui::TreeNode("Selection State: Single Selection"))
            {
                int n = 0;
                int eraseKey;
                for (auto entry : playList)
                {
                    
                    
                    std::string help = entry.first;
                    if (ImGui::Selectable(help.c_str() , selectedTrack == n))
                    {
                        if(!playing)
                        {
                            //get track path
                            std::string path = entry.second;

                            playing = true;
                            streamHandle = BASS_StreamCreateFile(FALSE,getFile(&path),0,0,0);
                            BASS_ChannelPlay(streamHandle,FALSE);
                            int error = BASS_ChannelGetData(streamHandle,fft, BASS_DATA_FFT2048);
                            if (error == -1)
                                std::cout << "Error FFT" << std::endl;
                            std::cout << "GETDATAVALUE" << error << std::endl; 



                        //display FFT Data
                            for (int i = 0 ;i<100;i++)
                            {
                                std::cout << fft[i] << std::endl;
                            }        
                            selectedTrack = n;
                        }
                        else 
                        {
                            BASS_ChannelStop(streamHandle);
                            std::string path = entry.second;

                            
                            streamHandle = BASS_StreamCreateFile(FALSE,getFile(&path),0,0,0);
                            BASS_ChannelPlay(streamHandle,FALSE);
                            selectedTrack = n;
                        }
                        
                    }
                    
                    if (ImGui::BeginPopupContextItem())
                    {
                        // your popup code
                        if (ImGui::Button("Delete Track"))
                        {
                            //only delete from playlist map
                            //Get track name(somehow)
                            eraseKey = n;
                            erased = true;

                        }
                        ImGui::EndPopup();
                    }
                        

                    n++;
                }
                if (erased)
                {
                    playList.erase(std::next(playList.begin(), eraseKey));
                    erased = false;
                }
                
                
                



                ImGui::TreePop();

            }
            if (ImGui::Button("Save folder"))
            {
                settingsfile.open("playerSettings.ini");
                settingsfile << playListFolder << std::endl;
                settingsfile.close();
            }
            

        ImGui::End();




        ImGui::SetNextWindowSize(ImVec2(200,400));
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::Begin("Control",0,ImGuiWindowFlags_NoCollapse);
            if (ImGui::Button("Press to Exit"))
            {
             window.close();
             ImGui::SFML::Shutdown();
            }
            std::string trackLength ;
            std::string trackTimeString;
            
            
            if (playing)
                {
                    QWORD len = BASS_ChannelGetLength(streamHandle,BASS_POS_BYTE);
                    double time = BASS_ChannelBytes2Seconds(streamHandle,len);

                    QWORD positon = BASS_ChannelGetPosition(streamHandle,BASS_POS_BYTE);
                    double trackTime = BASS_ChannelBytes2Seconds(streamHandle,positon);


                    double minutes = time / 60;
                    float percentage = time / trackTime;

                    //compute distance from current point of the track to (estimated) end point
                    float distance = time - trackTime;
                    //if distance falls below a threshold, track ending flag is set
                    if (distance < 1.5f)
                    {
                        //set trackEnding flag
                        trackEnding = true;
                        
                    }

                    if (ImGui::Button("Stop"))
                    {
                        playing = false;
                        BASS_ChannelStop(streamHandle);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Pause"))
                    {
                        BASS_ChannelPause(streamHandle);
                    }
                    ImGui::SameLine();

                    if(ImGui::Button("Start"))
                    {
                        BASS_ChannelStart(streamHandle);
                    }
                    static float val = 0.4;
                    ImGui::VSliderFloat("Volume",ImVec2(18,80), &val,0.0f,1.0f,"");
                    BASS_ChannelSetAttribute(streamHandle,BASS_ATTRIB_VOL,val);
                    ImGui::Text(std::to_string(val).c_str());
                    trackLength = std::string(std::to_string(time));
                    trackTimeString = std::string(std::to_string(trackTime));
                    ImGui::Text(trackLength.c_str());
                    ImGui::Text(trackTimeString.c_str());
                    static float f1 = percentage;
                    if (ImGui::SliderFloat("",&f1, 0.0f, 1.0f, "%.2f"))
                    {
                        double timeInTrack = time * f1;
                        QWORD bytePos = BASS_ChannelSeconds2Bytes(streamHandle,timeInTrack);
                        BASS_ChannelSetPosition(streamHandle,bytePos,BASS_POS_BYTE);

                    }
                    if (ImGui::Checkbox("Repeat Track",&repeatTrack))
                    {
                        if (autoPlay)
                            autoPlay = false;
                    }
                    if (ImGui::Checkbox("AutoPlay",&autoPlay))
                    {
                        if (repeatTrack)
                            repeatTrack = false;
                    }
                    if (autoPlay && trackEnding)
                    {
                        //get next track
                        //TODO: check boundarys
                       // std::cout << "track" << std::endl;
                        //std::cout << playListSize << std::endl;
                        if (selectedTrack+1 == playListSize)
                        {
                            autoPlay = false;
                            std::cout <<"auto false"<< selectedTrack << std::endl;

                        }
                        else
                        {
                            

                            selectedTrack += 1;
                            std::pair<std::string,std::string> entry = playList[selectedTrack];

                            //stop current track
                            BASS_ChannelStop(streamHandle);
                            //start with next track
                            g_filePath = entry.second;
                            streamHandle = BASS_StreamCreateFile(FALSE,getFile(&g_filePath),0,0,0);
                            BASS_ChannelPlay(streamHandle,FALSE);
                        }

                        

                        
                        
                            
                        trackEnding = false;

                    }
                    if (repeatTrack && trackEnding)
                    {

                    }


                }
        ImGui::End();

        

        
        window.clear(sf::Color::Transparent);
        
        ImGui::SFML::Render(window);
        window.display();
        //trackEnding __fallthrough

        trackEnding = false;
        
    }
    BASS_Free();

    ImGui::SFML::Shutdown();
}