#pragma once
#ifndef OLC_PGEX_HIDMANAGER_H
#define OLC_PGEX_HIDMANAGER_H

#include "olcPixelGameEngine.h"


/*
* FOR XBOX GAMEPAD
*/
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<Xinput.h>
#pragma comment(lib, "Xinput.lib")

#include <iostream>


namespace olc
{
	namespace hid
	{
		class HIDManager;
		class Mouse;
		class Keyboard;
		class Gamepad;
		struct Rumble;
		struct GamepadColor;
		class OLCHIDManager;
		class XBoxGamepad;
		class OLCKeyboard;
		class OLCMouse;


		/*
		* Interface classes.
		*/
		/*
		* More of a wrapper for the mouse, keyboard and gamepad classes.
		*
		* The HIDManager can manage a keyboard and mouse,
		* and/or a set of gamepads.
		*
		*/
		class HIDManager
		{
		public:


			/*
			* General Manager stuff.
			*/
			virtual void update() = 0;
			virtual int gamepad(std::string) = 0;
			virtual void enumerateGamepads(std::vector<std::string>*) = 0;
			virtual void setGamepadStickThreshhold(float) = 0; // Numerical threshhold for registering input.
			virtual void removeGamepad(const char*) = 0;
			virtual void storeGamepad(Gamepad*) = 0;

			/*
			* Keyboard stuff.
			*/
			virtual bool keyDown(int) = 0;
			virtual bool keyHeld(int) = 0;
			virtual bool keyReleased(int) = 0;
			virtual bool anyKeyDown() = 0;


			/*
			* Mouse stuff.
			*/
			virtual bool lmbDown() = 0;
			virtual bool rmbDown() = 0;
			virtual bool wheelDown() = 0;

			virtual bool lmbReleased() = 0;
			virtual bool rmbReleased() = 0;
			virtual bool wheelReleased() = 0;

			virtual float mouseX() = 0;
			virtual float mouseY() = 0;
			virtual void setMousePosition(float, float) = 0;


			/*
			* Gamepad stuff.
			*/
			virtual bool buttonDown(const char*, const char*) = 0;
			virtual void rumble(const char*, Rumble) = 0;
			virtual void rumbleOff(const char*) = 0;
			virtual float leftTrigger(const char*) = 0;
			virtual float rightTrigger(const char*) = 0;
			virtual float leftStickX(const char*) = 0;
			virtual float leftStickY(const char*) = 0;
			virtual float rightStickX(const char*) = 0;
			virtual float rightStickY(const char*) = 0;
			virtual float oneFingerTouchPadX(const char*) = 0; // Touching with one finger.
			virtual float oneFingerTouchPadY(const char*) = 0;
			virtual float twoFingerTouchPadX(const char*) = 0; // Touching with two fingers.
			virtual float twoFingerTouchPadY(const char*) = 0;

			virtual float gravityX(const char*) = 0; // Right and Left Axis.
			virtual float gravityY(const char*) = 0; // Upside down or not Axis.
			virtual float gravityZ(const char*) = 0; // Forward and Backward oriented Axis.

			virtual void setColor(const char*, GamepadColor) = 0;
			virtual GamepadColor color(const char*) = 0;

			virtual int batteryPercent(const char*) = 0;
			virtual std::string batteryString(const char*) = 0;
			virtual std::string name(const char*) = 0;
			virtual std::string vendor(const char*) = 0;
		};

		/*
		* Mouse 
		*/
		class Mouse
		{
		public:

			virtual void setMousePosition(float, float) = 0;

			virtual float mouseX() = 0;
			virtual float mouseY() = 0;

			virtual bool buttonDown(int) = 0;
			virtual bool buttonHeld(int) = 0;
			virtual bool buttonReleased(int) = 0;


			virtual void update() = 0;
		};



		/*
		* Keyboard
		*/
		class Keyboard
		{
		public:

