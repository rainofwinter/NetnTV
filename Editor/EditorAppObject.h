#pragma once

class AppObject;
class PropertyPage;
class GLWidget;
class EditorDocument;
class EditorScene;

class EditorAppObject
{
public:	
	EditorAppObject();
	virtual ~EditorAppObject();
	virtual EditorAppObject * clone() const = 0;
	virtual PropertyPage * propertyPage() const {return 0;}

	///response to a New Object command (may show appropriate object creation 
	///dialog box)
	virtual AppObject * createAppObject() const = 0;
	virtual boost::uuids::uuid appObjectType() const = 0;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const = 0;

	void setAppObject(AppObject * obj) {appObject_ = obj;}
	AppObject * appObject() const {return appObject_;}

	void setScene(EditorScene * scene) {parentScene_ = scene;}
	EditorScene * parentScene() const {return parentScene_;}

protected:
	EditorScene * parentScene_;
	AppObject * appObject_;

	
};