# Multi-ECU Control System

**Qt 5.15 / QML · C++ · MinGW 64-bit**

Çok kanallı ECU (Elektronik Kontrol Ünitesi) izleme, parametre yönetimi ve CAN-FD telemetri platformu. 7 branch'in merge edilmesiyle oluşturulmuştur.

---

## 🗂️ Proje Yapısı

```
Multi-ECU-Control-System/
├── ecu-control/            ← Ana Qt projesi (buradan açılır)
│   ├── EcuControl.pro      ← Qt Creator'da bu dosyayı aç
│   ├── qml/                ← QML arayüz dosyaları
│   │   ├── Main.qml
│   │   ├── EcuPanel.qml
│   │   ├── ParameterPage.qml
│   │   ├── SignalChartScreen.qml
│   │   ├── FirmwareUploadScreen.qml
│   │   └── components/
│   ├── src/
│   │   ├── main.cpp
│   │   ├── core/           ← C++ backend sınıfları
│   │   │   ├── dispatcher.h/.cpp       ← Merkezi QML ↔ Backend köprüsü
│   │   │   ├── modulesession.h/.cpp    ← Tek modülün durumu (QML property)
│   │   │   ├── moduleworker.h/.cpp     ← Threaded CAN worker
│   │   │   ├── datasimulator.h/.cpp    ← 16-byte sahte telemetri paketi (aybuke)
│   │   │   ├── parametermodel.h/.cpp   ← Parametre değer deposu (esra)
│   │   │   ├── rawdataparser.h/.cpp    ← Bit-level ham veri ayrıştırıcı (aybuke)
│   │   │   ├── excelparser.h/.cpp      ← .xlsx parametre okuyucu (aybuke)
│   │   │   ├── rawsignalloader.h/.cpp  ← CSV+hex dosya yükleyici
│   │   │   ├── signalfileloader.h/.cpp ← Sinyal dosyası okuyucu
│   │   │   ├── livesignalsource.h/.cpp ← Canlı veri kaynağı
│   │   │   ├── canfdemulator.h/.cpp    ← CAN-FD simülatörü
│   │   │   └── ccpprotocol.h/.cpp      ← CCP protokol katmanı
│   │   └── QXlsx/          ← Excel okuma kütüphanesi
│   ├── data/
│   │   ├── ecu_parameters.csv  ← Örnek parametre tanımları
│   │   └── raw_data.txt        ← Örnek ham hex telemetri
│   ├── tests/              ← QML unit testleri (taha branch)
│   └── build_qt515/        ← Derleme çıktısı (Qt 5.15)
│
├── tools/
│   ├── eda/                ← Python veri analiz araçları (esrasilahsor branch)
│   │   ├── loader.py, summary.py, visualization.py ...
│   └── can-lab/            ← Gerçek CAN donanımı referans kodu (bağlı değil)
│
└── README.md               ← Bu dosya
```

---

## ⚡ Hızlı Başlangıç

### Gereksinimler
- Qt 5.15.x (MinGW 64-bit)
- Qt Creator

### Derleme
1. **Qt Creator'ı** aç
2. `ecu-control/EcuControl.pro` dosyasını aç
3. Kit olarak **Qt 5.15.19 MinGW 64-bit** seç
4. `Ctrl+B` ile derle
5. `Ctrl+R` ile çalıştır

Veya komut satırından:
```bash
cd ecu-control/build_qt515
qmake ../EcuControl.pro -spec win32-g++
mingw32-make -f Makefile.Release -j4
```

---

## 🧩 Birleştirilen Branch'ler ve Katkılar

| Branch | Katkı | Durumu |
|--------|-------|--------|
| `main` (Singleton Combined) | Temel mimari: Dispatcher, ModuleSession, ModuleWorker, CAN-FD | ✅ Temel |
| `EceGulYuksel` | Log paneli, Fail-Safe, Acil Durdurma, Sıralı Başlatma | ✅ Entegre |
| `aybuke` | DataSimulator (16-byte), RawDataParser (bit-level), ExcelParser | ✅ Entegre |
| `esra_parameter-page` | ParameterModel, Parametre Kontrol UI (N1/EGT/FuelFlow/...) | ✅ Entegre |
| `taha` | 8 adet QML unit testi | ✅ `tests/` klasöründe |
| `esrasilahsor` | Python EDA araçları (loader, visualization, outlier...) | ✅ `tools/eda/` |
| `ihsan` | Yalnızca README değişikliği | — Atlandı |
| `ArdaAkcaalev` | Main ile aynı içerik | — Atlandı |

---

## 🎛️ Özellikler

