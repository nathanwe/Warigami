#ifndef __USER_DATA_PATH_H_
#define __USER_DATA_PATH_H_

#define _UNICODE
#include <string>

namespace os
{
	std::wstring s2ws(const std::string& as);
	std::string ws2s(const std::wstring& wstr);

#ifdef WIN32
	typedef std::wstring file_path;	
	file_path make_file_path(const std::string& path);
#endif

#ifdef __linux__
	typedef std::string file_path;
	file_path make_file_path(const std::string& path);
#endif

	class user_data_paths {
	public:
		static file_path get_user_data_path();
	private:
		static bool _is_folder_present;
	};

}

#endif