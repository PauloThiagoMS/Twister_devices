#include <Stepper.h>

const int stepsPerRevolution = 65;
const int speed_steps = 150;
Stepper myStepper(stepsPerRevolution, 8,10,9,11);

const int max_stages = 26;
const int stages = 3;
const int end_curssor_pin = 3;
const int l_buttom_pin = 4;
const int r_buttom_pin = 5;

int end_curssor = 0;

int timeout_max = max_stages;
int steps_per_stage = max_stages / stages;
int current_stage = 0;
int setupx = 0;

bool l_buttom = false;
bool r_buttom = false;

void setup(){
    myStepper.setSpeed(speed_steps);
    pinMode(end_curssor_pin, INPUT_PULLUP);
    pinMode(r_buttom_pin, INPUT_PULLUP);
    pinMode(l_buttom_pin, INPUT_PULLUP);
    Serial.begin(9600);
}

int find_0(){
    end_curssor = digitalRead(end_curssor_pin);
    int timeout = timeout_max;
    while ((end_curssor == HIGH) and (timeout > 0)){
        myStepper.step(-stepsPerRevolution);
        end_curssor = digitalRead(end_curssor_pin);
        timeout --;
    }
    if (timeout == 0){
        return 1;
    }
    return 0;
}

int goto_stage(int targ_stage){
    if ((targ_stage > stages) or (targ_stage < 0)){
        return 101;
    }
    if(targ_stage == 0){
        find_0();
    }else{
        myStepper.step(-(current_stage - targ_stage)*steps_per_stage*stepsPerRevolution);
    }
    current_stage = targ_stage;
    return 0;
}


void update_buttom_status(){
    if (digitalRead(l_buttom_pin) == LOW){
        l_buttom = true;
    }else{
        l_buttom = false;
    }
    if (digitalRead(r_buttom_pin) == LOW){
        r_buttom = true;
    }else{
        r_buttom = false;
    }
}

void release_motor(){
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    while(r_buttom or l_buttom){
        update_buttom_status();
    }
}

void loop(){
    if (setupx == 0){
        if (find_0() == 1){
            setupx = 2;
        }else{
            setupx = 1;
        }
    }
    if (setupx == 1){
        if (Serial.available() > 0){
            int comand = Serial.parseInt();
            Serial.read();
            Serial.print("<= ");
            Serial.println(comand);
            Serial.print("=> ");
            if (comand == 0){
                Serial.println(current_stage);
            }else{
                if (comand == 1){
                    release_motor();
                    Serial.println(0);
                }else{
                    if ((comand > 9) and ((comand - 10) <= stages)){
                        Serial.println(goto_stage(comand - 10));
                    }else{
                        Serial.println(101);
                    }
                }
            }
        }
        update_buttom_status();
        if (r_buttom and l_buttom){
            release_motor();
        }
        if (r_buttom){
            goto_stage(current_stage - 1);
        }
        if (l_buttom){
            goto_stage(current_stage + 1);
        }
    }
}
