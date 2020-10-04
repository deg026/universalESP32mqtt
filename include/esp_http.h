
#ifndef ESP_HTTP
#define ESP_HTTP

void http_setup();

void http_handle();

bool HTTP_auth();

void HTML_page(const String& title, const String& body);

String quoteEscape(const String& str);

extern String bestpins;

#endif
