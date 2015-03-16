#ifndef SHOPS_H
#define SHOPS_H

#include <QMap>
#include <QByteArray>
#include <cstdint>

QMap<uint32_t, uint32_t> parseShopsXml(QByteArray data);

#endif // SHOPS_H
