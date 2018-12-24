#include <LiquidCrystal.h>

//lcd init
const byte rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte firstPage = 0b101000;
byte secondPage = 0b101010;

//led
const byte n = 2;
const byte ledPin[n] = {9, 10};

//buzzerPin pin
const byte buzzerPin = 13;

//control pins
const byte up = 8;
const byte down = 7;

//keypress
bool upUp = true;
bool downUp = true;

//player
bool y = 0;
int score = 0;
byte lives = 3;

//enemy
byte enemyX = 15, enemyY = 0;

//bonus life
byte lifeX = -1, lifeY = 0;
int prob = 16;
int timer = 0;
int help = 0;

//state
bool lost = true;
bool played = false;
bool sound;
bool soundSel = false;
int menuTimer = 0;

void mainMenu()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hold button up");
  lcd.setCursor(0, 1);
  lcd.print("to play");
}

void lose(int Score)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You lose! Score:");
  lcd.setCursor(0, 1);
  lcd.print(String(Score));
}

void instructions()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hold: up to play");
  lcd.setCursor(4, 1 );
  lcd.print("down to menu");
}

void setLedBrightness(byte amount, byte brightness)
{
  for (byte i = 0; i < amount; i++)
  {
    analogWrite(ledPin[i], brightness);
  }
}

void makeSound(int frequency, int t)
{
  if (sound)
    tone(buzzerPin, frequency, t);
}

void setup() {
  //lcd
  lcd.begin(16, 2);
  lcd.command(0b101000);

  //leds
  for (byte i = 0; i < n; i++)
  {
    pinMode(ledPin[i], OUTPUT);
  }

  //controls
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  //sound
  pinMode(buzzerPin, OUTPUT);
  //init random
  randomSeed(analogRead(0));
}

void loop() {
  bool upUp = digitalRead(up);
  bool downUp = digitalRead(down);
  menuTimer++;
  if (!lost) {
    played = true;

    //draw
    lcd.clear();
    lcd.setCursor(1, y);
    lcd.print(">");//player

    lcd.setCursor(enemyX, enemyY);
    lcd.print("x");//enemy

    if (lifeX >= 0 && lifeX <= 16)
    {
      lcd.setCursor(lifeX, lifeY);
      lcd.print("1");//life
    }

    //check lives
    switch (lives)
    {
      case 1:
        setLedBrightness(n, 0);
        break;
      case 2:
        analogWrite(ledPin[0], 0);
        analogWrite(ledPin[1], 4);
        break;
      case 3:
        setLedBrightness(n, 4);
        break;
    }

    //bonus live probability
    if (random(0, 1024) == prob && lifeX < 0)
      lifeX = 25;

    //controls
    if (!upUp)
    {
      if (y != 0)
        makeSound(500, 20);
      y = 0;
    }
    if (!downUp)
    {
      if (y != 1)
        makeSound(500, 20);
      y = 1;
    }

    //enemy movement
    if (enemyX > 1)
      enemyX--;
    else
    {
      enemyX = 20;
      enemyY = random(0, 2);
    }

    //bonus live movement
    lifeX--;
    if (lifeY == 0 && timer >= 5)
    {
      lifeY = 1;
      timer = 0;
    }
    else if (lifeY == 1 && timer >= 5)
    {
      lifeY = 0;
      timer = 0;
    }
    timer++;

    //check collision with enemy
    if (enemyX == 1 && enemyY == y) //if col
    {
      if (lives > 1)
      {
        lives--;
        makeSound(100, 100);
      }
      else if (lives == 1)
      {
        lost = true;
        makeSound(100, 1000);
      }
    }
    else if (enemyX == 1 && enemyY != y) //if no col
    {
      score++;
      makeSound(5000, 20);
    }

    //check collision with bonus live
    if (lifeX == 1 && lifeY == y)
    {
      lifeX = -1;
      makeSound(3000, 50);
      makeSound(4500, 50);
      if (lives < 3)
        lives++;
      help += 5 + (score / 10);
    }

  }// if(!lost)
  else
  {
    if (!soundSel)
    {
      lcd.clear();
      lcd.print("Sound?");
      lcd.setCursor(0, 1);
      lcd.print("Up=yes, down=no");
      if (!upUp)
      {
        sound = true;
        soundSel = true;
        menuTimer=0;
      }
      else if (!downUp)
      {
        sound = false;
        soundSel = true;
        menuTimer=0;
      }
    }

    if (played == false && soundSel)
    {
      mainMenu();
    }
    else if (played == true && soundSel)
    {
      //use 2 pages to show score and instructions
      lcd.command(firstPage);
      lose(score);
      delay(1000);
      lcd.command(secondPage);
      instructions();
      delay(1000);
    }

    //reset
    if (!upUp&&menuTimer>50)
    {
      lost = false;
      score = 0;
      lives = 3;
    }
    if (!downUp)
    {
      played = false;
      score = 0;
      lives = 3;
    }

  }// else
  
  delay(100 - score + help);
}// loop
