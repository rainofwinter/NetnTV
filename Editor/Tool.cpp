#include "stdafx.h"
#include "Tool.h"
#include "GLWidget.h"

Tool::Tool(GLWidget * gl)
{
	glWidget_ = gl;	
	gl_ = glWidget_->renderer();
	document_ = glWidget_->document();
	active_ = false;
}



Tool::~Tool()
{
	uninit();
}

void Tool::init()
{
}

void Tool::uninit()
{
}

