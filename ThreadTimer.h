#pragma once

#include <map>

unsigned WINAPI ThreadTimerProc(void* lpParam);
#define WM_THREADTIMER WM_USER+1234

template <class T>
class ThreadTimer
{
public:
	struct ThreadTimerParam
	{
		UINT uTimerID;
		int nWaitTime;
		HANDLE hWaitEvent;
		LPVOID pInstance;
		HANDLE hThreadID;
		~ThreadTimerParam()
		{
			CloseHandle(hWaitEvent);
			CloseHandle(hThreadID);
		}
	};

	void ClearThreadTimer()
	{
		for (auto it = m_ThreadTimerParams.begin(); it != m_ThreadTimerParams.end(); it++)
		{
			SetEvent(it->second->hWaitEvent);
			it->second->uTimerID = 0;
		}
		m_ThreadTimerParams.clear();
	}

	UINT SetThreadTimer(UINT uID, int waitTime)
	{
		if (GetTimerParam(uID) != NULL)
			return uID;

		ThreadTimerParam * pParam = new ThreadTimerParam;
		pParam->uTimerID = uID;
		pParam->nWaitTime = waitTime;
		pParam->pInstance = (LPVOID)this;
		pParam->hWaitEvent = (HANDLE)CreateEvent(NULL, TRUE, FALSE, NULL);
		pParam->hThreadID = (HANDLE)_beginthreadex(NULL, 0, ThreadTimerProc<T>, (LPVOID)pParam, 0, NULL);

		m_ThreadTimerParams[uID] = pParam;

		return uID;
	}

	ThreadTimerParam * GetTimerParam(UINT uID)
	{
		auto it = m_ThreadTimerParams.find(uID);
		if (it != m_ThreadTimerParams.end())
			return it->second;
		return NULL;
	}

	void KillThreadTimer(UINT uID)
	{
		auto it = m_ThreadTimerParams.find(uID);
		if (it != m_ThreadTimerParams.end())
		{
			SetEvent(it->second->hWaitEvent);
			it->second->uTimerID = 0;
			m_ThreadTimerParams.erase(it);
		}
	}

	void DoThreadTimer(ThreadTimerParam * pParam)
	{
		T* pT = static_cast<T*>(this);
		while (WaitForSingleObject(pParam->hWaitEvent, pParam->nWaitTime) != WAIT_OBJECT_0)
		{
			::PostMessage(pT->m_hWnd, WM_THREADTIMER, pParam->uTimerID, NULL);
		}
		delete pParam;
	}

private:
	std::map<UINT, ThreadTimerParam*> m_ThreadTimerParams;
};

template <class T>
unsigned WINAPI ThreadTimerProc(void* lpParam)
{
	ThreadTimer<T>::ThreadTimerParam * pThreadParam = (ThreadTimer<T>::ThreadTimerParam*)lpParam;
	ThreadTimer<T> *pTimer = (ThreadTimer<T>*)pThreadParam->pInstance;
	if (pTimer)
	{
		pTimer->DoThreadTimer(pThreadParam);
	}
	return 0;
}