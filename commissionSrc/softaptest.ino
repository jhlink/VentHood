#include "application.h"

#pragma SPARK_NO_PREPROCESSOR
#include "Particle.h"
#include "softap_http.h"
#include "softiePage/webpage.h"
#include "SparkJson/SparkJson.h"

#define MAJOR 0
#define MINOR 3
#define PATCH 3

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

    if (strcmp(url, "/cfg") ==0) {
      char* data = body->fetch_as_string();
      // We'll need to use SparkJSON for this.
      //{"idx":0,"lightNm":"asdfasdf","fanNm":"dfasdfasd","email":"asdfasdfasdf"}
      Serial.print("POST Data: ");
      Serial.println(data);
      // free the data! IMPORTANT!
      free(data);
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

StaticJsonBuffer<512> jsonBuffer;
JsonObject& data = jsonBuffer.createObject();

void setup() {
    Serial.begin(9600);
  //  System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "Photon");
}

void wifiReset() {
  static unsigned long timer = millis();
  if ((millis() - timer) > 5000) {
    WiFi.clearCredentials();
    WiFi.listen();
    timer = millis();
  }
}

void testPublish() {
  char buffer[512];
  static unsigned long timer = millis();
  if ((millis() - timer) > 30000) {
    data["amznEmail"] = "blamg";
    data["fanDevName"] = "whatdu";
    data["lightDevName"] = "seodo";
    data.printTo(buffer, sizeof(buffer));
    Serial.println(data);
    timer = millis();
  }
}

void loop() {

}
