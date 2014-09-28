#ifndef WEBDRIVERXX_DETAIL_RESOURCE_H
#define WEBDRIVERXX_DETAIL_RESOURCE_H

#include "error_handling.h"
#include "http_client.h"
#include "../conversions.h"
#include "../response_status_code.h"
#include <picojson.h>

namespace webdriverxx {
namespace detail {

template<class ResponseReturnPolicy>
class ResourceBase { // copyable
public:
	ResourceBase(const IHttpClient* http_client, const std::string& url)
		: http_client_(http_client)
		, url_(url) {}

	const std::string& GetUrl() const {
		return url_;
	}

	template<class SubResource>
	SubResource GetSubResource(const std::string& name) const {
		return SubResource(http_client_, ConcatUrl(url_, name));
	}

	ResourceBase<ResponseReturnPolicy> GetSubResource(const std::string& name) const {
		return GetSubResource< ResourceBase<ResponseReturnPolicy> >(name);
	}

	picojson::value Get(const std::string& command = std::string()) const {
		return Download(command, &IHttpClient::Get, "GET");
	}

	template<typename T>
	T GetValue(const std::string& command) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return FromJson<T>(Get(command));
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt() <<
			"command: " << command
			)
	}

	std::string GetString(const std::string& command) const {
		return GetValue<std::string>(command);
	}

	bool GetBool(const std::string& command) const {
		return GetValue<bool>(command);
	}

	picojson::value Delete(const std::string& command = std::string()) const {
		return Download(command, &IHttpClient::Delete, "DELETE");
	}

	picojson::value Post(
		const std::string& command = std::string(),
		const picojson::value& upload_data = picojson::value()
		) const {
		return Upload(command, upload_data, &IHttpClient::Post, "POST");
	}

	void Post(
		const std::string& command,
		const std::string& arg_name,
		const std::string& arg_value
		) const {
		Post(command, JsonObject().With(arg_name, arg_value).Build());
	}	

	template<typename T>
	void PostValue(const std::string& command, const T& value) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		Post(command, ToJson(value));
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt() <<
			"command: " << command
			)
	}	

	picojson::value Put(
		const std::string& command = std::string(),
		const picojson::value& upload_data = picojson::value()
		) const {
		return Upload(command, upload_data, &IHttpClient::Put, "PUT");
	}

private:
	picojson::value Download(
		const std::string& command, 
		HttpResponse (IHttpClient::* member)(const std::string& url) const,
		const char* request_type
		) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return ProcessResponse((http_client_->*member)(
			ConcatUrl(url_, command)
			));
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(Fmt()
			<< "request: " << request_type
			<< ", command: " << command
			<< ", resource: " << url_
			)
	}

	static std::string ToUploadData(const picojson::value& upload_data)
	{
		return upload_data.is<picojson::null>() ?
			std::string() : upload_data.serialize();
	}

	picojson::value Upload(
		const std::string& command, 
		const picojson::value& upload_data,
		HttpResponse (IHttpClient::* member)(const std::string& url, const std::string& upload_data) const,
		const char* request_type
		) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return ProcessResponse((http_client_->*member)(
			ConcatUrl(url_, command),
			ToUploadData(upload_data)
			));
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(Fmt()
			<< "request: " << request_type
			<< ", command: " << command
			<< ", resource: " << url_
			<< ", data: " << ToUploadData(upload_data)
			)
	}

	picojson::value ProcessResponse(
		const HttpResponse& http_response
		) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		WEBDRIVERXX_CHECK(
			http_response.http_code / 100 != 4 &&
			http_response.http_code != 501,
			"HTTP code indicates that request is invalid");

		picojson::value response;
		std::string error_message;
		picojson::parse(response, http_response.body.begin(), http_response.body.end(), &error_message);

		WEBDRIVERXX_CHECK(error_message.empty(),
			Fmt() << "JSON parser error (" << error_message << ")"
			);

		WEBDRIVERXX_CHECK(response.is<picojson::object>(), "Server response is not an object");
		WEBDRIVERXX_CHECK(response.contains("status"), "Server response has no member \"status\"");
		WEBDRIVERXX_CHECK(response.get("status").is<double>(), "Response status code is not a number");
		const response_status_code::Value status =
			static_cast<response_status_code::Value>(static_cast<int>(response.get("status").get<double>()));
		WEBDRIVERXX_CHECK(response.contains("value"), "Server response has no member \"value\"");
		const picojson::value& value = response.get("value");

		if (http_response.http_code == 500) { // Internal server error
			WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Server returned HTTP code 500 and \"response.value\" is not an object");
			WEBDRIVERXX_CHECK(value.contains("message"), "Server response has no member \"value.message\"");
			WEBDRIVERXX_CHECK(value.get("message").is<std::string>(), "\"value.message\" is not a string");
			WEBDRIVERXX_THROW(Fmt() << "Server failed to execute command ("
				<< "message: " << value.get("message").to_str()
				<< ", status: " << response_status_code::ToString(status)
				<< ", status_code: " << status
				<< ")"
				);
		}
		WEBDRIVERXX_CHECK(status == response_status_code::kSuccess, "Non-zero response status code");
		WEBDRIVERXX_CHECK(http_response.http_code == 200, "Unsupported HTTP code");

		return ResponseReturnPolicy::ReturnResponse(response);
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(Fmt()
			<< "HTTP code: " << http_response.http_code
			<< ", body: " << http_response.body
			)
	}

	static
	std::string ConcatUrl(const std::string& a, const std::string& b, const char delim = '/') {
		std::string result = a.empty() ? b : a;
		if (!a.empty() && !b.empty()) {
			if (result[result.length()-1] != delim)
				result += delim;
			result.append(b[0] == delim ? b.begin() + 1 : b.begin(), b.end());
		}
		return result;
	}

private:
	const IHttpClient* http_client_;
	std::string url_;
};

struct ReturnFullResponse {
	static picojson::value ReturnResponse(picojson::value& response) {
		picojson::value result;
		response.swap(result);
		return result;
	}
};

struct ReturnValue {
	static picojson::value ReturnResponse(picojson::value& response) {
		picojson::value result;
		response.get("value").swap(result);
		return result;
	}
};

typedef ResourceBase<ReturnFullResponse> RootResource;
typedef ResourceBase<ReturnValue> Resource;

} // namespace detail
} // namespace webdriverxx

#endif
