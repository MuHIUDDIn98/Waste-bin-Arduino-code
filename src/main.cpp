#include <Keypad_I2C.h>             //keypad I2C library include 
#include <LiquidCrystal_I2C.h>      //lcd library include 
#include <HX711.h>                  //load cell module include 
#include <Wire.h>                   // for I2c
//#include <string>
using namespace std;

#define CALIBRATION_FACTOR 438.86       //calibaration factor using 
#define lcd_addr 0x27     
#define keypad_addr 0x20
#define numberCursorInitialPosition 4


char key;
char inputChar;
String inputString = "";
String Entered_NUm = "";
int set = numberCursorInitialPosition;
float WeightDisplay = 0;
String IRinfo ="";
int height = -1;           //initializing Height for storing height of object
double weight = -1;        //initializing weight for storing weight of object


bool flip_right_state = false;
bool flip_left_state = false;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = 11;

//IR sensor value store and ir sensor pin defination for pinMode setup
int IRsensorValue[4] = {0,0,0,0};
int IRpins[4] = {2,3,4,5};

//Motordriver pin defin
uint8_t MotorOutputValue[4] = {LOW,LOW,LOW,LOW};
int MotorPins[4] = {6,7,8,9};

//Keypad variables
bool disableNumInput = false;
const byte ROWS = 4; 
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1','4','7','*'},
    {'2','5','8','0'},
    {'3','6','9','#'},
    {'A','B','C','D'}
};

uint8_t empty[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//Switchpins number
uint8_t SwitchPinsValue[2]={1,1};
uint8_t SwitchPins[2] ={12,13};

//Keypad pins connected to the I2C-Expander pins P0-P6
byte rowPins[ROWS] = {0, 1, 2, 3};          // connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7};          // connect to the column pinouts of the keypad


//HX711 I2C load cell object
HX711 scale;

//LCD I2C object
LiquidCrystal_I2C lcd(lcd_addr,26,4);

//Keypad I2C object
Keypad_I2C I2C_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, keypad_addr, PCF8574 );


bool is_valid_number(String entered_number);
bool is_plastic();

void display_message(int col_num[4], int row_num[4], String message[4], bool lcd_clear_flag, int lines);
int IRarrayinfo();
void initialMsg();
void processing_msg();
void weight_height_display();
bool weight_reset();

bool flip_Right();
bool flip_Left();
bool flip_Stop();

bool Slider_Open();
bool Slider_Close();
bool Slider_Stop();





void setup() {
    Serial.begin(9600);
    I2C_Keypad.begin();
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    //setting up arduino digital  pins  2,3,4,5 for IR sensors  //debugmessage
    for(int i=0; i<4; i++){
        pinMode(IRpins[i], INPUT); 
    }

    //setting up arduino digital pins 6,7,8,9 for motorDriver control //debug message
    for(int i=0; i<4; i++){
        pinMode(MotorPins[i], OUTPUT); 
    }

    //setting up switch pins 12, 13 as INPUT_PULLUP  //debug message
    for(int i=0; i<2; i++){
        pinMode(SwitchPins[i], INPUT_PULLUP); 
    }

    lcd.init();
    lcd.begin(20,4);
    lcd.backlight();
    I2C_Keypad.begin();
    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare();    
    lcd.clear();
    lcd.createChar(0,empty);
    initialMsg();          
} 

