
//https://www.unixtimestamp.com/index.php para obtener fecha unix y settear el RTC (Real Time Clock) de Arduino
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>
File myFile;

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
int permiso=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//abertura de puerto serie y fija velocidad en baudios de transmision de datos
  while (!Serial);
  pinMode(13, OUTPUT);
  //sync de RTC de arduino con tiempo por usuario
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
 if (!SD.begin(4)) {//cuando apertura de tarjeta SD falla lo indica
      Serial.println("initialization failed!");
      while (1);
  }
  Serial.println("initialization done.");//apertura de microsd exitosa
  //myFile = SD.open("lecturas.txt", FILE_WRITE);
  }
  
void loop() {
  while (timeStatus()== timeNotSet){
    //codigo UNIX de sync es ingresado y aceptado por arduino 
  if (Serial.available()) {
    processSyncMessage();
  }
  //leds de muestran cuando sync
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }
  delay(1000);
  }
  //digitalClockDisplay(); 
  // put your main code here, to run repeatedly:
  if (!myFile){//si el archivo no ha sido abierto, se abre, de lo contario, se mantiene abierto
    myFile = SD.open("lecturas.txt", FILE_WRITE);//apertura de SD
   }
  
  int lectura=analogRead(A0);//entrada de lecturas ejecutadas por el sensor

  Serial.print("La lectura es: ");
  Serial.println(lectura);
//indicaciones del estado de humedad en el monitor serial
  if (lectura>=730){//condicionales dependiento de lectura por medio de rangos ya predeterminados
    Serial.println("El suelo esta demasiado seco");
  }else if (lectura<730 && lectura>=570){
    Serial.println("El suelo esta seco");
  }else if (lectura<570 && lectura>=451){
    Serial.println("El suelo esta humedo");
  }else if (lectura<450){
    Serial.println("Esta literalmente en agua");
  }
  
  //porcentajes
  int lecturaporcentaje=map(lectura, 800,430,0,100);//muestra de porcentaje de humedad
  Serial.print("La humedad es de: ");
  Serial.print(lecturaporcentaje);
  Serial.println("%");
  String fecha="";
  Serial.println("Data a memoria SD: "+String(lectura)+"|"+printDigits(day())+"/"+printDigits(month())+"/"+String(year())+"|"+printDigits(hour())+":"+printDigits(minute())+":"+printDigits(second()));
  //escritura de lectura en archivo .txt
  if (myFile) {
      Serial.print("Writing to lecturas.txt...");
      //Serial.print("Cosa: "+String(lectura));
      myFile.println(String(lectura)+"|"+printDigits(day())+"/"+printDigits(month())+"/"+String(year())+"|"+printDigits(hour())+":"+printDigits(minute())+":"+printDigits(second()));
    // close the file:
      myFile.close();//clausura de archivo
      Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening lecturas.txt");
  }
  delay(5000);
}

/*void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}*/

String printDigits(int digits){//cuando la fecha, hora, dia, etc es de un solo digito, funcion retorna un cero
  // utility function for digital clock display: prints preceding colon and leading 0
  //Serial.print(":");
  if(digits < 10){
    return ('0'+String(digits));
  //Serial.print(digits);
  }else{
    return (String(digits));
    }
}
//verificador que la fecha sea valida, mayor que enero del 2013
void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
