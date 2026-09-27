#include <WiFi.h>
#include <BluetoothSerial.h>


char menu = '0'; //variable para menu

BluetoothSerial BT;     //objeto de bluetooth

void MostrarMenu();

void setup() {
  Serial.begin(115200);   //puerto serie

  //Config Wifi
  WiFi.mode(WIFI_STA); //modo estación, puede conectarse a otras redes
  WiFi.disconnect(); //por si está conectado
  //Config BT
  BT.begin("EscanerBT");
  //Comienza el menu
  delay(500);
  Serial.println("Listo para conectarse...");
  MostrarMenu();
}

void loop() {
//MENU
  if (Serial.available()>0){          //si se encuentra disponible el puerto serie
    char a = Serial.read();           //realizo la lectura y evaluo
    switch (a){ 
      case '1':{
      // MODO SELECCIONADO: WIFI
      int num = WiFi.scanNetworks();  //obtengo el numero de redes disponibles
      if (num == 0) Serial.println("No se encontraron redes.");
      if (num){
        Serial.print("Se encontraron ");
        Serial.print(num);                        //numero de redes disponibles en pantalla
        Serial.println(" redes disponibles:");
        int i = 0;
        while (i < num){                          //bucle hasta el fin de las redes detectadas
          Serial.print(i+1);
          Serial.print(". Nombre|SSID: ");
          Serial.print(WiFi.SSID(i));                     //muestro el nombre de la red
          Serial.print(" - Señal|RSSI: ");
          Serial.print(WiFi.RSSI(i));                     //muestro el nivel de la señal de la red
          /*hago el calculo de la frecuencia a la que se está enviando la informacion.
          El ancho de banda de los esp32 (al menos el modelo utilizado), va desde los 2.412GHz a los 2.484GHz, separado entre 14 canales con 5MHz de diferencia
          por lo tanto, debemos usar una ecuacion para determinar la frecuencia de la transmision de informacion. Con ello ademas, considerando que
          la velocidad de propagacion de la onda es la de la velocidad de la luz, podemos determinar de manera sencilla la longitud de onda de la misma.
          */
          int canal = WiFi.channel(i);
          float frecMH, frecGH, wave_lcm;
          if (canal == 14) {
              frecMH = 2484.0;
            } else {
              frecMH = 2407.0 + (5.0 * canal);
            }
          wave_lcm = 30000.0 / frecMH;
          frecGH = frecMH/1000.0;
          Serial.print(" dBm - Frecuencia: ");
          Serial.print(frecGH,3);
          Serial.print(" GHz - Long. onda: ");
          Serial.print(wave_lcm,3);
          Serial.println(" cm");
          delay(50);
          i++;
        }
      }
      Serial.println("---------ESCANEO FINALIZADO---------");
      MostrarMenu();
      break;
      }
      case '2':{
        int num_bt, j=0;
        Serial.println("Inicio de escaneo Bluetooth, este proceso puede demorar 10 segundos...");   
        BTScanResults* dispo = BT.discover(10000);                                                  //busca los dispositivos disponibles
        num_bt = dispo -> getCount();
        if (num_bt == 0) Serial.println("No se encontraron redes.");
        if (num_bt){
          Serial.print("Se han encontrado ");
          Serial.print(num_bt);
          Serial.println(" dispositivos disponibles:");
          while(j<num_bt){
            BTAdvertisedDevice* res = dispo ->getDevice(j);
            Serial.print(j+1);
            Serial.print(". Nombre|MAC: ");
            if (res -> haveName()){
              Serial.print(res->getName().c_str());
            }else{
              Serial.print(res->getAddress().toString().c_str());
            }
            Serial.print(" - Señal|RSSI: ");
            Serial.print(res->getRSSI());
            Serial.println(" dBm");
            j++;
          }
        }
        //dispo -> clear();
        Serial.println("---------ESCANEO FINALIZADO---------");
        MostrarMenu();
      break;
      }
    }
  }
}

void MostrarMenu(){
  Serial.println("------------>MENU<------------");
  Serial.println("Escriba la opcion deseada para escanear:");
  Serial.println("1. Escanear WiFi");                             //Pantalla menú
  Serial.println("2. Escanear Bluetooth");
  Serial.println("3. Mantener el menu");
}