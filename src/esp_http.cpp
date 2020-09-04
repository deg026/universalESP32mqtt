
#include <global.h>
#include <WebServer.h>
#include <Update.h>
#include <esp_eeprom.h>
#include <esp_relay.h>
#include <esp_input.h>
#include <esp_analog.h>
#include <esp_mqtt.h>
#include <esp_temper.h>

WebServer httpserver(80);

const byte maxStrParamLength = 26;
String HTTP_user = "admin";
String HTTP_password = "admin";

const char* HTTP_realm = "Please authorize (def: admin/admin)";
String HTTP_autherror = "User authorization failed...";



String bestpins = " <pre>Best PINs usage, from best to worst (<B class='bi'>I</B>nput, <B class='bo'>O</B>utput, <B class='ba'>A</B>nalog, <B class='bud'>u</B> - pullup, <B class='bud'>d</B> - pulldown):\n"
                  " 39 - <B class='bi'>I</B><B class='bud'>d</B>\n"
                  " 36 - <B class='bi'>I</B><B class='bud'>d</B>\n"
                  " 35 - <B class='bi'>I</B><B class='bud'>d</B>\n"
                  " 34 - <B class='bi'>I</B><B class='bud'>d</B> / <B class='ba'>A</B>\n"
                  " 33 - <B class='bi'>I</B> / <B class='bo'>O</B> / <B class='ba'>A</B>\n"
                  " 32 - <B class='bi'>I</B> / <B class='bo'>O</B> / <B class='ba'>A</B>\n"
                  " 27 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 26 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 25 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 23 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 22 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 21 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 19 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 18 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 14 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  "  4 - <B class='bi'>I</B> / <B class='bo'>O</B>\n"
                  " 17 - <B class='bi'>I</B> / <B class='bo'>O</B> (in some device used for internal Flash)\n"
                  " 16 - <B class='bi'>I</B> / <B class='bo'>O</B> (in some device used for internal Flash)\n"
                  " 15 - <B class='bi'>I</B> (if pulled low there will be no output on the Serial port)\n"
                  "  5 - <B class='bi'>I</B> / <B class='bo'>O</B> (connected to internal LED on some devices)\n"
                  "  2 - <B class='bi'>I</B><B class='bud'>u</B> / <B class='bo'>O</B> (connected to internal LED on most devices)</pre>";

String style =
        "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
        "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
        "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
        "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
        "form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
        ".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

String quoteEscape(const String& str)
{
    String result = str;
    result.replace("\"", "&quot;");
    return result;
}



String getMacAddress()
{
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}


bool HTTP_auth()
{
    if (HTTP_user.length() && HTTP_password.length() && !httpserver.authenticate(HTTP_user.c_str(), HTTP_password.c_str()))
    {
        httpserver.requestAuthentication(BASIC_AUTH, HTTP_realm, HTTP_autherror);
        return false;
    }
    return true;
}


