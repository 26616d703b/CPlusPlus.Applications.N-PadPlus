#ifndef N_PAD_PLUS_TEXTAREA_H
#define N_PAD_PLUS_TEXTAREA_H

#include <QWidget>
#include <QIcon>
#include <QTabBar>
#include <QPainter>
#include <QPaintEvent>
#include <QSize>
#include <QResizeEvent>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBlock>

class TextArea : public QPlainTextEdit {

    Q_OBJECT

     public:
         TextArea(QWidget *parent = 0);
         ~TextArea();

         // Accessors and mutatots
         QString filePath()const;
         void setFilePath(const QString &path);
         bool isRecorded() const;
         void setRecorded(bool ok = true);

         // Other
         void lineNumberAreaPaintEvent(QPaintEvent *event);
         int lineNumberAreaWidth();

         // Events
     protected:
         void resizeEvent(QResizeEvent *event);
         void closeEvent(QCloseEvent *event);

     private slots:
         void highlightCurrentLine();
         void updateLineNumberArea(const QRect &, int);
         void updateLineNumberAreaWidth(int newBlockCount);

     private:
         QWidget *m_lineNumberArea;
         QString m_filePath;
         bool m_fileRecorded;
};

class lineNumberArea : public QWidget {

    public:
        lineNumberArea(TextArea *editor) : QWidget(editor) {
            m_textArea = editor;
        }

        QSize sizeHint() const {
            return QSize(m_textArea->lineNumberAreaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent *event) {
            m_textArea->lineNumberAreaPaintEvent(event);
        }

    private:
        TextArea *m_textArea;
 };

#endif  // !N_PAD_PLUS_TEXTAREA_H
