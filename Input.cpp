#include "Input.h"
#include "Core.h"

ENGINE_USING

DEFINITION_SINGLE(CInput)

CInput::CInput()	:
	m_pInput(nullptr),
	m_pKeyboard(nullptr)
{
}

CInput::~CInput()
{
	Safe_Delete_Map(m_mapAxis);

	SAFE_RELEASE(m_pKeyboard);
	SAFE_RELEASE(m_pInput);
}

bool CInput::Init()
{
	// directinput_version -> directinput_header_version
	if (FAILED(DirectInput8Create(WINDOWINSTANCE, DIRECTINPUT_HEADER_VERSION,
		IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return false;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return false;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(m_pKeyboard->Acquire()))
		return false;

	return true;
}

void CInput::Update(float fTime)
{
	ReadKeyboard();

	list<unsigned char>::iterator	iterKey;
	list<unsigned char>::iterator	iterKeyEnd = m_KeyList.end();

	bool	bFind = false;
	for (iterKey = m_KeyList.begin(); iterKey != iterKeyEnd; ++iterKey)
	{
		if (m_cKey[*iterKey] & 0x80)
		{
			if (!m_tKeyInfo[*iterKey].bPress && !m_tKeyInfo[*iterKey].bPush)
			{
				m_tKeyInfo[*iterKey].bPress = true;
				m_tKeyInfo[*iterKey].bPush = true;
			}

			else
			{
				m_tKeyInfo[*iterKey].bPress = false;
			}
		}

		else if (m_tKeyInfo[*iterKey].bPress || m_tKeyInfo[*iterKey].bPush)
		{
			m_tKeyInfo[*iterKey].bPress = false;
			m_tKeyInfo[*iterKey].bPush = false;
			m_tKeyInfo[*iterKey].bRelease = true;
		}

		else if (m_tKeyInfo[*iterKey].bRelease)
			m_tKeyInfo[*iterKey].bRelease = false;
	}

	unordered_map<string, PBindAxis>::iterator	iter;
	unordered_map<string, PBindAxis>::iterator	iterEnd = m_mapAxis.end();

	for (iter = m_mapAxis.begin(); iter != iterEnd; ++iter)
	{
		PBindAxis	pBind = iter->second;

		list<PKeyScale>::iterator	iter1;
		list<PKeyScale>::iterator	iter1End = pBind->KeyList.end();

		for (iter1 = pBind->KeyList.begin(); iter1 != iter1End; ++iter1)
		{
			float	fScale = 0.f;

			if (m_tKeyInfo[(*iter1)->cKey].bPush)
				fScale = (*iter1)->fScale;

			if (pBind->bBindFunction)
				pBind->func(fScale, fTime);
		}
	}
}

bool CInput::ReadKeyboard()
{
	HRESULT	result = m_pKeyboard->GetDeviceState(256, m_cKey);

	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_pKeyboard->Acquire();

		else
			return false;
	}

	return true;
}

PBindAxis CInput::FindAxis(const string & strName)
{
	unordered_map<string, PBindAxis>::iterator	iter = m_mapAxis.find(strName);

	if (iter == m_mapAxis.end())
		return nullptr;

	return iter->second;
}

void CInput::AddAxisKey(const string & strName, float fScale, 
	unsigned char cKey)
{
	PBindAxis	pBind = FindAxis(strName);

	if (!pBind)
	{
		pBind = new _tagBindAxis;
		pBind->strName = strName;
		m_mapAxis.insert(make_pair(strName, pBind));
	}

	PKeyScale	pKeyScale = new KeyScale;

	pKeyScale->fScale = fScale;
	pKeyScale->cKey = cKey;

	list<unsigned char>::iterator	iter;
	list<unsigned char>::iterator	iterEnd = m_KeyList.end();

	bool	bFind = false;
	for (iter = m_KeyList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == cKey)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
		m_KeyList.push_back(cKey);

	pBind->KeyList.push_back(pKeyScale);
}
