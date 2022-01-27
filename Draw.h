#pragma once
class CDraw
{
public: 
	void Init();
	int getWidht(const char* input, unsigned int font);
	void DrawRect(int x1, int y1, int x2, int y2, Color clr);
	void Text2(int x, int y, Color color, DWORD font, const wchar_t* text);
	void textold(int x, int y, const wchar_t * _Input, int font, Color color);
	std::wstring stringToWide(const std::string& text);
	void String(int x, int y, int r, int g, int b, const wchar_t *pszText);
	void StringA(unsigned long Font, bool center, int x, int y, int r, int g, int b, int a, const char *input, ...);
	void FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a);
	void FillRGBAOutlined(int x, int y, int w, int h, int r, int g, int b, int a);
	void Box(int x, int y, int w, int h, int r, int g, int b, int a);
	void LineRGBA(int x1, int y1, int x2, int y2, int r, int g, int b, int a);
	void Text(int x, int y, int r, int g, int b, int a, bool bCenter, unsigned int font, const char *fmt, ...);
	void ColoredCircle(int x, int y, int radius, int r, int g, int b, int a);
	void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow);
	void DrawTextureByteArray(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, const unsigned char* pRawRGBAData);
	unsigned int font_esp, font_GUI2, font_espinfo, font_espnum, font_build, event_font, aa_font, icon_esp, font_esp2;
	
};
extern HINSTANCE hAppInstance;
extern CDraw g_Draw;