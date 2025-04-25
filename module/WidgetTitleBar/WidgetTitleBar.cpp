#include "WidgetTitleBar.h"

#include <QMouseEvent>

#include "WidgetFrame.h"

#ifndef TITLE_HEIGHT
#define TITLE_HEIGHT 30
#endif

#ifndef BORDER_TOP_WIDTH
#define BORDER_TOP_WIDTH 5
#endif

#ifndef BORDER_BOTTOM_WIDTH
#define BORDER_BOTTOM_WIDTH 5
#endif

#ifndef BORDER_LEFT_WIDTH
#define BORDER_LEFT_WIDTH 5
#endif

#ifndef BORDER_RIGHT_WIDTH
#define BORDER_RIGHT_WIDTH 5
#endif

#ifndef CORNER_SIZE
#define CORNER_SIZE 7
#endif

constinit static QRect  g_startGeometry{};   /*当前窗口大小缓存区域*/
constinit static QPoint g_startPoint{};      /*开启伸缩标记点*/
constinit static QPoint g_tmpPoint{};        /*窗口移动事件鼠标缓存点*/
constinit static bool   g_topClicked{false}; /*顶部伸缩指针点击上下伸缩事件*/

WidgetTitleBar::WidgetTitleBar(WidgetFrame* _widget, QWidget* _parent) : QWidget{_parent}, m_widget{_widget}
{
    std::invoke(&WidgetTitleBar::initWidgetTitleBar, this);
    std::invoke(&WidgetTitleBar::conncetSignalToSlot, this);
}

WidgetTitleBar::~WidgetTitleBar() noexcept
{
    delete this;
}

auto WidgetTitleBar::initWidgetTitleBar() noexcept -> void
{
    this->setFixedHeight(30);
    this->setStyleSheet("background-color: red;");
}

auto WidgetTitleBar::setCursorType(const QPoint& _pos) noexcept -> void
{
    const int x{_pos.x()};
    const int y{_pos.y()};
    const int w{m_widget->width()};
    const int h{m_widget->height()};
    // 判断是否在角落（使用更大的 CORNER_SIZE）
    const bool topLeft{(x < CORNER_SIZE && y < CORNER_SIZE)};
    const bool topRight{(x > w - CORNER_SIZE && y < CORNER_SIZE)};
    const bool bottomLeft{(x < CORNER_SIZE && y > h - CORNER_SIZE)};
    const bool bottomRight{(x > w - CORNER_SIZE && y > h - CORNER_SIZE)};
    // 判断是否在边缘（使用较小的 BORDER_*_WIDTH）
    const bool left{(x < BORDER_LEFT_WIDTH) && !topLeft && !bottomLeft};
    const bool right{(x > w - BORDER_RIGHT_WIDTH) && !topRight && !bottomRight};
    const bool top{(y < BORDER_TOP_WIDTH) && !topLeft && !topRight};
    const bool bottom{(y > h - BORDER_BOTTOM_WIDTH) && !bottomLeft && !bottomRight};
    CursorType tmp{};
    if (topLeft)
    {
        tmp = CursorType::TopLeft;
    }
    else if (topRight)
    {
        tmp = CursorType::TopRight;
    }
    else if (bottomLeft)
    {
        tmp = CursorType::BottomLeft;
    }
    else if (bottomRight)
    {
        tmp = CursorType::BottomRight;
    }
    else if (top)
    {
        tmp = CursorType::Top;
    }
    else if (bottom)
    {
        tmp = CursorType::Bottom;
    }
    else if (left)
    {
        tmp = CursorType::Left;
    }
    else if (right)
    {
        tmp = CursorType::Right;
    }
    else
    {
        tmp = CursorType::None;
    }
    if (this->m_cursorType == tmp)
    {
        return;
    }
    this->m_cursorType = tmp;
    emit cursorTypeChanged();
    if (this->m_cursorType == CursorType::None)
    {
        emit titleStatusChanged(false);
    }
    else
    {
        emit titleStatusChanged(true);
    }
}

auto WidgetTitleBar::conncetSignalToSlot() noexcept -> void
{
    connect(this, &WidgetTitleBar::cursorTypeChanged, this, &WidgetTitleBar::setCursorEvent, Qt::AutoConnection);
    connect(this, &WidgetTitleBar::titleStatusChanged, this, &WidgetTitleBar::setTitleStatus, Qt::DirectConnection);
}

void WidgetTitleBar::mousePressSlot(QMouseEvent* _event)
{
    if ((_event->button() & Qt::LeftButton) && !m_widget->isMaximized() && m_cursorType != CursorType::None)
    {
        m_resizing      = true;
        g_startPoint    = _event->globalPos();
        g_startGeometry = m_widget->geometry();
    }

    if ((_event->button() & Qt::LeftButton) && !this->m_resizing && QRect{0, 0, m_widget->width(), this->height()}.contains(_event->pos()))
    {
        m_mouseHandle = true;
        g_tmpPoint    = _event->globalPos() - m_widget->pos();
    }
}

