#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <unordered_map>

#include <Xinput.h>
#pragma comment(lib, "xinput.lib")
#include "../../utils/nlohmann/json.hpp"

#define XINPUT_GAMEPAD_LEFT_TRIGGER  0x0400
#define XINPUT_GAMEPAD_RIGHT_TRIGGER 0x0800


static const std::unordered_map<std::string, int> VKMap = {
    {"VK_LBUTTON", VK_LBUTTON},
    {"VK_RBUTTON", VK_RBUTTON},
    {"VK_CANCEL", VK_CANCEL},
    {"VK_MBUTTON", VK_MBUTTON},
    {"VK_XBUTTON1", VK_XBUTTON1},
    {"VK_XBUTTON2", VK_XBUTTON2},
    {"VK_BACK", VK_BACK},
    {"VK_TAB", VK_TAB},
    {"VK_RETURN", VK_RETURN},
    {"VK_SHIFT", VK_SHIFT},
    {"VK_CONTROL", VK_CONTROL},
    {"VK_MENU", VK_MENU}, 
    {"VK_PAUSE", VK_PAUSE},
    {"VK_CAPITAL", VK_CAPITAL},
    {"VK_ESCAPE", VK_ESCAPE},
    {"VK_SPACE", VK_SPACE},
    {"VK_PRIOR", VK_PRIOR},
    {"VK_NEXT", VK_NEXT},   
    {"VK_END", VK_END},
    {"VK_HOME", VK_HOME},
    {"VK_LEFT", VK_LEFT},
    {"VK_UP", VK_UP},
    {"VK_RIGHT", VK_RIGHT},
    {"VK_DOWN", VK_DOWN},
    {"VK_F1", VK_F1}, {"VK_F2", VK_F2}, {"VK_F3", VK_F3}, {"VK_F4", VK_F4},
    {"VK_F5", VK_F5}, {"VK_F6", VK_F6}, {"VK_F7", VK_F7}, {"VK_F8", VK_F8},
    {"VK_F9", VK_F9}, {"VK_F10", VK_F10}, {"VK_F11", VK_F11}, {"VK_F12", VK_F12}
};

static const std::unordered_map<std::string, WORD> XButtonMap = {
    {"A", XINPUT_GAMEPAD_A},
    {"B", XINPUT_GAMEPAD_B},
    {"X", XINPUT_GAMEPAD_X},
    {"Y", XINPUT_GAMEPAD_Y},

    {"DPadUp",    XINPUT_GAMEPAD_DPAD_UP},
    {"DPadDown",  XINPUT_GAMEPAD_DPAD_DOWN},
    {"DPadLeft",  XINPUT_GAMEPAD_DPAD_LEFT},
    {"DPadRight", XINPUT_GAMEPAD_DPAD_RIGHT},

    {"LeftShoulder",  XINPUT_GAMEPAD_LEFT_SHOULDER},
    {"RightShoulder", XINPUT_GAMEPAD_RIGHT_SHOULDER},

    {"LeftThumbstick",  XINPUT_GAMEPAD_LEFT_THUMB},
    {"RightThumbstick", XINPUT_GAMEPAD_RIGHT_THUMB},


    {"Menu", XINPUT_GAMEPAD_START},
    {"View", XINPUT_GAMEPAD_BACK},


    {"Start", XINPUT_GAMEPAD_START},
    {"Back",  XINPUT_GAMEPAD_BACK},

    // custom
    {"LeftTrigger",  XINPUT_GAMEPAD_LEFT_TRIGGER},
    {"RightTrigger", XINPUT_GAMEPAD_RIGHT_TRIGGER},
};


struct GeneralCfg {
    int maxControllers = 1;
};

struct MouseCfg {
    int stickActivationKey = 0;
    std::string stick = "LeftStick";
    double sensitivity = 1.4;
    double deadzone = 0.15;
    double triggerSensitivity = 0.05;
    std::vector<std::string> mouseLeftClick;
    std::vector<std::string> mouseRightClick;

    WORD mouseLeftClickMask = 0;
    WORD mouseRightClickMask = 0;
};

