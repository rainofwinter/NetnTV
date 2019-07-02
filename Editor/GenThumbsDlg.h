#pragma once
#include "ui_GenThumbsDlg.h"
#include "MagazineArticle.h"

class EditorDocument;
class Document;
class Scene;
class Article;
class ArticleManager;


class GenThumbsDlg : public QDialog
{
	Q_OBJECT
public:
	GenThumbsDlg(
		EditorDocument * document,
		const QString & folder,
		const std::vector<Scene *> & scenes, 
		int thumbWidth, int thumbHeight,		
		QWidget * parent);

	~GenThumbsDlg();

	int numScenesToRender() const {return scenesToRender_.size();}

	const std::map<Scene *, QString> & thumbFileMap() const
	{
		return thumbFileMap_;
	}

private slots:
	void onCancel();
private:
	virtual void closeEvent(QCloseEvent * event);
	virtual void timerEvent(QTimerEvent * event);
	virtual void showEvent(QShowEvent * event);
private:
	Ui::GenThumbsDlg ui;

	QBasicTimer timer_;
	QString folder_;

	EditorDocument * document_;
	std::vector<MagazineArticleSPtr> articles_;
	std::vector<Scene *> scenesToRender_;
	int curScene_;
	/*
	TextureRenderer * textureRenderer_;
	Texture * texture_;
	*/
	unsigned char * tempBuffer_;
	int thumbWidth_, thumbHeight_;

	std::map<Scene *, QString> thumbFileMap_;

	QString statusText_;
	bool requestCancel_;
};