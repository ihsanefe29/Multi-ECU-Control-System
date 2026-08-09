#ifndef ECUEVENT_H
#define ECUEVENT_H

#include <QString>
#include <QMetaType>

// Bir olayın ciddiyet seviyesi (panelde renk bununla belirlenecek)
enum class EventSeverity {
    Info,       // bilgi (mavi)  - normal komut
    Warning,    // uyarı (turuncu) - önemsiz sorun
    Error,      // hata (kırmızı) - bağlantı koptu vs.
    Critical    // kritik (koyu kırmızı) - fail-safe tetikleyen
};

// Hatanın türü (her türe farklı tepki vereceğiz)
enum class EventType {
    Command,          // normal komut (hata değil)
    ConnectionLost,   // bağlantı koptu
    InvalidData,      // bozuk / aralık dışı veri
    FailSafe          // kritik durum, güvenli moda geçiş
};

#endif // ECUEVENT_H