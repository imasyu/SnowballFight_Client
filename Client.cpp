//// ヘッダ・ライブラリ
//#include <iostream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//
//#pragma comment( lib, "ws2_32.lib" )
//
//// ポート番号
//const unsigned short SERVERPORT = 8888;
//// 送受信するメッセージの最大値
//const unsigned int MESSAGELENGTH = 1024;
//
//int main()
//{
//	std::cout << "Chat Client" << std::endl;
//
//	// WinSock初期化
//	WSADATA wsaData;
//	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (ret != 0)
//	{
//		std::cout << "Error: WSAStartup ( ErrorCode:" << ret << " )" << std::endl;
//		return 1;	// おーわり！
//	}
//	std::cout << "Success: WSAStartup" << std::endl;
//
//	// ソケットの作成
//	int sock;
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//
//	if (sock < 0)
//	{
//		// エラーコードを出力
//		std::cout << "Error: socket( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//		// 終了
//		return 1;
//	}
//	std::cout << "Success: socket()" << std::endl;
//
//
//	// 接続先サーバのソケットアドレス情報設定
//	struct sockaddr_in serverAddr;
//	memset(&serverAddr, 0, sizeof(serverAddr));
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_port = htons(SERVERPORT);
//	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);	// ほんとはよくない。せめて127.0.0.1を定数化
//
//	// 接続要求
//	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
//	{
//		// エラーコードを出力
//		std::cout << "Error: connect( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//		// 終了
//		return 1;
//	}
//	std::cout << "Success: connect()" << std::endl;
//
//
//	// ソケットsockをノンブロッキングソケットにする
//	unsigned long cmdarg = 0x01;
//	ret = ioctlsocket(sock, FIONBIO, &cmdarg);
//
//	if (ret == SOCKET_ERROR)
//	{
//		// エラー処理
//		return 1;
//	}
//	std::cout << "Success: ioctlsocket()" << std::endl;
//
//
//	while (true)
//	{
//		char buff[MESSAGELENGTH];	// 送受信メッセージの格納領域
//
//		// 送信メッセージの入力
//		std::cout << "Input message:";
//		std::cin >> buff;
//
//		// 送信
//		ret = send(sock, buff, strlen(buff), 0);
//
//		if (ret != strlen(buff))
//		{
//			// エラーコードを出力
//			std::cout << "Error: recv( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//			// ぬける
//			break;
//		}
//
//		std::cout << "---wait message---" << std::endl;
//		// クライアントからのメッセージ受信
//		ret = recv(sock, buff, sizeof(buff) - 1, 0);
//
//		// ノンブロッキングの場合、受信データがない場合もエラー扱い
//		if (ret < 0)
//		{
//			// エラーコードが WSAEWOULDBLOCK の場合は、受信データがなかったってこと
//			if (WSAGetLastError() == WSAEWOULDBLOCK)
//			{
//				std::cout << "受信データなし" << std::endl;
//			}
//			else
//			{
//				// エラーコードを出力
//				std::cout << "Error: recv( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//				// ぬける
//				break;
//			}
//		}
//		else
//		{
//			// 終端記号の追加
//			buff[ret] = '\0';
//			// 出力
//			std::cout << "Receive message : " << buff << std::endl;
//		}
//	}
//
//	// 送受信ともに切断
//	// shutdown(sock, 0x02);
//	if (shutdown(sock, SD_BOTH) != 0)
//	{
//		// エラーコードを出力
//		std::cout << "Error: shutdown( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//	}
//	std::cout << "Success: shutdown() " << std::endl;
//
//	// ソケットの破棄
//	if (closesocket(sock) != 0)
//	{
//		// エラーコードを出力
//		std::cout << "Error: closesocket( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//	}
//	std::cout << "Success: closesocket() " << std::endl;
//
//	// WinSock終了処理
//	if (WSACleanup() != 0)
//	{
//		std::cout << "Error: WSACleanup ( ErrorCode:" << ret << " )" << std::endl;
//	}
//	std::cout << "Success: WSACleanup" << std::endl;
//	return 0;
//}
//