void HTML_page(const String& title, const String& body)
{
    // language=HTML
    String message = "<!DOCTYPE html><html lang='en'><head><title>ESP32 mqtt</title>\
<style>\
.checkbox {\
  vertical-align:top;\
  margin:0 3px 0 0;\
  width:17px;\
  height:17px;\
}\
.checkbox + label {\
  cursor:pointer;\
}\
.checkbox:not(checked) {\
  position:absolute;\
  opacity:0;\
}\
.checkbox:not(checked) + label {\
  position:relative;\
  padding:0 0 0 60px;\
}\
.checkbox:not(checked) + label:before {\
  content:'';\
  position:absolute;\
  top:-4px;\
  left:0;\
  width:50px;\
  height:26px;\
  border-radius:13px;\
  background:#CDD1DA;\
  box-shadow:inset 0 2px 3px rgba(0,0,0,.2);\
}\
.checkbox:not(checked) + label:after {\
  content:'';\
  position:absolute;\
  top:-2px;\
  left:2px;\
  width:22px;\
  height:22px;\
  border-radius:10px;\
  background:#FFF;\
  box-shadow:0 2px 5px rgba(0,0,0,.3);\
  transition:all .2s;\
}\
.checkbox:checked + label:before {\
  background:#9FD468;\
}\
.checkbox:checked + label:after {\
  left:26px;\
}\
#relayform td {\
  line-height: 20px;\
  text-align: right;\
  padding:    5px;\
}\
#rootform p {\
    margin: 9px;\
    width: 260px;\
}\
#rootform p b {\
    display: block;\
    float: right;\
}\
span.inputinfo {\
    display: inline-block;\
    border: solid 1px black;\
    background: #999999;\
    color: black;\
    width: 16px;\
    height: 16px;\
    padding: 4px;\
}\
span.inputinfo.inputon {\
    border: solid 1px #008800;\
    background: #aaffaa;\
    color: red;\
}\
b.bi {\
    color: green;\
}\
b.bo {\
    color: red;\
}\
b.ba {\
    color: blue;\
}\
b.bud {\
    color: orange;\
}\
</style>\
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>\
<script>\
function openUrl(url) {\
  var request = new XMLHttpRequest();\
  request.open('GET', url, true);\
  request.send(null);\
}\
</script>\
</head>\
<body><h2>ESP mqtt v" + String(appversion) + "</h2>\
<input type='button' value='Dashboard' onclick=\"location.href='/';\">\
<input type='button' value='Base setup' onclick=\"location.href='/setup';\">\
<input type='button' value='MQTT' onclick=\"location.href='/mqtt';\">\
<input type='button' value='Relays' onclick=\"location.href='/relay';\">\
<input type='button' value='Inputs' onclick=\"location.href='/input';\">\
<input type='button' value='Analog' onclick=\"location.href='/analog';\">\
<input type='button' value='Firmware' onclick=\"location.href='/firmware';\">\
<input type='button' value='Defaults!' onclick=\"if(confirm('Are you sure to reset settings to default?')) openUrl('/reset');\">\
<input type='button' value='Reboot!' onclick=\"if(confirm('Are you sure to reboot?')) openUrl('/reboot');\">\
<br><h3>" + title + "</h3>" + body + "</body></html>\
";


    httpserver.sendHeader("Connection", "close");
    httpserver.send(200, "text/html", message);
}


void page_root()
{
    if(!HTTP_auth())
        return;

    String message = "<form id='rootform'><p>ESP mac ID: <b>" + getMacAddress() + "</b></p><p>WiFi mode: <b>";

    switch (WiFi.getMode()) {
        case WIFI_OFF:
            message += "OFF";
            break;
        case WIFI_STA:
            message += "Station";
            break;
        case WIFI_AP:
            message += "Access Point";
            break;
        case WIFI_AP_STA:
            message += "Hybrid (AP+STA)";
            break;
        default:
            message += "Unknown!";
    }

    message += "</b></p><p>MQTT broker: <b>";

    if (!MQTT_isconnected())
        message += "OFF";
    else
        message += "ON";

    //Current UPtime in seconds
    uint32_t tm = millis() / 1000;

    message += " (" + MQTT_client + ")</b></p>"
               "<p>CPU freq: <b>" + String(ESP.getCpuFreqMHz()) + " MHz</b></p>"
               "<p>Free memory: <b>" + String(ESP.getFreeHeap()) + " b</b></p>"
               "<p>Flash size: <b>" + String(ESP.getFlashChipSize()) + " b</b></p>"
               "<p>Program size: <b>" + String(ESP.getSketchSize() + ESP.getFreeSketchSpace()) + " b</b></p>"
               "<p>Sketch size: <b>" + String(ESP.getSketchSize()) + " b</b></p>"
               "<p>Free for sketch: <b>" + String(ESP.getFreeSketchSpace()) + " b</b></p>"
               "<p>Uptime: <b>"
               + (tm > 60 * 60 * 24 ? String(int(tm / 60 / 60 / 24 % 365)) + "D " : "")
               + (tm > 60 * 60  ? String(int(tm / 60 / 60 % 24)) + "H " : "")
               + (tm > 60 ? String(int((tm + 1) / 60 % 60)) + "m " : "")
               + String(int(tm % 60)) + "s</b></p>"
               "<br><br><b>RELAYS:</b>" + RELAY_getInfo() +
               "<br><br><b>INPUTS:</b>" + INPUT_getInfo() +
               "<br><br><b>ANALOG:</b>" + ANALOG_getInfo();


    message += "<br><br><b>TEMPERATURE sensors:</b><br>" + TEMPER_getInfo();

    HTML_page("System dashboard:", message);
}


