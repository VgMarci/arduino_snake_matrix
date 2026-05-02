# 🐍 Arduino Snake Game - Egyedi 8x8 LED Mátrix és I2C Kijelző

Egy klasszikus "Kígyó" (Snake) játék fizikai megvalósítása Arduino Uno mikrokontroller, saját építésű 8x8-as NeoPixel LED kijelző és egy I2C LCD pontszámláló segítségével.

## 📌 1. Projekt Áttekintés

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



### 3.1. A Kijelző megépítése (Daisy-Chain)
Mivel a rendszer 8 különálló LED szalagból áll, az adatvonalat (Data Line) sorba kellett kötni. 
1. Az Arduino `D6`-os digitális lába kizárólag a **legelső szalag `DIN` (Data In)** lábához csatlakozik.
2. Az első szalag **`DOUT` (Data Out)** lába továbbítja a jelet a második szalag `DIN` lábába.
3. Ez a láncolat (szerpentin) egészen a 8. szalagig folytatódik. 
Ez a topológia biztosítja, hogy az Arduino az 1-es LED-től a 64-es LED-ig folyamatos címtartományként lássa a panelt.

### 3.2. Lábkiosztási Táblázat (Pinout)
<img width="763" height="506" alt="image" src="https://github.com/user-attachments/assets/ac17f72a-a36d-42d3-905d-055be56a2f0e" />



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