			virtual bool keyHeld(int) = 0;
			virtual bool keyDown(int) = 0;
			virtual bool keyReleased(int) = 0;
			virtual bool anyKeyDown() = 0;


			virtual void update() = 0;
		};

		/*
		* Gamepad
		*/
		/*
		* Rumble is measured in percent of max available value.
		*/
		struct Rumble
		{
			Rumble(int big, int little) : m_Big(big), m_Small(little)
			{
				/*
				* XBox
				* 100% = 65535
				* big% = (big	*	65535) / 100
				*/
				m_XboxRumbleRight = (big * 65535) / 100;
				m_XboxRumbleLeft = (little * 65535) / 100;
			}

			int m_XboxRumbleLeft = 0; // Big gyro.
			int m_XboxRumbleRight = 0; // Small gyro.

		private:
			int m_Big = 0;
			int m_Small = 0;
		};


#define HEAVY_RUMBLE Rumble(100, 100)
#define SMALL_RUMBLE Rumble(10, 10)
#define OFF_RUMBLE Rumble(0, 0)


		/*
		* Mainly for the PS Controller. As currently
		* the XBox does not support color output.
		*/
		struct GamepadColor
		{
			GamepadColor() {}

			GamepadColor(unsigned r, unsigned g, unsigned b)
			{
				m_R = (r > 255) ? 255 : r;
				m_G = (g > 255) ? 255 : g;
				m_B = (b > 255) ? 255 : b;
			}

			int m_R = 0, m_G = 0, m_B = 0;
		};

#define PADCOLOR_RED GamepadColor(255, 0, 0)
#define PADCOLOR_BLUE GamepadColor(0, 0, 255)
#define PADCOLOR_YELLOW GamepadColor(255, 255, 0)
#define PADCOLOR_WHITE GamepadColor(255, 255, 255)
#define PADCOLOR_PINK GamepadColor(255, 0, 255)
#define PADCOLOR_CYAN GamepadColor(0, 255, 255)
#define PADCOLOR_GREEN GamepadColor(0, 255, 0)
#define PADCOLOR_OFF GamepadColor(0, 0, 0)



		/*
		* Note:
		*
		* As the sony gamepads are more "feature-rich" we orientate the class on them.
		*
		* Any other gamepads, e.g. Xbox, override methods for features they dont have
		* with reasonable default values.
		* 
		*/
		class Gamepad
		{
		public:
			virtual int id() = 0;
			virtual bool refresh() = 0;

			virtual void rumble(Rumble) = 0;
			virtual void rumbleOff() = 0;

			virtual float leftTrigger() = 0;
			virtual float rightTrigger() = 0;

			virtual float leftStickX() = 0;
			virtual float leftStickY() = 0;
			virtual float rightStickX() = 0;
			virtual float rightStickY() = 0;

			virtual bool buttonDown(int) = 0;

			virtual float oneFingerTouchPadX() = 0; // Touching with one finger.
			virtual float oneFingerTouchPadY() = 0;
			virtual float twoFingerTouchPadX() = 0; // Touching with two fingers.
			virtual float twoFingerTouchPadY() = 0;

			virtual float gravityX() = 0; // Right and Left Axis.
			virtual float gravityY() = 0; // Upside down or not Axis.
			virtual float gravityZ() = 0; // Forward and Backward oriented Axis.


			virtual void setColor(GamepadColor) = 0;
			virtual GamepadColor color() = 0;

			/*
			* Basic information functions.
			*/
			virtual int batteryPercent() = 0;
			virtual std::string batteryString() = 0;
			virtual std::string name() = 0;
			virtual std::string vendor() = 0;

		};




		/*
		* Implementation classes.
		*/

		/*
		* OLC Keyboard
		*/
		class OLCKeyboard : public Keyboard
		{
		public:
			OLCKeyboard(olc::PixelGameEngine* pge) : pge(pge)
			{
			}

