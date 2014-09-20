#ifndef WEBDRIVERXX_RESPONSE_STATUS_CODE_H
#define WEBDRIVERXX_RESPONSE_STATUS_CODE_H 

namespace webdriverxx {
namespace response_status_code { 

enum Value {
	kSuccess = 0,
	kNoSuchDriver = 6,
	kNoSuchElement = 7,
	kNoSuchFrame = 8,
	kUnknownCommand = 9,
	kStaleElementReference = 10,
	kElementNotVisible = 11,
	kInvalidElementState = 12,
	kUnknownError = 13,
	kElementIsNotSelectable = 15,
	kJavaScriptError = 17,
	kXPathLookupError = 19,
	kTimeout = 21,
	kNoSuchWindow = 23,
	kInvalidCookieDomain = 24,
	kUnableToSetCookie = 25,
	kUnexpectedAlertOpen = 26,
	kNoAlertOpenError = 27,
	kScriptTimeout = 28,
	kInvalidElementCoordinates = 29,
	kIMENotAvailable = 30,
	kIMEEngineActivationFailed = 31,
	kInvalidSelector = 32,
	kSessionNotCreatedException = 33,
	kMoveTargetOutOfBounds = 34
}; 

inline
const char* ToString(Value code) {
	switch(code) {
	case kSuccess: return "The command executed successfully.";
	case kNoSuchDriver: return "A session is either terminated or not started"; 
	case kNoSuchElement: return "An element could not be located on the page using the given search parameters."; 
	case kNoSuchFrame: return "A request to switch to a frame could not be satisfied because the frame could not be found."; 
	case kUnknownCommand: return "The requested resource could not be found, or a request was received using an HTTP method that is not supported by the mapped resource."; 
	case kStaleElementReference: return "An element command failed because the referenced element is no longer attached to the DOM."; 
	case kElementNotVisible: return "An element command could not be completed because the element is not visible on the page."; 
	case kInvalidElementState: return "An element command could not be completed because the element is in an invalid state (e.g. attempting to click a disabled element)."; 
	case kUnknownError: return "An unknown server-side error occurred while processing the command."; 
	case kElementIsNotSelectable: return "An attempt was made to select an element that cannot be selected."; 
	case kJavaScriptError: return "An error occurred while executing user supplied JavaScript."; 
	case kXPathLookupError: return "An error occurred while searching for an element by XPath."; 
	case kTimeout: return "An operation did not complete before its timeout expired."; 
	case kNoSuchWindow: return "A request to switch to a different window could not be satisfied because the window could not be found."; 
	case kInvalidCookieDomain: return "An illegal attempt was made to set a cookie under a different domain than the current page."; 
	case kUnableToSetCookie: return "A request to set a cookie's value could not be satisfied."; 
	case kUnexpectedAlertOpen: return "A modal dialog was open, blocking this operation"; 
	case kNoAlertOpenError: return "An attempt was made to operate on a modal dialog when one was not open."; 
	case kScriptTimeout: return "A script did not complete before its timeout expired."; 
	case kInvalidElementCoordinates: return "The coordinates provided to an interactions operation are invalid."; 
	case kIMENotAvailable: return "IME was not available."; 
	case kIMEEngineActivationFailed: return "An IME engine could not be started."; 
	case kInvalidSelector: return "Argument was an invalid selector (e.g. XPath/CSS).";
	case kSessionNotCreatedException: return "A new session could not be created.";
	case kMoveTargetOutOfBounds: return "Target provided for a move action is out of bounds.";
	}
	return "Unknown";
} 

} // namespace response_status_code 
} // namespace webdriverxx 

#endif
