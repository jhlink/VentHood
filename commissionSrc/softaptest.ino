#include "application.h"

#pragma SPARK_NO_PREPROCESSOR
#include "Particle.h"
#include "softap_http.h"

#include "index.h"
//#include "html_orig.h"
//#include "script.h"
#include "script_orig.h"
#include "style.h"
#include "jsbn_1.h"
#include "jsbn_2.h"
#include "prng4.h"
#include "rng.h"
#include "rsa.h"

//SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(SEMI_AUTOMATIC);

struct Page
{
    const char* url;
    const char* mime_type;
    const char* data;
};

Page myPages[] = {
     { "/index.html", "text/html", index_html },
     { "/rsa-utils/rsa.js", "application/javascript", rsa_js },
     { "/style.css", "text/css", style_css },
     { "/rsa-utils/rng.js", "application/javascript", rng_js },
     { "/rsa-utils/jsbn_2.js", "application/javascript", jsbn_2_js },
     { "/rsa-utils/jsbn_1.js", "application/javascript", jsbn_1_js },
     { "/script.js", "application/javascript", script_js },
     { "/rsa-utils/prng4.js", "application/javascript", prng4_js },
     { nullptr }
};

void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved)
{
    Serial.printlnf("handling page %s", url);

    if (strcmp(url,"/index")==0) {
        Serial.println("sending redirect");
        Header h("Location: /index.html\r\n");
        cb(cbArg, 0, 301, "text/plain", &h);
        return;
    }

    int8_t idx = 0;
    for (;;idx++) {
        Page& p = myPages[idx];
        if (!p.url) {
            idx = -1;
            break;
        }
        else if (strcmp(url, p.url)==0) {
            break;
        }
    }

    if (idx==-1) {
        cb(cbArg, 0, 404, nullptr, nullptr);
    }
    else {
        cb(cbArg, 0, 200, myPages[idx].mime_type, nullptr);
        result->write(myPages[idx].data);
    }
}

STARTUP(softap_set_application_page_handler(myPage, nullptr));


void setup() {
    Serial.begin(9600);
  //  System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "Photon");
}

void loop() {
  static unsigned long timer = millis();
  if ((millis() - timer) > 5000) {
    WiFi.clearCredentials();
    WiFi.listen();
    timer = millis();
  }
}
