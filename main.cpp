#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "AKDENİZ";
const char* password = "mustafa5855";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WebServer server(80);

// Web Arayüzü (Büyük Kanvas ve Araçlar)
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>OLED Pro Paint</title>";
  html += "<style>body{text-align:center;background:#1a1a1a;color:white;font-family:sans-serif;margin:0;}";
  html += "canvas{background:white; border:3px solid #444; touch-action:none; cursor:crosshair; width:80%; max-width:600px;}";
  html += ".tools{padding:20px; background:#333; margin-bottom:10px;}";
  html += "button{padding:10px 20px; margin:5px; cursor:pointer; font-weight:bold;}";
  html += ".active{background:orange;}</style></head><body>";
  html += "<div class='tools'>";
  html += "Boyut: <input type='range' id='size' min='1' max='10' value='2'>";
  html += "<button id='penBtn' class='active' onclick='setTool(\"pen\")'>KALEM</button>";
  html += "<button id='eraBtn' onclick='setTool(\"era\")'>SİLGİ</button>";
  html += "<button onclick='clearCanvas()'>TEMİZLE</button>";
  html += "<button id='autoBtn' class='active' onclick='toggleAuto()'>OTO-GÖNDER: AÇIK</button>";
  html += "</div><canvas id='c' width='128' height='64'></canvas>";
  
  html += "<script>var c=document.getElementById('c'),x=c.getContext('2d'),drawing=false,auto=true,tool='pen';";
  html += "x.lineCap='round'; x.lineJoin='round';";
  html += "function setTool(t){tool=t; document.getElementById('penBtn').className=t=='pen'?'active':''; document.getElementById('eraBtn').className=t=='era'?'active':'';}";
  html += "function toggleAuto(){auto=!auto; document.getElementById('autoBtn').innerText='OTO-GÖNDER: '+(auto?'AÇIK':'KAPALI'); document.getElementById('autoBtn').className=auto?'active':'';}";
  
  html += "c.onmousedown=function(e){drawing=true; x.beginPath(); x.moveTo(getPos(e).x, getPos(e).y);};";
  html += "c.onmousemove=function(e){if(drawing){";
  html += "var p=getPos(e); x.globalCompositeOperation=tool=='era'?'destination-out':'source-over';";
  html += "x.lineWidth=document.getElementById('size').value; x.lineTo(p.x,p.y); x.stroke(); if(auto)send();}};";
  html += "c.onmouseup=function(){drawing=false;};";
  html += "function getPos(e){var r=c.getBoundingClientRect(); return {x:(e.clientX-r.left)*(128/r.width), y:(e.clientY-r.top)*(64/r.height)};}";
  html += "function clearCanvas(){x.clearRect(0,0,128,64); if(auto)send();}";
  
  html += "function send(){var img=x.getImageData(0,0,128,64).data; var bits='';";
  html += "for(var i=0;i<img.length;i+=4){bits+=(img[i+3]>100?'1':'0');}";
  html += "fetch('/data?b='+bits);}</script></body></html>";
  server.send(200, "text/html", html);
}

void handleData() {
  if (server.hasArg("b")) {
    String bits = server.arg("b");
    display.clearDisplay();
    for (int i = 0; i < bits.length(); i++) {
      if (bits[i] == '1') display.drawPixel(i % 128, i / 128, WHITE);
    }
    display.display();
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  display.setCursor(0,10);
  display.println("BAGLANDI!");
  display.println(WiFi.localIP());
  display.display();

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