			bool keyHeld(int mask) override
			{
				return pge->GetKey((olc::Key)mask).bHeld;
			}
			bool keyDown(int mask) override
			{
				return pge->GetKey((olc::Key)mask).bPressed;

			}
			bool keyReleased(int mask) override
			{
				return pge->GetKey((olc::Key)mask).bReleased;

			}
			bool anyKeyDown() override
			{
				for (int i = 0; i < olc::Key::ENUM_END; i++)
				{
					if (pge->GetKey((olc::Key)i).bPressed)
					{
						return true;
					}
				}

				return false;
			}

			void update() override
			{
				return; // Not needed in olc.
			}


		private:

			olc::PixelGameEngine* pge = nullptr;
		};

		/*
		* OLC Mouse
		*/
		class OLCMouse : public Mouse
		{
		public:
			OLCMouse(olc::PixelGameEngine* pge) : pge(pge)
			{
			}


			void setMousePosition(float x, float y) override
			{
				return; // Not Supported in olc.
			}

			float mouseX() override
			{
				return pge->GetMousePos().x;
			}
			float mouseY() override
			{
				return pge->GetMousePos().y;
			}

			/*
			* Note: 0 := LMB. 1 := RMB. 2 := Wheel Press.
			*/
			bool buttonDown(int mask) override
			{
				return pge->GetMouse(mask).bPressed;
			}
			bool buttonHeld(int mask) override
			{
				return pge->GetMouse(mask).bHeld;
			}
			bool buttonReleased(int mask) override
			{
				return pge->GetMouse(mask).bReleased;
			}

			void update() override
			{
				return; // Not needed for olc.
			}


		private:

			olc::PixelGameEngine* pge = nullptr;
		};


		/*
		* XBox Gamepad
		*/
		class XBoxGamepad : public Gamepad
		{
		public:
			XBoxGamepad(int id) : m_DeviceID(id)
			{
				m_DeviceName = "xinput_" + std::to_string(m_DeviceID);
			}

			~XBoxGamepad()
			{

			}

			int id() override
			{
				return m_DeviceID;
			}
			bool refresh() override
			{
				if (m_DeviceID == -1)
					isConnected();

				if (m_DeviceID != -1)
				{
					ZeroMemory(&m_State, sizeof(XINPUT_STATE));
					if (XInputGetState(m_DeviceID, &m_State) != ERROR_SUCCESS)
					{
						m_DeviceID = -1;
						return false;
					}

					float normLX = fmaxf(-1, (float)m_State.Gamepad.sThumbLX / 32767);
					float normLY = fmaxf(-1, (float)m_State.Gamepad.sThumbLY / 32767);

					m_LeftStickX = (abs(normLX) < m_DeadZoneX ? 0 : (abs(normLX) - m_DeadZoneX) * (normLX / abs(normLX)));
					m_LeftStickY = (abs(normLY) < m_DeadZoneY ? 0 : (abs(normLY) - m_DeadZoneY) * (normLY / abs(normLY)));

					if (m_DeadZoneX > 0) m_LeftStickX *= 1 / (1 - m_DeadZoneX);
					if (m_DeadZoneY > 0) m_LeftStickY *= 1 / (1 - m_DeadZoneY);

					float normRX = fmaxf(-1, (float)m_State.Gamepad.sThumbRX / 32767);
					float normRY = fmaxf(-1, (float)m_State.Gamepad.sThumbRY / 32767);

					m_RightStickX = (abs(normRX) < m_DeadZoneX ? 0 : (abs(normRX) - m_DeadZoneX) * (normRX / abs(normRX)));
					m_RightStickY = (abs(normRY) < m_DeadZoneY ? 0 : (abs(normRY) - m_DeadZoneY) * (normRY / abs(normRY)));

					if (m_DeadZoneX > 0) m_RightStickX *= 1 / (1 - m_DeadZoneX);
					if (m_DeadZoneY > 0) m_RightStickY *= 1 / (1 - m_DeadZoneY);

					m_LeftTrigger = (float)m_State.Gamepad.bLeftTrigger / 255;
					m_RightTrigger = (float)m_State.Gamepad.bRightTrigger / 255;

					return true;
				}
				return false;
			}

