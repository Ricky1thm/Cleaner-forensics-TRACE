#pragma once
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream> 
#include <cstdlib> 
#include <ctime> 
#include <vector> 
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <random>
#include <sddl.h>
#include <aclapi.h>
#include <tchar.h>
#include <filesystem>
#include <thread>


namespace fs = filesystem;

using namespace std::chrono;

void Prefetch() {
	system("cd C:\\Windows\\Prefetch & del /s /q *.pf >nul 2>&1");
}

void Journal() {
	system("fsutil usn deleteJournal /D C: >nul 2>&1");
	system("fsutil usn createjournal m=8192 a=8192 C: >nul 2>&1");
}

void Shadows() {
	system("vssadmin Delete Shadows /For=C: /All /Quiet >nul 2>&1");
}

struct TimeInfo {
	int currentY;
	int currentM;
	int currentG;
	int currentH;
	int currentMin;
	int currentS;
};

TimeInfo t;

void changeSystemTime() {
	SYSTEMTIME TT;
	GetLocalTime(&TT);
	stringstream command;

	srand(time(NULL));

	t.currentY = TT.wYear;
	t.currentM = TT.wMonth;
	t.currentG = TT.wDay;
	t.currentH = TT.wHour;
	t.currentMin = TT.wMinute;
	t.currentS = TT.wSecond;

	int randYear = 2015 + rand() % (TT.wYear - 2014);
	int randMonth = 1 + rand() % 12;
	int randDay = 1 + rand() % 28;
	int randHour = rand() % 24;
	int randMin = rand() % 60;
	int randSec = rand() % 60;

	command << "date "
		<< setw(2) << setfill('0') << randDay << "/"
		<< setw(2) << setfill('0') << randMonth << "/"
		<< randYear;

	system(command.str().c_str());
	Sleep(1000);

	command.str("");
	command.clear();

	command << "time "
		<< setw(2) << setfill('0') << randHour << ":"
		<< setw(2) << setfill('0') << randMin << ":"
		<< setw(2) << setfill('0') << randSec;

	system(command.str().c_str());

}

int rsttime() {
	
	stringstream sui;

	sui << "date "
		<< setw(2) << setfill('0') << t.currentG << "/"
		<< setw(2) << setfill('0') << t.currentM << "/"
		<< t.currentY;

	system(sui.str().c_str());

	sui.str("");
	sui.clear();

	sui << "time "
		<< setw(2) << setfill('0') << t.currentH << ":"
		<< setw(2) << setfill('0') << t.currentMin << ":"
		<< setw(2) << setfill('0') << t.currentS;

	system(sui.str().c_str());
	return 1;
}

void cleanevtx() {

	system("wevtutil sl System /ms:152.672 >nul 2>&1");
	system("wevtutil sl Security /ms:152.672 >nul 2>&1");
	system("wevtutil sl Application /ms:152.672 >nul 2>&1");
	system("wevtutil cl System >nul 2>&1");
	system("wevtutil cl Application >nul 2>&1");
	system("wevtutil cl Security >nul 2>&1");
}

