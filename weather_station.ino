#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 4     // digital pin connected to the DHT sensor for 1-wire connection
#define DHTTYPE DHT11 // type of used DHT sensor

void set_led(float temp, float humid);
void create_special_chars();
String BT_conn_and_print();
void put_data_to_display(float temp, float humid, String print_bt_data, int *LCD_state1, int *prev_lcd_state1, float *prev_temp1, float *prev_humid1, String *prev_print_bt_data1);


// Declaration insance of LiquidCrystal_I2C Class for LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2); // define size -> 16 segments, 2 rows

// Declaration insance of DHT Class for DHT11 sensor (temperature and humidity)
DHT dht(DHTPIN, DHTTYPE);

// Create an custon char to display - degree sign
// Use thisd page to preare correct sign -> https://maxpromer.github.io/LCD-Character-Creator/
byte degree[] = {
  B00010,
  B00101,
  B00010,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
/*  
byte hearth_empty[] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000
};
  
byte hearth_filled[] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};
*/

// Variables declaration
String blt_data;
String print_data;
String bt_data_to_print;
int lcd_state;
int prev_lcd_state;
float prev_temp;
float prev_humid;
String prev_print_bt_data;

// Mandatory function in Artuino language
void setup() {
  // put your setup code here, to run once:
  
  // *********************** DISPLAY SETTINGS *************************
  // Initializing communication with LCD display
  lcd.begin();
  // ********************* DHT SENSOR SETTINGS ************************
  // Initializing communication with DHT sensor
  dht.begin();
  // ********************* BLUETOOTH SETTINGS ************************
  // Initializing communication with bluetooth module - baudrate: 9600
  Serial.begin(9600);

  // Setup pins 6, 7 and 8 on Arduino board as digital outputs;
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

// Mandatory function in Artuino language
void loop() {
  // put your main code here, to run repeatedly:

  // Wait a 0,5 second between measurements
  delay(500);

  // Prepare all variables for measurements
  // Read humidity measurement from sensor
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default) from sensor
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    // define position of cursor -> first row and first column
    lcd.setCursor(0, 0); 
    // print message on display about failed
    lcd.print("Failed to read"); 
    // define position of cursor -> second row and first column
    lcd.setCursor(1, 0);
    lcd.print("from DHT sensor!");
    return;
  }

  // Calling a functions
  create_special_chars();
  set_led(t, h);
  bt_data_to_print = BT_conn_and_print();
  put_data_to_display(t, h, bt_data_to_print, lcd_state, prev_lcd_state, prev_temp, prev_humid, prev_print_bt_data);
}

void set_led(float temp, float humid)
{
  // Logic to show actual state of temperature on LED diodes
  if((temp >= 20 && temp <= 26) && (humid >= 70 && humid <= 85))
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
  else if(temp == 19 || temp == 27)
  {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }
  else if((humid >= 60 && humid < 70) || (humid > 85 && humid <= 90))
  {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }
  else
  {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
  }
}

void create_special_chars()
{
  // Create prepared before custon character - degree sign
  lcd.createChar(0, degree);
  //lcd.createChar(1, hearth_empty);
  //lcd.createChar(2, hearth_filled);
}

void put_data_to_display(float temp, float humid, String print_bt_data, int &LCD_state1, int &prev_lcd_state1, float &prev_temp1, float &prev_humid1, String &prev_print_bt_data1)
{
  if (print_bt_data == "Temperature" || print_bt_data == "Temp")
  {
    LCD_state1 = 0;
  }
  else if (print_bt_data == "BT" || print_bt_data == "Bluetooth")
  {
    LCD_state1 = 1;
  }
  
  if (LCD_state1 == 0)
  {
    // Clear display first
    if (prev_lcd_state1 != LCD_state1 || prev_temp1 != temp || prev_humid1 != humid)
    {
      lcd.clear();
    }
    // Put data on LCD display
    lcd.setCursor(0, 0); 
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.write(0); // -> use degree sign
    lcd.print("C");
  
    lcd.setCursor(0, 1);
    lcd.print("Humid: ");
    lcd.print(humid);
    lcd.print("%");

    prev_lcd_state1 = LCD_state1;
    prev_temp1 = temp;
    prev_humid1 = humid;
  }
  else
  {
    // Clear display first
    if (prev_lcd_state1 != LCD_state1 || prev_print_bt_data1 != print_bt_data)
    {
      lcd.clear();
    }
    // Set obtained data via BT to LCD display
    lcd.setCursor(0, 0);
    lcd.print("BT: ");
    lcd.print(print_data);

    prev_lcd_state1 = LCD_state1;
    prev_print_bt_data1 = print_bt_data;
  }
}

String BT_conn_and_print()
{
  // Connection over bluetooth
  while(Serial.available())
  {
    delay(50);
    char c = Serial.read();
    blt_data += c;
  }
  if(blt_data.length() > 0)
  {
    Serial.println(blt_data);
    print_data = blt_data;
    blt_data = "";
  }
  return print_data;
}