			void rumble(Rumble r) override
			{
				XINPUT_VIBRATION vibration;
				ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
				vibration.wLeftMotorSpeed = r.m_XboxRumbleRight;
				vibration.wRightMotorSpeed = r.m_XboxRumbleLeft;
				XInputSetState(m_DeviceID, &vibration);
			}
			void rumbleOff() override
			{
				XINPUT_VIBRATION vibration;
				ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
				vibration.wLeftMotorSpeed = 0;
				vibration.wRightMotorSpeed = 0;
				XInputSetState(m_DeviceID, &vibration);
			}

			float leftTrigger() override
			{
				return m_LeftTrigger;
			}
			float rightTrigger() override
			{
				return m_RightTrigger;
			}

			float leftStickX() override
			{
				return m_LeftStickX;
			}
			float leftStickY() override
			{
				return m_LeftStickY;
			}
			float rightStickX() override
			{
				return m_RightStickX;
			}
			float rightStickY() override
			{
				return m_RightStickY;
			}

			bool buttonDown(int mask) override
			{
				return (m_State.Gamepad.wButtons & (WORD)mask) != 0;
			}

			float oneFingerTouchPadX() override
			{
				return 0;
			}
			float oneFingerTouchPadY() override
			{
				return 0;
			}
			float twoFingerTouchPadX() override
			{
				return 0;
			}
			float twoFingerTouchPadY() override
			{
				return 0;
			}

			float gravityX() override
			{
				return 0;
			}
			float gravityY() override
			{
				return 0;
			}
			float gravityZ() override
			{
				return 0;
			}

			void setColor(GamepadColor) override { return; }
			GamepadColor color() override { return PADCOLOR_OFF; }

			/*
			* Basic information functions.
			*
			* Note: For PS Devices we cant derive information for
			* Thus we say either "unknown" or -1 to indicate it.
			*/
			int batteryPercent() override
			{
				XINPUT_BATTERY_INFORMATION battery;
				XInputGetBatteryInformation(m_DeviceID, BATTERY_DEVTYPE_GAMEPAD, &battery);

				return battery.BatteryLevel;
			}
			std::string batteryString() override
			{
				XINPUT_BATTERY_INFORMATION battery;
				XInputGetBatteryInformation(m_DeviceID, BATTERY_DEVTYPE_GAMEPAD, &battery);

				switch (battery.BatteryLevel)
				{
				case BATTERY_LEVEL_EMPTY:
					return "empty";
				case BATTERY_LEVEL_LOW:
					return "low";
				case BATTERY_LEVEL_MEDIUM:
					return "medium";
				case BATTERY_LEVEL_FULL:
					return "full";
				default:
					return "unknown";
				}
			}
			std::string name() override
			{
				return m_DeviceName;
			}
			std::string vendor() override
			{
				return m_DeviceVendor;
			}

		private:

			std::string m_DeviceName;
			const char* m_DeviceVendor = "Microsoft";

			int m_DeviceID = -1;


			float m_LeftStickX;
			float m_LeftStickY;
			float m_RightStickX;
			float m_RightStickY;
			float m_LeftTrigger;
			float m_RightTrigger;

			int m_DeadZoneX = 0.05f;
			int m_DeadZoneY = 0.02f;

			XINPUT_STATE m_State;

		private:

			bool isConnected()
			{
				int controllerId = -1;

				for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
				{
					XINPUT_STATE state;
					ZeroMemory(&state, sizeof(XINPUT_STATE));

					if (XInputGetState(i, &state) == ERROR_SUCCESS)
						controllerId = i;
				}

				m_DeviceID = controllerId;
				return controllerId != -1;
			}

		};



