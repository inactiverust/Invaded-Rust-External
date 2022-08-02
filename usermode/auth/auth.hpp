#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "../offsets.hpp"

#include "C://Users/ItzzJ/Desktop/Themida/ThemidaSDK/Include/C/ThemidaSDK.h"

namespace KeyAuth {
	class api {
	public:
		std::string name, ownerid, secret, version, url, sslPin;

		api(std::string name, std::string ownerid, std::string secret, std::string version, std::string url, std::string sslPin) : name(name), ownerid(ownerid), secret(secret), version(version), url(url), sslPin(sslPin) {}

		void ban();
		void init();
		void check();
		void log(std::string msg);
		void license(std::string key);
		std::string var(std::string varid);
		std::string webhook(std::string id, std::string params);
		void setvar(std::string var, std::string vardata);
		std::string getvar(std::string var);
		bool checkblack();
		void upgrade(std::string username, std::string key);
		void login(std::string username, std::string password);
		void web_login();
		void button(std::string value);
		std::vector<unsigned char> download(std::string fileid);
		void regstr(std::string username, std::string password, std::string key);

		class data_class {
		public:
			// app data
			std::string numUsers;
			std::string numOnlineUsers;
			std::string numKeys;
			std::string version;
			std::string customerPanelLink;
			// user data
			std::string username;
			std::string ip;
			std::string hwid;
			std::string createdate;
			std::string lastlogin;
			std::vector<std::string> subscriptions;
			std::string expiry;
			// response data
			bool success;
			std::string message;
		};
		data_class data;

	private:
		std::string sessionid, enckey;
	};
}

namespace auth
{
	std::string tm_to_readable_time(tm ctx) {
		char buffer[80];

		strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

		return std::string(buffer);
	}

	static std::time_t string_to_timet(std::string timestamp) {
		auto cv = strtol(timestamp.c_str(), NULL, 10); // long

		return (time_t)cv;
	}

	static std::tm timet_to_tm(time_t timestamp) {
		std::tm context;

		localtime_s(&context, &timestamp);

		return context;
	}

	using namespace KeyAuth;

	std::string name = _("InvadedRust"); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
	std::string ownerid = _("0TSzhgf2a6"); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
	std::string secret = _("516840c5e3a6cf085dbc66bd2a417a6e7bbc6a868b4abd173b8c9bba6ba4153e"); // app secret, the blurred text on licenses tab and other tabs
	std::string version = _("1.0"); // leave alone unless you've changed version on website
	std::string url = _("https://keyauth.win/api/1.1/"); // change if you're self-hosting
	std::string sslPin = _("ssl pin key (optional)"); // don't change unless you intend to pin public certificate key. you can get here in the "Pin SHA256" field https://www.ssllabs.com/ssltest/analyze.html?d=keyauth.win&latest. If you do this you need to be aware of when SSL key expires so you can update it

	api KeyAuthApp(name, ownerid, secret, version, url, sslPin);

	void setup_vars()
	{
		classes::oAdminConVar = std::stoull(KeyAuthApp.var(_("AdminConVar")));
		classes::oBaseEntity = std::stoull(KeyAuthApp.var(_("BaseEntity")));
		classes::oConsoleSystem = std::stoull(KeyAuthApp.var(_("ConsoleSystem")));
		classes::oGraphicConVar = std::stoull(KeyAuthApp.var(_("GraphicsConVar")));
		classes::oMainCamera = std::stoull(KeyAuthApp.var(_("MainCamera")));
		classes::oOcclusionCulling = std::stoull(KeyAuthApp.var(_("OcclusionCulling")));
		classes::oTODSky = std::stoull(KeyAuthApp.var(_("TODSky")));
	}

	void download_file(std::string fileid, const char* filename)
	{
		std::vector<std::uint8_t> bytes = KeyAuthApp.download(fileid);
		std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);
		file.write((char*)bytes.data(), bytes.size());
		file.close();
	}

	void do_auth()
	{
		VM_EAGLE_BLACK_START
			SetConsoleTitleA(skCrypt("Login"));
		std::cout << skCrypt("\n Connecting . . .");
		KeyAuthApp.init();
		if (!KeyAuthApp.data.success)
		{
			std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
			Sleep(1500);
			exit(0);
		}
		std::cout << skCrypt("\n Application Version: ") << KeyAuthApp.data.version;
		KeyAuthApp.check();

		std::cout << skCrypt("\n\n Enter Key: ");

		std::string key;

		std::cin >> key;

		KeyAuthApp.license(key);

		if (!KeyAuthApp.data.success)
		{
			std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
			Sleep(1500);
			exit(0);
		}
		std::cout << skCrypt("\n Subscription expiry: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.expiry)));
		KeyAuthApp.check();
		std::cout << skCrypt("\n ") << KeyAuthApp.data.message << "\n";
		setup_vars();
		VM_EAGLE_BLACK_END
	}
}