struct Binding {
    std::vector<std::string> when; 
    std::string type;
    std::vector<int> keys;

    WORD xinputMask = 0;
};

struct AppConfig {
    GeneralCfg general;
    MouseCfg mouse;
    std::vector<Binding> bindings;
};

inline AppConfig LoadConfig(const std::wstring& path) {
    AppConfig cfg;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening JSON file!" << std::endl;
        return cfg;
    }

    nlohmann::json configData;

    try {
        file >> configData; 
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return cfg;
    }

    if (configData.contains("general")) {
        nlohmann::json g = configData["general"];
        if (g.contains("maxControllers")) cfg.general.maxControllers = g["maxControllers"];
    }

    if (configData.contains("mouse")) {
        nlohmann::json m = configData["mouse"];
        if (m.contains("stickActivationKey")) {
            std::string keyName = m["stickActivationKey"];
            if (auto it = XButtonMap.find(keyName); it != XButtonMap.end()) {
                cfg.mouse.stickActivationKey = it->second;
            } else {
                std::cerr << "Unknown key in 'stickActivationKey': " << keyName<< "\n";
            }
        }
        if (m.contains("stick")) cfg.mouse.stick = m["stick"];
        if (m.contains("sensitivity")) cfg.mouse.sensitivity = m["sensitivity"];
        if (m.contains("deadzone")) cfg.mouse.deadzone = m["deadzone"];
        if (m.contains("mouseLeftClick")) cfg.mouse.mouseLeftClick = m["mouseLeftClick"].get<std::vector<std::string>>();
        if (m.contains("mouseRightClick")) cfg.mouse.mouseRightClick = m["mouseRightClick"].get<std::vector<std::string>>();
        if (m.contains("triggerSensitivity")) cfg.mouse.triggerSensitivity = m["triggerSensitivity"];


        WORD rightClickMask = 0;
        for (const std::string& keyName : cfg.mouse.mouseRightClick) {
            if (auto it = XButtonMap.find(keyName); it != XButtonMap.end()) {
                rightClickMask |= it->second;
            } else if (keyName == "None") {
                // dont do nothing ignore
                std::cout << "None found in mouseRightClick, ignoring\n" << keyName<< "\n";
            } else {
                std::cerr << "Unknown gamepad button in 'mouseRightClick': " << keyName<< "\n";
            }
        }
        cfg.mouse.mouseRightClickMask = rightClickMask;

        WORD leftClickMask = 0;
        for (const std::string& keyName : cfg.mouse.mouseLeftClick) {
            if (auto it = XButtonMap.find(keyName); it != XButtonMap.end()) {
                leftClickMask |= it->second;
            } else if (keyName == "None") {
                // dont do nothing ignore
            } else {
                std::cerr << "Unknown gamepad button in 'mouseLeftClick': " << keyName<< "\n";
            }
        }
        cfg.mouse.mouseLeftClickMask = leftClickMask;
    }

    if (configData.contains("bindings")) {
        for (nlohmann::json& b : configData["bindings"]) {
            Binding bind;

            if (b.contains("when")) {
                bind.when = b["when"].get<std::vector<std::string>>();

                WORD mask = 0;

                for (const std::string& keyName : bind.when) {
                    if (auto it = XButtonMap.find(keyName); it != XButtonMap.end()) {
                        mask |= it->second;
                    } else if (keyName == "None") {
                        // dont do nothing ignore
                    } else {
                        std::cerr << "Unknown gamepad button in 'when': " << keyName<< "\n";
                    }
                }
                bind.xinputMask = mask;
            }

            if (b.contains("do")) {
                nlohmann::json d = b["do"];
                if (d.contains("type")) bind.type = d["type"];
                if (d.contains("keys")) {
                    std::vector<std::string> keys = d["keys"].get<std::vector<std::string>>();

                    for (const std::string& keyName : keys) {
                        if (auto it = VKMap.find(keyName); it != VKMap.end()) {
                            bind.keys.push_back(it->second);
                        } else {
                            std::cerr << "Unknown key in 'do': " << keyName<< "\n";
                        }
                    }

                }
            }
            cfg.bindings.push_back(bind);
        }   
    }


    return cfg;
}