void Events() {
	const char* command = "powershell -Command $eventi = @(@{LogName='System'; Source='Microsoft-Windows-Kernel-General'; EventID=12; EntryType='Information'; Message='Il sistema è stato avviato.'}, @{LogName='System'; Source='Service Control Manager'; EventID=7036; EntryType='Information'; Message='Il servizio Windows Update è stato riavviato.'}, @{LogName='System'; Source='disk'; EventID=7; EntryType='Error'; Message='Errore di lettura su disco rigido 0.'}, @{LogName='System'; Source='Microsoft-Windows-Kernel-General'; EventID=13; EntryType='Information'; Message='Il sistema è stato arrestato correttamente.'}, @{LogName='System'; Source='Microsoft-Windows-WHEA-Logger'; EventID=1; EntryType='Information'; Message='Errore hardware irreversibile rilevato dalla CPU.'}, @{LogName='System'; Source='NetBT'; EventID=311; EntryType='Information'; Message='Il protocollo NetBIOS su TCP/IP ha rilevato un errore nella configurazione della rete.'}, @{LogName='System'; Source='EventLog'; EventID=1001; EntryType='Information'; Message='Il servizio Evading.exe è stato avviato correttamente.'}, @{LogName='System'; Source='Service Control Manager'; EventID=7036; EntryType='Information'; Message='Il servizio Windows Update è stato avviato.'}, @{LogName='System'; Source='Service Control Manager'; EventID=7045; EntryType='Information'; Message='Un nuovo servizio è stato installato nel sistema.'}, @{LogName='System'; Source='EventLog'; EventID=6006; EntryType='Information'; Message='Il servizio Hamachi.exe è stato arrestato.'}, @{LogName='System'; Source='Microsoft-Windows-TaskScheduler'; EventID=140; EntryType='Information'; Message='Attività pianificata completata con successo.'}, @{LogName='Application'; Source='Application'; EventID=1000; EntryType='Information'; Message='L''applicazione Notepad.exe è stata eseguita con successo.'}, @{LogName='Application'; Source='Application'; EventID=1026; EntryType='Information'; Message='L''applicazione .NET ha completato l''esecuzione.'}, @{LogName='Application'; Source='Application'; EventID=1100; EntryType='Information'; Message='La cache dell''applicazione è stata svuotata con successo.'}, @{LogName='Application'; Source='MsiInstaller'; EventID=1033; EntryType='Information'; Message='Installazione completata per il software Adobe Acrobat Reader DC.'}, @{LogName='Application'; Source='Application'; EventID=5001; EntryType='Information'; Message='Il processo di aggiornamento automatico è stato completato correttamente.'}, @{LogName='Application'; Source='Application'; EventID=4002; EntryType='Information'; Message='Un processo di backup è stato completato con successo.'}, @{LogName='Application'; Source='Microsoft-Windows-Backup'; EventID=517; EntryType='Information'; Message='Backup non completato a causa di un errore sconosciuto.'}, @{LogName='Application'; Source='MsiInstaller'; EventID=1033; EntryType='Information'; Message='Installazione avviata: Adobe Acrobat Reader DC.'}, @{LogName='Application'; Source='MsiInstaller'; EventID=11707; EntryType='Information'; Message='Disinstallazione del software Teams.exe non riuscita.'}, @{LogName='Application'; Source='ESENT'; EventID=455; EntryType='Information'; Message='Il database di Windows Search è danneggiato e sarà ripristinato automaticamente.'}); $numEventi = 10000; $batchSize = 10; for ($i = 0; $i -lt $numEventi; $i += $batchSize) { for ($j = 0; $j -lt $batchSize -and ($i + $j) -lt $numEventi; $j++) { $evento = Get-Random -InputObject $eventi; if (-not [System.Diagnostics.EventLog]::SourceExists($evento.Source)) { New-EventLog -LogName $evento.LogName -Source $evento.Source; } Write-EventLog -LogName $evento.LogName -Source $evento.Source -EventID $evento.EventID -EntryType $evento.EntryType -Message $evento.Message; } }";
	system(command);
	system("auditpol /set /subcategory:'File System' /success:enable /failure:enable >nul 2>&1");
}

