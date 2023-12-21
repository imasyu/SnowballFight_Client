//// �w�b�_�E���C�u����
//#include <iostream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//
//#pragma comment( lib, "ws2_32.lib" )
//
//// �|�[�g�ԍ�
//const unsigned short SERVERPORT = 8888;
//// ����M���郁�b�Z�[�W�̍ő�l
//const unsigned int MESSAGELENGTH = 1024;
//
//int main()
//{
//	std::cout << "Chat Client" << std::endl;
//
//	// WinSock������
//	WSADATA wsaData;
//	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (ret != 0)
//	{
//		std::cout << "Error: WSAStartup ( ErrorCode:" << ret << " )" << std::endl;
//		return 1;	// ���[���I
//	}
//	std::cout << "Success: WSAStartup" << std::endl;
//
//	// �\�P�b�g�̍쐬
//	int sock;
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//
//	if (sock < 0)
//	{
//		// �G���[�R�[�h���o��
//		std::cout << "Error: socket( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//		// �I��
//		return 1;
//	}
//	std::cout << "Success: socket()" << std::endl;
//
//
//	// �ڑ���T�[�o�̃\�P�b�g�A�h���X���ݒ�
//	struct sockaddr_in serverAddr;
//	memset(&serverAddr, 0, sizeof(serverAddr));
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_port = htons(SERVERPORT);
//	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);	// �ق�Ƃ͂悭�Ȃ��B���߂�127.0.0.1��萔��
//
//	// �ڑ��v��
//	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
//	{
//		// �G���[�R�[�h���o��
//		std::cout << "Error: connect( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//		// �I��
//		return 1;
//	}
//	std::cout << "Success: connect()" << std::endl;
//
//
//	// �\�P�b�gsock���m���u���b�L���O�\�P�b�g�ɂ���
//	unsigned long cmdarg = 0x01;
//	ret = ioctlsocket(sock, FIONBIO, &cmdarg);
//
//	if (ret == SOCKET_ERROR)
//	{
//		// �G���[����
//		return 1;
//	}
//	std::cout << "Success: ioctlsocket()" << std::endl;
//
//
//	while (true)
//	{
//		char buff[MESSAGELENGTH];	// ����M���b�Z�[�W�̊i�[�̈�
//
//		// ���M���b�Z�[�W�̓���
//		std::cout << "Input message:";
//		std::cin >> buff;
//
//		// ���M
//		ret = send(sock, buff, strlen(buff), 0);
//
//		if (ret != strlen(buff))
//		{
//			// �G���[�R�[�h���o��
//			std::cout << "Error: recv( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//			// �ʂ���
//			break;
//		}
//
//		std::cout << "---wait message---" << std::endl;
//		// �N���C�A���g����̃��b�Z�[�W��M
//		ret = recv(sock, buff, sizeof(buff) - 1, 0);
//
//		// �m���u���b�L���O�̏ꍇ�A��M�f�[�^���Ȃ��ꍇ���G���[����
//		if (ret < 0)
//		{
//			// �G���[�R�[�h�� WSAEWOULDBLOCK �̏ꍇ�́A��M�f�[�^���Ȃ��������Ă���
//			if (WSAGetLastError() == WSAEWOULDBLOCK)
//			{
//				std::cout << "��M�f�[�^�Ȃ�" << std::endl;
//			}
//			else
//			{
//				// �G���[�R�[�h���o��
//				std::cout << "Error: recv( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//				// �ʂ���
//				break;
//			}
//		}
//		else
//		{
//			// �I�[�L���̒ǉ�
//			buff[ret] = '\0';
//			// �o��
//			std::cout << "Receive message : " << buff << std::endl;
//		}
//	}
//
//	// ����M�Ƃ��ɐؒf
//	// shutdown(sock, 0x02);
//	if (shutdown(sock, SD_BOTH) != 0)
//	{
//		// �G���[�R�[�h���o��
//		std::cout << "Error: shutdown( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//	}
//	std::cout << "Success: shutdown() " << std::endl;
//
//	// �\�P�b�g�̔j��
//	if (closesocket(sock) != 0)
//	{
//		// �G���[�R�[�h���o��
//		std::cout << "Error: closesocket( ErrorCode: " << WSAGetLastError() << ")" << std::endl;
//	}
//	std::cout << "Success: closesocket() " << std::endl;
//
//	// WinSock�I������
//	if (WSACleanup() != 0)
//	{
//		std::cout << "Error: WSACleanup ( ErrorCode:" << ret << " )" << std::endl;
//	}
//	std::cout << "Success: WSACleanup" << std::endl;
//	return 0;
//}
//