		/*
		* Human Interface Device Manager
		*/
		class OLCHIDManager : public olc::PGEX, public HIDManager
		{
		public:
			OLCHIDManager(OLCMouse* mouse, OLCKeyboard* keyboard) : m_Keyboard(keyboard), m_Mouse(mouse)
			{
				_startUp();
				_searchForGamepad();
			}

			~OLCHIDManager()
			{
				delete m_Mouse;
				delete m_Keyboard;
			}

			/*
			* General Manager stuff.
			*/
			void update() override
			{
				using namespace std;

				m_Mouse->update();
				m_Keyboard->update();

				// Search for new gamepads.
				_searchForGamepad();

				// Check whether we have Gamepads to be update.
				for (int i = 0; i < m_Gamepad.size(); i++)
				{
					if (m_Gamepad[i])
					{

						cout << i << ".) Gamepad: " << m_Gamepad[i]->name() << endl;


						if (!m_Gamepad[i]->refresh())
						{
#ifdef _DEBUG
							cout << "Could not refresh \"" << m_Gamepad[i]->vendor() << "\"-\"" << m_Gamepad[i]->name() << "\", removing Gamepad" << endl;
#endif

							removeGamepad(m_Gamepad[i]->name().c_str());
						}
					}
				}
			}


			void removeGamepad(const char* name) override
			{
				for (int i = 0; i < m_Gamepad.size(); i++)
				{
					if (m_Gamepad[i])
					{
						if (strcmp(m_Gamepad[i]->name().c_str(), name) == 0)
						{
							delete m_Gamepad[i];
							m_Gamepad[i] = nullptr;
							m_GamepadCount--;
							break;
						}
					}
				}
			}


			void storeGamepad(Gamepad* pad) override
			{
				for (int i = 0; i < m_Gamepad.size(); i++)
				{
					if (m_Gamepad[i] == nullptr)
					{
						m_Gamepad[i] = pad;
						break;
					}
				}
			}

			int gamepad(std::string name) override
			{
				for (auto pad : m_Gamepad)
				{
					if (pad)
					{
						if (strcmp(pad->name().c_str(), name.c_str()) == 0)
						{
							return pad->id();
						}
					}
				}

				return -1;
			}
			void enumerateGamepads(std::vector<std::string>* vec) override
			{
				for (auto pad : m_Gamepad)
				{
					if (pad)
					{
						vec->push_back(pad->name());
					}
				}
			}


			void setGamepadStickThreshhold(float v) override
			{
				m_GamepadThreshhold = v;
			}

			/*
			* Keyboard stuff.
			*/
			bool keyDown(int mask) override
			{
				return m_Keyboard->keyDown(mask);
			}
			bool keyHeld(int mask) override
			{
				return m_Keyboard->keyHeld(mask);
			}
			bool keyReleased(int mask) override
			{
				return m_Keyboard->keyReleased(mask);
			}
			bool anyKeyDown() override
			{
				return m_Keyboard->anyKeyDown();
			}


			/*
			* Mouse stuff.
			*/
			bool lmbDown() override
			{
				return m_Mouse->buttonDown(0);

			}
			bool rmbDown() override
			{
				return m_Mouse->buttonDown(1);

			}
			bool wheelDown() override
			{
				return m_Mouse->buttonDown(2);

			}

			bool lmbReleased() override
			{
				return m_Mouse->buttonReleased(0);

			}
			bool rmbReleased() override
			{
				return m_Mouse->buttonReleased(1);

			}
			bool wheelReleased() override
			{
				return m_Mouse->buttonReleased(2);

			}

			float mouseX() override
			{
				return m_Mouse->mouseX();
			}
			float mouseY() override
			{
				return m_Mouse->mouseY();
			}
			void setMousePosition(float x, float y) override
			{
				m_Mouse->setMousePosition(x, y);
			}


