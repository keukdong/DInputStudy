#pragma once

#include "Engine.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagKeyScale
{
	float		fScale;
	unsigned char cKey;
}KeyScale, *PKeyScale;

typedef struct ENGINE_DLL _tagBindAxis
{
	string	strName;
	bool	bBindFunction;
	function<void(float, float)>	func;
	list<PKeyScale>	KeyList;

	_tagBindAxis()	:
		bBindFunction(false)
	{
	}

	~_tagBindAxis()
	{
		Safe_Delete_VecList(KeyList);
	}
}BindAxis, *PBindAxis;

typedef struct ENGINE_DLL _tagKeyInfo
{
	bool	bPress;
	bool	bPush;
	bool	bRelease;

	_tagKeyInfo() :
		bPress(false),
		bPush(false),
		bRelease(false)
	{
	}
}KeyInfo, *PKeyInfo;

class ENGINE_DLL CInput
{
private:
	IDirectInput8*			m_pInput;
	IDirectInputDevice8*	m_pKeyboard;

private:
	unordered_map<string, PBindAxis>	m_mapAxis;
	list<unsigned char>	m_KeyList;
	unsigned char	m_cKey[256];
	KeyInfo		m_tKeyInfo[256];

public:
	bool Init();
	void Update(float fTime);

private:
	bool ReadKeyboard();
	PBindAxis FindAxis(const string& strName);

public:
	void AddAxisKey(const string& strName, float fScale,
		unsigned char cKey);

	void BindAxis(const string& strName, void(*pFunc)(float, float))
	{
		PBindAxis	pBind = FindAxis(strName);

		if (!pBind)
		{
			pBind = new _tagBindAxis;
			pBind->strName = strName;
			m_mapAxis.insert(make_pair(strName, pBind));
		}

		pBind->bBindFunction = true;
		pBind->func = bind(pFunc, placeholders::_1, placeholders::_2);
	}

	template <typename T>
	void BindAxis(const string& strName, T* pObj, void(T::*pFunc)(float, float))
	{
		PBindAxis	pBind = FindAxis(strName);

		if (!pBind)
		{
			pBind = new _tagBindAxis;
			pBind->strName = strName;
			m_mapAxis.insert(make_pair(strName, pBind));
		}

		pBind->bBindFunction = true;
		pBind->func = bind(pFunc, pObj, placeholders::_1, placeholders::_2);
	}

	DECLARE_SINGLE(CInput)
};

ENGINE_END
