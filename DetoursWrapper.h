#pragma once

#include <vector>
#include <detours.h>
#include <assert.h>
#include "Singleton.h"
#include "StdLog.h"

#pragma comment(lib, "detours.lib")

class Detours : public Singleton<Detours>
{
private:
	struct FunTable
	{
		PVOID* mOldFun;
		PVOID  mNewFun;

		BOOL operator == (const FunTable & other)
		{
			return *mOldFun == *other.mOldFun && mNewFun == other.mNewFun;
		}
	};

	std::vector<FunTable> mFunTable;

public:
	template <typename T1, typename T2>
	bool Attach(T1 &  oldfun, T2 & newfun)
	{
		FunTable tab;
		tab.mOldFun = &(PVOID&)oldfun;
		tab.mNewFun = newfun;
		auto it = find(mFunTable.begin(), mFunTable.end(), tab);
		if (it != mFunTable.end())
			return FALSE;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(tab.mOldFun, tab.mNewFun);

		mFunTable.push_back(tab);

		LONG ret = Commit();
		assert(ret == NO_ERROR);
		return ret == NO_ERROR;
	}

	template <typename T1, typename T2>
	bool Detach(T1 & oldfun, T2 & newfun)
	{
		FunTable tab;
		tab.mOldFun = &(PVOID&)oldfun;
		tab.mNewFun = newfun;
		auto it = find(mFunTable.begin(), mFunTable.end(), tab);
		if (it == mFunTable.end())
			return FALSE;
		
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(tab.mOldFun, tab.mNewFun);
			
		bool ret = Commit() == NO_ERROR;
		mFunTable.erase(it);
		return ret;
	}

	PVOID WINAPI Find(PCSTR pszModule, PCSTR pszFunction)
	{
		return DetourFindFunction(pszModule, pszFunction);
	}

	long Commit()
	{
		return DetourTransactionCommit();
	}

	virtual bool Init()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		return true;
	}

	virtual void Uninit()
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		for (unsigned int a = 0; a < mFunTable.size(); a++)
		{
			FunTable & tab = mFunTable[a];
			DetourDetach(tab.mOldFun, tab.mNewFun);
		}
	}
};

#define DETOURS_FUNC_IMPLEMENT(RetType, CallType, FunctionName, ...) \
	RetType CallType Hook_##FunctionName(## __VA_ARGS__)

#define DETOURS_FUNC_DECLARE_VOID(RetType, CallType, FunctionName) \
	typedef RetType(CallType *FuncDefine_##FunctionName)(); \
	FuncDefine_##FunctionName Real_##FunctionName = (FuncDefine_##FunctionName)FunctionName; \
	RetType CallType Hook_##FunctionName()

#define DETOURS_FUNC_CALLREAL_VOID(FunctionName) \
	Real_##FunctionName()

#define DETOURS_FUNC_DECLARE(RetType, CallType, FunctionName, ...) \
	typedef RetType(CallType *FuncDefine_##FunctionName)(## __VA_ARGS__); \
	FuncDefine_##FunctionName Real_##FunctionName = (FuncDefine_##FunctionName)FunctionName; \
	RetType CallType Hook_##FunctionName(## __VA_ARGS__)

#define DETOURS_FUNC_CALLREAL(FunctionName, ...) \
	Real_##FunctionName(## __VA_ARGS__)

#define DETOURS_FUNC_ATTACH(FunctionName) \
	Detours::Instance()->Attach(Real_##FunctionName, Hook_##FunctionName);

#define DETOURS_FUNC_DETACH(FunctionName) \
	Detours::Instance()->Detach(Real_##FunctionName, Hook_##FunctionName);