void loop() {
    
    key = I2C_Keypad.getKey();
    inputChar = key;
    
    Serial.println("Weight ");
    Serial.println(weight); 

    if(key >= 48 && key <= 57){  
        
        if(inputString.length() < 11){
            if(set == numberCursorInitialPosition) {
                initialMsg();
            }
            lcd.setCursor(set,3);
            lcd.write(key);
            inputString += (char)inputChar;
            set++;
        }   

    } else if(key =='#'){
        
        Entered_NUm = inputString;
          weight_reset();
        if(is_valid_number(Entered_NUm) ){

            //to-do step 3 : calling server to get user data
            
            //to-do step 4 : displaying user information 

            //step 5 : handling new bottle input 
                        
            //Serial.println("before slider open call");  //debug message

            bool is_slider_open = Slider_Open();
             //Serial.println("slider open  value " + String(is_slider_open));  //debug message
            if(is_slider_open) {
                int col_arr[2] = {0, 0};
                int row_arr[2] = {1, 2};
                String message[2] = {"Put bottle inside.." , "within 14 second !"};
                display_message(col_arr, row_arr, message, true, 2);

                double temp_weight = -1;   //initializing temporary weight 
                String countdown_time = "";

                long slider_close_previous_time = millis();

                while(millis() - slider_close_previous_time < 15000) {  

                //count down logic displaying  in LCD screen (format second)
                    long time_difference = millis() - slider_close_previous_time;

                    if(time_difference >= 0 && time_difference <= 1000){
                      countdown_time = "14";
                    }
                    else if(time_difference >= 1001 && time_difference <= 2000){
                      countdown_time = "13";
                    }
                    else if(time_difference >= 2001 && time_difference <= 3000){
                      countdown_time = "12";
                    }
                    else if(time_difference >= 3001 && time_difference <= 4000){
                      countdown_time = "11";
                    } 
                    else if(time_difference >= 4001 && time_difference <= 5000){
                      countdown_time = "10";
                    } 
                    else if(time_difference >= 5001 && time_difference <= 6000){
                      countdown_time = "09";
                    } 
                    else if(time_difference >= 6001 && time_difference <= 7000){
                      countdown_time = "08";
                    } 
                    else if(time_difference >= 7001 && time_difference <= 8000){
                      countdown_time = "07";
                    }
                    else if(time_difference >= 8001 && time_difference <= 9000){
                      countdown_time = "06";
                    } 
                    else if(time_difference >= 9001 && time_difference <= 10000){
                      countdown_time = "05";
                    } 
                    else if(time_difference >= 10001 && time_difference <= 11000){
                      countdown_time = "04";
                    } 
                    else if(time_difference >= 11001 && time_difference <= 12000){
                      countdown_time = "03";
                    } 
                    else if(time_difference >= 12001 && time_difference <= 13000){
                      countdown_time = "02";
                    } 
                    else if(time_difference >= 13001 && time_difference <= 14000){
                      countdown_time = "01";
                    } 
                    else if(time_difference >= 14001 && time_difference <= 15000){
                      countdown_time = "00";
                    }

                    int col_arr[1] = {17};
                    int row_arr[1] = {0};
                    String message[1] = {countdown_time + "s"};
                    display_message(col_arr, row_arr, message, false, 1);


                    int ir_value = IRarrayinfo();
                    
                    if(scale.is_ready()){
                        //Serial.println("Inside scale is ready"); //debug message 
                        temp_weight = round(scale.get_units(5));
                       }
                    //Serial.println(" temp weight :"+ String(temp_weight)); //debug message 
                    if(ir_value > -1 && temp_weight > 4){
                        height = ir_value;
                        weight = temp_weight;
                        //Serial.println("height & weight " + String(height) +" "+ String(weight)); //debug message 
                        break;
                    }

                }
                 
                //door closing delay after object detection
                delay(2000);
                Slider_Close();
                delay(1000);
                processing_msg();
                weight_height_display();
                scale.tare();


                //selecting plastic or waste product
                
                
                if(is_plastic()){
                   flip_right_state = flip_Right();
                    // if(flip_right_state){
                    //     flip_left_state = flip_Left();
                    // }

                }
                if(!is_plastic()){
                   flip_left_state = flip_Left();
                    // if(flip_left_state){
                    //     flip_right_state = flip_Right();
                    // }

                }            
             weight_reset();
            }
            
            
        }
         else {
            long previous_time = millis();

            while(millis() - previous_time <3000){
                
                int col_arr[1] = {0};
                int row_arr[1] = {3};
                String message[1] = {"Invalid number!!"};
                display_message(col_arr, row_arr, message, false, 1);
            }
            
            //resetting cursor
            set = numberCursorInitialPosition; 
            inputString="";
            lcd.clear();
            initialMsg();
            weight_reset();

        }
    }

    switch (key) {
        case 'A':
            inputString="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Weight: ");
            if(scale.is_ready())
                weight = round(scale.get_units(5));
            lcd.setCursor(8,0);
            lcd.print(weight);
            flip_Right();
            break;

        case 'B':
            inputString="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("IR Reading: ");
            lcd.setCursor(12,0);
            lcd.print(IRarrayinfo());
            flip_Left();
            break;

        case 'C':
            scale.tare();
            if(scale.is_ready())
                weight = round(scale.get_units(5));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Weight Reset: ");
            lcd.setCursor(14,0);
            lcd.print(weight);
            
            break;

        case 'D':
            initialMsg();
            inputString="";
            set=numberCursorInitialPosition;
            

            break;

        case '*':
            if(inputString.length() != 0){
                inputString.remove(inputString.length());
                if(set > numberCursorInitialPosition){
                    set--;
                    lcd.setCursor(set,3);
                    lcd.write(byte(0));
                }

            }
            break;               
        
    }   

}

