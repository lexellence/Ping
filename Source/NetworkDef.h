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

		std::string serverIP;
		int serverPort;
		//int clientUDPPort;
	};
}