auto WidgetTitleBar::setCursorEvent() noexcept -> void
{
    if (m_widget->isMaximized())
    {
        return;
    }
    switch (this->m_cursorType)
    {
        case CursorType::Top:
        {
            [[fallthrough]];
        }
        case CursorType::Bottom:
        {
            m_widget->setCursor(Qt::SizeVerCursor);
            break;
        }
        case CursorType::Left:
        {
            [[fallthrough]];
        }
        case CursorType::Right:
        {
            m_widget->setCursor(Qt::SizeHorCursor);
            break;
        }
        case CursorType::TopLeft:
        {
            [[fallthrough]];
        }
        case CursorType::BottomRight:
        {
            m_widget->setCursor(Qt::SizeFDiagCursor);
            break;
        }
        case CursorType::TopRight:
        {
            [[fallthrough]];
        }
        case CursorType::BottomLeft:
        {
            m_widget->setCursor(Qt::SizeBDiagCursor);
            break;
        }
        default:
        {
            m_widget->setCursor(Qt::ArrowCursor);
            break;
        }
    }
}

auto WidgetTitleBar::setTitleStatus(const bool _flag) noexcept -> void
{
    if (_flag)
    {
        // QEvent leaveEvent(QEvent::Leave);
        // for (auto& button : m_titleButtons | std::views::values)
        // {
        //     QApplication::sendEvent(button, &leaveEvent);
        //     button->update();
        // }
    }
    this->setAttribute(Qt::WA_TransparentForMouseEvents, _flag);
}

void WidgetTitleBar::mouseMoveSlot(QMouseEvent* _event)
{
    if (QRect end_geometry{g_startGeometry}; m_resizing)
    {
        switch (QPoint tmp_point{_event->globalPos() - g_startPoint}; m_cursorType)
        {
            case CursorType::Top:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                break;
            }
            case CursorType::Bottom:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                break;
            }
            case CursorType::Left:
            {
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::Right:
            {
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            case CursorType::TopLeft:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::TopRight:
            {
                end_geometry.setTop(end_geometry.top() + tmp_point.y());
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            case CursorType::BottomLeft:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                end_geometry.setLeft(end_geometry.left() + tmp_point.x());
                break;
            }
            case CursorType::BottomRight:
            {
                end_geometry.setBottom(end_geometry.bottom() + tmp_point.y());
                end_geometry.setRight(end_geometry.right() + tmp_point.x());
                break;
            }
            default:
            {
                std::unreachable();
            }
        }
        if (end_geometry.width() >= m_widget->minimumWidth() && end_geometry.height() >= m_widget->minimumHeight())
        {
            m_widget->setGeometry(end_geometry);
        }
    }
    else
    {
        this->setCursorType(_event->pos());
    }

    if (m_mouseHandle)
    {
        if (m_widget->isMaximized()) [[Q_UNLIKELY]]
        {
            // 计算当前鼠标在窗口中的横向比例
            float ratio_x{static_cast<float>(_event->pos().x()) / static_cast<float>(m_widget->width())};
            // 记录当前鼠标全局坐标
            QPoint current_globalpos{_event->globalPos()};
            // 取消最大化
            m_widget->showNormal();
            // 计算鼠标应该对应的新窗口位置,保持垂直偏移不变
            m_widget->move(current_globalpos - QPoint(static_cast<int>(ratio_x * m_widget->width()), g_tmpPoint.y()));
            // 更新鼠标拖动偏移
            g_tmpPoint.setX(current_globalpos.x() - m_widget->pos().x());
            g_tmpPoint.setY(this->height() / 2);
        }
        else if (g_topClicked)
        {
            m_widget->resize(m_widget->width(), m_screen->availableGeometry().height() / 2);
            g_topClicked = false;
        }
        else
        {
            m_widget->move(_event->globalPos() - g_tmpPoint);
        }
    }
}

void WidgetTitleBar::mouseReleaseSlot(QMouseEvent* _event)
{
    this->m_resizing    = false;
    this->m_mouseHandle = false;
}

void WidgetTitleBar::mouseDoubleClickSlot(QMouseEvent* _event)
{
    if (_event->button() == Qt::LeftButton)
    {
        if (QRect{0, 0, m_widget->width(), this->height()}.contains(_event->pos()) && m_cursorType == CursorType::None)
        {
            m_widget->isMaximized() ? m_widget->showNormal() : m_widget->showMaximized();
        }
        if (m_cursorType == CursorType::Top || m_cursorType == CursorType::Bottom)
        {
            m_widget->move(m_widget->x(), 0);
            // 返回扣除任务栏后的可用区域
            m_widget->resize(m_widget->width(), m_screen->availableGeometry().height());
            g_topClicked = true;
        }
    }
}

void WidgetTitleBar::mouseLeaveSlot(QMouseEvent* _event)
{
    m_widget->setCursor(Qt::ArrowCursor);
}
