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

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <QPushButton>
#include <QVector>
#include <QIcon>
#include <QColor>
#include <QMouseEvent>

namespace IconRole
{
    const int Initial = 0;

    const int Hovered = 1 << 0;
    const int Checked = 1 << 1;
    const int Pressed = 1 << 2;

    const int RoleCount = 1 << 3;
}

class Button: public QPushButton
{
    Q_OBJECT
    typedef QPushButton super;
    typedef int IconRole;

    template <class T>
    using TwoD = QVector<QVector<T> >;
public:
    explicit Button(const QPixmap& image, QWidget* parent = 0);

    void setColor(const QColor& color, IconRole role, int iconId = 0);
    // NOTE:
    //  (1) addIcon() returns an iconId of the given QPixmap
    //  (2) An iconId can be used to swapIcon()
    //  (3) The iconId of the image passed as the 1st argument for the constructor
    //      is OriginalIcon which is 0
    int addIcon(const QPixmap& image);
    void swapIcon(int iconId);
    static const int OriginalIcon = 0;
    void setCheckedIcon(const QPixmap& image, int iconId = 0);

    void setIcon(const QIcon &icon) = delete;
    void setText(const QString &text) = delete;
public slots:
    void setChecked(bool checked);
private slots:
    void onClick(bool checked);
protected:
    void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
#ifdef BUTTON_DEBUG
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
#endif
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    QIcon& icon(IconRole role);
    QIcon& icon(IconRole role, int iconId);
private:
    TwoD<QIcon> m_icons = TwoD<QIcon>(1, QVector<QIcon>(IconRole::RoleCount));
    TwoD<QColor> m_colors = TwoD<QColor>(1, QVector<QColor>(IconRole::RoleCount));
    QVector<QPixmap> m_images = QVector<QPixmap>(1);
    QMap<int, QPixmap> m_checkedImages;
    IconRole m_role;
    int m_iconId = OriginalIcon;
    int m_iconCount = 1;

    void setIcon(IconRole role);
    void setIcon(IconRole role, int iconId);
};

#endif
