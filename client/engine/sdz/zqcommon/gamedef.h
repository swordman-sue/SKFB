
#ifndef GAMEDEF_H
#define GAMEDEF_H

namespace GameType
{
	// Ĭ����Ļ�ߴ�(win32)
	// ����:1136x640��������533x946
	const float SCREEN_WIDTH_DEF = 1136;
	const float SCREEN_HEIGHT_DEF = 640;

	// Ĭ����Ʒֱ��ʳߴ�
	// ������1136x640��������640x1136
	const float SCREEN_DESIGN_WIDTH_DEF = 960;
	const float SCREEN_DESIGN_HEIGHT_DEF = 640;

	// ��Ⱦ���
	enum RenderQueue
	{
		GRQ_MAX = 8888,				//�������
	};
}

#endif
