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

#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QPlainTextEdit>
#include <QString>
#include <QFocusEvent>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class TextEdit: public QPlainTextEdit {
Q_OBJECT
    typedef QPlainTextEdit super;
public:
    explicit TextEdit(QWidget* parent = 0): super(parent) {}
    explicit TextEdit(const QString& text, QWidget* parent = 0): super(text, parent) {}
signals:
    void focusChanged(bool focused);
protected:
    void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
};

#endif
