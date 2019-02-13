/****************************************************************************
**
** Copyright (C) 2019 Albert Hsu
**
** This file is part of Dictation.
**
** Dictation is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Dictation is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Dictation.  If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifdef  LOG
#error  "LOG should not be defined"
#else
#ifdef  BUTTON_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "button.h"
#include "utils/debug.h"
#include "utils/color.h"

Button::Button(const QPixmap& image, QWidget* parent): super(parent)
{
    m_images[OriginalIcon] = image;

    const IconRole role = IconRole::Initial;
    icon(role) = image;
    setIcon(role);

    setStyleSheet("Button {padding:0px; border:none;}");

    CONNECT(this, &Button::clicked, this, &Button::onClick);
}

void Button::setColor(const QColor& color, IconRole role, int iconId)
{
    Q_ASSERT(role >= 0 && role < IconRole::RoleCount);
    Q_ASSERT(iconId >= 0 && iconId < m_iconCount);
    Q_ASSERT(color.isValid());

    m_colors[iconId][role] = color;

    QPixmap image;
    QMap<int, QPixmap>::const_iterator it;
    if (role & IconRole::Checked &&
        (it = m_checkedImages.find(iconId)) != m_checkedImages.end()) {
        image = it.value();
    } else {
        image = m_images.at(iconId);
    }

    icon(role, iconId) = Color::fillColor(image, color);
    if (role == m_role && iconId == m_iconId)
        setIcon(role, iconId);
}

QIcon& Button::icon(IconRole role)
{
    return icon(role, m_iconId);
}

QIcon& Button::icon(IconRole role, int iconId)
{
    Q_ASSERT(role >= 0 && role < IconRole::RoleCount);
    Q_ASSERT(iconId >= 0 && iconId < m_iconCount);
    return m_icons[iconId][role];
}

void Button::enterEvent(QEvent* event)
{
    LOG << event;
    IconRole role = IconRole::Initial;
    if (isCheckable() && isChecked())
        role |= IconRole::Checked;

    // To test if the method is invoked by Button::MouseMoveEvent()
    const QIcon& icon = dynamic_cast<QMouseEvent*>(event) ?
        this->icon((role |= IconRole::Hovered | IconRole::Pressed)) :
        this->icon((role |= IconRole::Hovered));
    if ( ! icon.isNull())
        setIcon(role);
    else
        super::enterEvent(event);
}

void Button::leaveEvent(QEvent* event)
{
    LOG;
    IconRole role = IconRole::Initial;
    if (isCheckable() && isChecked())
        role |= IconRole::Checked;

    if ( ! this->icon(role | IconRole::Hovered).isNull())
        setIcon(role);
    else
        super::leaveEvent(event);
}

void Button::mousePressEvent(QMouseEvent* event)
{
    LOG;
    if (event->button() != Qt::LeftButton) {
        super::mousePressEvent(event);
        return;
    }

    IconRole role = IconRole::Initial;
    if (isCheckable() && isChecked())
        role |= IconRole::Checked;

    const QIcon& icon = this->icon((role |= IconRole::Hovered | IconRole::Pressed));
    if ( ! icon.isNull())
        setIcon(role);
    // NOTE: Invoke the inherited mousePressEvent to trigger clicked(bool) signal
    super::mousePressEvent(event);
}

#ifdef BUTTON_DEBUG
void Button::mouseReleaseEvent(QMouseEvent* event)
{
    LOG;
    super::mouseReleaseEvent(event);
}
#endif

void Button::mouseMoveEvent(QMouseEvent* event)
{
    LOG << rect().contains(event->pos());

    // Preventing duplicate invocations of enter or leaveEvents
    static bool hovering = true;

    // NOTE: QMouseEvent::button() always returns Qt::NoButton for mouseMoveEvents
    if ( ! (event->buttons() & Qt::LeftButton)) return;

    if (hovering != rect().contains(event->pos())) // Logical XOR
        (hovering = ! hovering) ? enterEvent(event) :leaveEvent(event);

    // NOTE: leaveEvent may get triggered twice, where once is expected,
    //  during a drag & drop movement. But it's OK
}

void Button::onClick(bool checked)
{
    LOG << checked;

    // NOTE: (1) QAbstractButton::nextCheckState() is invoked when button is clicked
    //  (2) toggled(bool checked) is triggered when setChecked(bool checked) is called
    IconRole role = IconRole::Initial;
    if (isCheckable() && isChecked())
        role |= IconRole::Checked;
    // NOTE: Button::onClick(bool) can be invoked directly or by a signal,
    //  thus the button is not nessesarily being hovered at that moment
    const IconRole maybeHovered = m_role & IconRole::Hovered;
    const QIcon& icon = this->icon(role | maybeHovered);
    if ( ! icon.isNull())
        role |= maybeHovered;
    setIcon(role);
}

void Button::setIcon(IconRole role, int iconId)
{
    super::setIcon(icon((m_role = role), iconId));
}

void Button::setIcon(IconRole role)
{
    setIcon(role, m_iconId);
}

int Button::addIcon(const QPixmap& image)
{
    const int iconId = m_iconCount++;

    m_images.append(image);
    m_icons.append(QVector<QIcon>(IconRole::RoleCount));
    m_colors.append(QVector<QColor>(IconRole::RoleCount));
    for (int i = 0; i < IconRole::RoleCount; ++i) {
        const QColor& color = m_colors.at(0).at(i);
        if (color.isValid())
            setColor(color, i, iconId);
    }

    return iconId;
}

void Button::swapIcon(int iconId)
{
    Q_ASSERT(iconId >= 0 && iconId < m_iconCount);

    if (m_iconId != iconId)
        setIcon(m_role, (m_iconId = iconId));
}

// NOTE: Must be invoked before calling setColor()
void Button::setCheckedIcon(const QPixmap& image, int iconId)
{
    Q_ASSERT(iconId >= 0 && iconId < m_iconCount);
    for (int i = 0; i < IconRole::RoleCount; ++i)
        Q_ASSERT( ! m_colors.at(iconId).at(i).isValid());

    m_checkedImages.insert(iconId, image);
}

void Button::setChecked(bool checked)
{
    if (checked == isChecked()) return;
    super::setChecked(checked);
    onClick(checked);
}

#undef LOG
