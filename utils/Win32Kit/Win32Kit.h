_Pragma("once");

#include <QScreen>
#include <QWidget>
#include <QWindow>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "user32.lib")
#endif

class Win32Kit
{
public:
    static auto adjustCustomerArea(const MSG* _msg) noexcept -> void;

    static auto achieveRoundedCorners(const MSG* _msg) noexcept -> HRESULT;

    static auto coordinateMapping(const POINT& _point, const QWidget* _widget, const QWidget* _titleBar) noexcept -> QPoint;

    static auto scalingCorrection(const POINT& _point, const QWindow* _handle) noexcept -> QPoint;
};
