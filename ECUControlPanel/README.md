# ECU Control Panel

ECU Control Panel, Elektronik Kontrol Ünitelerinden (ECU) alınan ham telemetri verilerinin okunması, ayrıştırılması, dönüştürülmesi ve kullanıcı arayüzünde görüntülenmesi amacıyla geliştirilen Qt/C++ ve QML tabanlı bir masaüstü uygulamasıdır.

Proje kapsamında ECU bağlantı ve kontrol işlemlerinin yanı sıra ham veri analizi, parametre yönetimi, sistem logları ve gerçek zamanlı veri görselleştirme özellikleri geliştirilmiştir.

---

## 🚀 Proje Özellikleri

- ECU kontrol paneli
- Birden fazla ECU'nun aynı arayüz üzerinden yönetilmesi
- ECU bağlantı / bağlantı kesme işlemleri
- ECU güç kontrolü
- ECU çalışma modu seçimi
- Üç durumlu ECU switch yapısı: `OFF`, `ON`, `KILL`
- Emergency Stop yönetimi
- Sistem olaylarının loglanması
- Ham hexadecimal veri okuma
- Ham verinin parametrelere göre ayrıştırılması
- `Signed`, `Unsigned` ve `Float` veri tiplerinin desteklenmesi
- Excel üzerinden parametre tanımlarının okunması
- Parametre dönüşüm formüllerinin uygulanması
- ECU telemetri verilerinin anlık olarak arayüzde görüntülenmesi
- ECU ve parametre seçimi
- Güncel parametre değerinin görüntülenmesi
- Gerçek zamanlı telemetri grafiği
- Farklı parametrelerin grafik üzerinde izlenebilmesi
- Telemetri verisi için test amaçlı veri simülasyonu

---

## 🛠️ Kullanılan Teknolojiler

| Teknoloji | Kullanım Alanı |
|---|---|
| C++ | Backend ve veri işleme |
| Qt 5.15 | Uygulama framework'ü |
| QML | Kullanıcı arayüzü |
| Qt Quick Controls | Arayüz bileşenleri |
| Qt Quick Layouts | Arayüz yerleşimi |
| Qt Charts | Telemetri grafiklerinin oluşturulması |
| Qt Signals & Slots | Bileşenler arası iletişim |
| QXlsx | Excel dosyalarının okunması |
| Qt Resource System | QML ve kaynak dosyalarının projeye dahil edilmesi |
| Git | Sürüm kontrolü |

---

## 📁 Proje Yapısı

```text
ECUControlPanel/
│
├── backend/
│   ├── collector/
│   ├── dispatcher/
│   ├── manager/
│   ├── parser/
│   └── simulator/
│
├── qml/
│   ├── components/
│   │   ├── ECUPanel.qml
│   │   ├── Header.qml
│   │   ├── SystemLog.qml
│   │   ├── StatusLed.qml
│   │   ├── ThreeStateSwitch.qml
│   │   └── ...
│   │
│   ├── DataVisualization.qml
│   └── main.qml
│
├── QXlsx/
├── data/
│   ├── ECU_Data.xlsx
│   └── raw_data.txt
├── docs/
├── resources/
├── .gitignore
├── ECUControlPanel.pro
├── main.cpp
└── qml.qrc
```

---

## 🏗️ Yazılım Mimarisi

Proje, backend ve frontend sorumluluklarının ayrıştırıldığı katmanlı bir yapıda geliştirilmiştir.

### ECU kontrol akışı

```text
┌──────────────────────┐
│       QML UI         │
│                      │
│ ECU Control Panel    │
│ Data Visualization   │
│ System Log           │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│     UICollector      │
│      UI → Backend     │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│     Dispatcher       │
│    Mesaj yönlendirme │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│      ECUManager      │
│    ECU kontrol mantığı│
└──────────────────────┘
```

### Ham veri işleme akışı

```text
Raw Data
   │
   ▼
┌─────────────────┐
│  RawDataReader  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐       ┌─────────────────┐
│  RawDataParser  │◄──────│   ExcelParser   │
└────────┬────────┘       └─────────────────┘
         │
         ▼
   Parsed Values
         │
         ▼
      QML UI
```

---

## 📡 Ham Veri İşleme

Ham telemetri verileri hexadecimal formatta okunmaktadır.

Örneğin:

```text
00 01 02 03 04 05 06 07 ...
```

`RawDataReader`, veri dosyasındaki hexadecimal karakterleri byte'lara dönüştürür.

Ardından `RawDataParser`, Excel dosyasından alınan parametre tanımlarını kullanarak ham veriyi ilgili parametrelere ayırır.

Her parametre için RAM adresi, veri genişliği, veri tipi, parametre adı ve dönüşüm formülü kullanılmaktadır.

---

## 🔢 Desteklenen Veri Tipleri

### Unsigned

Ham byte'lar unsigned integer olarak yorumlanır.

```text
00 01 02 03 → 66051
```

### Signed

Signed integer değerleri desteklenmektedir. İşaret biti kontrol edilerek negatif değerler hesaplanabilmektedir.

### Float

4 byte uzunluğundaki veriler IEEE 754 floating-point formatında yorumlanmaktadır.

---

## 🔄 Parametre Dönüşümleri

Ham değerler gerektiğinde Excel'de tanımlanan dönüşüm formülleri kullanılarak fiziksel değerlere dönüştürülmektedir.

Örneğin:

