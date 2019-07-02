#pragma once
#include <QOBJECT>
#include <QPushButton>
#include <ui_MemoDlg.h>
#include <ui_MemosDlg.h>
#include <ui_SearchDlg.h>
#include "TextFeatures.h"

class STWindow;
class Scene;
class Document;

///////////////////////////////////////////////////////////////////////////////
class TextUiBtn : public QWidget
{
	Q_OBJECT
public:
	TextUiBtn(const QString & img, const QString & imgPressed);
	virtual QSize sizeHint() const;
signals:
	void clicked();

private:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
private:
	QPixmap img_, imgPressed_;
	QPixmap * curPixmap_;
	
};

///////////////////////////////////////////////////////////////////////////////

class TextUiWidget : public QWidget
{
public:
	TextUiWidget();	
};


///////////////////////////////////////////////////////////////////////////////

class SearchWidget : public QWidget
{
	Q_OBJECT
public:
	SearchWidget();
	virtual QSize sizeHint() const;
	QString searchStr() const {return input_->text();}
	void setText(const QString & str);
private:
	virtual void mousePressEvent(QMouseEvent *);
signals:
	void onSearch();
	void onPrev();
	void onNext();
private:
	virtual void paintEvent(QPaintEvent * event);
	QPixmap bg_;
	QPixmap inputBg_;
	QPixmap inputFg_;
	QLineEdit * input_;
	TextUiBtn * nextBtn_, * prevBtn_, * searchBtn_;
};


///////////////////////////////////////////////////////////////////////////////
class TextSelectUI;

class SearchResultsWidget : public QWidget
{
	Q_OBJECT
public:
	SearchResultsWidget();
	virtual QSize sizeHint() const;
	void setResults(const std::wstring & searchStr, const std::vector<TFSearchResult> & results);
signals:
	void onClose();
	void move(SceneObject *, unsigned, unsigned);


private slots:
	void onMove();

private:
	virtual void mousePressEvent(QMouseEvent *);

private:
	virtual void paintEvent(QPaintEvent * event);
	QPixmap bg_;
	QPixmap title_;	
	TextUiBtn * closeBtn_;

	QWidget * results_;
	QScrollArea * resultsArea_;

	unsigned int len_;
	std::vector<TextUiBtn *> btns_;
	std::vector< std::pair<SceneObject *, unsigned> > indices_;
};
///////////////////////////////////////////////////////////////////////////////
class MemosWidget : public QWidget
{
	Q_OBJECT
public:
	MemosWidget();
	virtual QSize sizeHint() const;
	void populateMemos(TextFeatures * tf);

	virtual void hideEvent(QHideEvent *);
signals:
	void onClose();
	void move(SceneObject *, unsigned);


private slots:
	void onMove();
	void onDel();
private:
	virtual void mousePressEvent(QMouseEvent *);

private:
	virtual void paintEvent(QPaintEvent * event);
	QPixmap bg_;
	QPixmap title_;	
	TextUiBtn * closeBtn_;
	TextFeatures * tf_;
	std::vector<Memo *> memos_;

	QWidget * results_;
	QScrollArea * resultsArea_;	

	std::vector<TextUiBtn *> btns_;
	std::vector<TextUiBtn *> delBtns_;
	std::vector<QWidget *> toDelete_;
	std::vector<QLabel *> labels_;
	std::vector< std::pair<SceneObject *, unsigned> > indices_;
};

///////////////////////////////////////////////////////////////////////////////
class NewMemoWidget : public QWidget
{
	Q_OBJECT
public:
	NewMemoWidget();
	virtual QSize sizeHint() const;

	void setMemo(Memo * memo);
	Memo * memo() const {return memo_;}
	QString text() const {return textEdit_->toPlainText();}
signals:
	void onClose();
	void onSave();
private:
	virtual void mousePressEvent(QMouseEvent *);

private:
	virtual void paintEvent(QPaintEvent * event);
	QPixmap bg_;
	QPixmap title_;	
	QPixmap titleBar_;
	TextUiBtn * closeBtn_;
	TextUiBtn * saveBtn_;
	TextFeatures * tf_;
	QTextEdit * textEdit_;
	Memo * memo_;

};
///////////////////////////////////////////////////////////////////////////////

class TextSelectUI : public QObject
{
	Q_OBJECT
public:
	TextSelectUI(STWindow * window, QWidget * parent = 0);
	~TextSelectUI();
	//QWidget * widget() {return widget_;}

	void memoClicked(Memo * memo);
	void showUi(const QPoint & pt);
	void hideUi();

	void resized();
	
private slots:
	void onPushHighlightBtn();
	void onPushSearchBtn();
	void onPushMemoBtn();
	void onCopy();
	void onGoogle();
	void onHiliteColor();
	void onHiliteDelete();
	
	void onPrev();
	void onNext();
	void onSearchResults();

	void move(SceneObject * text, unsigned int index, unsigned int len);
	void moveMemo(SceneObject * text, unsigned int index);
	void onSearchResClose();
	void onMemosClose();

	void onNewMemo();
	void onNewMemoClose();
	void onNewMemoSave();

	void onMemos();

	void onColor0();
	void onColor1();
	void onColor2();
	void onColor3();
	void onColor4();
protected:
	TextUiBtn * highlightBtn_;	
	TextUiBtn * memoBtn_;
	TextUiBtn * copyBtn_;
	TextUiBtn * searchBtn_;
	TextUiBtn * googleBtn_;
	TextUiBtn * upArrow_;
	TextUiBtn * downArrow_;

	TextUiBtn * newMemoBtn_, *memosBtn_;

	TextUiBtn * memoBtn2_;
	TextUiBtn * hiliteColorBtn_;
	TextUiBtn * deleteBtn_;

	std::vector<TextUiBtn *> colorBtns_;

	bool isOrientTop_;

	QWidget * widget_;
	QWidget * widgetHilite_;
	QWidget * widgetColors_;
	QWidget * widgetMemo_;
	SearchWidget * widgetSearch_;
	SearchResultsWidget * widgetSearchResults_;
	QString lastSearchStr_;

	MemosWidget * widgetMemos_;
	NewMemoWidget * widgetNewMemo_;

	STWindow * mainWindow_;

	enum UiMode
	{
		Start,
		Hilite,
		HiliteColor,
		Search,
		MemoBar,
		SearchResults,		
		Memos,
		NewMemo
	} mode_;

	QPoint pos_;
	bool showing_;
};