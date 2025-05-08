#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

struct body {
	string memory;
};

size_t WriteCallBack(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t total_size = size * nmemb;
	((body*)userp)->memory.append((char*)contents, total_size);
	return total_size;
}

class utente {

private:
	
	bool accesso = false;
	const char* url = "https://keyauth.win/api/1.2/?type=init&ver=2.16a&name=Rickys&ownerid=cBh9igeszQ&c2bf5c6e8caf4c127362133db74c79e6c91c68877d0210392400812bc95b8e63";
	
	bool mm = false;
public:
	const char* names;
	const char* pwd;
	string sessionid;


	string init() {
		CURL* curl;
		CURLcode res;
		body response;
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
			struct curl_slist* headers = NULL;
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
			res = curl_easy_perform(curl);
			
		
			if (res != CURLE_OK) {
				cerr << "errore nella richiesta con il server! \n" << endl;
				exit(1);
			}
			json pars = json::parse(response.memory);
			if (pars.contains("success"))
			{
				if (pars["success"] == true) {
					sessionid = pars["sessionid"];
					return sessionid;
				}
				else
				{
					exit(1);
					
				}
			}
		}
		curl_easy_cleanup(curl);
		return sessionid;
	}

	bool access(string value) {

		
		CURL* curl2;
		CURLcode res2;
		body response2;
		curl_global_init(CURL_GLOBAL_ALL);
		curl2 = curl_easy_init();
		if (curl2) {
			curl_easy_setopt(curl2, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl2, CURLOPT_URL, value.c_str());
			curl_easy_setopt(curl2, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl2, CURLOPT_DEFAULT_PROTOCOL, "https");
			struct curl_slist* headers2 = NULL;
			curl_easy_setopt(curl2, CURLOPT_HTTPHEADER, headers2);
			curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, WriteCallBack);
			curl_easy_setopt(curl2, CURLOPT_WRITEDATA, (void*)&response2);
			res2 = curl_easy_perform(curl2);
			

			if (res2 != CURLE_OK) {
				cerr << "errore nella richiesta con il server! \n" << endl;
				cerr << curl_easy_strerror(res2) << endl;
				exit(1);
			}
			json pars2 = json::parse(response2.memory);
			if (pars2.contains("success"))
			{
				if (pars2["success"] == true) {
					
					cout << "login effettuato!" << endl;
					return true;
				}
				else
				{
					cout << "KTM" << endl;
					return false;
				}
			}
		}
		curl_easy_cleanup(curl2);
		return "";
	}

	bool License(string value) {


		CURL* curl3;
		CURLcode res3;
		body response3;
		curl_global_init(CURL_GLOBAL_ALL);
		curl3 = curl_easy_init();
		if (curl3) {
			curl_easy_setopt(curl3, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(curl3, CURLOPT_URL, value.c_str());
			curl_easy_setopt(curl3, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl3, CURLOPT_DEFAULT_PROTOCOL, "https");
			struct curl_slist* headers2 = NULL;
			curl_easy_setopt(curl3, CURLOPT_HTTPHEADER, headers2);
			curl_easy_setopt(curl3, CURLOPT_WRITEFUNCTION, WriteCallBack);
			curl_easy_setopt(curl3, CURLOPT_WRITEDATA, (void*)&response3);
			res3 = curl_easy_perform(curl3);


			if (res3 != CURLE_OK) {
				cerr << "errore nella richiesta con il server! \n" << endl;
				cerr << curl_easy_strerror(res3) << endl;
				exit(1);
			}
			json pars3 = json::parse(response3.memory);
			if (pars3.contains("success"))
			{
				if (pars3["success"] == true) {

					cout << "Upgrade Effettuato con Successo" << endl;
					return true;
				}
				else
				{
					cout << "Key non Valida!" << endl;
					return false;
				}
			}
		}
		curl_easy_cleanup(curl3);
		return "";
	}
	
};

class menu {
public:
	int scelta;
	int cast;
};