```text
Raw Value: 4627
Conversion: x * 0.1 (V)
Converted Value: 462.7 V
```

Başka bir örnek:

```text
Raw Value: 21589
Conversion: x * 0.001 (V)
Converted Value: 21.589 V
```

---

## 📊 Veri Görüntüleme

Data Visualization ekranında kullanıcı:

- ECU seçebilir.
- Görüntülenecek parametreyi seçebilir.
- Parametrenin güncel değerini görebilir.
- Parametrenin teknik bilgilerini inceleyebilir.
- Telemetri verisini zaman içerisinde grafik üzerinde izleyebilir.

Görüntülenen teknik bilgiler:

- ECU
- Address
- Width
- Type
- Conversion

---

## 📈 Gerçek Zamanlı Grafik

Telemetri verilerinin zaman içerisindeki değişimini göstermek amacıyla Qt Charts kullanılmaktadır.

```text
X Axis → Sample / Time
Y Axis → Parameter Value
```

Seçilen parametrenin değerleri belirli aralıklarla güncellenerek grafik üzerine eklenmektedir.

---

## 🧪 Telemetri Veri Simülasyonu

Gerçek ECU bağlantısı bulunmadığında geliştirme ve test süreçlerini sürdürebilmek amacıyla test amaçlı bir veri simülasyonu kullanılmaktadır.

Simülatör:

- ECU telemetri paketleri oluşturur.
- Parametre değerlerini zaman içerisinde değiştirebilir.
- Oluşturulan ham verinin mevcut veri işleme pipeline'ından geçmesini sağlar.

Simülatörün amacı gerçek ECU iletişiminin yerine geçmek değil, geliştirme ve UI testlerini kolaylaştırmaktır.

---

## 🖥️ Kullanıcı Arayüzü

Ana kontrol ekranında birden fazla ECU paneli bulunmaktadır.

Her ECU panelinde:

- ECU durumu
- Power durumu
- Mode seçimi
- Connect
- Disconnect
- Power kontrolü
- Three-State Switch
- Emergency Stop durumu

görüntülenmektedir.

Örnek ECU durumları:

- `OFFLINE`
- `STANDBY`
- `READY`
- `POWERED`
- `EMERGENCY`

---

## 📝 System Log

Uygulama içerisinde gerçekleştirilen önemli işlemler System Log bölümünde görüntülenmektedir.

Örneğin:

```text
22:11:44    INFO       ECU A-1 Connected
22:12:03    INFO       ECU A-1 Mode changed to Test
22:12:15    WARNING    ECU A-1 Emergency Stop Activated
```

---

## 🔌 ECU Kontrol Akışı

Temel bağlantı akışı:

```text
Connect
   │
   ▼
Connected
   │
   ▼
Switch ON
   │
   ▼
Power ON
```

Emergency Stop durumunda:

```text
KILL
 │
 ├── Connected → false
 ├── Powered   → false
 └── Emergency Stop Active
```

---

## 📋 Parametre Tanımları

Parametre bilgileri Excel dosyasından okunmaktadır.

Örnek parametreler:

| Parameter | Address | Width | Type | Conversion |
|---|---:|---:|---|---|
| RPM | 0 | 4 | Unsigned | None |
| Temperature | 4 | 2 | Signed | x * 0.1 |
| Voltage | 6 | 2 | Unsigned | x * 0.1 |
| Current | 10 | 4 | Float | None |
| Bus Voltage | 18 | 2 | Unsigned | x * 0.1 (V) |
| State of Charge (SOC) | 80 | 2 | Unsigned | x * 0.1 (%) |
| Cell 1 Voltage | 84 | 2 | Unsigned | x * 0.001 (V) |

Parametre sayısı ve tanımları Excel dosyasına göre dinamik olarak okunmaktadır.

---

## ⚙️ Veri Akışı

```text
Raw Data Source
      │
      ▼
RawDataReader / DataSimulator
      │
      ▼
RawDataParser
      │
      ├── Parameter Definitions
      │          ▲
      │          │
      │     ExcelParser
      │
      ▼
Converted Values
      │
      ▼
QML / Data Visualization
      │
      ▼
Real-Time Chart
```

---

## ▶️ Çalıştırma

1. Qt 5.15.x kurulmalıdır.
2. Qt Creator açılmalıdır.
3. `ECUControlPanel.pro` dosyası açılmalıdır.
4. Uygun Qt kit'i seçilmelidir.
5. Proje build edilmelidir.
6. Uygulama çalıştırılmalıdır.

### Önerilen ortam

- Qt 5.15.x
- MinGW 64-bit
- Qt Creator
- C++
- QML

---

## 📂 Veri Dosyaları

### `ECU_Data.xlsx`

ECU parametrelerinin tanımlarını içerir.

### `raw_data.txt`

Hexadecimal ham ECU verilerini içerir.

Örnek:

```text
000102030405060708090A0B0C0D...
```

---

## 🎯 Projenin Amacı

ECU Control Panel'in temel amacı, ECU sistemlerinden elde edilen telemetri verilerinin okunmasını, işlenmesini ve kullanıcıya anlaşılır bir arayüz üzerinden sunulmasını sağlamaktır.

```text
Ham ECU Verisi
      ↓
Veri Okuma
      ↓
Veri Ayrıştırma
      ↓
Parametre Dönüşümü
      ↓
Güncel Değer
      ↓
Gerçek Zamanlı Görselleştirme
```

---

## 👩‍💻 Geliştirici

**Aybüke Turgun**
