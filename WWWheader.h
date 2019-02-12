#ifndef SwissalpS_QtSssSapp_WWWHEADER_H
#define SwissalpS_QtSssSapp_WWWHEADER_H

#include <QByteArray>
#include <QObject>



namespace SwissalpS { namespace QtSssSapp {



class WWWheader : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(WWWheader)

private:

protected:
	QString sTitle;
	QString sValue;

public:
	// null constructor for convenience
	explicit WWWheader(QObject *pParent = nullptr) :
		QObject(pParent), sTitle(""), sValue("") {}

	// constructor for request
	explicit WWWheader(QByteArray &aubRaw, QObject *pParent = nullptr);
	// constructor for response
	explicit WWWheader(const QString &sTitle, const QString &sValue = "",
					   QObject *pParent = nullptr) :
		QObject(pParent), sTitle(sTitle), sValue(sValue) {}

	inline virtual ~WWWheader() {}

	inline bool isNull() const { return this->sTitle.isEmpty(); }
	inline void setTitle(const QString &sT) { this->sTitle = sT; }
	inline void setValue(const QString &sV) { this->sValue = sV; }
	inline QString title() const { return this->sTitle; }
	inline QString toString() const { return QString("%1: %2")
				.arg(this->sTitle).arg(this->sValue); }

	inline QString value() const { return this->sValue; }

}; // WWWheader



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWHEADER_H
