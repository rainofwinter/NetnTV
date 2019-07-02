#ifndef Interpolator_h__
#define Interpolator_h__

enum InterpolatorType
{
	InterpolatorStep,
	InterpolatorLinear,
	InterpolatorEaseIn,
	InterpolatorEaseOut,
	Custom
};


class Interpolator
{
public:
	virtual const char * name() const = 0;
	virtual ~Interpolator() {}
	virtual float interpolate(float t) = 0;
	virtual InterpolatorType type() const = 0;
};


class StepInterpolator : public Interpolator
{
public:
	virtual const char * name() const {return "step";}
	virtual float interpolate(float t) {return 0;}
	virtual InterpolatorType type() const {return InterpolatorStep;}
};

class LinearInterpolator : public Interpolator
{
public:
	virtual const char * name() const {return "linear";}
	virtual float interpolate(float t) {return t;}
	virtual InterpolatorType type() const {return InterpolatorLinear;}
};

class EaseInInterpolator : public Interpolator
{
public:
	virtual const char * name() const {return "ease in";}
	virtual float interpolate(float t) {return t*t*t;}
	virtual InterpolatorType type() const {return InterpolatorEaseIn;}
};

class EaseOutInterpolator : public Interpolator
{
public:
	virtual const char * name() const {return "ease out";}
	virtual float interpolate(float t) {return 1+(t - 1)*(t - 1)*(t - 1);}
	virtual InterpolatorType type() const {return InterpolatorEaseOut;}
};

extern LinearInterpolator gLinearInterpolator;
extern StepInterpolator gStepInterpolator;
extern EaseInInterpolator gEaseInInterpolator;
extern EaseOutInterpolator gEaseOutInterpolator;
#endif // Interpolator_h__