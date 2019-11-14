/**************************************************************************************\
** File: AppDef.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the AppDef struct
**
\**************************************************************************************/
#include "pch.h"
#include "AppDef.h"
#include "Exceptions.h"
namespace Pong
{
	void AppDef::LoadFrom(const std::string& appFilePath)
	{
		d2d::HjsonValue data{ d2d::FileToHJSON(appFilePath) };
		if(!d2d::IsNonNull(data))
			throw LoadSettingsFileException{ appFilePath + ": Invalid file" };

		// Get root level values
		d2d::HjsonValue gamepadsData;
		d2d::HjsonValue windowData;
		try {
			gamepadsData = d2d::GetMemberValue(data, "gamepads");
			windowData = d2d::GetMemberValue(data, "window");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ appFilePath + ": Invalid value: " + e.what() };
		}

		// Get gamepad settings
		try {
			gamepads.controllerDBFilePath = d2d::GetString(gamepadsData, "controllerDBFilePath");
			gamepads.deadZoneStick = d2d::GetFloat(gamepadsData, "deadZoneStick");
			gamepads.deadZoneTrigger = d2d::GetFloat(gamepadsData, "deadZoneTrigger");
			gamepads.aliveZoneStick = d2d::GetFloat(gamepadsData, "aliveZoneStick");
			gamepads.aliveZoneTrigger = d2d::GetFloat(gamepadsData, "aliveZoneTrigger");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ appFilePath + ": Invalid value: gamepads." + e.what() };
		}

		// Get window settings
		try {
			window.title = d2d::GetString(windowData, "title");
			window.fullScreen = d2d::GetBool(windowData, "fullScreen");

			window.position.at(0) = d2d::GetVectorInt(windowData, "position", 0);
			window.position.at(1) = d2d::GetVectorInt(windowData, "position", 1);

			window.size.at(0) = d2d::GetVectorInt(windowData, "size", 0);
			window.size.at(1) = d2d::GetVectorInt(windowData, "size", 1);

			window.vsync = d2d::GetBool(windowData, "vsync");
			window.doubleBuffer = d2d::GetBool(windowData, "doubleBuffer");
			window.antiAliasingSamples = d2d::GetInt(windowData, "antiAliasingSamples");
			window.fpsUpdateDelay = 1.0f / d2d::GetFloat(windowData, "fpsUpdatesPerSecond");

			window.colorChannelBits.at(0) = d2d::GetVectorInt(windowData, "colorChannelBits", 0);
			window.colorChannelBits.at(1) = d2d::GetVectorInt(windowData, "colorChannelBits", 1);
			window.colorChannelBits.at(2) = d2d::GetVectorInt(windowData, "colorChannelBits", 2);
			window.colorChannelBits.at(3) = d2d::GetVectorInt(windowData, "colorChannelBits", 3);

			//window.clearColor = d2d::GetColorFloat(windowData, "clearColor");

			window.imageExtensions = 0;
			for(unsigned i = 0; i < windowData["imageExtensions"].size(); ++i)
			{
				std::string flagString{ d2d::GetVectorString(windowData, "imageExtensions", i) };
				int flag{ d2d::StringToImageInitFlag(flagString) };
				if(!flag)
					throw LoadSettingsFileException{ appFilePath + "Invalid value: window.imageExtensions[" + d2d::ToString(i) + "]: " + flagString };
				window.imageExtensions |= flag;
			}

			window.gl.versionMajor = d2d::GetVectorInt(windowData, "glVersion", 0);
			window.gl.versionMinor = d2d::GetVectorInt(windowData, "glVersion", 1);
			window.gl.pointSmoothing = d2d::GetBool(windowData, "pointSmoothing");
			window.gl.lineSmoothing = d2d::GetBool(windowData, "lineSmoothing");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ appFilePath + ": Invalid value: window." + e.what() };
		}

		try {
			Validate();
		}
		catch(const SettingOutOfRangeException& e) {
			throw LoadSettingsFileException{ appFilePath + ": App setting out of range: " + e.what() };
		}
	}
	void AppDef::Validate() const
	{
		// gamepads
		if(gamepads.controllerDBFilePath.empty()) throw SettingOutOfRangeException{ "gamepads.controllerDBFilePath" };
		if(gamepads.deadZoneStick < 0.0f) throw SettingOutOfRangeException{ "gamepads.deadZoneStick" };
		if(gamepads.deadZoneTrigger < 0.0f) throw SettingOutOfRangeException{ "gamepads.deadZoneTrigger" };
		if(gamepads.aliveZoneStick < 0.0f) throw SettingOutOfRangeException{ "gamepads.aliveZoneStick" };
		if(gamepads.aliveZoneTrigger < 0.0f) throw SettingOutOfRangeException{ "gamepads.aliveZoneTrigger" };

		// window
		if(window.title.empty()) throw SettingOutOfRangeException{ "window.title" };
		if(window.size[0] < 0) throw SettingOutOfRangeException{ "window.size[0]" };
		if(window.size[1] < 0) throw SettingOutOfRangeException{ "window.size[1]" };
		if(!d2d::Window::VALID_ANTI_ALIASING_SAMPLES.Contains(window.antiAliasingSamples))
			throw SettingOutOfRangeException{ "window.antiAliasingSamples" };

		for(unsigned i = 0; i < window.colorChannelBits.size(); ++i)
			if(window.colorChannelBits[i] <= 0)
				throw SettingOutOfRangeException{ "window.colorChannelBits[" + d2d::ToString(i) + "]" };

		if(window.fpsUpdateDelay < 0) throw SettingOutOfRangeException{ "window.fpsUpdateDelay" };
		if(window.imageExtensions == 0) throw SettingOutOfRangeException{ "window.imageExtensions" };

		// gl
		if(window.gl.versionMajor < 0) throw SettingOutOfRangeException{ "window.glVersion[0]" };
		if(window.gl.versionMinor < 0) throw SettingOutOfRangeException{ "window.glVersion[1]" };
	}
}