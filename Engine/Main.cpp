
//
//�@�ŏI�X�V���F2023/10/20
//


#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "global.h"
#include "RootObject.h"
#include "Model.h"
#include "Image.h"
#include "Camera.h"
#include "Input.h"
#include "Audio.h"
#include "VFX.h"

#pragma comment( lib, "ws2_32.lib" )

// �|�[�g�ԍ�
const unsigned short SERVERPORT = 8888;
// ����M���郁�b�Z�[�W�̍ő�l
const unsigned int MESSAGELENGTH = 1024;

#pragma comment(lib,"Winmm.lib")

#pragma comment( lib, "ws2_32.lib" )

//�萔�錾
const char* WIN_CLASS_NAME = "SampleGame";	//�E�B���h�E�N���X��


//�v���g�^�C�v�錾
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif

	srand((unsigned)time(NULL));
	SetCurrentDirectory("Assets");

	//�������t�@�C���isetup.ini�j����K�v�ȏ����擾
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");		//�X�N���[���̕�
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");	//�X�N���[���̍���
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");				//FPS�i��ʍX�V���x�j
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\setup.ini");		//�L���v�V�����Ɍ��݂�FPS��\�����邩�ǂ���




	//�E�B���h�E���쐬
	HWND hWnd = InitApp(hInstance, screenWidth, screenHeight, nCmdShow);

	//Direct3D����
	Direct3D::Initialize(hWnd, screenWidth, screenHeight);

	//�J����������
	Camera::Initialize();

	//���͏����i�L�[�{�[�h�A�}�E�X�A�R���g���[���[�j�̏���
	Input::Initialize(hWnd);

	//�I�[�f�B�I�i���ʉ��j�̏���
	Audio::Initialize();


	//���[�g�I�u�W�F�N�g����
	//���ׂẴQ�[���I�u�W�F�N�g�̐e�ƂȂ�I�u�W�F�N�g
	RootObject* pRootObject = new RootObject;
	pRootObject->Initialize();

	// WinSock������
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return 1;	// ���[���I
	}

	// �\�P�b�g�̍쐬
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
	{
		// �G���[�R�[�h���o��
		// �I��
		return 1;
	}


	// �ڑ���T�[�o�̃\�P�b�g�A�h���X���ݒ�
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);	// �ق�Ƃ͂悭�Ȃ��B���߂�127.0.0.1��萔��

	// �ڑ��v��
	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		// �G���[�R�[�h���o��
		// �I��
		return 1;
	}


	// �\�P�b�gsock���m���u���b�L���O�\�P�b�g�ɂ���
	unsigned long cmdarg = 0x01;
	ret = ioctlsocket(sock, FIONBIO, &cmdarg);

	if (ret == SOCKET_ERROR)
	{
		// �G���[����
		return 1;
	}
	//���b�Z�[�W���[�v�i�����N����̂�҂j
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//���b�Z�[�W����i���������D��j
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//���b�Z�[�W�Ȃ��i�����ŃQ�[���̏����j
		else
		{
			//���Ԍv��
			timeBeginPeriod(1);	//���Ԍv���̐��x���グ��
			static int FPS = 0;								//��ʍX�V�񐔂̃J�E���^
			static DWORD lastFpsResetTime = timeGetTime();	//�Ō�ɃL���v�V������FPS��\������
			static DWORD lastUpdateTime = timeGetTime();	//�Ō�ɉ�ʂ��X�V��������
			DWORD nowTime = timeGetTime();					//���݂̎���

			//�L���v�V�����Ɍ��݂�FPS��\������
			if (isDrawFps)
			{
				//1�b�i1000�~���b�j�o������
				if (nowTime - lastFpsResetTime > 1000)
				{
					//FPS�̒l��\��
					char string[16];
					wsprintf(string, "FPS:%d", FPS);
					SetWindowText(GetActiveWindow(), string);
					FPS = 0;
					lastFpsResetTime = nowTime;
				}
			}


			//�w�肵�����ԁiFPS��60�ɐݒ肵���ꍇ��60����1�b�j�o�߂��Ă�����X�V����
			if ((nowTime - lastUpdateTime) * fpsLimit > 1000.0f)
			{
				//���Ԍv���֘A
				lastUpdateTime = nowTime;	//���݂̎��ԁi�Ō�ɉ�ʂ��X�V�������ԁj���o���Ă���
				FPS++;						//��ʍX�V�񐔂��J�E���g����

				char buff[MESSAGELENGTH];	// ����M���b�Z�[�W�̊i�[�̈�
				buff[0] = 'b';

				// ���M
				ret = send(sock, buff, strlen(buff), 0);

				if (ret != strlen(buff))
				{
					// �G���[�R�[�h���o��
					// �ʂ���
					break;
				}

				// �N���C�A���g����̃��b�Z�[�W��M
				ret = recv(sock, buff, sizeof(buff) - 1, 0);

				// �m���u���b�L���O�̏ꍇ�A��M�f�[�^���Ȃ��ꍇ���G���[����
				if (ret < 0)
				{
					// �G���[�R�[�h�� WSAEWOULDBLOCK �̏ꍇ�́A��M�f�[�^���Ȃ��������Ă���
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
					}
					else
					{
						// �G���[�R�[�h���o��
						// �ʂ���
						break;
					}
				}
				else
				{
					// �I�[�L���̒ǉ�
					buff[ret] = '\0';
					buff[1] = '\n';

					// �o��				
					OutputDebugString(buff);

				}


				//���́i�L�[�{�[�h�A�}�E�X�A�R���g���[���[�j�����X�V
				Input::Update();

				//�S�I�u�W�F�N�g�̍X�V����
				//���[�g�I�u�W�F�N�g��Update���Ă񂾂��ƁA�����I�Ɏq�A����Update���Ă΂��
				pRootObject->UpdateSub();

				//�J�������X�V
				Camera::Update();

				//�G�t�F�N�g�̍X�V
				VFX::Update();


				//���̃t���[���̕`��J�n
				Direct3D::BeginDraw();

				//�S�I�u�W�F�N�g��`��
				//���[�g�I�u�W�F�N�g��Draw���Ă񂾂��ƁA�����I�Ɏq�A����Update���Ă΂��
				pRootObject->DrawSub();

				//�G�t�F�N�g�̕`��
				VFX::Draw();

				//�`��I��
				Direct3D::EndDraw();



				
				//������Ƌx�܂���
				Sleep(1);
			}
			timeEndPeriod(1);	//���Ԍv���̐��x��߂�
		}
	}

	

	//���낢����
	VFX::Release();
	Audio::AllRelease();
	Model::AllRelease();
	Image::AllRelease();
	pRootObject->ReleaseSub();
	SAFE_DELETE(pRootObject);
	Direct3D::Release();

	return 0;
}


