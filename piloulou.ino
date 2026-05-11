/************
piloulou.ino
Lit la valeur d'un capteur d'humidité et fait sonner un buzzer
Seuil réglable.
************/

/************
PENSE BETE
tone(pin, frequencey, duration)
tone(pin, frequencey)
 ************/

/************
   LIBRARIES
 ************/
#include <Deuligne.h>
//#include  <Chrono.h>

/************
   PIN WIRING
 ************/
#define BTN_PLUS 12   // pin digital input button plus : need to set internal pullup resistor
#define BTN_MOINS 11  // pin digital input button plus : need to set internal pullup resistor
#define BUZZER 5      // pin digital Buzzer tone
#define LED_PIN 13    // pin built-in led 
#define SENSOR_PIN A0
#define SENSOR_LIMIT 970

/******************
  GLOBAL VARIABLES
 ******************/
int consigne = 0;     // consigne to compare
int btnp;             //bouton plus
int btnm;             //bouton moins
int sensor_value = 0; //sensor value read from ADC [0 ; 1024]
int sensor_limit = SENSOR_LIMIT; //sensor limit (may be modified during execution with buttons)
unsigned long chrono;

/******************
  LIBRARIES INSTANCES
******************/
Deuligne lcd;

/******************
  SETUP
******************/
void setup() {
  pinMode(BTN_PLUS, INPUT);           // set BTN_PLUS to input
  digitalWrite(BTN_PLUS, HIGH);       // turn on pullup resistors on BTN_PLUS
  pinMode(BTN_MOINS, INPUT);          // set BTN_MOINS to input
  digitalWrite(BTN_MOINS, HIGH);      // turn on pullup resistors on BTN_MOINS
  pinMode(LED_PIN, OUTPUT);           // set LED to input
  lcd.init();
  lcd.setCursor(0, 0);
  print_message("démarrage");
}

void buzzer_on(){
  tone(BUZZER, 700);
  digitalWrite(LED_PIN, HIGH);
}
void buzzer_off(){
  noTone(BUZZER);
  digitalWrite(LED_PIN, LOW);
}

/* print actual sensor reading and threshold limit */
void print_standard (){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("seuil");  
  lcd.setCursor(0, 1);
  lcd.print(sensor_limit);
  lcd.setCursor(8, 0);
  lcd.print("capteur");  
  lcd.setCursor(8, 1);
  lcd.print(sensor_value);
}

/* display message for 2 seconds */
void print_message (char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000);
}

void alarm() {
  buzzer_on();
  print_standard ();
  while(1){
    if (bouton_moins() || bouton_plus()) {
      buzzer_off();  
    }
  }
}

/* return true if bouton+ is pressed */
bool bouton_plus(){ return (digitalRead(BTN_PLUS) == LOW); }

/* return true if bouton- is pressed */
bool bouton_moins(){ return (digitalRead(BTN_MOINS) == LOW); }

/* return true if chrono has elapsed. */
bool reveil(int duration){
  if (millis() > (chrono + duration)) { return(true); }
  return (false);
}

/* recheck if both button are still pressed in 3 seconds
if yes go to special_function 
if not return to main loop 
*/
void confirm_special_function() {
  print_message("confirmer?");
  if (bouton_moins() && bouton_plus()) {
    special_function();
  }
  else {
    return(0);
  }
}

/*
Réglage du seuil avec boutons +/- par pas de 10.
retour à la boucle principale au bout de 2 secondes sans appui bouton.
*/
void special_function(){
  print_message("debut réglage");
  chrono=millis();
  while (1) {
    delay(100); // for debounce
    print_standard();
    if (bouton_plus()) {
      sensor_limit = sensor_limit - 10;
      chrono=millis();
    }
    if (bouton_moins()) {
      sensor_limit = sensor_limit + 10;
      chrono=millis();
    }
    if (reveil(2000)) {
      print_message("fin réglage");
      return(0);
    }
  }
}  

/**********
MAIN LOOP
Read sensor value
Display sensor value
raise alarm if threshold exceeded
go to special function if both button pressed
*/
void loop() {
  print_standard ();
  delay(200); // for debounce
  sensor_value = analogRead(SENSOR_PIN);
  if (sensor_value < sensor_limit){ alarm(); }
  if (bouton_moins() && bouton_plus()) { confirm_special_function(); } // go to special function 
}

