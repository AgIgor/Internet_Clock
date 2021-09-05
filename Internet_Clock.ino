  #include <NTPClient.h>
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include <Wire.h>
  #include <BH1750.h>
  #include <Adafruit_NeoPixel.h>

  Adafruit_NeoPixel pixels (24, 1, NEO_GRB + NEO_KHZ800);

  BH1750 lightMeter;

  int lux,
      luxMax = 10,// MAX PARA AUMENTAR BRILHO
      luxMin = 5;// MIN PARA DIMINUIR BRILHO

  const char *ssid     = "VIVOFIBRA-9501";
  const char *password = "rgw7ucm3GT";


  const long utcOffsetInSeconds =  -10800;
  char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"};

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds,60000);

  int Segundo;
  int
    Minuto,
    newMinuto,
    dezenaM,
    unidadeM;
  int
    Hora,
    dezenaH,
    unidadeH;

  int
    pixelHue,
    brilhoMin   = 5,
    brilhoMax   = 255;

  bool lux_flag;
  bool LED;

  int long pontoMillis;

  int contSeg = 0,
      contMin = 0,
      contHor = 0;

  byte displayConfig[12][7]= {{0,0,1,2,4,5,6},  //Digito 0
                              {0,0,0,0,0,0,4},  //Digito 1
                              {0,0,0,1,3,5,6},  //Digito 2
                              {0,0,0,1,3,4,5},  //Digito 3
                              {0,0,0,0,2,3,4},  //Digito 4
                              {1,1,1,2,3,4,5},  //Digito 5
                              {1,1,2,3,4,5,6},  //Digito 6
                              {0,0,0,0,0,1,4},  //Digito 7
                              {0,1,2,3,4,5,6},  //Digito 8
                              {0,0,1,2,3,4,5},  //Digito 9
                              {0,0,0,0,1,2,3},  //Grau
                              {0,0,0,0,0,3,6}}; //Umidade;

void setup(){
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pixels.begin();
  pixels.setBrightness(brilhoMax);
  pixels.clear();

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    int valLed;
    if(valLed > 24){
     valLed = 0;
     pixels.clear();
    }else{
      valLed++;
    }
    pixels.setPixelColor(valLed, pixels.Color(255, 0, 0));
    pixels.show();
  }
  pixels.clear();
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Wire.pins(0, 2);
  Wire.begin(0, 2);
  lightMeter.begin();
  timeClient.begin();
  luxRead();
  ntpClock();
  delay(50);
}//END SETUP
//===============================================//
void loop() {
    if(millis() - pontoMillis >= 1000){
      pontoMillis = millis();
      piscaPonto();
    }
    ntpClock();
    delay(1);


 /*   pontoMillis = millis();
      contSeg ++;
      piscaPonto();
      luxRead();

      if(contSeg >= 60){
        contSeg = 0;
        contMin ++;
        ntpClock();

        if(contMin >= 60){
          contMin = 0;
          contHor ++;
            if(contHor >= 24){
              contHor = 0;
              ESP.restart();
            }
          }
        }
    }
    display();*/

//===============================================//
}//END LOOP
void ntpClock(){
    timeClient.update();
        Hora = (timeClient.getHours());
        Minuto = (timeClient.getMinutes());
        Segundo = (timeClient.getSeconds());
        if(newMinuto != Minuto){
          newMinuto = Minuto;
          if(Hora >= 23 and Segundo >= 59){
            ESP.restart();
          }
          trataDigitos();
          luxRead();
          display();
        }//END NEW MINUTO
}//END NTP CLOCK
//===============================================//
void luxRead(){
    lux = lightMeter.readLightLevel();
    if(lux >= luxMax){
      pixels.setBrightness(brilhoMax);
      //pixelHue = millis();
      lux_flag = true;
    }//END IF LUX MAX
//===============================================//
    if(lux <= luxMin){
      pixels.setBrightness(brilhoMin);
      //pixelHue = 0;
      lux_flag = false;
    }//END IF LUX MIN
    if(lux_flag){
      pixelHue = millis();
    }else{
      pixelHue = 0;
    }//END ELSE LUX FLAG
//===============================================//
}//END LUX READ

void piscaPonto(){
  LED = !LED;
    if(LED){
      pixels.setPixelColor(14, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      pixels.show();
    }else{
      pixels.setPixelColor(14, pixels.Color(0, 0, 0));
      pixels.show();
    }
}//END PISCA PONTOS
void trataDigitos(){
//=========================//Hora//=========================//
    if(Hora >=13){Hora = Hora -=12;}
    dezenaH = Hora;
    unidadeH = dezenaH;
    dezenaH = dezenaH/10;
    unidadeH = unidadeH % 10;
//=========================//Minuto//=========================//
    dezenaM = Minuto;
    unidadeM = dezenaM;
    dezenaM = dezenaM/10;
    unidadeM = unidadeM % 10;
    pixels.clear();
    display();
}//END TRATA DIGITOS

void display(){
    for (int ID = 0; ID < 7; ID++){
      pixels.setPixelColor((displayConfig[unidadeM][ID]), pixels.gamma32(pixels.ColorHSV(pixelHue))); //LEDS UNIDADE DE Minuto
      pixels.setPixelColor((displayConfig[dezenaM][ID]+7), pixels.gamma32(pixels.ColorHSV(pixelHue)));//LEDS DEZENA DE Minuto
      pixels.setPixelColor((displayConfig[unidadeH][ID]+15), pixels.gamma32(pixels.ColorHSV(pixelHue)));//LEDS UNIDADE DE Hora
      if(dezenaH > 0){
        pixels.setPixelColor((displayConfig[dezenaH][ID]+22), pixels.gamma32(pixels.ColorHSV(pixelHue))); //LEDS DEZENA DE Hora
      }else{
        pixels.setPixelColor((displayConfig[dezenaH][ID]+22), pixels.Color(0, 0, 0)); //LEDS DEZENA DE Hora
      }
      pixels.show();
    }
}//END VOID DISPLAY
