#include <curl/curl.h> // TODO: remove this line
#include <gtest/gtest.h>
#include <string> // TODO: remove this line
#include <vector> // TODO: remove this line

// TODO: remove this function
size_t WriteCallback(void *data, size_t size, size_t nmemb,
                            void* userdata)
{
	std::vector<char>* body;
	body = reinterpret_cast<std::vector<char>*>(userdata);
	const char* begin = reinterpret_cast<char*>(data);
	body->insert(body->end(), begin, begin+size*nmemb);
	return (size * nmemb);
}

// TODO: remove this function
void Get(const std::string& url)
{
  /** create return struct */
	std::vector<char> data;

  // use libcurl
  CURL *curl = NULL;
  CURLcode res = CURLE_OK;

  curl = curl_easy_init();
  ASSERT_TRUE(curl);
  if (curl)
  {
    /** set basic authentication if present*/
    // if(RestClient::user_pass.length()>0){
    //   curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //   curl_easy_setopt(curl, CURLOPT_USERPWD, RestClient::user_pass.c_str());
    // }
    /** set user agent */
//	curl_easy_setopt(curl, CURLOPT_USERAGENT, RestClient::user_agent);
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    /** set the header callback function */
//	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RestClient::header_callback);
    /** callback object for headers */
//	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    ASSERT_EQ(CURLE_OK, res);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

	ASSERT_EQ(200, http_code);
	ASSERT_TRUE(data.size() > 0);
	ASSERT_TRUE(data[0] == '{');
	ASSERT_TRUE(data[data.size()-1] == '}');
  }
}

TEST(Skeleton, CanRunTests) {
	SUCCEED();
}

// TODO: remove this test
TEST(Skeleton, CanUseCurl) {
	Get("http://localhost:7777/status");
}