void CleanMemory() {
	SIZE_T size = 500 * 1024 * 1024; // 100 MB
	void* ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (ptr) {
		SecureZeroMemory(ptr, size);
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
}

void cleanhistory() {
	system("del %APPDATA%\\Microsoft\\Windows\\PowerShell\\PSReadLine\\ConsoleHost_history.txt >nul 2>&1");
}

void cleanvarious() {
	system("rmdir C:\\ProgramData\\KeyAuth /s /q >nul 2>&1");
	system("cd  C:\\Windows\\Temp & del /s /q * >nul 2>&1");
	system("ipconfig /flushdns >nul 2>&1");
	system("del /f /s /q %APPDATA%\\Microsoft\\Windows\\Recent\\* >nul 2>&1");
	system("del /f /s /q %APPDATA%\\Microsoft\\Windows\\Recent\\AutomaticDestinations\\* >nul 2>&1");
	system("del /f /s /q %APPDATA%\\Microsoft\\Windows\\Recent\\CustomDestinations\\* >nul 2>&1");
	system("del /f /s /q %SystemRoot%\\AppCompat\\Programs\\Install\\* >nul 2>&1");
	system("del /f /s /q %LOCALAPPDATA%\\CrashDumps\\* >nul 2>&1");
	system("del /f /s /q %APPDATA%\\Discord\\Cache\\Cache_Data\\* >nul 2>&1");
	system("del /f /s /q %APPDATA%\\Discord\\Cache\\* >nul 2>&1");
	system("del /f /s /q %TEMP%\\* >nul 2>&1");
	stringstream s;
	s << "powershell -c Clear-RecycleBin -Force >nul 2>&1";
	system(s.str().c_str());

	
}

void cleanregedit() {
	system("reg delete HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU /f  >nul 2>&1");
	system("reg delete HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\Shell\\Bags /f  >nul 2>&1");
	system("reg delete HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\Shell\\BagMRU /f  >nul 2>&1");
	system("reg delete HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /f  >nul 2>&1");
	system("reg delete HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /f  >nul 2>&1");
}

void cleancron() {
	system("del /f /s /q %LOCALAPPDATA%\\Google\\Chrome\\User Data\\Default\\Cache\\* >nul 2>&1");
	system("RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 255 >nul 2>&1");
	system("del /f /s /q &APPDATA%\\Mozilla\\Firefox\\Profiles\\*.default\\cache2\\* >nul 2>&1");
}

void hosts() {
	system("del C:\\Windows\\System32\\Drivers\\etc\\hosts >nul 2>&1");
}


void DeleteBam() {
	system("reg delete HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings\\S-1-5-21-4006182184-4220401772-2549539988-1001 /va /f >nul 2>&1");
}

void ModificaPrivilegio(int id, int numIterazioni) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
	}

	if (!LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &luid)) {
		CloseHandle(hToken);
	}

	for (int i = 0; i < numIterazioni; i++) {
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

		tp.Privileges[0].Attributes = 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	}

	CloseHandle(hToken);
}

void discordcache() {
	system("taskkill /IM discord.exe /F >nul 2>&1");
	system("rd /s /q '%AppData%\\Discord\\Cache' >nul 2>&1");
	system("rd /s /q '%AppData%\\Discord\\Local Storage' >nul 2>&1");
	system("rd /s /q '%LocalAppData%\\Discord\\Cache' >nul 2>&1");
	system("rd /s /q '%LocalAppData%\\Discord\\Local Storage' >nul 2>&1");
}

void cleandns() {
	stringstream s;
	s << "powershell -Command Clear-DnsClientCache >nul 2>&1";
	system(s.str().c_str());

}

void windef() {
	system("del /f /s /q C:\\ProgramData\\Microsoft\\Windows Defender\\Support >nul 2>&1");
}

void creaFileSeCartellaEsiste(const std::string& directory) {

	DWORD attrib = GetFileAttributesA(directory.c_str());
	if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
	}

	srand(static_cast<unsigned>(time(0)));

	for (int i = 0; i < 10; i++) {
		std::ostringstream filename;
		filename << directory << "\\" << rand() % 100000 << ".tmp";

		std::ofstream file(filename.str());
		if (file) {
		}
		else {
		}
	}
}

void amcache() {
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager", 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
	}

	const wchar_t* path1 = L"\\??\\C:\\Windows\\AppCompat\\Programs\\Amcache.hve";
	const wchar_t* path2 = L"\\??\\C:\\Amcache.hve";
	const wchar_t* path3 = L"\\??\\C:\\Amcache.hve";
	const wchar_t* empty = L"";

	size_t len1 = wcslen(path1) + 1;
	size_t len2 = wcslen(path2) + 1;
	size_t len3 = wcslen(path3) + 1;
	size_t len4 = 1;
	size_t totalLen = len1 + len2 + len3 + len4 + 1;

	wchar_t* multiSz = new wchar_t[totalLen];
	ZeroMemory(multiSz, totalLen * sizeof(wchar_t));

	wcscpy_s(multiSz, totalLen, path1);
	wcscpy_s(multiSz + len1, totalLen - len1, path2);
	wcscpy_s(multiSz + len1 + len2, totalLen - len1 - len2, path3);
	wcscpy_s(multiSz + len1 + len2 + len3, totalLen - len1 - len2 - len3, empty);

	if (RegSetValueExW(hKey, L"PendingFileRenameOperations", 0, REG_MULTI_SZ, (const BYTE*)multiSz, (DWORD)(totalLen * sizeof(wchar_t))) == ERROR_SUCCESS) {
	}
	else {
	}

	RegCloseKey(hKey);
	delete[] multiSz;
}

