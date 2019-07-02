customObject._getData = function(data)
{
	return this.xml.toString();
}

customObject.setDataPrivate = function(data) {
    var xml = this.xml = XML(data);
    this.fontFile = xml.font.toString();
    this.text = xml.text.toString();
    this.width = parseInt(xml.width);
    this.height = parseInt(xml.height);
    this.correctChoice = parseInt(xml.correctChoice);
    this.checkedImage = xml.checkedImage.toString();
    this.uncheckedImage = xml.uncheckedImage.toString();
    this.checkAnswerImage = xml.checkAnswerImage.toString();
    this.correctImage = xml.correctImage.toString();
    this.incorrectImage = xml.incorrectImage.toString();
    this.fontSize = parseInt(xml.fontSize.toString());
    this.columns = parseInt(xml.columns.toString());
    if (this.columns <= 0 || this.columns != this.columns) this.columns = 1;
    this.margins = 10;
    this.curChoice = -1;

}

customObject.getChoice = function()
{
	return this.curChoice;
}

customObject.setChoice = function(choiceIndex)
{
	var imgObj;
	if (this.curChoice >= 0) {
		imgObj = this.choices[this.curChoice].uncheckedImgObj;
		imgObj.setVisible(true);
	}
	this.curChoice = choiceIndex;
	if (choiceIndex >= 0)
	{
		var curChoice = this.choices[this.curChoice]
		imgObj = curChoice.uncheckedImgObj;

		imgObj.setVisible(false);
		this.checkedImg.setTransform({ translation: {
			x: curChoice.checkedX,
			y: curChoice.checkedY,
			z: 0
		}
		});
		this.checkedImg.setVisible(true);
	}
	else
		this.checkedImg.setVisible(false);
}


customObject._getReferencedFiles = function() {
    var ret = new Array();
    ret.push(this.fontFile);
    ret.push(this.checkedImage);
    ret.push(this.uncheckedImage);
    ret.push(this.correctImage);
    ret.push(this.incorrectImage);
    ret.push(this.checkAnswerImage);
    return ret;
}

customObject._setReferencedFiles = function(newFilenames) {
    this.fontFile = newFilenames[0];
    this.checkedImage = newFilenames[1];
    this.uncheckedImage = newFilenames[2];
    this.correctImage = newFilenames[3];
    this.incorrectImage = newFilenames[4];
    this.checkAnswerImage = newFilenames[5];

    this.xml.font = this.fontFile;
    this.xml.checkedImage = this.checkedImage;
    this.xml.uncheckedImage = this.uncheckedImage;
    this.xml.correctImage = this.correctImage;
    this.xml.incorrectImage = this.incorrectImage;
    this.xml.checkAnswerImage = this.checkAnswerImage;
}

customObject._setData = function(data)
{	
	this.setDataPrivate(data);
}

customObject._init = function(data) {
    this.setDataPrivate(data);
    this.checkAnswerTimeoutId = -1;
    this.textProps =
		[{ index: 0, color: { r: 0.0, g: 0.0, b: 0.0 }, font: { bold: false, pointSize: this.fontSize, fontFile: this.fontFile}}];

    this.bgRect = this.root.createSceneObject("Rectangle");
    this.bgRect.setWidth(this.width);
    this.bgRect.setHeight(this.height);
    this.bgRect.setColor({ r: 1.0, g: 1.0, b: 1.0 });

    this.textObj = this.root.createSceneObject("Text");
    this.textObj.setText(this.text);
    this.textObj.setProperties(this.textProps);
    this.textObj.setTransform({ translation: { x: this.margins, y: this.margins, z: 0} });
    this.textObj.setBoundaryWidth(this.width - this.margins * 2);

    this.checkedImg = this.root.createSceneObject("Image");
    this.checkedImg.setFileName(this.checkedImage);
    this.checkedImg.setVisible(false);

    this.checkAnswerImg = this.root.createSceneObject("Image");
    this.checkAnswerImg.setFileName(this.checkAnswerImage);

    function getOnCheckAnswer(thisObject) {
        return function(evt) {
            var checkedWidth = thisObject.checkedImg.getWidth();
            var checkedHeight = thisObject.checkedImg.getHeight();

            if (thisObject.curChoice < 0) return;
            var choice = thisObject.choices[thisObject.curChoice];

            if (thisObject.curChoice == thisObject.correctChoice) {
                var x = choice.checkedX + thisObject.checkedImg.getWidth() / 2 - thisObject.correctImg.getWidth() / 2;
                var y = choice.checkedY + thisObject.checkedImg.getHeight() / 2 - thisObject.correctImg.getHeight() / 2;
                thisObject.correctImg.setTransform({ translation: { x: x, y: y, z: 0} });

                thisObject.correctImg.setVisible(true);
                thisObject.incorrectImg.setVisible(false);
            }
            else {
                var x = choice.checkedX + thisObject.checkedImg.getWidth() / 2 - thisObject.incorrectImg.getWidth() / 2;
                var y = choice.checkedY + thisObject.checkedImg.getHeight() / 2 - thisObject.incorrectImg.getHeight() / 2;
                thisObject.incorrectImg.setTransform({ translation: { x: x, y: y, z: 0} });

                thisObject.correctImg.setVisible(false);
                thisObject.incorrectImg.setVisible(true);

            }
            thisObject.checkDlgGrp.setVisible(true);
            this.checkAnswerTimeoutId = setTimeout(function() { thisObject.checkDlgGrp.setVisible(false); document.triggerRedraw(); }, 1000);
        }
    }

    this.checkAnswerImg.addEventListener("Tap", getOnCheckAnswer(this));

    this.choices = new Array();
    for (var i = 0; i < this.xml.choice.length(); ++i) {
        var choiceObj = this.root.createSceneObject("Text");
        choiceObj.setText(this.xml.choice[i].toString());
        choiceObj.setProperties(this.textProps);

        var uncheckedImg = this.root.createSceneObject("Image");
        uncheckedImg.setFileName(this.uncheckedImage);
        choiceObj.uncheckedImgObj = uncheckedImg;

        function getOnTap(thisObject, choiceIndex) {
            return function(evt) {
                thisObject.setChoice(choiceIndex);
            }
        }
        uncheckedImg.addEventListener("Tap", getOnTap(this, i));
        choiceObj.addEventListener("Tap", getOnTap(this, i));

        this.choices.push(choiceObj);

    }

    /*
    function getOnCheckDlgTap(thisObj) {
    return function(evt) {
    thisObj.checkDlgGrp.setVisible(false);
    }
    }*/

    this.checkDlgGrp = this.root.createSceneObject("Group");
    this.checkDlgGrp.setVisible(false);
    //this.checkDlgGrp.addEventListener("Tap", getOnCheckDlgTap(this));
    this.checkDlgBgRect = this.checkDlgGrp.createSceneObject("Rectangle");
    this.checkDlgBgRect.setWidth(this.width);
    this.checkDlgBgRect.setHeight(this.height);
    this.checkDlgBgRect.setColor({ r: 0.0, g: 0.0, b: 0.0 });
    this.checkDlgBgRect.setOpacity(0.0);

    this.correctImg = this.checkDlgGrp.createSceneObject("Image");
    this.correctImg.setFileName(this.correctImage);
    this.correctImg.setVisible(false);

    this.incorrectImg = this.checkDlgGrp.createSceneObject("Image");
    this.incorrectImg.setFileName(this.incorrectImage);
    this.incorrectImg.setVisible(false);

    this.root.setVisible(false);
}

