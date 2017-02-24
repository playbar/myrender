#pragma once
class MojingLoadSo
{
public:
	MojingLoadSo(char* sPath);
	~MojingLoadSo();

	bool IsLoaded();
	void * FindFunction(char * sFuncName);

private:
	void *m_Handle;
	char m_Path[256];
};

