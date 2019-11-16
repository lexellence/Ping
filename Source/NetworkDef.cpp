/**************************************************************************************\
** File: NetworkDef.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the NetworkDef struct
**
\**************************************************************************************/
#include "pch.h"
#include "NetworkDef.h"
#include "Exceptions.h"

namespace Pong
{
	void NetworkDef::LoadFrom(const std::string& gameFilePath)
	{
		d2d::HjsonValue data{ d2d::FileToHJSON(gameFilePath) };
		if(!d2d::IsNonNull(data))
			throw LoadSettingsFileException{ gameFilePath + ": Invalid file" };

		try	{
			serverIP = d2d::GetString(data, "serverIP");
			serverPort = d2d::GetInt(data, "serverPort");
			clientUDPPort = d2d::GetInt(data, "clientUDPPort");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Invalid value: " + e.what() };
		}

		try {
			Validate();
		}
		catch(const SettingOutOfRangeException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Setting out of range: " + e.what() };
		}
	}
	void NetworkDef::Validate() const
	{
		if(serverIP.empty()) throw SettingOutOfRangeException{ "serverIP" };
		if(serverPort <= 0) throw SettingOutOfRangeException{ "serverPort" };
		if(clientUDPPort <= 0) throw SettingOutOfRangeException{ "clientPortUDP" };
	}
}