# Frontend Geliştirici Kılavuzu: Arayüz ve QML Mimarisi

Bu kılavuz, Multi-ECU Kontrol Sistemi arayüzünün (QML) yapısını, C++ backend ile nasıl haberleştiğini, responsive (esnek) tasarım kurallarını ve gelecekte eklenebilecek Light/Dark mode entegrasyonunu açıklamaktadır.

---

## 1. Genel Mimari ve Haberleşme Akışı (Mediator Pattern)

Arayüz (QML) ile C++ backend katmanı arasında **doğrudan bağımlılık yoktur**. QML, arka plandaki işçilerle (worker thread'ler) veya doğrudan soketlerle konuşmaz. Tüm iletişim tek bir merkezi aracı (Mediator) olan **`Dispatcher`** sınıfı üzerinden yürütülür.

```
                    ┌────────────────────────┐
                    │        QML GUI         │
                    └───────────┬────────────┘
                                │ (Sinyaller & Metot Çağrıları)
                                ▼
                    ┌────────────────────────┐
                    │  Dispatcher (C++)      │  ◄── [Merkezi Köprü]
                    └─────┬────────────┬─────┘
                          │            │
         ┌────────────────┘            └────────────────┐
         ▼                                              ▼
┌─────────────────┐                            ┌─────────────────┐
│ ModuleSession A1│                            │ ParameterModel  │
└─────────────────┘                            └─────────────────┘
 (Modül Durumları)                              (Canlı Veri Havuzu)
```

### Altın Kurallar:
1. **Veri Okuma:** QML ekranları güncel durum bilgilerini (bağlantı durumu, güç durumu, aktif loglar) `Dispatcher`'ın ve ona bağlı `ModuleSession` nesnelerinin Q_PROPERTY'lerinden okur.
2. **Komut Gönderme:** Arayüzde bir butona basıldığında doğrudan arka plan slotu tetiklenmez. Bunun yerine `Dispatcher.connectModule(ecuId, moduleNumber)` gibi `Q_INVOKABLE` fonksiyonlar çağrılır.
3. **Canlı Parametreler:** Hız, sıcaklık, yakıt akışı gibi canlı CAN telemetri verileri `Dispatcher.parameterModel` içinde depolanır. QML bileşenleri bu verileri `parameterModel.getValue("N1")` şeklinde çekerek kendini günceller.

---

## 2. QML Klasör Yapısı ve Bileşenler

Arayüz kodları `ecu-control/qml/` dizini altındadır:

* **`Main.qml`:** Uygulamanın ana penceresidir. Üst başlığı (Header), Acil Durdurma/Sıralı Başlat butonlarını, Olay Akışı (Log) panelini barındırır. Ekranlar arası geçişleri (Control ↔ Chart ↔ Firmware) yönetir.
* **`EcuPanel.qml`:** 4 adet ECU kartının her birini temsil eder. İçinde bağlantı butonu, mod seçimi (ComboBox), güç anahtarı ve canlı metrik göstergesi (RpmGauge) bulunur.
* **`components/` (Özel Bileşenler):**
  * `RpmGauge.qml`: Metrik değerlerini (N1, EGT, Yakıt, Titreşim) dairesel grafik şeklinde çizen Canvas tabanlı esnek gösterge.
  * `LedIndicator.qml`: Modülün anlık durumuna (Bağlantı yok, aktif, uyarı, kritik hata) göre renk değiştiren ve yavaşça soluyup sönen animasyonlu LED.
  * `PowerSwitchItem.qml`: Modülün elektriksel güç durumunu kontrol eden iki aşamalı (ON/OFF) buton grubu.

---

## 3. Responsive (Esnek) Tasarım ve Binding Kuralları

Ekran boyutu büyüdüğünde veya küçüldüğünde tüm bileşenlerin oransal olarak ölçeklenmesi için aşağıdaki tasarım kuralları uygulanmıştır:

1. **Sabit Piksel (Width/Height) Kullanımından Kaçınma:** Grid ve listelerde `width: 300` gibi sabit değerler yerine `Layout.fillWidth: true` ve `Layout.fillHeight: true` kullanılarak boş alanlar otomatik doldurulur.
2. **Oransal Font ve Boyutlar:** `Main.qml`'deki başlık boyutu gibi metinler, pencere yüksekliğine bağlanmıştır: `font.pixelSize: Math.max(16, window.height * 0.028)`.
3. **Canvas Ölçekleme:** `RpmGauge.qml` içindeki dairesel kadran çizimi, Canvas'ın anlık genişlik ve yüksekliğine göre yarıçapını (`r = Math.min(width * 0.42, height * 0.58)`) hesaplar ve her boyut değişiminde `requestPaint()` ile kendini sıfırdan çizer.

---

## 4. Light / Dark Mode ve Tema Yönetimi (`Theme.qml`)

Mentörün son istediği dinamik renk/tema geçişini uygulamak için önerilen yapı **`Theme.qml`** singleton tasarımıdır.

### Adım 1: Tema Tanımlama (`Theme.qml`)
`qml/components/Theme.qml` adında bir dosya oluşturup renk paletleri şu şekilde ayrıştırılabilir:

```qml
pragma Singleton
import QtQuick 2.15

QtObject {
    id: sharedTheme

    // Aktif tema modu kontrolü
    property bool isDarkMode: true

    // ── Renk Paletleri ──
    readonly property color backgroundColor: isDarkMode ? "#0f1623" : "#f8fafc"
    readonly property color cardColor:       isDarkMode ? "#0d1829" : "#ffffff"
    readonly property color cardBorder:     isDarkMode ? "#1e293b" : "#e2e8f0"
    
    // Metin Renkleri
    readonly property color textPrimary:     isDarkMode ? "#f8fafc" : "#0f172a"
    readonly property color textSecondary:   isDarkMode ? "#64748b" : "#475569"
    
    // Panel ve Log Renkleri
    readonly property color logPanelBg:      isDarkMode ? "#0a1628" : "#f1f5f9"
    readonly property color separatorColor:  isDarkMode ? "#1e3a5f" : "#cbd5e1"
}
```

### Adım 2: QML Dosyalarında Kullanımı
Diğer QML dosyalarında renkleri sabit yazmak yerine bu temaya bağlamak gerekir:

```qml
// Main.qml
Window {
    color: Theme.backgroundColor // Dinamik olarak değişir
    
    Text {
        color: Theme.textPrimary
    }
}
```

### Adım 3: QML'de Tema Değiştirme Butonu
Arayüze eklenecek küçük bir Switch veya Buton yardımıyla tek tıkla tüm arayüzün rengi değiştirilebilir:

```qml
Switch {
    text: "Dark Mode"
    checked: Theme.isDarkMode
    onCheckedChanged: Theme.isDarkMode = checked
}
```

---

## 5. Canlı Gösterge (Metric Gauge) Özelleştirmeleri

Kullanıcı `EcuPanel` üzerindeki gauge'un üstündeki ComboBox'tan hangi veriyi izlemek istediğini seçebilir:
* **N1:** Modül devri (%)
* **EGT:** Egzoz Gazı Sıcaklığı (°C)
* **Yakıt:** Anlık Yakıt Akışı (kg/h)
* **Titreşim:** Motor Titreşim Seviyesi (g)

Seçilen parametrenin limitlerine göre (Örn: N1 için 0-105, EGT için 300-950) kadranın minimum, maksimum değerleri, birim etiketi ve ibre rengi dinamik olarak güncellenir.
