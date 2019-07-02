#include "stdafx.h"
#include "TextSelectUI.h"
#include "STWindow.h"
#include "Text.h"
#include "Document.h"
#include "TextFeatures.h"
#include "DocumentTemplate.h"
#include "Utils.h"
#include "Global.h"
#include "PcScene.h"
#include <boost/regex.hpp>
#include "Text_2.h"

using namespace std;

void regexReplace(
	std::wstring * str, const wchar_t * searchRegEx, const wchar_t * replaceStr)
{
	using namespace boost;
	std::wstringstream ss;
	std::ostream_iterator<wchar_t, wchar_t> ssi(ss);
	boost::regex_replace(ssi, str->begin(), str->end(), 
		wregex(searchRegEx), replaceStr, 
		match_default | format_all | match_default);	
	*str = ss.str();
}

///////////////////////////////////////////////////////////////////////////////

TextUiBtn::TextUiBtn(const QString & img, const QString & imgPressed) :
img_(img), imgPressed_(imgPressed)
{
	curPixmap_ = &img_;	
}

void TextUiBtn::paintEvent(QPaintEvent * )
{
	QPainter p(this);
	p.drawPixmap(0, 0, img_.width(), img_.height(), *curPixmap_);

}

void TextUiBtn::mousePressEvent(QMouseEvent *)
{
	curPixmap_ = &imgPressed_;
	update();
}

void TextUiBtn::mouseReleaseEvent(QMouseEvent *)
{
	bool emitClicked = false;
	if (curPixmap_ == &imgPressed_) emitClicked = true;
	curPixmap_ = &img_;	
	update();
	if (emitClicked) emit clicked();
}

void TextUiBtn::mouseMoveEvent(QMouseEvent * event)
{
	QPoint pos = event->pos();
	if (pos.x() < 0 || pos.x() > width() || pos.y() < 0 || pos.y() > height())
		curPixmap_ = &img_;
	else
		curPixmap_ = &imgPressed_;
	
	update();
}

QSize TextUiBtn::sizeHint() const
{
	return img_.size();
}

TextUiWidget::TextUiWidget()
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);
}


///////////////////////////////////////////////////////////////////////////////

SearchWidget::SearchWidget()
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	bg_ = QPixmap(":/PlayerQt/Resources/image_pad/menu/back.png");
	inputBg_ = QPixmap(":/PlayerQt/Resources/image_pad/menu/search_bar_back.png");
	inputFg_ = QPixmap(":/PlayerQt/Resources/image_pad/menu/search_bar.png");

	prevBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/previous_bt.png", ":/PlayerQt/Resources/image_pad/menu/previous_bt_touch.png");
	connect(prevBtn_, SIGNAL(clicked()), this, SIGNAL(onPrev()));
	nextBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/next_bt.png", ":/PlayerQt/Resources/image_pad/menu/next_bt_touch.png");
	connect(nextBtn_, SIGNAL(clicked()), this, SIGNAL(onNext()));
	searchBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/search_bt.png", ":/PlayerQt/Resources/image_pad/menu/search_bt_touch.png");
	connect(searchBtn_, SIGNAL(clicked()), this, SIGNAL(onSearch()));
	
	searchBtn_->setParent(this);
	searchBtn_->show();
	searchBtn_->move(314, bg_.height()/2 - searchBtn_->height()/2);
	
	prevBtn_->setParent(this);
	prevBtn_->show();

	nextBtn_->setParent(this);
	nextBtn_->show();  

	prevBtn_->move(
		bg_.width() - 2* nextBtn_->width() - 10, 
		bg_.height() / 2 - prevBtn_->height() / 2);	
	       
	nextBtn_->move(
		bg_.width() - nextBtn_->width() - 10, 
		bg_.height() / 2 - nextBtn_->height() / 2);

	input_ = new QLineEdit(this);
	input_->show();	
	input_->setGeometry(28, bg_.height()/2 - inputFg_.height()/2, inputFg_.width()-10, inputFg_.height());
	
	QPalette p = palette();
	p.setBrush(input_->backgroundRole(), QBrush(Qt::transparent));
	input_->setPalette(p);
	input_->setFrame(false);
	
	
}

void SearchWidget::setText(const QString & str)
{
	input_->setFocus();
	input_->setText(str);
}
 
void SearchWidget::mousePressEvent(QMouseEvent * event)
{
	event->accept();
}

QSize SearchWidget::sizeHint() const
{
	return QSize(bg_.width(), bg_.height());
}

void SearchWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, bg_.width(), bg_.height(),  bg_);
	p.drawPixmap(5, bg_.height()/2 - inputBg_.height()/2, inputBg_.width(), inputBg_.height(), inputBg_);
	p.drawPixmap(23, bg_.height()/2 - inputFg_.height()/2, inputFg_.width(), inputFg_.height(), inputFg_);
}
///////////////////////////////////////////////////////////////////////////////

SearchResultsWidget::SearchResultsWidget()
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	bg_ = QPixmap(":/PlayerQt/Resources/image_pad/list/search_back_land.png");
	title_ = QPixmap(":/PlayerQt/Resources/image_pad/list/search_text.png");

	closeBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/list/close_bt.png", ":/PlayerQt/Resources/image_pad/list/close_bt_touch.png");
	connect(closeBtn_, SIGNAL(clicked()), this, SIGNAL(onClose()));
	
	closeBtn_->setParent(this);
	closeBtn_->show();
	closeBtn_->move(554, 23);

	resultsArea_ = new QScrollArea(this);	
	resultsArea_->setGeometry(30, 77, 586, 620);
	
	QPalette p;
	p.setBrush(resultsArea_->backgroundRole(), Qt::white);
	resultsArea_->setPalette(p);
	resultsArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	resultsArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	results_ = NULL;
}

QSize SearchResultsWidget::sizeHint() const
{
	return QSize(bg_.width(), bg_.height());
}

void SearchResultsWidget::mousePressEvent(QMouseEvent * event)
{
	event->accept();
}

void SearchResultsWidget::setResults(const std::wstring & searchStr, const std::vector<TFSearchResult> & results)
{
	int rw = resultsArea_->viewport()->width();
	int rowHeight = 49;
	int numRes = 0;

	for (int i = 0; i < (int)results.size(); ++i)
	{
		for (int j = 0; j < (int)results[i].indices.size(); ++j) numRes++;
	}

	delete results_;
	results_ = new QWidget;	
	QPalette p;
	p.setBrush(results_->backgroundRole(), QBrush(QImage(":/PlayerQt/Resources/image_pad/list/listBg.png")));
	results_->setPalette(p);
	results_->setMinimumSize(rw, rowHeight * numRes);	
	btns_.clear();
	indices_.clear();
	numRes = 0;

	len_ = searchStr.size();

	for (int i = 0; i < (int)results.size(); ++i)
	{
		for (int j = 0; j < (int)results[i].indices.size(); ++j)
		{
			TextUiBtn * btn = new TextUiBtn(":/PlayerQt/Resources/image_pad/list/move_bt.png", ":/PlayerQt/Resources/image_pad/list/move_bt_touch.png");
			connect(btn, SIGNAL(clicked()), this, SLOT(onMove()));
			btns_.push_back(btn);
			btn->setParent(results_);
			btn->show();
			int btnRMargin = 8;
			int x = rw - btnRMargin - btn->width();
			int y = numRes * rowHeight + rowHeight/2 - btn->height()/2;
			btn->move(x, y);

			QLabel * label = new QLabel;

			wstring str = L"";
			if(results[i].text->type() == Text().type())
				str = ((Text*) results[i].text)->textString();
			else if(results[i].text->type() == Text_2().type())
				str = ((Text_2*) results[i].text)->textString();

			int startIndex = results[i].indices[j];
			int endIndex = startIndex + searchStr.size();

			startIndex = results[i].indices[j] - 30;
			endIndex = results[i].indices[j] + searchStr.size() + 30;

			if (startIndex < 0) startIndex = 0;
			if (endIndex > str.size()) endIndex = str.size();

			wstring substr = str.substr(startIndex, endIndex - startIndex);

			regexReplace(&substr, L"[\\n\\r\\t]", L" ");
			//qt rich text doesn't support things like &lt;
			//regexReplace(&substr, L"<", L"&lt;");
			//regexReplace(&substr, L">", L"&gt;");
			//regexReplace(&substr, L"&", L"&amp;");

			substr.insert(results[i].indices[j] - startIndex + searchStr.size(), L"</font>");
			substr.insert(results[i].indices[j] - startIndex, L"<font color='#ff0000'>");
			label->setFont(QFont("", 12, 2));
			label->setTextFormat(Qt::RichText);
			label->setText(QString::fromStdWString(substr));
			label->setParent(results_);
			label->show();
			x = 3;
			y = numRes * rowHeight;
			label->setGeometry(x, y, rw - x - btn->width() - btnRMargin - 3, rowHeight);

			numRes++;

			indices_.push_back(make_pair(results[i].text, results[i].indices[j]));
		}		
	}	
	resultsArea_->setWidgetResizable(true);
	resultsArea_->setWidget(results_);
}


