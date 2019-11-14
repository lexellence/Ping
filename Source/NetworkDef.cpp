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

		d2d::HjsonValue clientData;
		d2d::HjsonValue serverData;
		try {
			clientData = d2d::GetMemberValue(data, "client");
			serverData = d2d::GetMemberValue(data, "server");
		}
		catch(const d2d::HjsonFailedQueryException& e) {
			throw LoadSettingsFileException{ gameFilePath + ": Invalid value: " + e.what() };
		}

		try	{
			client.remoteIP = d2d::GetString(clientData, "remoteIP");
			client.remotePort = d2d::GetInt(clientData, "remotePort");
			client.localPort = d2d::GetInt(clientData, "localPort");
			server.localPort = d2d::GetInt(serverData, "localPort");
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
		if(client.remoteIP.empty()) throw SettingOutOfRangeException{ "client.remoteIP" };
		if(client.remotePort <= 0) throw SettingOutOfRangeException{ "client.remotePort" };
		if(client.localPort <= 0) throw SettingOutOfRangeException{ "client.localPort" };
		if(server.localPort <= 0) throw SettingOutOfRangeException{ "server.localPort" };
	}
}