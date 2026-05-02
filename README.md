# 🐍 Arduino Snake Game - Egyedi 8x8 LED Mátrix és I2C Kijelző

Egy klasszikus "Kígyó" (Snake) játék fizikai megvalósítása Arduino Uno mikrokontroller, saját építésű 8x8-as NeoPixel LED kijelző és egy I2C LCD pontszámláló segítségével.

## 📌 1. Projekt Áttekintés

🎮 **Próbáld ki te is!** A teljes, működő áramkör és a szimuláció elérhető ezen a linken: 
[https://www.tinkercad.com/things/dxJx3nrXOPf-snake]

A projekt célja egy teljes mértékben játszható...

A projekt célja egy teljes mértékben játszható, hardveres alapokon nyugvó retro ügyességi játék megtervezése és leprogramozása. A projekt különlegessége, hogy a fő kijelző nem egy előregyártott mátrix modul, hanem **8 darab 8-LED-es NeoPixel szalagból** lett felépítve "daisy-chain" (vödörlánc) technológiával, soros adatátvitellel.

**Főbb funkciók:**
- Valós idejű, 4-irányú gombos vezérlés.
-  Dinamikusan növekvő kígyó test és véletlenszerű alma generálás.
-  Hardveres ütközésvizsgálat (Game Over állapot fallal vagy saját testtel).
-  Valós idejű pontszám-kijelzés és állapotjelentés egy 16x2-es I2C LCD panelen.



## 🛠️ 2. Hardveres Követelmények és Alkatrészek

A projekt megépítéséhez a következő komponensekre van szükség:


**Arduino Uno R3** : 1 db,  A rendszer "agya", a játéklogika futtatása.
**NeoPixel 8-as szalag** : 8 db,  Ezek alkotják a 8x8-as (64 LED-es) fő kijelzőt. 
**LCD 16x2 (I2C)** : 1 db,  Pontszám és játékállapot (Game Over) megjelenítése. 
**Nyomógomb (Pushbutton)** : 4 db, Irányítás (Fel, Le, Balra, Jobbra). 
**Próbapanel (Breadboard)** :  1 db,  Az áramelosztás. 
**Kábelek (Jumper wires)** 

 **Megjegyzés az ellenállásokról:** A projekt kialakítása során a nyomógombokhoz nem használtunk fizikai lehúzó/felhúzó ellenállásokat. Ehelyett az Arduino szoftveres `INPUT_PULLUP` funkcióját alkalmaztuk, amely aktiválja a mikrokontroller belső 20kΩ-os felhúzó ellenállásait, ezáltal letisztultabb és hibatűrőbb áramkört kapunk.



## 🔌 3. Áramköri Tervezés és "Vödörlánc" Logika
<img width="763" height="506" alt="image" src="https://github.com/user-attachments/assets/ac17f72a-a36d-42d3-905d-055be56a2f0e" />


### 3.1. A Kijelző megépítése (Daisy-Chain)
Mivel a rendszer 8 különálló LED szalagból áll, az adatvonalat (Data Line) sorba kellett kötni. 
1. Az Arduino `D6`-os digitális lába kizárólag a **legelső szalag `DIN` (Data In)** lábához csatlakozik.
2. Az első szalag **`DOUT` (Data Out)** lába továbbítja a jelet a második szalag `DIN` lábába.
3. Ez a láncolat (szerpentin) egészen a 8. szalagig folytatódik. 
Ez a topológia biztosítja, hogy az Arduino az 1-es LED-től a 64-es LED-ig folyamatos címtartományként lássa a panelt.

### 3.2. Lábkiosztási Táblázat (Pinout)
**Nyomógombok:**
- FEL gomb -> `D2` + `GND`
- LE gomb -> `D3` + `GND`
- BALRA gomb -> `D4` + `GND`
- JOBBRA gomb -> `D5` + `GND`

**Kijelzők:**
- NeoPixel Mátrix Adatvonal -> `D6`
-  LCD I2C SDA (Adat) -> `A4`
-   LCD I2C SCL (Órajel) -> `A5`
-   Minden eszköz -> Közös `5V` és `GND`

## 💻 4. Szoftveres Architektúra és Adatszerkezetek

A játék logikája procedurális megközelítésben, C++ nyelven (Arduino keretrendszerben) íródott. A program két fő külső könyvtárat használ a hardverek absztrakciójához:
*   `Adafruit_NeoPixel.h`: A soros adatvonalon kommunikáló LED-ek vezérlésére.
*   `Adafruit_LiquidCrystal.h`: Az I2C buszon kommunikáló kijelző meghajtására.

### 4.1. Változók és Memóriakezelés
Az Arduino Uno (ATmega328P) mindössze 2KB SRAM memóriával rendelkezik, így az adatszerkezeteket optimalizálni kellett. A kígyó testének tárolásához egydimenziós tömböket használunk.
```cpp

int snakeX[64];
int snakeY[64];
int snakeLen = 3; 
```
Mivel a játéktér 8x8-as, a kígyó maximális hossza 64 blokk lehet. A snakeX és snakeY tömbök indexei a kígyó testrészeit jelölik:

`[0]` index: A kígyó feje (ez irányítja a mozgást).
`[1]`-től `[snakeLen -1]`-ig: A kígyó teste és farka.

### 4.2. Irányvektorok**
A kígyó mozgását egy 2D-s koordinátarendszerben értelmezzük, ahol a bal felső sarok a `(0,0)` pont. A mozgás irányát a `dirX` és `dirY` változók határozzák meg:

- Jobbra: `dirX = 1`, `dirY = 0`
- Balra: `dirX = -1`, `dirY = 0`
- Lefelé: `dirX = 0`, `dirY = 1`
- Felfelé: `dirX = 0`, `dirY = -1`



## 🔄 5. A Játék Ciklusa (Game Loop) és Algoritmusok
A fő `loop()` függvény egy diszkrét időlépésekben (tick) működő állapotgépet valósít meg. Minden iteráció egy képkockát (frame) jelent a játékban.

### 5.1. A fő algoritmus folyamatábrája
A folyamatábra a játék egyetlen időlépésének (tick) logikáját mutatja be.

```cpp
    A[Gombok beolvasása] --> B{Game Over állapot?}
    B -- Igen --> C[Game Over képernyő és villogás]
    C --> D[Visszatérés return]
    B -- Nem --> E[Új irány meghatározása]
    E --> F[Test mozgatása tömb másolással]
    F --> G[Fej léptetése az új irányba]
    G --> H{Ütközés a fallal?}
    H -- Igen --> I[gameOver = true]
    H -- Nem --> J{Ütközés saját testtel?}
    J -- Igen --> I
    J -- Nem --> K{Alma megevése?}
    K -- Igen --> L[Hossz és Pontszám növelése]
    L --> M[Új alma generálása]
    M --> N
    K -- Nem --> N[Képernyő törlése]
    N --> O[Alma rajzolása pirossal]
    O --> P[Kígyó rajzolása zölddel]
    P --> Q[delay 400ms - Sebesség]```

```

### 5.2. A Kígyó mozgási algoritmusa
A program nem mozgat minden egyes pixelt külön. Ehelyett a "követés" (follow-the-leader) elvét alkalmazza: minden iterációban a kígyó testrészei megkapják az előttük lévő testrész pozícióját, a fej pedig előrelép a vektor irányába.

```cpp
// Test mozgatása 
for (int i = snakeLen - 1; i > 0; i--) {
  snakeX[i] = snakeX[i - 1];
  snakeY[i] = snakeY[i - 1];
}
// Fej mozgatása
snakeX[0] += dirX;
snakeY[0] += dirY;
```
### 5.3. 2D Koordináták leképezése 1D Címtartományra
A NeoPixel szalagok lineárisak, így az 1-től 64-ig terjedő memóriacímet át kell számolni a logikai 8x8-as koordinátákból (x, y).
A `getPixel(x, y)` függvény végzi el ezt a matematikai transzformációt. Mivel a hardver megépítése során minden szalagot balról jobbra (`DIN` -> `DOUT`) láncoltunk, az algoritmus a következő egyszerű képletet használja:

`Cím = Y * 8 + X`



## 🍎 6. Játékmechanika: Alma Generálás
Az alma (food) megjelenése véletlenszerű. A `spawnFood()` függvény tartalmaz egy hibakezelő logikát, amely biztosítja, hogy az alma ne generálódjon olyan koordinátára, ahol a kígyó teste éppen tartózkodik (mert az lehetetlenné tenné az elérését, vagy azonnali felszedést eredményezne).

```cpp
bool valid = false;
while (!valid) {
  foodX = random(0, 8); 
  foodY = random(0, 8);
  valid = true;
  // Ellenőrzés
  for (int i = 0; i < snakeLen; i++) {
    if (snakeX[i] == foodX && snakeY[i] == foodY) {
      valid = false; 
      break;
    }
  }
}
```
Mivel a `random()` függvény a processzor órajele alapján generál pszeudo-véletlen számokat, minden indulásnál más helyen fog megjelenni a célpont.



## 🐛 7. Tesztelés és Hibakeresés (Debugging)

A projekt fejlesztése és a Tinkercad környezetben történő szimulációja során több hardveres és szoftveres kihívással is szembe kellett nézni. A hibakeresés (debugging) folyamata a következő főbb iterációkból állt:

### 7.1. LED Mátrix Párhuzamos vs. Soros Kötése
-  **A probléma:** Az első tesztek során a 8 darab LED szalag párhuzamosan lett az adatvonalra (D6) kötve. Ennek eredményeképpen a kijelzőn nem egyetlen pixel mozgott, hanem egész oszlopok/sorok villantak fel egyszerre, mivel minden szalag ugyanazt az utasítást kapta meg egyidőben.
-  **A megoldás:** A hardver áttervezése "Daisy-Chain" (vödörlánc) topológiára. A szalagok `DIN` és `DOUT` lábainak szigorú sorbakötésével az adatcsomagok megfelelően tudtak végigfolyni mind a 64 LED-en.



### 7.3. Játéksebesség és Játszhatóság
-  **A probléma:** Az eredeti `delay(300)` beállítással a kígyó túl gyorsan mozgott a 8x8-as, viszonylag szűk pályán, így a falnak ütközés szinte elkerülhetetlen volt az emberi reakcióidő korlátai miatt.
-  **A megoldás:** A szimulációs környezetben a főciklus késleltetésének finomhangolása (`delay(400)`), amely optimális egyensúlyt teremtett a kihívás és a játszhatóság között.



## 🚀 8. Továbbfejlesztési Lehetőségek

Bár a jelenlegi verzió egy teljesen funkcionális MVP (Minimum Viable Product), a jövőben a hardver és a szoftver is bővíthető további funkciókkal:

1.  **Dinamikus nehézségi szint (Gyorsulás):**
    A kód módosításával elérhető, hogy a `delay()` értéke csökkenjen (a játék gyorsuljon) minden egyes megevett alma után.
    *Tervezett kódmódosítás:*
    ```cpp
    int gameSpeed = 500;
    if (gameSpeed > 100) gameSpeed -= 10;
    delay(gameSpeed);
    ```

2.  **Hangeffektek (Piezo Buzzer):**
    Egy passzív piezo hangszóró integrálása a rendszerbe (pl. a `D7` digitális lábon), amely a `tone()` függvény segítségével rövid csipogást ad ki alma megevésekor, és Game Over dallamot játszik le ütközéskor.

3.  **High Score (Legmagasabb pontszám) mentése:**
    Az Arduino beépített EEPROM memóriájának (`#include <EEPROM.h>`) használatával a legmagasabb elért pontszám tartósan elmenthető lenne, így az az eszköz kikapcsolása után is megmaradna, és kiírható lenne az I2C kijelzőre a kezdőképernyőn.



## 🏁 9. Összegzés

A projekt sikeresen demonstrálja, hogyan lehet alapszintű elektronikai komponensekből (mikrokontroller, LED szalagok, I2C kijelző, nyomógombok) egy komplex, interaktív rendszert felépíteni. A fejlesztés során alkalmazott soros adatkommunikációs és multiplexálási elvek kiváló alapot nyújtanak komolyabb beágyazott rendszerek (Embedded Systems) tervezéséhez is.