### Ana Kontrol Ekranı
- **4 Modül** (ECU A × 2, ECU B × 2) bağımsız thread'lerde yönetilir
- Her modül: Güç on/off, bağlan/bağlantı kes, durum göstergesi, RPM gauge
- **Sıralı Başlatma** (▶): Modülleri aşırı yük bindirmeden sırayla başlatır
- **Acil Durdurma** (⛔): Tüm modülleri anında durdurur

### Fail-Safe Sistemi *(EceGulYuksel branch)*
- Sistem sağlık kontrolü: herhangi bir modül kritik duruma girerse tüm sistem güvenli moda geçer
- Ekranın üstüne kırmızı uyarı bandı açılır
- **Olay Akışı (Log Paneli):** Sağda canlı `[BİLGİ]` / `[UYARI]` / `[HATA]` / `[KRİTİK]` log akışı

### Parametre Kontrolü *(esra branch)*
- N1, EGT, Fuel Flow, Bleed Valve, Vibration parametrelerini slider ve ComboBox ile ayarla
- Parametre değerleri `ParameterModel`'e yazılır, QML'den `Dispatcher.parameterModel` ile erişilir
- **Excel'den parametre yükleme:** `Dispatcher.loadExcelParameters(url)` ile `.xlsx` dosyasından parametre tablosu okunur

### Ham Veri Ayrıştırma *(aybuke branch)*
- `RawDataParser`: bit düzeyinde ham ECU paketini, Excel'deki adres tablosuna göre parçalar
- Desteklenen tipler: `uint8/16/32`, `int8/16/32`, `float`
- Desteklenen formüller: `x * n`, `x / n`, `x + n`, `x - n`, `Enum`, `Hex`
- `DataSimulator`: her 500ms'de **16-byte** telemetri paketi üretir:

```
[0-1]  Header    : 0xAA 0x55
[2-3]  N1        : int16 LE  (%)
[4-5]  EGT       : int16 LE  (°C)
[6-7]  Fuel Flow : int16 LE  (kg/h)
[8-9]  Bleed Valve: int16 LE (enum)
[10-13] Vibration: float LE  (0.0-5.0)
[14-15] Checksum : uint16 LE (16-bit XOR)
```

### Sinyal Analizi *(ana branch)*
- CSV veya canlı kaynaktan sinyal verisi yükle
- QtCharts ile zamana bağlı gerçek zamanlı grafik
- Parametre seçici, teknik bilgi paneli (adres, genişlik, tip, formül)

### Yazılım Yükleme *(ana branch)*
- Seçilen modüle firmware binary yükleme
- CCP protokolü üzerinden ilerleme takibi

---

## 🔗 can-lab Hakkında

`tools/can-lab/` klasörü, **gerçek CAN donanımına** (PCAN, Vector kartı vb.) bağlanmak için geliştirilmiş bir referans implementasyonudur.

**Şu an `ecu-control` uygulamasına entegre değildir.** Uygulamada bunun yerine `canfdemulator.cpp` (sanal CAN-FD) kullanılmaktadır. Gerçek donanım bağlantısı gerektiğinde `CommunicationCore` → `Dispatcher` entegrasyonu yapılabilir.

---

## 🐍 Python EDA Araçları

`tools/eda/` klasöründeki araçlar *(esrasilahsor branch)* telemetri CSV verilerini analiz etmek için kullanılır:

```python
from eda import loader, summary, visualization, outlier, quality

df = loader.load("data/telemetry.csv")
summary.describe(df)
visualization.plot_signals(df)
outlier.detect(df)
```

---

## 🗃️ Parametre Dosya Formatları

### Excel (`.xlsx`) — ExcelParser
| Sütun | İçerik | Örnek |
|-------|--------|-------|
| A | RAM Adresi | 0, 4, 6 |
| B | Bit Genişliği | 16, 8, 32 |
| C | Bit Offset | 0 |
| D | Veri Tipi | uint16, float |
| E | Parametre Adı | N1, EGT, Voltage |
| F | Dönüşüm Formülü | x * 0.1 (V) |

### CSV — RawSignalLoader
```
ramAddress,dataOffset,dataWidth,dataType,parameterName,conversionFormula
0,0,16,uint16,N1,x * 1
2,0,16,uint16,EGT,x * 1
```

---

## 👥 Katkıda Bulunanlar

| İsim | Branch | Katkı |
|------|--------|-------|
| Aybüke Turgun | `aybuke` | DataSimulator, RawDataParser, ExcelParser |
| Ece Gül Yüksel | `EceGulYuksel` | Fail-Safe, Log, Sıralı Başlatma |
| Esra (Parametre) | `esra_parameter-page` | ParameterModel, Parametre UI |
| Esra Silahsor | `esrasilahsor` | Python EDA araçları |
| Taha | `taha` | QML Unit Testleri |