void page_firmware()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "\
<form id='firmwareform' name='firmware' method='POST' enctype='multipart/form-data'>\
<input type='file' name='update' id='file' onchange='sub(this)' style='display:none'>\
<label id='file-input' for='file'>   Choose file...</label>\
<input type='submit' class=btn value='Update'>\
<br><br>\
<div id='prg'></div><br><div id='prgbar'><div id='bar'></div></div>\
<script>\
function sub(obj){\
    var fileName = obj.value.split('\\\\');\
    $('#file-input').html('   '+ fileName[fileName.length-1]);\
}\
$('form').on('submit', function(e){\
    e.preventDefault();\
    var data = new FormData($('#firmwareform')[0]);\
    $.ajax({\
        url: '/update',\
        type: 'POST',\
        data: data,\
        contentType: false,\
        processData:false,\
        xhr: function() {\
            var xhr = new window.XMLHttpRequest();\
            xhr.upload.addEventListener('progress', function(evt) {\
                if (evt.lengthComputable) {\
                    var per = evt.loaded / evt.total;\
                    $('#prg').html('progress: ' + Math.round(per*100) + '%');\
                    $('#bar').css('width',Math.round(per*100) + '%');\
                }\
            }, false);\
            return xhr;\
        },\
    });\
});\
</script>\
</form>\
";

    HTML_page("Firmware update:", message);
}


void page_setup()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "\
<form id='setupform' name='setup' method='get' action='/store'>\
<b>WiFi SSID:</b>\
<br><input type='text' name='ssid' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(WF_ssidSTA) + "'>\
<br><b>WiFi password:</b>\
<br><input type='text' name='ssidpass' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(WF_passwordSTA) + "'>\
<br><br><br>\
<b>WebServer user:</b>\
<br><input type='text' name='httpuser' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(HTTP_user) + "'>\
<br><b>WebServer password:</b>\
<br><input type='text' name='httppass' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(HTTP_password) + "'>\
<br><br><br>\
<b>LED pin:</b>\
<br><input type='text' name='ledpin' size='2' value='" + (LED_PIN > -1 ? String(LED_PIN) : "") + "'>\
<br><br><br>\
<b>1-Wire pin:</b>\
<br><input type='text' name='wirepin' size='2' value='" + (ONEWIRE_PIN > -1 ? String(ONEWIRE_PIN) : "") + "'>\
<br><b>1-Wire update period:</b>\
<br><input type='text' name='wireupdate' size='2' value='" + String(TEMPER_timeout) + "'> sec\
<br><br><input type='submit' value='Save'>\
</form>\
";

    HTML_page("Base Setup:", message);
}


void page_mqtt()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "\
<form id='mqttform' name='mqtt' method='get' action='/store'>\
<b>Server:</b>\
<br><input type='text' name='mqttserver' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_server) + "'>\
 (leave blank to ignore MQTT)\
<br><b>Port:</b>\
<br><input type='text' name='mqttport' maxlength=5 value='" + String(MQTT_port) + "'>\
<br><b>Client name:</b>\
<br><input type='text' name='mqttclient' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_client) + "'>\
<br><b>User:</b> (if authorization is required on MQTT server)\
<br><input type='text' name='mqttuser' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_user) + "'>\
 (leave blank to ignore MQTT authorization)\
<br><b>Password:</b>\
<br><input type='password' name='mqttpass' maxlength=" + String(maxStrParamLength) + " value='" + quoteEscape(MQTT_pass) + "'>\
<br><input type='submit' value='Save'>\
</form>\
";
    HTML_page("MQTT Setup:", message);
}


void page_analog()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='analogform' name='analog' method='get' action='/store'>\
<input type='hidden' name='analogsetup' value='1'>\
<table border='0'><tr><td><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < analogcount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='apinid" + String(i) + "' value='" + (analogPins[i] > -1 ? String(analogPins[i]) : "") + "'>";
    }

    message += "</td></tr></table><br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Analog Setup:", message);
}


