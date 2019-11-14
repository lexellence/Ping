/**************************************************************************************\
** File: NetworkDef.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the NetworkDef struct
**
\**************************************************************************************/
#pragma once
namespace Pong
{
	struct NetworkDef
	{
		void LoadFrom(const std::string& filePath);
		void Validate() const;

		struct
		{
			std::string remoteIP;
			int remotePort;
			int localPort;
		} client;
		struct
		{
			int localPort;
		} server;
	};
}
