_Pragma("once");

#include <QString>

class ConfigLoader
{
public:
    explicit(true) ConfigLoader();
    ~ConfigLoader() noexcept = default;

public:
    auto setConfigSystem(const QString& _dir, const QString& _file) -> void;

    auto setTomlPath(const QString& _filePath) -> void;

    auto loadFromFile(const QString& _value, const QString& _key = QString{}) -> QString;

private:
    QString m_filePath{};
};
