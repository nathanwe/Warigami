#ifndef __USER_DATA_PATH_H_
#define __USER_DATA_PATH_H_

#define _UNICODE
#include <string>

namespace os
{

#ifdef WIN32
	typedef std::wstring file_path;
#endif

#ifdef __linux__
	typedef std::string file_path;
#endif

	class user_data_paths {
	public:
		static file_path get_user_data_path();
	private:
		static bool _is_folder_present;
	};

}

#endif