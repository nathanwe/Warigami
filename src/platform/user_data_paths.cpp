#include "platform/user_data_paths.hpp"
#include <sstream>

#ifdef WIN32
#include <Shlobj.h>
#endif

#ifdef __linux__
#include <sys/stat.h>
#include <iostream>
#endif

#ifdef WIN32
// From https://stackoverflow.com/questions/22585326/how-to-convert-stdwstring-to-lpctstr-in-c
std::string ws2s(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), 0, 0, 0, 0);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), &strTo[0], size_needed, 0, 0);
	return strTo;
}
#endif

os::file_path os::user_data_paths::get_user_data_path() {

	#ifdef WIN32
	PWSTR path;
	HRESULT result = SHGetKnownFolderPath(
		FOLDERID_RoamingAppData,  // %APPDATA% (%USERPROFILE%\AppData\Roaming)
		KF_FLAG_DEFAULT, // No flags
		NULL, // Current user
		&path
	);
	if (result == S_OK) {
		std::wstringstream ss;
		ss << path << L"\\DigiPenWizardPeople";

		CoTaskMemFree(static_cast<void*>(path));

		if (!_is_folder_present) {
			// Create folder
			if (!CreateDirectoryW(ss.str().c_str(), NULL) &&
				ERROR_ALREADY_EXISTS != GetLastError())
			{
				// The folder doesn't exist and failed to be created
				return NULL;
			}
			_is_folder_present = true;
		}
		
		return ss.str();
	}
	else {
		return NULL;
	}
	#endif

	#ifdef __linux__
	auto homePath = getenv("HOME");
	std::stringstream ss;
	ss << homePath << "/.config";
	if (!_is_folder_present) {
		// Create folder
		int result = mkdir(ss.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (!result && result != EEXIST)
		{
			// The .config folder doesn't exist and failed to be created
			return NULL;
		}
		ss << "/DigiPenWizardPeople";
		result = mkdir(ss.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (!result && result != EEXIST)
		{
			// The Wizard People folder doesn't exist and failed to be created
			return NULL;
		}
		_is_folder_present = true;
	}
	return ss.str();
	#endif
}

bool os::user_data_paths::_is_folder_present = false;