void page_relay()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='relayform' name='relay' method='get' action='/store'>\
<input type='hidden' name='relaysetup' value='1'>\
<table border='0'><tr><td><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < relaycount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='pinid" + String(i) + "' value='" + (relayPins[i] > -1 ? String(relayPins[i]) : "") + "'>";
    }

    message += "</td><td><b>State on boot:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='onboot" + String(i) + "' value='1' ";
        if (relayOnBoot[i]) message += "checked ";
        message += "> ON &nbsp; <input type='radio' name='onboot" + String(i) + "' value='0' ";
        if (! relayOnBoot[i]) message += "checked ";
        message += "> OFF";
    }

    message += "</td></tr></table><br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Relay Setup:", message);
}


void page_input()
{
    if(!HTTP_auth())
        return;

    // language=HTML
    String message = "<form id='inputform' name='input' method='get' action='/store'>\
<input type='hidden' name='inputsetup' value='1'>\
<table border='0'><tr><td style='padding-right: 15px;'><b>PIN IDs:</b>\
";
    byte i;

    for (i = 0; i < inputcount; i++)
    {
        message += "<br>#" + String(i) + ": <input type='text' size='2' name='inputpin" + String(i) + "' value='" + (inputPins[i] > -1 ? String(inputPins[i]) : "") + "'>";
    }

    message += "</td><td style='border-left: dashed 1px black;padding: 0px 15px;'><b>Input type:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='inputtype" + String(i) + "' value='0' ";
        if (inputTypes[i] % 10 == 0) message += "checked ";
        message += "> NORMAL &nbsp; <input type='radio' name='inputtype" + String(i) + "' value='1' ";
        if (inputTypes[i] % 10 == 1) message += "checked ";
        message += "> PULLUP &nbsp; <input type='radio' name='inputtype" + String(i) + "' value='2' ";
        if (inputTypes[i] % 10 == 2) message += "checked ";
        message += "> PULLDOWN";
    }

    message += "</td><td style='border-left: dashed 1px black;padding: 0px 15px;'><b>MTQQ send:</b>";

    for (i = 0; i < relaycount; i++)
    {
        message += "<br><input type='radio' name='inputsend" + String(i) + "' value='0' ";
        if (inputTypes[i] < 10) message += "checked ";
        message += "> HIGH & LOW &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='1' ";
        if (inputTypes[i] >= 10 && inputTypes[i] < 20) message += "checked ";
        message += "> only HIGH &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='2' ";
        if (inputTypes[i] >= 20 && inputTypes[i] < 30) message += "checked ";
        message += "> only LOW &nbsp; <input type='radio' name='inputsend" + String(i) + "' value='3' ";
        if (inputTypes[i] >= 30) message += "checked ";
        message += "> BUTTON (click, longpress, dblclick)";
    }

    message += "</td></tr></table><br><input type='submit' value='Save'></form>" + bestpins;

    HTML_page("Inputs Setup:", message);
}