void SearchResultsWidget::onMove()
{
	QObject * obj = sender();
	for (int i = 0; i < (int)btns_.size(); ++i)
	{
		if (btns_[i] == obj)
		{
			emit move(indices_[i].first, indices_[i].second, len_);
		}
	}
}

void SearchResultsWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, bg_.width(), bg_.height(),  bg_);
	p.drawPixmap(22, 22, title_.width(), title_.height(),  title_);

}
///////////////////////////////////////////////////////////////////////////////
MemosWidget::MemosWidget()
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	bg_ = QPixmap(":/PlayerQt/Resources/image_pad/list/search_back_land.png");
	title_ = QPixmap(":/PlayerQt/Resources/image_pad/list/memolist_text.png");

	closeBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/list/close_bt.png", ":/PlayerQt/Resources/image_pad/list/close_bt_touch.png");
	connect(closeBtn_, SIGNAL(clicked()), this, SIGNAL(onClose()));
	
	closeBtn_->setParent(this);
	closeBtn_->show();
	closeBtn_->move(554, 23);

	resultsArea_ = new QScrollArea(this);	
	resultsArea_->setGeometry(30, 77, 586, 620);
	
	QPalette p;
	p.setBrush(resultsArea_->backgroundRole(), Qt::white);
	resultsArea_->setPalette(p);
	resultsArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	resultsArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	results_ = NULL;
}

QSize MemosWidget::sizeHint() const
{
	return QSize(bg_.width(), bg_.height());
}

void MemosWidget::mousePressEvent(QMouseEvent * event)
{
	event->accept();
}

void MemosWidget::populateMemos(TextFeatures * tf)
{
	tf_ = tf;
	int rw = resultsArea_->viewport()->width();
	int rowHeight = 49;
	int numRes = 0;

	vector<Memo *> memos;
	tf->getMemos(memos);

	if (memos_ == memos) return;
	memos_ = memos;

	numRes = (int)memos.size();

	delete results_;
	results_ = new QWidget;	
	QPalette p;
	p.setBrush(results_->backgroundRole(), QBrush(QImage(":/PlayerQt/Resources/image_pad/list/listBg.png")));
	results_->setPalette(p);
	results_->setMinimumSize(rw, rowHeight * numRes);	
	btns_.clear();
	delBtns_.clear();
	indices_.clear();
	labels_.clear();
	numRes = 0;

	for (int i = 0; i < (int)memos.size(); ++i)
	{		
		TextUiBtn * btn = new TextUiBtn(":/PlayerQt/Resources/image_pad/list/move_bt.png", ":/PlayerQt/Resources/image_pad/list/move_bt_touch.png");
		connect(btn, SIGNAL(clicked()), this, SLOT(onMove()));
		btns_.push_back(btn);
		btn->setParent(results_);
		btn->show();
		int btnRMargin = 8;
		int x = rw - btnRMargin - btn->width();
		int y = numRes * rowHeight + rowHeight/2 - btn->height()/2;
		btn->move(x, y);

		int btnSpacing = 3;

		TextUiBtn * delBtn = new TextUiBtn(":/PlayerQt/Resources/image_pad/list/delete_bt.png", ":/PlayerQt/Resources/image_pad/list/delete_bt_touch.png");
		connect(delBtn, SIGNAL(clicked()), this, SLOT(onDel()));
		delBtns_.push_back(delBtn);
		delBtn->setParent(results_);
		delBtn->show();		
		x = rw - btnRMargin - btn->width() - delBtn->width() - btnSpacing;
		delBtn->move(x, y);

		QLabel * label = new QLabel;
		labels_.push_back(label);
		label->setFont(QFont("", 12, 2));
		label->setText(QString::fromUtf8(memos[i]->memo.c_str()));
		label->setParent(results_);
		label->show();
		
		x = 3;
		y = numRes * rowHeight;
		label->setGeometry(x, y, rw - x - btn->width() - delBtn->width() - btnRMargin - btnSpacing - 3, rowHeight);

		numRes++;
		indices_.push_back(make_pair(memos[i]->textObj, memos[i]->charIndex));				
	}	
	resultsArea_->setWidgetResizable(true);
	resultsArea_->setWidget(results_);
}

void MemosWidget::onMove()
{
	QObject * obj = sender();
	for (int i = 0; i < (int)btns_.size(); ++i)
	{
		if (btns_[i] == obj)
		{
			emit move(indices_[i].first, indices_[i].second);
		}
	}
}

void MemosWidget::hideEvent(QHideEvent *)
{
	for (int i = 0; i < (int)toDelete_.size(); ++i)
	{
		delete toDelete_[i];
	}

	toDelete_.clear();
}

