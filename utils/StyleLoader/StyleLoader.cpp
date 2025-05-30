#include "StyleLoader.h"

#include <QFile>
#include <QTextStream>

auto StyleLoader::loadFromFile(const QString& _filePath) -> QString
{
    QFile file{_filePath};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString{};
    }
    QTextStream stream{&file};
    QString     content{stream.readAll()};
    file.close();
    return content;
}
