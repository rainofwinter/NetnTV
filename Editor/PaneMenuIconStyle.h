#include <QCleanlooksStyle>

class PaneMenuIconStyle : public QCleanlooksStyle
{
	Q_OBJECT
public:
	int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0) const
	{
		int s = QCleanlooksStyle::pixelMetric(metric, option, widget);
		if(metric == QStyle::PM_SmallIconSize)
		{
			s = 38;
		}
		return s;
	}
};