void MemosWidget::onDel()
{
	QObject * obj = sender();
	for (int i = 0; i < (int)delBtns_.size(); ++i)
	{
		if (delBtns_[i] == obj)
		{
			tf_->deleteMemo(memos_[i]);

			memos_.erase(memos_.begin() + i);

			toDelete_.push_back(delBtns_[i]);
			delBtns_[i]->setParent(NULL);
			delBtns_.erase(delBtns_.begin() + i);
			
			delete btns_[i];
			btns_.erase(btns_.begin() + i);

			delete labels_[i];
			labels_.erase(labels_.begin() + i);
						
			indices_.erase(indices_.begin() + i);
			break;			
		}
	}

	//rearrange
	int rw = resultsArea_->viewport()->width();
	int rowHeight = 49;
	int numRes = 0;

	numRes = (int)memos_.size();
	results_->setMinimumSize(rw, rowHeight * numRes);	
	numRes = 0;

	for (int i = 0; i < (int)memos_.size(); ++i)
	{				
		int btnRMargin = 8;
		int x = rw - btnRMargin - btns_[i]->width();
		int y = numRes * rowHeight + rowHeight/2 - btns_[i]->height()/2;
		btns_[i]->move(x, y);

		int btnSpacing = 3;
		x = rw - btnRMargin - btns_[i]->width() - delBtns_[i]->width() - btnSpacing;
		delBtns_[i]->move(x, y);
	
		x = 3;
		y = numRes * rowHeight;
		labels_[i]->setGeometry(x, y, rw - x - btns_[i]->width() - delBtns_[i]->width() - btnRMargin - btnSpacing - 3, rowHeight);
		numRes++;			
	}	
}

void MemosWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, bg_.width(), bg_.height(),  bg_);
	p.drawPixmap(22, 22, title_.width(), title_.height(),  title_);
}

///////////////////////////////////////////////////////////////////////////////

NewMemoWidget::NewMemoWidget()
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	bg_ = QPixmap(":/PlayerQt/Resources/image_pad/mback_land.png");
	title_ = QPixmap(":/PlayerQt/Resources/image_pad/list/memo_text.png");
	titleBar_ = QPixmap(":/PlayerQt/Resources/image_pad/mtop.png");

	closeBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/mclose_bt.png", ":/PlayerQt/Resources/image_pad/mclose_bt_touch.png");
	connect(closeBtn_, SIGNAL(clicked()), this, SIGNAL(onClose()));

	closeBtn_->setParent(this);
	closeBtn_->show();
	closeBtn_->move(530, 32);

	saveBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/msave_bt.png", ":/PlayerQt/Resources/image_pad/msave_bt_touch.png");
	connect(saveBtn_, SIGNAL(clicked()), this, SIGNAL(onSave()));

	saveBtn_->setParent(this);
	saveBtn_->show();
	saveBtn_->move(34, 32);
	
	textEdit_ = new QTextEdit(this);
	textEdit_->setGeometry(45, 95, 540, 278);
	QPalette p = palette();
	p.setBrush(textEdit_->backgroundRole(), QBrush(Qt::transparent));
	textEdit_->setPalette(p);
}

QSize NewMemoWidget::sizeHint() const
{
	return QSize(bg_.width(), bg_.height());
}

void NewMemoWidget::mousePressEvent(QMouseEvent * event)
{
	event->accept();
}

void NewMemoWidget::setMemo(Memo * memo)
{
	memo_ = memo;
	if (memo_)	
		textEdit_->setText(QString::fromUtf8(memo->memo.c_str()));	
	else
		textEdit_->clear();
}

void NewMemoWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, bg_.width(), bg_.height(),  bg_);
	p.drawPixmap(0, -1, titleBar_.width(), titleBar_.height(),  titleBar_);
	p.drawPixmap(bg_.width() / 2 - title_.width() / 2, 35, title_.width(), title_.height(),  title_);
}

///////////////////////////////////////////////////////////////////////////////

