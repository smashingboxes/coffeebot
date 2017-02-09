// This #include statement was automatically added by the Particle IDE.
#include "LiquidCrystal/LiquidCrystal.h"

LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

char row[17];

enum time_type_t {
  COFFEE_SECONDS,
  COFFEE_MINUTES,
  COFFEE_HOURS,
  COFFEE_DAYS
};

struct coffee_time_t {
  enum time_type_t type;
  int count;
};

void millis_to_time(struct coffee_time_t *time, int milliseconds) {
  int seconds = milliseconds / 1000;
  int minutes = seconds / 60;
  int hours = minutes / 60;
  int days = hours / 24;

  if(seconds < 60) {
    time->type = COFFEE_SECONDS;
    time->count = seconds;
    return;
  }

  if(minutes < 60) {
    time->type = COFFEE_MINUTES;
    time->count = minutes;
    return;
  }

  if(hours < 24) {
    time->type = COFFEE_HOURS;
    time->count = hours;
    return;
  }

  if(days > 999) days = 999;
  time->type = COFFEE_DAYS;
  time->count = days;
}

int format_diff(int milliseconds) {
  struct coffee_time_t time;
  char *plural_string;

  millis_to_time(&time, milliseconds);

  if(time.count == 1) {
    plural_string = "";
  } else {
    plural_string = "s";
  }

  switch(time.type) {
    case COFFEE_SECONDS:
      sprintf(row, "%i second%s", time.count, plural_string);
      break;

    case COFFEE_MINUTES:
      sprintf(row,  "%i minute%s", time.count, plural_string);
      break;

    case COFFEE_HOURS:
      sprintf(row,  "%i hour%s", time.count, plural_string);
      break;

    case COFFEE_DAYS:
      sprintf(row,  "%i day%s", time.count, plural_string);
      break;
  }

  return strlen(row);
}

int last_pressed_left;
int last_pressed_right;
int last_printed;

bool left_is_pressed;
bool right_is_pressed;

void setup() {
  lcd.begin(16,2);
  last_pressed_left = millis();
  last_pressed_right = millis();
  last_printed = last_pressed_left;

  // Left Button
  pinMode(D6, INPUT_PULLDOWN);
  left_is_pressed = false;

  // Right Button
  pinMode(D7, INPUT_PULLDOWN);
  right_is_pressed = false;
}

void loop() {
  int current_time = millis();
  int str_len;
  int i;

  if(digitalRead(D6) == HIGH) {
    if(left_is_pressed == false) {
      Spark.publish("pressed", "left");
      delay(50);
      last_pressed_left = current_time;
      left_is_pressed = true;
    }
  } else {
    left_is_pressed = false;
  }

  if(digitalRead(D7) == HIGH) {
    if(right_is_pressed == false) {
      Spark.publish("pressed", "right");
      delay(50);
      last_pressed_right = current_time;
      right_is_pressed = true;
    }
  } else {
    right_is_pressed = false;
  }

  if(current_time > (last_printed + 500)) {
    last_printed = current_time;
    lcd.home();

    str_len = format_diff(current_time - last_pressed_left);
    lcd.print(row);
    for(i = 0; i < (16 - str_len); i++) {
      lcd.print(" ");
    }

    lcd.setCursor(0, 1);
    str_len = format_diff(current_time - last_pressed_right);
    for(i = 0; i < (16 - str_len); i++) {
      lcd.print(" ");
    }
    lcd.print(row);
  }
}
