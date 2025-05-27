_Pragma("once");
#include <QString>

class StyleLoader
{
public:
    static auto loadFromFile(const QString& _filePath) -> QString;
};
