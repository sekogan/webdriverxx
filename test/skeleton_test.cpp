// TODO: remove this file
#include <webdriverxx.h>
#include <curl/curl.h>
#include <gtest/gtest.h>
#include <string>
#include <memory>

size_t CurlWriteCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	std::string* data_received = reinterpret_cast<std::string*>(userdata);
	const auto buffer_size = size * nmemb;
	data_received->append(reinterpret_cast<char*>(buffer), buffer_size);
	return buffer_size;
}

void HttpGet(const std::string& url)
{
	std::string data_received;

	CURL* curl = curl_easy_init();
	ASSERT_TRUE(curl);

	if (!curl)
		return;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data_received);
	const auto result = curl_easy_perform(curl);

	long http_code = 0;
	if (result == CURLE_OK)
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	curl_easy_cleanup(curl);

	ASSERT_EQ(CURLE_OK, result);
	ASSERT_EQ(200, http_code);
	ASSERT_TRUE(data_received.size() > 0);
	ASSERT_TRUE(data_received[0] == '{');
	ASSERT_TRUE(data_received[data_received.length()-1] == '}');
}

TEST(Skeleton, CanRunTests) {
	SUCCEED();
}

TEST(Skeleton, CanUseCurl) {
	HttpGet("http://localhost:7777/status");
}

TEST(Skeleton, Cxx11) {
	std::unique_ptr<int> p(new int(123));
	[]{
		SUCCEED();
	}();
	auto x = 1;
	(void)p;
	(void)x;
}