int IRarrayinfo(){
    for(int i =0;i<4;i++ ){
        IRsensorValue[i]= digitalRead(IRpins[i]); //updating array 
    }

    if(IRsensorValue[3] == 0 && IRsensorValue[2] == 0 && IRsensorValue[1] == 0 && IRsensorValue[0]== 0)
        return 4;
    else if(IRsensorValue[3] == 0 && IRsensorValue[2] == 0 && IRsensorValue[1] == 0 && IRsensorValue[0] == 1 )
        return 3;
    else if(IRsensorValue[3] == 0 && IRsensorValue[2] == 0 && IRsensorValue[1] == 1 && IRsensorValue[0] == 1)
        return 2;
    else if(IRsensorValue[3] == 0 && IRsensorValue[2] == 1 && IRsensorValue[1] == 1 && IRsensorValue[0] == 1)
        return 1;
    else if(IRsensorValue[3] == 1 && IRsensorValue[2] == 1 && IRsensorValue[1] == 1 && IRsensorValue[0] == 1)
        return 0;
    else
        return -1;
}


void initialMsg(){
    int col_arr[3] = {0, 1, 1};
    int row_arr[3] = {0, 1, 2};
    String message[3] = {"__Save YOUR Planet__", "Put YOUR Phone", "Number to start.."};
    display_message(col_arr, row_arr, message, true, 3);

}


bool flip_Right(){ 
    digitalWrite(MotorPins[0],HIGH);
    digitalWrite(MotorPins[1],LOW);
    delay(1000);
    while(!digitalRead(SwitchPins[1])){}
    delay(100);    
    while(digitalRead(SwitchPins[1])){} 
    return flip_Stop();
}

bool flip_Left(){
    digitalWrite(MotorPins[0],LOW);
    digitalWrite(MotorPins[1],HIGH);
    delay(1000);
    while(!digitalRead(SwitchPins[1])){}
    return flip_Stop();
}

bool flip_Stop(){
    digitalWrite(MotorPins[0],LOW);
    digitalWrite(MotorPins[1],LOW);
    delay(1000);
    int col_arr[1] = {1};
    int row_arr[1] = {1};
    String message[1] = {"flipper stopped"};
    display_message(col_arr, row_arr, message, true, 1);
    return true;
}



bool Slider_Open(){
     //Serial.println("inside slider open call"); //debug message
    digitalWrite(MotorPins[2], LOW);
    digitalWrite(MotorPins[3], HIGH);
    delay(1000);
    
    while(digitalRead(SwitchPins[0])){
        //Serial.println("inside slider open while"); //debug message
        int col_arr[1] = {0};
        int row_arr[1] = {2};
        String message[1] = {"Slider Opening...!!"};
        display_message(col_arr, row_arr, message, true, 1);        
       
    }
        
    return Slider_Stop();
}

bool Slider_Close(){
    Serial.println("Inside slider closing ");
    digitalWrite(MotorPins[2],HIGH);
    digitalWrite(MotorPins[3],LOW);
    delay(1000);
    while(digitalRead(SwitchPins[0])){
    int col_arr[2] = {1, 1};
    int row_arr[2] = {0, 1};
    String message[2] = {" Warnning!!!!", "Slider Closing...!!"};
    display_message(col_arr, row_arr, message, true, 2);
    }
    return Slider_Stop();
}

bool Slider_Stop(){
    //Serial.println("inside slider stop"); //debug message 
    digitalWrite(MotorPins[2],LOW);
    digitalWrite(MotorPins[3],LOW);
    delay(1000);
    int col_arr[1] = {1};
    int row_arr[1] = {1};
    String message[1] = {"Slider stopped"};
    display_message(col_arr, row_arr, message, true, 1);

    return true;
}

void display_message(int col_num[4], int row_num[4], String message[4], bool lcd_clear_flag, int lines){

    if(lcd_clear_flag){
        lcd.clear();
    }

    for(int i=0; i<lines; i++) {
        lcd.setCursor(col_num[i], row_num[i]);
        lcd.print(message[i]);

    }
    
    
}

bool is_plastic(){
    if(weight>1 && weight < 20 && height != -1){
        return true;
    }
return false;
}



bool is_valid_number(String entered_number){

    if(entered_number.length() == 11 ){

        String first_three_digits = entered_number.substring(0, 3);

        if(first_three_digits == "013" || first_three_digits == "014" ||
           first_three_digits == "015" || first_three_digits == "016" ||
           first_three_digits == "017" || first_three_digits == "018" ||
           first_three_digits == "019") {

            return true;
        }       
    } 

    return false;
}

void processing_msg(){
    int col_arr[1] = {0};
    int row_arr[1] = {1};
    String message[1] = {"processing..."};
    display_message(col_arr, row_arr, message, true, 1);
}
  
void weight_height_display(){
    int col_arr[2] = {0, 0};
    int row_arr[2] = {1, 2};
    String message[2] = {"Height :" + String(height) , "weight :" + String(weight)};
    display_message(col_arr, row_arr, message, true, 2);

}

bool weight_reset(){
    scale.tare();
    if(scale.is_ready())
        weight = round(scale.get_units(5));
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Weight Reset: ");
    lcd.setCursor(14,0);
    lcd.print(weight);

return true;
}
    