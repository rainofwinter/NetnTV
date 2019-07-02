#ifndef Error_h__
#define Error_h__

enum ErrorCode
{
	Ok,
	KeyFrameExists,
	KeyFrameNotFound,
	WrongKeyFrameType,
	NoTargetObject,
	ChannelNotFound,
	FileIO,
	InvalidArguments,
	InvalidState,
	GenericError
};

#define HANDLE_GL_ERR() if (GLenum err = glGetError()){std::cout << "gl error " << err << " at " << __FILE__ << ":" << __LINE__ << "\n";}

#endif // Error_h__