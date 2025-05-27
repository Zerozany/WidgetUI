#include "StyleLoader.h"

#include <QFile>
#include <QTextStream>

// 定义静态成员函数（推荐保留static）
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