TextSelectUI::TextSelectUI(STWindow * window, QWidget * parent) : QObject(parent), mainWindow_(window)
{
	showing_ = false;

	widget_ = new TextUiWidget;
	widgetHilite_ = new TextUiWidget;
	widgetMemo_ = new TextUiWidget;
	widgetColors_ = new TextUiWidget;
	widgetSearch_ = new SearchWidget;
	widgetSearchResults_ = new SearchResultsWidget;
	widgetMemos_ = new MemosWidget;
	widgetNewMemo_ = new NewMemoWidget;
	upArrow_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/arrow.png", ":/PlayerQt/Resources/image_pad/menu/arrow.png");
	downArrow_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/arrow_2.png", ":/PlayerQt/Resources/image_pad/menu/arrow_2.png");

	connect(widgetNewMemo_, SIGNAL(onClose()), this, SLOT(onNewMemoClose()));
	connect(widgetNewMemo_, SIGNAL(onSave()), this, SLOT(onNewMemoSave()));

	connect(widgetSearch_, SIGNAL(onSearch()), this, SLOT(onSearchResults()));
	connect(widgetSearch_, SIGNAL(onPrev()), this, SLOT(onPrev()));
	connect(widgetSearch_, SIGNAL(onNext()), this, SLOT(onNext()));

	qRegisterMetaType<Text *>("Text *");
	connect(widgetSearchResults_, SIGNAL(move(Text *, unsigned, unsigned)), this, SLOT(move(Text *, unsigned, unsigned)));
	connect(widgetSearchResults_, SIGNAL(onClose()), this, SLOT(onSearchResClose()));

	connect(widgetMemos_, SIGNAL(move(Text *, unsigned)), this, SLOT(moveMemo(Text *, unsigned)));
	connect(widgetMemos_, SIGNAL(onClose()), this, SLOT(onMemosClose()));
	
	upArrow_->setAttribute(Qt::WA_OpaquePaintEvent, true);
	downArrow_->setAttribute(Qt::WA_OpaquePaintEvent, true);

	QGraphicsProxyWidget * added = window->scene()->addWidget(widget_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(downArrow_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(upArrow_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetHilite_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetMemo_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetColors_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetSearch_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetSearchResults_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetMemos_);	
	added->setZValue(100.0);
	added = window->scene()->addWidget(widgetNewMemo_);	
	added->setZValue(100.0);

	widget_->hide();
	upArrow_->hide();
	downArrow_->hide();
	widgetHilite_->hide();
	widgetMemo_->hide();
	widgetColors_->hide();
	widgetSearch_->hide();
	widgetSearchResults_->hide();
	widgetMemos_->hide();
	widgetNewMemo_->hide();
	
	highlightBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/1_highlite.png", ":/PlayerQt/Resources/image_pad/menu/1_highlite_touch.png");
	connect(highlightBtn_, SIGNAL(clicked()), this, SLOT(onPushHighlightBtn())); 
	memoBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/2_memo.png", ":/PlayerQt/Resources/image_pad/menu/2_memo_touch.png");
	connect(memoBtn_, SIGNAL(clicked()), this, SLOT(onPushMemoBtn()));
	copyBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/3_copy.png", ":/PlayerQt/Resources/image_pad/menu/3_copy_touch.png");
	connect(copyBtn_, SIGNAL(clicked()), this, SLOT(onCopy()));
	searchBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/4_search.png", ":/PlayerQt/Resources/image_pad/menu/4_search_touch.png");
	connect(searchBtn_, SIGNAL(clicked()), this, SLOT(onPushSearchBtn())); 
	googleBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/5_google.png", ":/PlayerQt/Resources/image_pad/menu/5_google_touch.png");
	connect(googleBtn_, SIGNAL(clicked()), this, SLOT(onGoogle()));	

	newMemoBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/newmemo_bt.png", ":/PlayerQt/Resources/image_pad/menu/newmemo_bt_touch.png");
	connect(newMemoBtn_, SIGNAL(clicked()), this, SLOT(onNewMemo()));
	memosBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/memolist_bt.png", ":/PlayerQt/Resources/image_pad/menu/memolist_bt_touch.png");
	connect(memosBtn_, SIGNAL(clicked()), this, SLOT(onMemos()));

	memoBtn2_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/2_memo.png", ":/PlayerQt/Resources/image_pad/menu/2_memo_touch.png");
	connect(memoBtn2_, SIGNAL(clicked()), this, SLOT(onPushMemoBtn()));
	hiliteColorBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/6_highlite.png", ":/PlayerQt/Resources/image_pad/menu/6_highlite_touch.png");
	connect(hiliteColorBtn_, SIGNAL(clicked()), this, SLOT(onHiliteColor()));
	deleteBtn_ = new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/8_delete.png", ":/PlayerQt/Resources/image_pad/menu/8_delete_touch.png");
	connect(deleteBtn_, SIGNAL(clicked()), this, SLOT(onHiliteDelete())); 


	colorBtns_.push_back(new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/9_red.png", ":/PlayerQt/Resources/image_pad/menu/9_red_touch.png"));
	connect(colorBtns_.back(), SIGNAL(clicked()), this, SLOT(onColor0()));
	colorBtns_.push_back(new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/10_orange.png", ":/PlayerQt/Resources/image_pad/menu/10_orange_touch.png"));
	connect(colorBtns_.back(), SIGNAL(clicked()), this, SLOT(onColor1()));
	colorBtns_.push_back(new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/11_yellow.png", ":/PlayerQt/Resources/image_pad/menu/11_yellow_touch.png"));
	connect(colorBtns_.back(), SIGNAL(clicked()), this, SLOT(onColor2()));
	colorBtns_.push_back(new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/12_bluepng.png", ":/PlayerQt/Resources/image_pad/menu/12_bluepng_touch.png"));
	connect(colorBtns_.back(), SIGNAL(clicked()), this, SLOT(onColor3()));
	colorBtns_.push_back(new TextUiBtn(":/PlayerQt/Resources/image_pad/menu/13_green.png", ":/PlayerQt/Resources/image_pad/menu/13_green_touch.png"));
	connect(colorBtns_.back(), SIGNAL(clicked()), this, SLOT(onColor4()));

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	gridLayout->addWidget(highlightBtn_, 0, 0);	
	gridLayout->addWidget(memoBtn_, 0, 1);
	gridLayout->addWidget(copyBtn_, 0, 2);
	gridLayout->addWidget(searchBtn_, 0, 3);
	gridLayout->addWidget(googleBtn_, 0, 4);
	widget_->setLayout(gridLayout);

	gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	gridLayout->addWidget(hiliteColorBtn_, 0, 0);			
	gridLayout->addWidget(memoBtn2_, 0, 1);
	gridLayout->addWidget(deleteBtn_, 0, 2);
	widgetHilite_->setLayout(gridLayout);

	gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	gridLayout->addWidget(newMemoBtn_, 0, 0);			
	gridLayout->addWidget(memosBtn_, 0, 1);
	widgetMemo_->setLayout(gridLayout);

	gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	for (int i = 0; i < (int)colorBtns_.size(); ++i) gridLayout->addWidget(colorBtns_[i], 0, i);
	widgetColors_->setLayout(gridLayout);

	
	mode_ = Start;
}

void TextSelectUI::move(SceneObject * text, unsigned int index, unsigned int len)
{
	mainWindow_->document()->textFeatures()->showText(text, index);	
	mainWindow_->document()->textFeatures()->selectPart(text, index, index + len);
	mode_ = Search;
	showUi(pos_);
}

void TextSelectUI::moveMemo(SceneObject * text, unsigned int index)
{
	mainWindow_->document()->textFeatures()->showText(text, index);	
	mainWindow_->document()->textFeatures()->selectPart(text, index, index);
	mode_ = MemoBar;
	showUi(pos_);
}

void TextSelectUI::onPrev()
{
	TextFeatures * tf = mainWindow_->document()->textFeatures();	
	QString searchStr = widgetSearch_->searchStr(); 

	QByteArray bytes = searchStr.toUtf8();
	std::string utf8Str(bytes.begin(), bytes.end());
	vector<TFSearchResult> results;
	tf->search(results, utf8Str);

	if (results.empty()) 
	{
		//no results
		return;
	}

	QString selStr = QString::fromUtf8(tf->selectedText().c_str());

	if (selStr == searchStr)
	{
		int startIndex, endIndex;
		SceneObject * text = tf->selTextObj(&startIndex, &endIndex);
		for (int i = 0; i < (int)results.size(); ++i)
		{
			if (results[i].text != text) continue;
			for (int j = 0; j < (int)results[i].indices.size(); ++j)
			{
				if (results[i].indices[j] == startIndex)
				{
					//move previous
					int prevI = i;
					int prevJ = j - 1;
					
					if (prevJ < 0)
					{
						prevI = i - 1;
						if (prevI < 0) prevI = results.size() - 1;
						prevJ = results[prevI].indices.size() - 1;						
					}
					move(results[prevI].text,  results[prevI].indices[prevJ], searchStr.size());
				}
			}
		}
	}
	else
		move(results[0].text, results[0].indices[0], searchStr.size());
}

void TextSelectUI::onNext()
{
	TextFeatures * tf = mainWindow_->document()->textFeatures();	
	QString searchStr = widgetSearch_->searchStr(); 

	QByteArray bytes = searchStr.toUtf8();
	std::string utf8Str(bytes.begin(), bytes.end());
	vector<TFSearchResult> results;
	tf->search(results, utf8Str);

	if (results.empty()) 
	{
		//no results
		return;
	}

	QString selStr = QString::fromUtf8(tf->selectedText().c_str());

	if (selStr == searchStr)
	{
		int startIndex, endIndex;
		SceneObject * text = tf->selTextObj(&startIndex, &endIndex);
		for (int i = 0; i < (int)results.size(); ++i)
		{
			if (results[i].text != text) continue;
			for (int j = 0; j < (int)results[i].indices.size(); ++j)
			{
				if (results[i].indices[j] == startIndex)
				{
					//move next
					int nextI = i;
					int nextJ = j + 1;

					if (nextJ >= results[nextI].indices.size())
					{
						nextI = i + 1;
						if (nextI >= results.size()) nextI = 0;
						nextJ = 0;						
					}
					move(results[nextI].text,  results[nextI].indices[nextJ], searchStr.size());
				}
			}
		}
	}
	else
		move(results[0].text, results[0].indices[0], searchStr.size());
}

void TextSelectUI::onSearchResults()
{
	QString searchStr = widgetSearch_->searchStr(); 

	if (lastSearchStr_ != searchStr) 
	{
		QByteArray bytes = searchStr.toUtf8();
		std::string utf8Str(bytes.begin(), bytes.end());
		vector<TFSearchResult> results;
		mainWindow_->document()->textFeatures()->search(results, utf8Str);
		widgetSearchResults_->setResults(searchStr.toStdWString(), results);

		lastSearchStr_ = searchStr;
	}
	
	mode_ = SearchResults;
	showUi(pos_);
}

void TextSelectUI::onSearchResClose()
{	
	mode_ = Search;
	showUi(pos_);
}

void TextSelectUI::onMemosClose()
{	
	mode_ = MemoBar;
	showUi(pos_);
}

TextSelectUI::~TextSelectUI()
{
	delete widget_;
}

void TextSelectUI::hideUi()
{
	widget_->hide();
	upArrow_->hide();
	downArrow_->hide();
	widgetHilite_->hide();
	widgetMemo_->hide();
	widgetColors_->hide();
	widgetSearch_->hide();
	widgetSearchResults_->hide();
	widgetMemos_->hide();
	widgetNewMemo_->hide();
	mode_ = Start;
	showing_ = false;
}

void TextSelectUI::showUi(const QPoint & qpos)
{
	pos_ = qpos;
	widget_->hide();
	widgetHilite_->hide();
	widgetMemo_->hide();
	widgetColors_->hide();
	widgetSearch_->hide();
	downArrow_->hide();
	upArrow_->hide();	
	widgetSearchResults_->hide();
	widgetMemos_->hide();
	widgetNewMemo_->hide();

	QWidget * widget;
	switch(mode_)
	{
	case Start: widget = widget_; break;
	case Hilite: widget = widgetHilite_; break;
	case HiliteColor: widget = widgetColors_; break;
	case Search: widget = widgetSearch_; break;
	case MemoBar: widget = widgetMemo_; break;
	case SearchResults: widget = widgetSearchResults_; break;	
	case Memos: widget = widgetMemos_; break;
	case NewMemo: widget = widgetNewMemo_; break;
	}

	widget->show();

	if (mode_ < SearchResults)
	{
		int margin = 65;
		int posY = qpos.y() - widget->height() - downArrow_->height() -  margin;
		int posX = qpos.x() - widget->width()/2;

		if(posX < 0)
			posX = 0;
		if(posX + widget->width() > mainWindow_->width())
			posX -= (posX + widget->width()) - mainWindow_->width();

		if (posY > 0)
		{
			widget->move(posX, posY);	
			downArrow_->show();
			/*downArrow_->move(
				posX + widget->width()/2 - downArrow_->width()/2, 
				posY + widget->height());*/

			downArrow_->move(qpos.x() - downArrow_->width()/2, posY + widget->height());
		}
		else
		{		
			posY = qpos.y() + 5 + upArrow_->height();
			widget->move(posX, posY);
			upArrow_->show();
			/*upArrow_->move(
				posX + widget->width()/2 - upArrow_->width()/2,
				posY - upArrow_->height());*/

			upArrow_->move(qpos.x() - upArrow_->width()/2, posY - widget->height());
		}
	}
	else
	{	
		QGraphicsView * view = mainWindow_->graphicsView();
		int w = view->width();
		int h = view->height();
		widget->move(w / 2 - widget->width() / 2, h / 2 - widget->height() / 2);

	}
	
	showing_ = true;
	
	
}

void TextSelectUI::onPushHighlightBtn()
{
	mode_ = Hilite;
	showUi(pos_);
}

void TextSelectUI::onHiliteColor()
{
	mode_ = HiliteColor;
	showUi(pos_);
}

void TextSelectUI::onHiliteDelete()
{
	mainWindow_->document()->textFeatures()->removeHighlight();
}

void TextSelectUI::onColor0()
{
	mainWindow_->document()->textFeatures()->
		addHighlight(Color(255.0 / 255.0, 0.0 / 255.0, 28.0 / 255.0, 255.0 / 255.0));	
}

void TextSelectUI::onColor1()
{
	mainWindow_->document()->textFeatures()->
		addHighlight(Color(251.0 / 255.0, 149.0 / 255.0, 40.0 / 255.0, 255.0 / 255.0));
}

void TextSelectUI::onColor2()
{
	mainWindow_->document()->textFeatures()->
		addHighlight(Color(254.0 / 255.0, 232.0 / 255.0, 99.0 / 255.0, 255.0 / 255.0));
}

void TextSelectUI::onColor3()
{
	mainWindow_->document()->textFeatures()->
		addHighlight(Color(139.0 / 255.0, 230.0 / 255.0, 252.0 / 255.0, 255.0 / 255.0));
}

void TextSelectUI::onColor4()
{
	mainWindow_->document()->textFeatures()->
		addHighlight(Color(139.0 / 255.0, 252.0 / 255.0, 168.0 / 255.0, 255.0 / 255.0));
}

void TextSelectUI::onPushSearchBtn()
{
	std::string utf8Str = mainWindow_->document()->textFeatures()->selectedText();	
	widgetSearch_->setText(QString::fromUtf8(utf8Str.c_str()));
	mode_ = Search;
	showUi(pos_);
}

void TextSelectUI::onPushMemoBtn()
{
	mode_ = MemoBar;
	showUi(pos_);
}

void TextSelectUI::onNewMemo()
{
	Document * d = mainWindow_->document();
	TextFeatures * tf = d->textFeatures();

	int startIndex, endIndex;
	SceneObject * text = tf->selTextObj(&startIndex, &endIndex);
	Memo * memo = NULL;
	vector<Memo *> memos;
	tf->getMemos(memos);
	for (int i = 0; i < (int)memos.size(); ++i)
	{
		if (memos[i]->textObj == text && memos[i]->charIndex == startIndex)
			memo = memos[i];
	}

	widgetNewMemo_->setMemo(memo);
	mode_ = NewMemo;
	showUi(pos_);
}

void TextSelectUI::onMemos()
{
	mode_ = Memos;
	widgetMemos_->populateMemos(mainWindow_->document()->textFeatures());
	showUi(pos_);
}

void TextSelectUI::onGoogle()
{
	Document * d = mainWindow_->document();
	string utf8SelText = d->textFeatures()->selectedText();
	if (utf8SelText.empty()) return;
	QUrl url(QString ("http://www.google.com/search?as_q=") + QString::fromUtf8(&utf8SelText[0]));
	QDesktopServices::openUrl(url);

}

void TextSelectUI::onCopy()
{
	Document * d = mainWindow_->document();
	string utf8SelText = d->textFeatures()->selectedText();
	if (utf8SelText.empty()) return;
	QApplication::clipboard()->setText(QString::fromUtf8(&utf8SelText[0]));
}

void TextSelectUI::memoClicked(Memo * memo)
{
	mode_ = NewMemo;
	Document * doc = mainWindow_->document();
	widgetNewMemo_->setMemo(memo);
	showUi(pos_);
}

void TextSelectUI::onNewMemoSave()
{
	Document * doc = mainWindow_->document();
	QByteArray utf8 = widgetNewMemo_->text().toUtf8();
	std::string memoStr(utf8.begin(), utf8.end());
	TextFeatures * tf = doc->textFeatures();
	tf->setMemo(widgetNewMemo_->memo(), memoStr);
	mode_ = MemoBar;

	int startIndex, endIndex;
	SceneObject * text = tf->selTextObj(&startIndex, &endIndex);
	if (text) showUi(pos_);	
	else hideUi();
	
}

void TextSelectUI::onNewMemoClose()
{
	Document * doc = mainWindow_->document();
	TextFeatures * tf = doc->textFeatures();
	mode_ = MemoBar;
	int startIndex, endIndex;
	SceneObject * text = tf->selTextObj(&startIndex, &endIndex);
	if (text) showUi(pos_);	
	else hideUi();
}

void TextSelectUI::resized()
{
	
	Document * doc = mainWindow_->document();
	Vector2 pos = doc->textFeatures()->selectedTextPos();		

	QPoint qpos = QPoint(
		pos.x, 
		pos.y);	

	if (showing_) showUi(qpos);
}