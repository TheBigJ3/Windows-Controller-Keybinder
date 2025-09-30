# Windows Controller Keybinder

### About Me
Hi! I’m Jerry — a Computer Science student at **California State University, Fullerton (Class of 2029)**.  

This project started when I moved into my college apartment and realized I forgot to bring a keyboard and mouse for my PC. I thought I could just use my Xbox controller, but quickly learned the joystick doesn’t actually move the mouse… making it basically useless for normal navigation.  

So, I built this **C++ project** to let me bind **keyboard and mouse inputs** to my Xbox controller. The goal was to make it **super customizable**, not only to avoid interfering with gameplay but to also allow **endless mapping possibilities**.

---

## 🎮 How It Works
- Uses **XInput** to read Xbox controller input.  
- Uses **Windows Virtual-Key codes (VK codes)** to simulate keyboard/mouse outputs.  
- You can fully customize what each controller button does.  

To avoid interfering with games, the program listens for controller input **only when an activation key is held down**.  
- Default: the **“View / Back” button** must be held for joystick movements or binds to trigger.

---

## 🔧 Key Features
- ✅ Map Xbox controller buttons, triggers, and sticks to **any keyboard key**  
- ✅ Flexible activation system so binds don’t break gameplay  
- ✅ Extendable mapping system for custom inputs  

### Example: Custom Inputs
```cpp
{"LeftTrigger",  XINPUT_GAMEPAD_LEFT_TRIGGER},
{"RightTrigger", XINPUT_GAMEPAD_RIGHT_TRIGGER},
```

Defined as:
```cpp
#define XINPUT_GAMEPAD_LEFT_TRIGGER  0x0400
#define XINPUT_GAMEPAD_RIGHT_TRIGGER 0x0800
```

---

## 📚 Documentation
- [XInput (Microsoft Docs)](https://learn.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad)  
- [Virtual-Key Codes (Microsoft Docs)](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)  

---

## Suggestions 
Currently I dont have it automatically add to your startup applications but you can easily set this yourself one time.

---

## 📄 License
This project is open-source under the MIT License.
