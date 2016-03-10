#include "textarea.h"

                                                // CONSTRUCTOR

TextArea::TextArea(QWidget *parent) : QPlainTextEdit(parent),
                                      m_fileRecorded(true)
{
    m_lineNumberArea = new lineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

                                                // DESTRUCTOR

TextArea::~TextArea() {

    delete m_lineNumberArea;
}

                                                // ACCESSORS and MUTATORS


QString TextArea::filePath() const {

    return m_filePath;
}

void TextArea::setFilePath(const QString &path) {

    m_filePath = path;
}

bool TextArea::isRecorded() const {

    return (m_fileRecorded) ? true : false;
}

void TextArea::setRecorded(bool ok) {

    m_fileRecorded = ok;
}

                                                // OTHER

void TextArea::lineNumberAreaPaintEvent(QPaintEvent *event) {

     QPainter painter(m_lineNumberArea);
     painter.fillRect(event->rect(), Qt::gray);

     QTextBlock block = firstVisibleBlock();
     int blockNumber = block.blockNumber();
     int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
     int bottom = top + (int) blockBoundingRect(block).height();

     while (block.isValid() && top <= event->rect().bottom()) {

         if (block.isVisible() && bottom >= event->rect().top()) {

             QString number = QString::number(blockNumber + 1);
             painter.setPen(Qt::white);
             painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(), Qt::AlignCenter, number);
         }

         block = block.next();
         top = bottom;
         bottom = top + (int) blockBoundingRect(block).height();
         ++blockNumber;
     }
}

int TextArea::lineNumberAreaWidth() {

     int max = qMax(1, blockCount());
     int digits = 1;

     while (max >= 10) {

         max /= 10;
         ++digits;
     }

     int space = 3 + fontMetrics().width(QLatin1String("30")) * digits;

     return space;
}

                                            //EVENTS

void TextArea::resizeEvent(QResizeEvent *e) {

     QPlainTextEdit::resizeEvent(e);
     QRect cr = contentsRect();

     m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void TextArea::closeEvent(QCloseEvent *event) {

    event->accept();
}


                                            // SLOTS

void TextArea::highlightCurrentLine() {

     QList<QTextEdit::ExtraSelection> extraSelections;

     if (!isReadOnly()){

         QTextEdit::ExtraSelection selection;
         QColor lineColor = QColor(Qt::green).lighter(160);

         selection.format.setBackground(lineColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);
         selection.cursor = textCursor();
         selection.cursor.clearSelection();
         extraSelections.append(selection);
     }

     setExtraSelections(extraSelections);
}

void TextArea::updateLineNumberArea(const QRect &rect, int dy)
{
     if (dy) {

         m_lineNumberArea->scroll(0, dy);
     }
     else {

         m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
     }

     if (rect.contains(viewport()->rect()))
         updateLineNumberAreaWidth(0);
}

void TextArea::updateLineNumberAreaWidth(int /* newBlockCount */) {

     setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
