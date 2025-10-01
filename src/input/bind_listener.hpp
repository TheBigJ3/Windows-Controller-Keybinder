#pragma once

#include "../loader/config_loader.hpp"
#include <Windows.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>  
#include <Xinput.h>
#include <algorithm>

using namespace winrt;
using namespace Windows::Gaming::Input;
#pragma comment(lib, "xinput.lib")


class BindListener {
    private:
        MouseCfg mouseCfg_;
        GeneralCfg generalCfg_;
        std::vector<Binding> bindings_;
        std::vector<Binding*> activeBindings_[XUSER_MAX_COUNT];

        bool mouseRightClickActive_[XUSER_MAX_COUNT] = {false};
        bool mouseLeftClickActive_[XUSER_MAX_COUNT]  = {false};
        bool stickActive_[XUSER_MAX_COUNT] = {false};

        DWORD lastPacket_[XUSER_MAX_COUNT] = {0};

        static bool containsPtr(const std::vector<Binding*>& v, Binding* p) {
            return std::find(v.begin(), v.end(), p) != v.end();
        }

        static void erasePtr(std::vector<Binding*>& v, Binding* p) {
            auto it = std::find(v.begin(), v.end(), p);
            if (it != v.end()) v.erase(it);
        }

        static inline bool maskDown(const WORD& buttons, const WORD& mask) {
            return (mask != 0) && ((buttons & mask) == mask);
        }

        inline void incorporateTriggerToB(WORD& b, const int& tR, const int& tL) {
            if (tR > (int)(mouseCfg_.triggerSensitivity * 255)) b |= XINPUT_GAMEPAD_RIGHT_TRIGGER;
            if (tL > (int)(mouseCfg_.triggerSensitivity * 255)) b |= XINPUT_GAMEPAD_LEFT_TRIGGER;
        }

        void InputEngine(DWORD user, Binding& bind, WORD buttons) {
            auto& activeBinds = activeBindings_[user];
            const bool isDown = maskDown(buttons,bind.xinputMask);
            const bool wasActive = containsPtr(activeBinds, &bind);

            std::cout << "Binding of type " << bind.type << " isDown: " << isDown << " wasActive: " << wasActive << "\n";
            
            if (isDown && !wasActive) {
                for (int vk : bind.keys) {
                    keybd_event((BYTE)vk, 0, 0, 0);
                }
                activeBinds.push_back(&bind);
            }  else if (!isDown && wasActive) {
                for (int vk : bind.keys) {
                    keybd_event((BYTE)vk, 0, KEYEVENTF_KEYUP, 0);
                }
                erasePtr(activeBinds, &bind);
            }
        }

        inline void solveMouseMovement(SHORT thumbX, SHORT thumbY) {
            ShowCursor(TRUE);

            // Move the mouse multiplying by sensitivity
            // Apply deadzone
            const double deadzone = mouseCfg_.deadzone;
            const double sensitivity = mouseCfg_.sensitivity;

            double normX = static_cast<double>(thumbX) / 32768.0;
            double normY = static_cast<double>(thumbY) / 32768.0;

            // Apply deadzone
            if (std::abs(normX) < deadzone) normX = 0.0;
            if (std::abs(normY) < deadzone) normY = 0.0;

            // Apply sensitivity
            normX *= sensitivity;
            normY *= sensitivity;

            // Move mouse
            int moveX = static_cast<int>(normX * 10); // Scale factor for movement
            int moveY = static_cast<int>(normY * 10);

            if (moveX != 0 || moveY != 0) {
                mouse_event(MOUSEEVENTF_MOVE, moveX, -moveY, 0, 0); // Invert Y for screen coordinates
            }
        }

        void inline joystickMovement(const XINPUT_STATE& state) {
            if (mouseCfg_.stick == "RightStick") {
                solveMouseMovement(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
            } else {
                solveMouseMovement(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
            }
        }


        void MouseUpdate(const XINPUT_STATE& state, DWORD user) {
            WORD buttons = state.Gamepad.wButtons;

            const bool rightMouseClickIsDown = maskDown(buttons, mouseCfg_.mouseRightClickMask);
            const bool leftMouseClickIsDown  = maskDown(buttons, mouseCfg_.mouseLeftClickMask);

            std::cout << "Right Mouse Click is down: " << rightMouseClickIsDown << "\n";
            std::cout << "Left Mouse Click is down: " << leftMouseClickIsDown << "\n";

            std::cout << "Total Mask" << buttons << "\n";

            
            if (rightMouseClickIsDown && !mouseRightClickActive_[user]) {
                // press mouse right
                mouse_event(MOUSEEVENTF_RIGHTDOWN, 0,0,0,0);
                mouseRightClickActive_[user] = true;
            } else if (!rightMouseClickIsDown && mouseRightClickActive_[user]) {
                // release mouse right
                mouse_event(MOUSEEVENTF_RIGHTUP, 0,0,0,0);
                mouseRightClickActive_[user] = false;
            }

            if (leftMouseClickIsDown && !mouseLeftClickActive_[user]) {
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0,0,0,0);
                mouseLeftClickActive_[user] = true;
            } else if (!leftMouseClickIsDown && mouseLeftClickActive_[user]) {
                mouse_event(MOUSEEVENTF_LEFTUP, 0,0,0,0);
                mouseLeftClickActive_[user] = false;
            }


            // Handle mouse movement
            // Is Activation Key Active
            const bool stickActivationIsDown = (mouseCfg_.stickActivationKey == 0 || maskDown(buttons, mouseCfg_.stickActivationKey));
            stickActive_[user] = stickActivationIsDown;
            std::cout << "Stick Activation is down: " << stickActivationIsDown << "\n";

            if (!stickActivationIsDown) return;

            std::cout << "RightThumb Pos: (" << state.Gamepad.sThumbRX << "," << state.Gamepad.sThumbRY << ")\n";
            std::cout << "LeftThumb Pos: (" << state.Gamepad.sThumbLX << "," << state.Gamepad.sThumbLY << ")\n";

            joystickMovement(state);
        }
    public:
        BindListener(AppConfig cfg) : mouseCfg_(cfg.mouse), generalCfg_(cfg.general), bindings_(std::move(cfg.bindings)) {
            std::cout << "XInput polling ready\n";
        };


        void Update() {
            for (DWORD user = 0; user < XUSER_MAX_COUNT; ++user) {
                XINPUT_STATE state;
                ZeroMemory(&state,sizeof(state));

                DWORD result = XInputGetState(user,&state);

                if (result != ERROR_SUCCESS) continue; // not connected

                if (state.dwPacketNumber == lastPacket_[user]) { // no change since last read; skip spam
                    
                    if (stickActive_[user]) {
                        joystickMovement(state);
                    }

                    continue;
                }
                lastPacket_[user] = state.dwPacketNumber;

                int tRight = int(state.Gamepad.bRightTrigger);
                int tLeft = int(state.Gamepad.bLeftTrigger);
                incorporateTriggerToB(state.Gamepad.wButtons, tRight, tLeft);

                WORD b = state.Gamepad.wButtons;

                std::cout << "Controller " << user << " state changed. Buttons bitmask: " << b << " Trigger R: " << tRight << " Trigger L: " << tLeft << "\n";


                for (auto& bind : bindings_) {
                    if (bind.xinputMask == 0) continue;
                    InputEngine(user, bind, b);
                }
                
                MouseUpdate(state,user);
            }

            
        }
};