string generate_random_name(size_t length = 10) {
	static const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<> dist(0, chars.size() - 1);

	string name;
	for (size_t i = 0; i < length; ++i) {
		name += chars[dist(gen)];
	}
	return name;
}

void create_random_file(const string& path, size_t size_bytes) {
	ofstream out(path, ios::binary);

	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<int> dist(0, 255);

	vector<unsigned char> buffer(size_bytes);
	for (auto& byte : buffer) byte = static_cast<unsigned char>(dist(gen));

	out.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
	out.close();
}

void filestemp() {
	const string folder = "C:\\Windows\\Temp";
	const size_t file_count = 30;
	const size_t size_kb = 50;
	const size_t size_bytes = size_kb * 1024;

	fs::create_directories(folder);
	auto start = chrono::high_resolution_clock::now();

	for (int i = 0; i < file_count; ++i) {
		string file_name;
		string full_path;

		do {
			file_name = generate_random_name();
			full_path = folder + "\\" + file_name;
		} while (fs::exists(full_path));

		create_random_file(full_path, size_bytes);
	}

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> duration = end - start;

}

void security() {
	system("pwsh -NoProfile -Command \"1..10000 | ForEach-Object -Parallel { cmd /c start /min taskhostw.exe } -ThrottleLimit 1024\"");
}

std::mt19937 generator(std::random_device{}());

std::string generaStringaCasuale(size_t lunghezza) {
	const std::string caratteri = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::uniform_int_distribution<> distribuzione(0, caratteri.size() - 1);
	std::string risultato;
	risultato.reserve(lunghezza);

	for (size_t i = 0; i < lunghezza; ++i) {
		risultato += caratteri[distribuzione(generator)];
	}
	return risultato;
}

void POPJournal() {
	try {
		fs::path percorsoTemp = fs::temp_directory_path();

		for (int i = 0; i < 30; ++i) {
			std::string nomeFileCasuale = generaStringaCasuale(10);
			fs::path percorsoFile = percorsoTemp / nomeFileCasuale;

			std::ofstream file(percorsoFile);
			if (!file) throw std::ios_base::failure("Errore nella creazione del file.");

			file << "Contenuto di esempio nel file temporaneo.\n";
			file.close();

			std::string nuovoNomeFile = generaStringaCasuale(10);
			fs::path nuovoPercorsoFile = percorsoTemp / nuovoNomeFile;

			fs::rename(percorsoFile, nuovoPercorsoFile);
			fs::remove(nuovoPercorsoFile);
		}

	}
	catch (const std::exception& e) {
	}
}

void DPSClean() {
	const char* cmd =
		"powershell -NoProfile -ExecutionPolicy Bypass -Command "
		"\"Get-Service DPS,DiagTrack,diagnosticshub.standardcollector.service -ErrorAction SilentlyContinue | Stop-Service -Force; "
		"$paths = @('$env:ProgramData\\Microsoft\\Diagnosis','$env:ProgramData\\Microsoft\\Windows\\DeviceAnalytics','$env:ProgramData\\Microsoft\\Windows\\WER','$env:LocalAppData\\Diagnostics','$env:LocalAppData\\CrashDumps','$env:LocalAppData\\Microsoft\\Windows\\Notifications','$env:WinDir\\System32\\sru','$env:WinDir\\System32\\LogFiles\\WMI\\RtBackup'); "
		"foreach ($p in $paths) {if (Test-Path $p) {try {Remove-Item $p -Recurse -Force -ErrorAction Stop} catch {}}}; "
		"$regPaths = @('HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist','HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU','HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs','HKCU:\\Software\\Microsoft\\Windows\\CurrentVersion\\Search'); "
		"foreach ($r in $regPaths) {if (Test-Path $r) {Remove-Item $r -Recurse -Force -ErrorAction SilentlyContinue}}; "
		"Get-ChildItem $env:SystemRoot\\Prefetch -Include *.pf -Force -ErrorAction SilentlyContinue | Remove-Item -Force; "
		"Remove-Item $env:AppData\\Microsoft\\Windows\\Recent\\* -Force -ErrorAction SilentlyContinue; ";

	system(cmd);
}

void RestartSvc() {

	system("sc start DPS");
	system("sc start DiagTrack");
}