# ♻️ Automated Reverse Vending Machine (RVM)

A smart waste management solution designed to **identify, weigh, and sort plastic bottles**.  
This system is built around an **Arduino-based controller** that authenticates users via mobile numbers, evaluates waste objects using sensors, and automatically sorts them using motorized mechanisms.

---

## 🚀 System Logic & Flow

### 1️⃣ User Authentication
- Users enter an **11-digit mobile number** using an **I2C Keypad**.
- The system validates Bangladeshi mobile prefixes (e.g., `017`, `018`, etc.).
- Invalid numbers are rejected immediately.

### 2️⃣ Intake Phase
- Once validated, the **Slider Door opens**.
- A **14-second countdown timer** appears on the LCD.
- The user deposits the bottle within this time window.

### 3️⃣ Safety Monitoring
- An **IR obstacle sensor** continuously checks the doorway.
- If an obstruction is detected:
  - The slider motor **stops immediately**
  - A warning message prompts the user to **“Unblock the Door”**

### 4️⃣ Data Acquisition
- **Weight Measurement**
  - Captured using a **Load Cell + HX711 module**
- **Height Measurement**
  - Calculated using a **4-level IR sensor array**

### 5️⃣ Sorting Logic
- The `is_plastic()` function validates the object:
  - Acceptable weight range: **1g – 20g**
- Based on the result:
  - **Plastic Item** → Right bin
  - **Non-Plastic / Invalid** → Left bin
- Sorting is performed via a **Flipper Motor**

---

## 🛠 Hardware Configuration

| Component            | Pin / Address | Description                          |
|---------------------|--------------|--------------------------------------|
| LCD (20x4)          | `0x27 (I2C)` | Main user interface                  |
| Keypad              | `0x20 (I2C)` | 4x4 keypad via PCF8574               |
| HX711 DOUT          | `9`          | Load cell data pin                   |
| HX711 SCK           | `10`         | Load cell clock pin                  |
| IR Sensor Array     | `14,15,16,17`| Height detection (Analog)            |
| Obstacle IR Sensor  | `4`          | Door safety sensor                   |
| Motor A             | `5,6`        | Flipper motor control                |
| Motor B             | `7,8`        | Slider (door) motor control          |
| Limit Switches      | `2,3`        | Motor end-stop detection             |

---

## 💻 Installation & Setup

### 1️⃣ Required Libraries
Install the following libraries in the **Arduino IDE**:

- `Keypad_I2C`
- `LiquidCrystal_I2C`
- `HX711`
- `Wire`

---

### 2️⃣ Load Cell Calibration
Before deployment, calibrate the load cell according to your hardware.

Update the calibration factor in the code:

```cpp
#define CALIBRATION_FACTOR 438.86
```



### 3️⃣ IR Sensor Sensitivity

Default IR detection threshold: 650

- If operating in:

- Very bright environments → Increase threshold

- Very dark environments → Decrease threshold

- Adjust this value inside the IRarrayinfo() function.



### ♻️ Project Purpose

This Automated RVM aims to:

- Encourage plastic recycling

- Reduce manual waste sorting

- Enable smart, sensor-driven waste management

- Support future expansion into reward systems and data tracking

### 📌 Future Enhancements (Optional)

- GSM/SMS or mobile wallet reward integration

- Cloud-based usage analytics

- QR-based user identification

- Machine learning-based object classification