void page_store()
{
    if(!HTTP_auth())
        return;

    String argName, argValue;
    byte r, i;

    bool isRelaySetup = (httpserver.arg("relaysetup") == "1");
    bool isInputSetup = (httpserver.arg("inputsetup") == "1");
    bool isAnalogSetup = (httpserver.arg("analogsetup") == "1");

    if(isRelaySetup)
    {
        for (r = 0; r < relaycount; r++)
        {
            relayOnBoot[r] = false;
            relayPins[r] = -1;
        }
    }
    if(isInputSetup)
    {
        for (r = 0; r < inputcount; r++)
        {
            inputTypes[r] = 0;
            inputPins[r] = -1;
        }
    }

    for (i = 0; i < httpserver.args(); i++)
    {
        argName = httpserver.argName(i);
        argValue = httpserver.arg(i);

        if(argValue.length() <= maxStrParamLength)
        {
            if(isRelaySetup)
            {
                for (r = 0; r < relaycount; r++)
                {
                    if (argName == ("onboot" + String(r)))
                    {
                        relayOnBoot[r] = (argValue.toInt() > 0 ? 1 : 0);
                        break;
                    }
                    else if (argName == ("pinid" + String(r)))
                    {
                        relayPins[r] = (argValue.toInt() > 0 || argValue == "0" ? argValue.toInt() : -1);
                        break;
                    }
                }
                continue;
            }
            else if(isInputSetup)
            {
                for (r = 0; r < inputcount; r++)
                {
                    if (argName == ("inputtype" + String(r)))
                    {
                        inputTypes[r] += (argValue.toInt() > 0 && argValue.toInt() < 3 ? argValue.toInt() : 0);
                        break;
                    }
                    else if (argName == ("inputsend" + String(r)))
                    {
                        inputTypes[r] += 10 * (argValue.toInt() > 0 && argValue.toInt() < 4 ? argValue.toInt() : 0);
                        break;
                    }
                    else if (argName == ("inputpin" + String(r)))
                    {
                        inputPins[r] = (argValue.toInt() > 0 || argValue == "0" ? argValue.toInt() : -1);
                        break;
                    }
                }
                continue;
            }
            else if(isAnalogSetup)
            {
                for (r = 0; r < analogcount; r++)
                {
                    if (argName == ("apinid" + String(r)))
                    {
                        analogPins[r] = (argValue.toInt() > 0 && argValue.toInt() < 99 ? argValue.toInt() : -1);
                        break;
                    }
                }
                continue;
            }
            else if (argName == "ssid")
                WF_ssidSTA = argValue;
            else if (argName == "ssidpass")
                WF_passwordSTA = argValue;
            else if (argName == "httpuser")
                HTTP_user = argValue;
            else if (argName == "httppass")
                HTTP_password = argValue;
            else if (argName == "ledpin")
                LED_PIN = argValue.toInt();
            else if (argName == "wireupdate" && argValue.toInt() > 1)
                TEMPER_timeout = argValue.toInt();
            else if (argName == "wirepin")
                ONEWIRE_PIN = (argValue.toInt() > 0 || argValue == "0" ? argValue.toInt() : -1);
            else if (argName == "mqttserver")
                MQTT_server = argValue;
            else if (argName == "mqttport")
                MQTT_port = argValue.toInt();
            else if (argName == "mqttuser")
                MQTT_user = argValue;
            else if (argName == "mqttpass")
                MQTT_pass = argValue;
            else if (argName == "mqttclient")
                MQTT_client = argValue;
        }
    }

    eeprom_writeConfig();

    // language=HTML
    String message = "\
Configuration stored successfully.\
<br><i>You must reboot module to apply new configuration!</i>\
<br><br>Wait for 5 sec. or click <a href='/'>this</a> to return to main page.\
";
    HTML_page("Store Setup:", message);
}


void page_switch()
{
    if(!HTTP_auth())
        return;

    int id = httpserver.arg("s").toInt();
    String on = httpserver.arg("on");

    RELAY_switch(id, on == "true");

    httpserver.send(200, "text/html", "OK");
}


void page_reboot()
{
    if(!HTTP_auth())
        return;

    ESP.restart();
}


void page_reset()
{
    if(!HTTP_auth())
        return;

    eeprom_reset(true);
    ESP.restart();
}


void http_setup()
{
    httpserver.onNotFound([]() {
        httpserver.send(404, "text/plain", "FileNotFound");
    });

    httpserver.on("/", HTTP_GET, page_root);
    httpserver.on("/store", HTTP_GET, page_store);
    httpserver.on("/setup", HTTP_GET, page_setup);
    httpserver.on("/mqtt", HTTP_GET, page_mqtt);
    httpserver.on("/relay", HTTP_GET, page_relay);
    httpserver.on("/input", HTTP_GET, page_input);
    httpserver.on("/analog", HTTP_GET, page_analog);
    httpserver.on("/switch", HTTP_GET, page_switch);
    httpserver.on("/reset", HTTP_GET, page_reset);
    httpserver.on("/reboot", HTTP_GET, page_reboot);
    httpserver.on("/firmware", HTTP_GET, page_firmware);
    httpserver.on("/update", HTTP_POST, []() {
        if(!HTTP_auth())
            return;

        httpserver.sendHeader("Connection", "close");
        httpserver.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        if(!HTTP_auth())
            return;

        HTTPUpload& upload = httpserver.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
            Update.begin(UPDATE_SIZE_UNKNOWN);
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
            Update.write(upload.buf, upload.currentSize);
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
            Update.end(true);
        }
    });
    httpserver.begin();
}

void http_handle()
{
    httpserver.handleClient();
}