//�E�B���h�E�̍쐬
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow)
{
	//�E�B���h�E�N���X�i�݌v�}�j���쐬
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//���̍\���̂̃T�C�Y
	wc.hInstance = hInstance;						//�C���X�^���X�n���h��
	wc.lpszClassName = WIN_CLASS_NAME;				//�E�B���h�E�N���X��
	wc.lpfnWndProc = WndProc;						//�E�B���h�E�v���V�[�W��
	wc.style = CS_VREDRAW | CS_HREDRAW;				//�X�^�C���i�f�t�H���g�j
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);	//�A�C�R��
	wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);	//�������A�C�R��
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	//�}�E�X�J�[�\��
	wc.lpszMenuName = nullptr;						//���j���[�i�Ȃ��j
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//�w�i�i���j
	RegisterClassEx(&wc);

	//�E�B���h�E�T�C�Y�̌v�Z
	RECT winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

	//�^�C�g���o�[�ɕ\��������e
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\setup.ini");

	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,					//�E�B���h�E�N���X��
		caption,						//�^�C�g���o�[�ɕ\��������e
		WS_OVERLAPPEDWINDOW,			//�X�^�C���i���ʂ̃E�B���h�E�j
		CW_USEDEFAULT,					//�\���ʒu���i���܂����j
		CW_USEDEFAULT,					//�\���ʒu��i���܂����j
		winRect.right - winRect.left,	//�E�B���h�E��
		winRect.bottom - winRect.top,	//�E�B���h�E����
		nullptr,						//�e�E�C���h�E�i�Ȃ��j
		nullptr,						//���j���[�i�Ȃ��j
		hInstance,						//�C���X�^���X
		nullptr							//�p�����[�^�i�Ȃ��j
	);

	//�E�B���h�E��\��
	ShowWindow(hWnd, nCmdShow);

	return hWnd;
}


//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	//�E�B���h�E�����
	case WM_DESTROY:
		PostQuitMessage(0);	//�v���O�����I��
		return 0;

	//�}�E�X��������
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}