customObject._uninit = function() {

    var children = this.root.getChildren();
    for (var i = 0; i < children.length; ++i) {
        this.root.removeChild(children[i]);
    }

    if (this.checkAnswerTimeoutId >= 0) clearTimeout(this.checkAnswerTimeoutId);
}

customObject._onLoaded = function() {
    this.root.setVisible(true);

    var checkAnswerImageWidth = this.checkAnswerImg.getWidth();
    var checkAnswerImageHeight = this.checkAnswerImg.getHeight();
    var checkedWidth = this.checkedImg.getWidth();
    var checkedHeight = this.checkedImg.getHeight();
    var uncheckedWidth = this.choices[0].uncheckedImgObj.getWidth();
    var uncheckedHeight = this.choices[0].uncheckedImgObj.getHeight();
    var maxCWidth = Math.max(uncheckedWidth, checkedWidth);
    var maxCHeight = Math.max(uncheckedHeight, checkedHeight);

    var colWidth = (this.width - this.margins) / this.columns - this.margins;

    var minPerCol = Math.floor(this.choices.length / this.columns);
    var maxPerCol = Math.ceil(this.choices.length / this.columns);
    var choicesPerCol = []; for (var i = 0; i < this.columns; ++i) choicesPerCol[i] = minPerCol;
    var totalQs = minPerCol * this.columns;
    var curCol = 0;
    while (totalQs < this.choices.length) {
        choicesPerCol[curCol++]++;
        totalQs++;
        if (curCol >= this.columns) curCol = 0;
    }

    this.checkAnswerImg.setTransform({ translation: { x: this.width / 2 - checkAnswerImageWidth / 2, y: this.height - this.margins - checkAnswerImageHeight, z: 0} });

    var extents = this.textObj.getExtents();
    var choicesHeight = this.height - extents.maxPt.y - 2 * this.margins - checkAnswerImageHeight;
    var curCol = 0;
    var curRow = 0;
    for (var i = 0; i < this.choices.length; ++i) {

        var choiceExtents = this.choices[i].getExtents();
        var x = this.margins + maxCWidth + 4 + curCol * (colWidth + this.margins);
        var y = extents.maxPt.y + choicesHeight / (maxPerCol + 1) * (curRow + 1);
        var checkedX = x - 4 - maxCWidth / 2 - checkedWidth / 2;
        var uncheckedX = x - 4 - maxCWidth / 2 - uncheckedWidth / 2;

        this.choices[i].checkedX = checkedX;
        this.choices[i].checkedY = y - checkedHeight / 2;

        this.choices[i].setTransform({ translation: { x: x, y: y - choiceExtents.maxPt.y / 2, z: 0} });
        this.choices[i].uncheckedImgObj.setTransform({ translation: {
            x: uncheckedX,
            y: y - uncheckedHeight / 2,
            z: 0
        }
        });

        ++curRow;
        if (curRow >= choicesPerCol[curCol]) {
            curRow = 0;
            ++curCol;
        }
    }

}


