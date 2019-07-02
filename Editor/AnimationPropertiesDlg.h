#pragma once
#include <QDialog>
#include <ui_AnimationPropertiesDlg.h>

class Animation;

class AnimationPropertiesDlg : public QDialog
{
public:
	AnimationPropertiesDlg(Animation * animation, QWidget * parent);
	bool repeat() const;
	SceneObject * syncObj() const;
private slots:
	void onOk();
private:
	int getSyncObjIndex(SceneObject * obj) const;
private:
	Ui::AnimationPropertiesDlg ui;

	std::vector<SceneObject *> sceneSyncObjs_;
};