			/*
			* Gamepad stuff.
			*/
			bool buttonDown(const char* id, const char* mask) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							// Give correct integer mask depending on vendor.
							if (strcmp(it->vendor().c_str(), "Microsoft") == 0)
							{
								return it->buttonDown(m_GamepadButtonMapping[mask].first); // XBox
							}
							else
							{
								return it->buttonDown(m_GamepadButtonMapping[mask].second); // PlayStation
							}
						}
					}
				}
			}
			void rumble(const char* id, Rumble r) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->rumble(r);
						}
					}
				}
			}
			void rumbleOff(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							it->rumbleOff();
						}
					}
				}
			}
			float leftTrigger(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->leftTrigger();
						}
					}
				}
			}
			float rightTrigger(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->rightTrigger();
						}
					}
				}
			}
			float leftStickX(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							float v = it->leftStickX();

							// Account for positive and negative values.
							return (std::abs(v) >= m_GamepadThreshhold) ? v : 0.0f;
						}
					}
				}
			}
			float leftStickY(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							float v = it->leftStickY();

							// Account for positive and negative values.
							return (std::abs(v) >= m_GamepadThreshhold) ? v : 0.0f;
						}
					}
				}
			}
			float rightStickX(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							float v = it->rightStickX();

							// Account for positive and negative values.
							return (std::abs(v) >= m_GamepadThreshhold) ? v : 0.0f;
						}
					}
				}
			}
			float rightStickY(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							float v = it->rightStickY();

							// Account for positive and negative values.
							return (std::abs(v) >= m_GamepadThreshhold) ? v : 0.0f;
						}
					}
				}
			}
			float oneFingerTouchPadX(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->oneFingerTouchPadX();
						}
					}
				}
			}
			float oneFingerTouchPadY(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->oneFingerTouchPadY();
						}
					}
				}
			}
			float twoFingerTouchPadX(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->twoFingerTouchPadX();
						}
					}
				}
			}
			float twoFingerTouchPadY(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->twoFingerTouchPadY();
						}
					}
				}
			}

			float gravityX(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->gravityX();
						}
					}
				}
			}
			float gravityY(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->gravityY();
						}
					}
				}
			}
			float gravityZ(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->gravityZ();
						}
					}
				}
			}

			void setColor(const char* id, GamepadColor c) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							it->setColor(c);
						}
					}
				}
			}
			GamepadColor color(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->color();
						}
					}
				}
			}

			int batteryPercent(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->batteryPercent();
						}
					}
				}
			}
			std::string batteryString(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->batteryString();
						}
					}
				}
			}
			std::string name(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->name();
						}
					}
				}
			}
			std::string vendor(const char* id) override
			{
				for (auto it : m_Gamepad)
				{
					if (it)
					{
						if (strcmp(it->name().c_str(), id) == 0)
						{
							return it->vendor();
						}
					}
				}
			}


		private:

			Keyboard* m_Keyboard = nullptr;
			Mouse* m_Mouse = nullptr;


			/*
			* Internal naming convention of the Gamepads is like
			* e.g. "xinput_1" for a XBox controller with id of 1 or
			*	   "dualshock_0" for a PS controller with id of 0.
			*
			* Thus they can have same ids but the names are distinct.
			*/
			std::array<Gamepad*, 4> m_Gamepad;
			int m_GamepadCount = 0;
			float m_GamepadThreshhold = 0.0f;

			/*
			* We map buttons for convenience like
			*
			* GamepadButtonMapping["button_up"] = std::make_pair(XINPUT_GAMEPAD_DPAD_UP, JSMASK_UP);
			*
			* to be uniform with XBox and PlayStation buttons etc. definitions,
			* where the "first" in the pair is XBox and "second" is for PlayStation.
			*
			* The Mappings are defined on HIDManager creation.
			*/
			std::map<const char*, std::pair<int, int>> m_GamepadButtonMapping;
			bool m_Initialized;

		private:

			void _startUp()
			{
				if (!m_Initialized)
				{
					// "Round" Gamepad Buttons.
					m_GamepadButtonMapping["button_up"] = std::make_pair(XINPUT_GAMEPAD_Y, 0);
					m_GamepadButtonMapping["button_down"] = std::make_pair(XINPUT_GAMEPAD_A, 0);
					m_GamepadButtonMapping["button_right"] = std::make_pair(XINPUT_GAMEPAD_B, 0);
					m_GamepadButtonMapping["button_left"] = std::make_pair(XINPUT_GAMEPAD_X, 0);

					// "DPad" Gamepad Buttons.
					m_GamepadButtonMapping["button_dpad_up"] = std::make_pair(XINPUT_GAMEPAD_DPAD_UP, 0);
					m_GamepadButtonMapping["button_dpad_down"] = std::make_pair(XINPUT_GAMEPAD_DPAD_DOWN, 0);
					m_GamepadButtonMapping["button_dpad_right"] = std::make_pair(XINPUT_GAMEPAD_DPAD_RIGHT, 0);
					m_GamepadButtonMapping["button_dpad_left"] = std::make_pair(XINPUT_GAMEPAD_DPAD_LEFT, 0);

					/*
					* JSMASK_HOME and JSMASK_PS are same.
					*
					* Note: The "XBox Silver Guide" button and "PlayStation central PS" button are reserved,
					* so we do not want to register them.
					*
					* If a Gamepad vendor did not provide an equivalent, we set for him -1.
					*/
					// Special Buttons
					m_GamepadButtonMapping["button_back"] = std::make_pair(XINPUT_GAMEPAD_BACK, 0);
					m_GamepadButtonMapping["button_start"] = std::make_pair(XINPUT_GAMEPAD_START, 0);
					m_GamepadButtonMapping["button_touchpad"] = std::make_pair(-1, 0);


					// Analog Sticks as buttons.
					m_GamepadButtonMapping["button_stick_left"] = std::make_pair(XINPUT_GAMEPAD_LEFT_THUMB, 0);
					m_GamepadButtonMapping["button_stick_right"] = std::make_pair(XINPUT_GAMEPAD_RIGHT_THUMB, 0);

					// Shoulder Buttons.
					m_GamepadButtonMapping["button_shoulder_left"] = std::make_pair(XINPUT_GAMEPAD_LEFT_SHOULDER, 0);
					m_GamepadButtonMapping["button_shoulder_right"] = std::make_pair(XINPUT_GAMEPAD_RIGHT_SHOULDER, 0);



					m_Initialized = true;


					/*
					* Define Gamepads as nullptr on startup.
					*/
					ZeroMemory(&m_Gamepad[0], sizeof(std::array<Gamepad*, 4>));


					/*
					* Define the threshhold for registration of input.
					*/
					m_GamepadThreshhold = 0.1f;
				}
			}

			void _searchForGamepad()
			{
				// Skip search if no new devices can be supported.
				if (m_GamepadCount == m_Gamepad.size()) return;


				// XBox.
				bool device_connected = false;
				int controllerId = -1;
				for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
				{
					XINPUT_STATE state;
					ZeroMemory(&state, sizeof(XINPUT_STATE));

					if (XInputGetState(i, &state) == ERROR_SUCCESS)
					{
						controllerId = i;

						// Check whether we have this device already connected.
						// And here dont check for PS Gamepads.
						for (auto pad : m_Gamepad)
						{
							if (pad)
							{
								device_connected = pad->id() == controllerId;

								if (device_connected) break;
							}
						}

						// Store newly connected device.
						if (!device_connected)
						{
							XBoxGamepad* xbox_pad = new XBoxGamepad(controllerId);

							storeGamepad(xbox_pad);
							m_GamepadCount++;
						}
					}
				}
			}

			

		};





	}
}


#endif